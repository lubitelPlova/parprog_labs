#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include "kernels.h"
#include <cmath>


__global__ void resizeBilinearKernel(const unsigned char* src, unsigned char* dst, 
                                     int srcW, int srcH, int srcStep,
                                     int dstW, int dstH, int dstStep,
                                     int channels) {
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    if (x < dstW && y < dstH) {
        float srcX = (x + 0.5f) * (float)srcW / dstW - 0.5f;
        float srcY = (y + 0.5f) * (float)srcH / dstH - 0.5f;

        srcX = fmaxf(0.0f, fminf((float)srcW - 1.0f, srcX));
        srcY = fmaxf(0.0f, fminf((float)srcH - 1.0f, srcY));

        int x0 = (int)floorf(srcX);
        int y0 = (int)floorf(srcY);
        int x1 = min(x0 + 1, srcW - 1);
        int y1 = min(y0 + 1, srcH - 1);

        float dx = srcX - x0;
        float dy = srcY - y0;

        for (int c = 0; c < channels; c++) {
            unsigned char p00 = src[y0 * srcStep + x0 * channels + c];
            unsigned char p01 = src[y0 * srcStep + x1 * channels + c];
            unsigned char p10 = src[y1 * srcStep + x0 * channels + c];
            unsigned char p11 = src[y1 * srcStep + x1 * channels + c];

            float val = (1 - dx) * (1 - dy) * p00 +
                        dx * (1 - dy) * p01 +
                        (1 - dx) * dy * p10 +
                        dx * dy * p11;

            dst[y * dstStep + x * channels + c] = static_cast<unsigned char>(val + 0.5f);
        }
    }
}

__global__ void rgb2grayKernel(const unsigned char* input, unsigned char* output, 
                               int width, int height, int step) {
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    if (x < width && y < height) {
        int idx = y * step + x * 3;
        unsigned char b = input[idx];
        unsigned char g = input[idx + 1];
        unsigned char r = input[idx + 2];

        float gray = (b + g + r)/3.0f;
        output[y * width + x] = static_cast<unsigned char>(gray + 0.5f);
    }
}

__global__ void fuseMultiScaleKernel(const unsigned char* img1, int w1, int h1, int step1,
                                     const unsigned char* img2, int w2, int h2, int step2,
                                     const unsigned char* img3, int w3, int h3, int step3,
                                     unsigned char* result, int outW, int outH, int outStep) {
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    if (x < outW && y < outH) {
        // Пиксель из оригинала (полное разрешение)
        int idx1 = y * step1 + x;
        unsigned char val1 = img1[idx1];

        // Пиксель из уменьшенного×2 (целочисленное деление)
        int x2 = x >> 1;  // x / 2
        int y2 = y >> 1;  // y / 2
        int idx2 = (y2 < h2 && x2 < w2) ? (y2 * step2 + x2) : 0;
        unsigned char val2 = (y2 < h2 && x2 < w2) ? img2[idx2] : val1;

        // Пиксель из уменьшенного×4 (целочисленное деление)
        int x4 = x >> 2;  // x / 4
        int y4 = y >> 2;  // y / 4
        int idx4 = (y4 < h3 && x4 < w3) ? (y4 * step3 + x4) : 0;
        unsigned char val4 = (y4 < h3 && x4 < w3) ? img3[idx4] : val1;

        // Усреднение (можно добавить веса)
        float fused = (val1 + val2 + val4) / 3.0f;
        result[y * outStep + x] = static_cast<unsigned char>(fused + 0.5f);
    }
}

// ============================================================================
// Функции-обертки
// ============================================================================

void gpuResize(const unsigned char* src, unsigned char* dst, 
               int srcW, int srcH, int srcStep,
               int dstW, int dstH, int dstStep,
               int channels) {
    dim3 blockSize(16, 16);
    dim3 gridSize((dstW + 15) / 16, (dstH + 15) / 16);
    resizeBilinearKernel<<<gridSize, blockSize>>>(src, dst, srcW, srcH, srcStep, 
                                                   dstW, dstH, dstStep, channels);
    cudaDeviceSynchronize();
}

void gpuConvertBgrToGray(const unsigned char* input, unsigned char* output, 
                         int width, int height, int step) {
    dim3 blockSize(16, 16);
    dim3 gridSize((width + 15) / 16, (height + 15) / 16);
    rgb2grayKernel<<<gridSize, blockSize>>>(input, output, width, height, step);
    cudaDeviceSynchronize();
}

void gpuFuseMultiScale(const unsigned char* img1, int w1, int h1, int step1,
                       const unsigned char* img2, int w2, int h2, int step2,
                       const unsigned char* img3, int w3, int h3, int step3,
                       unsigned char* result, int outW, int outH, int outStep) {
    dim3 blockSize(16, 16);
    dim3 gridSize((outW + 15) / 16, (outH + 15) / 16);
    fuseMultiScaleKernel<<<gridSize, blockSize>>>(img1, w1, h1, step1,
                                                   img2, w2, h2, step2,
                                                   img3, w3, h3, step3,
                                                   result, outW, outH, outStep);
    cudaDeviceSynchronize();
}