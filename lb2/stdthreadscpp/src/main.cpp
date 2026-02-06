#include <iostream>
#include <string>
#include <filesystem>
#include <opencv2/opencv.hpp>
#include "utils.hpp"

int main(int argc, char *argv[])
{

    std::filesystem::path input_name;
    std::filesystem::path output_name;
    int thresh_val;
    if (argc < 2)
    {
        printf("Usage: %s [INPUT_IMAGE_NAME]\nNote that input must be jpg or png\n", argv[0]);
        return 0;
    }
    else if (argc == 3)
    {
       input_name = (std::string)argv[1];
       std::filesystem::path stem = input_name.stem();
       output_name = "out_" + stem.string() + ".png";
       thresh_val = std::stoi(argv[2]);
        
    }
    else
    {
        printf("wrong num of arguments: %d\n", argc);
        return 1;
    }

    cv::Mat input_img = cv::imread(input_name, cv::IMREAD_COLOR);

    cv::Mat gray = intensity(input_img);
    thresh(gray, thresh_val);


    cv::imwrite(output_name, gray);

    return 0;
}