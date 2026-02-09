#pragma once

#include <opencv2/opencv.hpp>

cv::Mat intensity(const cv::Mat &bgr_image, const int num_threads);

cv::Mat thresh(const cv::Mat &gray_image, const int thresh, const int num_threads);

cv::Mat erode(cv::Mat &img, const int num_threads);
