#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include "kernels.h"
#include <cmath>

__global__ void resizeTwice(const unsigned char *src, unsigned char *dst,
                            int width, int height, int step,
                            int channels)
{
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    int out_width = width >> 1;
    int out_height = height >> 1;
    int out_step = out_width * channels;

    if (x < out_width && y < out_height)
    {

        for (int c = 0; c < channels; c++)
        {
            unsigned char p00 = src[(2 * y) * step + (2 * x) * channels + c];
            unsigned char p01 = src[(2 * y) * step + (2 * x + 1) * channels + c];
            unsigned char p10 = src[(2 * y + 1) * step + (2 * x) * channels + c];
            unsigned char p11 = src[(2 * y + 1) * step + (2 * x + 1) * channels + c];

            unsigned int val = (p00 + p01 + p10 + p11) / 4;

            dst[y * out_step + x * channels + c] = static_cast<unsigned char>(val + 0.5f);
        }
    }
}

__global__ void rgb2gray(const unsigned char *input, unsigned char *output,
                         int width, int height, int step)
{
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    if (x < width && y < height)
    {
        int idx = y * step + x * 3;
        unsigned char b = input[idx];
        unsigned char g = input[idx + 1];
        unsigned char r = input[idx + 2];

        float gray = (b + g + r) / 3.0f;
        output[y * width + x] = static_cast<unsigned char>(gray + 0.5f);
    }
}

__global__ void fuseGrayscale(const unsigned char *img1, int w1, int h1, int step1,
                              const unsigned char *img2, int w2, int h2, int step2,
                              const unsigned char *img3, int w3, int h3, int step3,
                              unsigned char *result)
{
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    if (x < w1 && y < h1)
    {
        unsigned int fused = (img1[y * step1 + x] + img2[(y >> 1) * step2 + (x >> 1)] + img3[(y >> 2) * step3 + (x >> 2)] )/ 3;
        result[y * step1 + x] = static_cast<unsigned char>(fused);
    }
}

void gpuResize(const unsigned char *src, unsigned char *dst,
               int width, int height, int step,
               int channels)
{
    dim3 blockSize(16, 16);
    dim3 gridSize((width / 2 + 15) / 16, (height / 2 + 15) / 16);
    resizeTwice<<<gridSize, blockSize>>>(src, dst, width, height, step, channels);
    cudaDeviceSynchronize();
}

void gpuConvertBgrToGray(const unsigned char *input, unsigned char *output,
                         int width, int height, int step)
{
    dim3 blockSize(16, 16);
    dim3 gridSize((width + 15) / 16, (height + 15) / 16);
    rgb2gray<<<gridSize, blockSize>>>(input, output, width, height, step);
    cudaDeviceSynchronize();
}

void gpuFuseGrayscale(const unsigned char *img1, int w1, int h1, int step1,
                      const unsigned char *img2, int w2, int h2, int step2,
                      const unsigned char *img3, int w3, int h3, int step3,
                      unsigned char *result)
{
    dim3 blockSize(16, 16);
    dim3 gridSize((w1 + 15) / 16, (h1 + 15) / 16);
    fuseGrayscale<<<gridSize, blockSize>>>(img1, w1, h1, step1,
                                           img2, w2, h2, step2,
                                           img3, w3, h3, step3,
                                           result);
    cudaDeviceSynchronize();
}