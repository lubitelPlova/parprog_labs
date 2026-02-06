#pragma once

#include <opencv2/opencv.hpp>

cv::Mat intensity(const cv::Mat &bgr_image);

void thresh(cv::Mat &gray_image, const int thresh);