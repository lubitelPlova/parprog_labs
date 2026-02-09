#include "utils.hpp"
#include <algorithm>
#include <vector>
#include <thread>

void intensity_rows(const cv::Mat &bgr_image, cv::Mat &out_image, const int start_row, const int end_row)
{
    for (int y = start_row; y < end_row; y++)
    {
        for (int x = 0; x < bgr_image.cols; x++)
        {
            cv::Vec3b pixel = bgr_image.at<cv::Vec3b>(y, x);
            out_image.at<uchar>(y, x) = (pixel[2] + pixel[1] + pixel[0]) / 3;
        }
    }
}

cv::Mat intensity(const cv::Mat &bgr_image, const int num_threads)
{
    cv::Mat gray(bgr_image.rows, bgr_image.cols, CV_8UC1);
    std::vector<std::thread> threads;
    int rows_per_thread = bgr_image.rows / num_threads;

    for (int i = 0; i < num_threads; i++)
    {
        int start = i * rows_per_thread;
        int end = (i == num_threads - 1) ? bgr_image.rows : start + rows_per_thread;
        threads.emplace_back([&bgr_image, &gray, start, end]() {
            intensity_rows(bgr_image, gray, start, end);
        });
    }

    for (auto &thread : threads)
    {
        thread.join();
    }

    return gray;
}

void thresh_rows(const cv::Mat &gray_image, cv::Mat &out_image, const int thresh, const int start_row, const int end_row)
{
    for (int y = start_row; y < end_row; y++)
    {
        for (int x = 0; x < gray_image.cols; x++)
        {
            out_image.at<uchar>(y, x) = (gray_image.at<uchar>(y, x) >= thresh) ? 255 : 0;
        }
    }
}

cv::Mat thresh(const cv::Mat &gray_image, const int thresh, const int num_threads)
{
    cv::Mat bin(gray_image.rows, gray_image.cols, CV_8UC1);
    std::vector<std::thread> threads;
    int rows_per_thread = gray_image.rows / num_threads;

    for (int i = 0; i < num_threads; i++)
    {
        int start = i * rows_per_thread;
        int end = (i == num_threads - 1) ? gray_image.rows : start + rows_per_thread;
        threads.emplace_back([&gray_image, &bin, thresh, start, end]() {
            thresh_rows(gray_image, bin, thresh, start, end);
        });
    }

    for (auto &thread : threads)
    {
        thread.join();
    }

    return bin;

}

void erode_rows(const cv::Mat &bin_image, cv::Mat& out_image, const int start_row, const int end_row)
{
    for (int y = start_row; y < end_row; y++)
    {
        const unsigned char *prev_row = bin_image.ptr<uchar>(y-1);
        const unsigned char *cur_row = bin_image.ptr<uchar>(y);
        const unsigned char *next_row = bin_image.ptr<uchar>(y+1);
        unsigned char *out_row = out_image.ptr<uchar>(y);

        for (int x = 1; x < bin_image.cols - 1; x++)
        {
            const int base_idx = x;
            const int prev_idx = base_idx - 1;
            const int next_idx = base_idx + 1;

            unsigned char lowest_pix = std::min(prev_row[prev_idx],
                                                prev_row[base_idx]);
            lowest_pix = std::min(lowest_pix, prev_row[next_idx]);

            lowest_pix = std::min(lowest_pix, cur_row[prev_idx]);
            lowest_pix = std::min(lowest_pix, cur_row[base_idx]);
            lowest_pix = std::min(lowest_pix, cur_row[next_idx]);

            lowest_pix = std::min(lowest_pix, next_row[prev_idx]);
            lowest_pix = std::min(lowest_pix, next_row[base_idx]);
            lowest_pix = std::min(lowest_pix, next_row[next_idx]);

            out_row[base_idx] = lowest_pix;
        }
    }
}

cv::Mat erode(cv::Mat &img, const int num_threads)
{
    cv::Mat eroded(img.rows, img.cols, CV_8UC1);

    std::vector<std::thread> threads;
    int rows_per_thread = (img.rows-2) / num_threads;

    for (int i = 0; i < num_threads; i++)
    {
        int start = i * rows_per_thread + 1;
        int end = (i == num_threads - 1) ? img.rows - 1 : start + rows_per_thread - 1;
        threads.emplace_back([&img, &eroded, start, end]() {
            erode_rows(img, eroded, start, end);
        });
    }

    for (auto &thread : threads)
    {
        thread.join();
    }

    return eroded;
}