#include <iostream>
#include <chrono>
#include <string>
#include <filesystem>

#include <opencv2/opencv.hpp>
#include "kernels.h"
#include <cuda_runtime.h>

#define CUDA_CHECK(call) \
    { \
        cudaError_t err = call; \
        if (err != cudaSuccess) { \
            std::cerr << "CUDA error: " << cudaGetErrorString(err) << std::endl; \
            exit(EXIT_FAILURE); \
        } \
    }

int main(int argc, char *argv[])
{

    std::filesystem::path input_name;
    std::filesystem::path output_name;
    int thresh_val;
    int num_threads;
    if (argc < 2)
    {
        printf("Usage: %s [INPUT_IMAGE_NAME] [THRESH_VAL] [NUM_THREADS] \nNote that input must be jpg or png\n", argv[0]);
        return 0;
    }
    else if (argc == 2)
    {
        input_name = (std::string)argv[1];
        std::filesystem::path stem = input_name.stem();
        output_name = "out_" + stem.string() + ".png";
    }
    else
    {
        printf("wrong num of arguments: %d\n", argc);
        return 1;
    }

    cv::Mat input_img = cv::imread(input_name, cv::IMREAD_COLOR);

     int srcW = input_img.cols;
    int srcH = input_img.rows;
    int srcStep = input_img.step;

    std::cout << "Original size: " << srcW << "x" << srcH << std::endl;

    int w2 = srcW / 2, h2 = srcH / 2;
    int w4 = srcW / 4, h4 = srcH / 4;

    size_t rgbSize = srcH * srcStep;
    size_t gray1Size = srcH * srcW;
    size_t gray2Size = h2 * w2;
    size_t gray3Size = h4 * w4;

    unsigned char *d_original, *d_half, *d_quarter;
    unsigned char *d_gray1, *d_gray2, *d_gray3;
    unsigned char *d_result;

    CUDA_CHECK(cudaMalloc(&d_original, rgbSize));
    CUDA_CHECK(cudaMalloc(&d_half, h2 * (w2 * 3))); 
    CUDA_CHECK(cudaMalloc(&d_quarter, h4 * (w4 * 3))); 
    
    CUDA_CHECK(cudaMalloc(&d_gray1, gray1Size)); 
    CUDA_CHECK(cudaMalloc(&d_gray2, gray2Size));  
    CUDA_CHECK(cudaMalloc(&d_gray3, gray3Size));
    
    CUDA_CHECK(cudaMalloc(&d_result, gray1Size)); 

    CUDA_CHECK(cudaMemcpy(d_original, input_img.ptr(), rgbSize, cudaMemcpyHostToDevice));

    std::cout << "Creating scaled copies on GPU..." << std::endl;
    gpuResize(d_original, d_half, srcW, srcH, srcStep, w2, h2, w2 * 3, 3);
    gpuResize(d_original, d_quarter, srcW, srcH, srcStep, w4, h4, w4 * 3, 3);

    std::cout << "Converting to Grayscale on GPU..." << std::endl;
    gpuConvertBgrToGray(d_original, d_gray1, srcW, srcH, srcStep);
    gpuConvertBgrToGray(d_half, d_gray2, w2, h2, w2 * 3);
    gpuConvertBgrToGray(d_quarter, d_gray3, w4, h4, w4 * 3);

    std::cout << "Fusing multi-scale images on GPU..." << std::endl;
    gpuFuseMultiScale(d_gray1, srcW, srcH, srcW,
                      d_gray2, w2, h2, w2,
                      d_gray3, w4, h4, w4,
                      d_result, srcW, srcH, srcW);

    cv::Mat out(srcH, srcW, CV_8UC1);
    CUDA_CHECK(cudaMemcpy(out.ptr(), d_result, gray1Size, cudaMemcpyDeviceToHost));

    CUDA_CHECK(cudaFree(d_original));
    CUDA_CHECK(cudaFree(d_half));
    CUDA_CHECK(cudaFree(d_quarter));
    CUDA_CHECK(cudaFree(d_gray1));
    CUDA_CHECK(cudaFree(d_gray2));
    CUDA_CHECK(cudaFree(d_gray3));
    CUDA_CHECK(cudaFree(d_result));

    cv::imwrite("full" + (std::string)output_name, out);

    return 0;
}