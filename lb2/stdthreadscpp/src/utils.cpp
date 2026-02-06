#include "utils.hpp"

cv::Mat intensity(const cv::Mat &bgr_image)
{
    cv::Mat gray(bgr_image.rows, bgr_image.cols, CV_8UC1);

    for (int y = 0; y < bgr_image.rows; y++)
    {
        for (int x = 0; x < bgr_image.cols; x++)
        {
            cv::Vec3b pixel = bgr_image.at<cv::Vec3b>(y, x);
            gray.at<uchar>(y, x) = (pixel[2] + pixel[1] + pixel[0]) / 3;
        }
    }
    return gray;
}

void thresh(cv::Mat &gray_image, const int thresh)
{
    for (int y = 0; y < gray_image.rows; y++)
    {
        for (int x = 0; x < gray_image.cols; x++)
        {
            gray_image.at<uchar>(y, x) = (gray_image.at<uchar>(y, x) >= thresh) ? 255 : 0;
        }
    }
}