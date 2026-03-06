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
        printf("Usage: %s [INPUT_IMAGE_NAME] \nNote that input must be jpg or png\n", argv[0]);
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

    auto start_total = std::chrono::high_resolution_clock::now();

    auto start_read = std::chrono::high_resolution_clock::now();
    cv::Mat input_img = cv::imread(input_name, cv::IMREAD_COLOR);
    auto end_read = std::chrono::high_resolution_clock::now();
    auto duration_read = std::chrono::duration_cast<std::chrono::microseconds>(end_read - start_read);

    int input_w = input_img.cols;
    int input_h = input_img.rows;
    int input_step = input_img.step;

    std::cout << "Original size: " << input_w << "x" << input_h << std::endl;

    int w2 = input_w / 2, h2 = input_h / 2;
    int w4 = input_w / 4, h4 = input_h / 4;

    size_t rgbSize = input_h * input_step;
    size_t gray1Size = input_h * input_w;
    size_t gray2Size = h2 * w2;
    size_t gray3Size = h4 * w4;

    unsigned char *d_original;
    unsigned char *d_gray1, *d_gray2, *d_gray3;
    unsigned char *d_result;

    CUDA_CHECK(cudaMalloc(&d_original, rgbSize)); 
    
    CUDA_CHECK(cudaMalloc(&d_gray1, gray1Size)); 
    CUDA_CHECK(cudaMalloc(&d_gray2, gray2Size));  
    CUDA_CHECK(cudaMalloc(&d_gray3, gray3Size));
    
    CUDA_CHECK(cudaMalloc(&d_result, gray1Size)); 

    CUDA_CHECK(cudaMemcpy(d_original, input_img.ptr(), rgbSize, cudaMemcpyHostToDevice));

    auto start_gray1 = std::chrono::high_resolution_clock::now();
    gpuConvertBgrToGray(d_original, d_gray1, input_w, input_h, input_step);
    auto end_gray1 = std::chrono::high_resolution_clock::now();
    auto duration_gray1 = std::chrono::duration_cast<std::chrono::microseconds>(end_gray1 - start_gray1);

    auto start_resize1 = std::chrono::high_resolution_clock::now();
    gpuResize(d_gray1, d_gray2, input_w, input_h, input_w, 1);
    auto end_resize1 = std::chrono::high_resolution_clock::now();
    auto duration_resize1 = std::chrono::duration_cast<std::chrono::microseconds>(end_resize1 - start_resize1);

    auto start_resize2 = std::chrono::high_resolution_clock::now();
    gpuResize(d_gray2, d_gray3, w2, h2, w2, 1);  
    auto end_resize2 = std::chrono::high_resolution_clock::now();
    auto duration_resize2 = std::chrono::duration_cast<std::chrono::microseconds>(end_resize2 - start_resize2);


    auto start_fuse = std::chrono::high_resolution_clock::now();
    gpuFuseGrayscale(d_gray1, input_w, input_h, input_w,
                      d_gray2, w2, h2, w2,
                      d_gray3, w4, h4, w4,
                      d_result);
    auto end_fuse = std::chrono::high_resolution_clock::now();
    auto duration_fuse = std::chrono::duration_cast<std::chrono::microseconds>(end_fuse - start_fuse);

    cv::Mat out(input_h, input_w, CV_8UC1);
    CUDA_CHECK(cudaMemcpy(out.ptr(), d_result, gray1Size, cudaMemcpyDeviceToHost));

    CUDA_CHECK(cudaFree(d_original));
    CUDA_CHECK(cudaFree(d_gray1));
    CUDA_CHECK(cudaFree(d_gray2));
    CUDA_CHECK(cudaFree(d_gray3));
    CUDA_CHECK(cudaFree(d_result));

    auto start_write = std::chrono::high_resolution_clock::now();
    cv::imwrite("full" + (std::string)output_name, out);
    auto end_write = std::chrono::high_resolution_clock::now();
    auto duration_write = std::chrono::duration_cast<std::chrono::microseconds>(end_write - start_write);

    auto end_total = std::chrono::high_resolution_clock::now();
    auto duration_total = std::chrono::duration_cast<std::chrono::microseconds>(end_total - start_total);

    std::cout << "\nTimings:" << std::endl;
    std::cout << "\timage read:        " << duration_read.count() << " microseconds" << std::endl;
    std::cout << "\tgrayscale full:    " << duration_gray1.count() << " microseconds" << std::endl;
    std::cout << "\tresize half:       " << duration_resize1.count() << " microseconds" << std::endl;
    std::cout << "\tresize quarter:    " << duration_resize2.count() << " microseconds" << std::endl;
    std::cout << "\tfuse grayscale:    " << duration_fuse.count() << " microseconds" << std::endl;
    std::cout << "\tsave image:        " << duration_write.count() << " microseconds" << std::endl;
    std::cout << "\t-----------------------------------------" << std::endl;
    std::cout << "\ttotal time:        " << duration_total.count() << " microseconds" << std::endl;

    return 0;
}