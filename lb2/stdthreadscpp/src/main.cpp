#include <iostream>
#include <chrono>
#include <string>
#include <filesystem>
#include <opencv2/opencv.hpp>
#include "utils.hpp"
#include <thread>

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
    else if (argc == 4)
    {
        input_name = (std::string)argv[1];
        std::filesystem::path stem = input_name.stem();
        output_name = "out_" + stem.string() + ".png";
        thresh_val = std::stoi(argv[2]);
        num_threads = std::stoi(argv[3]);
    }
    else
    {
        printf("wrong num of arguments: %d\n", argc);
        return 1;
    }

    if (num_threads < 1 || num_threads > 32)
    {
        std::cout << "Wrong num of threads: " << num_threads << std::endl;
        std::cout << "Threads must be in [1 ; " << std::thread::hardware_concurrency() << "]" << std::endl;
        return 1;
    }
    std::cout << "Num threads: " << num_threads << std::endl;
    cv::Mat input_img = cv::imread(input_name, cv::IMREAD_COLOR);

    auto start = std::chrono::high_resolution_clock::now();
    cv::Mat out = intensity(input_img, num_threads);
    auto end = std::chrono::high_resolution_clock::now();
    auto intensity_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << "Intensity: " << intensity_us << " us" << std::endl;

    start = std::chrono::high_resolution_clock::now();
    out = thresh(out, thresh_val, num_threads);
    end = std::chrono::high_resolution_clock::now();
    auto thresh_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << "Threshold: " << thresh_us << " us" << std::endl;

    cv::imwrite(output_name, out);

    start = std::chrono::high_resolution_clock::now();
    out = erode(out, num_threads);
    end = std::chrono::high_resolution_clock::now();
    auto erode_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << "Erode: " << erode_us << " us" << std::endl;

    auto total_us = intensity_us + thresh_us + erode_us;
    std::cout << "Total: " << total_us << " us (" << total_us / 1000.0 << " ms)" << std::endl;

    cv::imwrite("full" + (std::string)output_name, out);

    return 0;
}