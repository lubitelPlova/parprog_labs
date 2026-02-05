#include "io_image.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

int has_extension(const char *filename, const char *ext)
{
    const char *dot = strchr(filename, '.');

    if (!dot || dot == filename)
    {
        return 0;
    }

    return strcmp(dot, ext) == 0;
}

int relief_parallel_by_row(unsigned char *img, const int width, const int height, const int channels)
{
    const int total_size = width * height * channels;
    const int row_stride = width * channels;

    unsigned char *out_img = (unsigned char *)aligned_alloc(64, total_size);
    if (!out_img)
    {
        fprintf(stderr, "Memory allocation failed\n");
        return -1;
    }
    memcpy(out_img, img, total_size);

    const int k00 = -2, k01 = -1, k02 = 0;
    const int k10 = -1, k11 = 1, k12 = 1;
    const int k20 = 0, k21 = 1, k22 = 2;

    #pragma omp parallel for schedule(static)
    for (int y = 1; y < height - 1; y++)
    {
        unsigned char *prev_row = img + (y - 1) * row_stride;
        unsigned char *cur_row = img + y * row_stride;
        unsigned char *next_row = img + (y + 1) * row_stride;
        unsigned char *out_row = out_img + y * row_stride;

        for (int x = 1; x < width - 1; x++)
        {
            const int base_idx = x * channels;
            const int prev_idx = base_idx - channels;
            const int next_idx = base_idx + channels;

            for (int channel = 0; channel < channels; channel++)
            {
                int sum_of_pix = 0;

                sum_of_pix += prev_row[prev_idx + channel] * k00;
                sum_of_pix += prev_row[base_idx + channel] * k01;
                sum_of_pix += prev_row[next_idx + channel] * k02;

                sum_of_pix += cur_row[prev_idx + channel] * k10;
                sum_of_pix += cur_row[base_idx + channel] * k11;
                sum_of_pix += cur_row[next_idx + channel] * k12;

                sum_of_pix += next_row[prev_idx + channel] * k20;
                sum_of_pix += next_row[base_idx + channel] * k21;
                sum_of_pix += next_row[next_idx + channel] * k22;

                if (sum_of_pix < 0)
                    sum_of_pix = 0;
                else if (sum_of_pix > 255)
                    sum_of_pix = 255;

                out_row[base_idx + channel] = (unsigned char)sum_of_pix;
            }
        }
    }
    memcpy(img, out_img, total_size);
    free(out_img);
    return 0;
}

unsigned char *resize_2lower(unsigned char *img, const int width, const int height, const int channels,
                             int *out_width, int *out_height)
{
    *out_width = width / 2;
    *out_height = height / 2;

    const int total_size = (*out_width) * (*out_height) * channels;

    const int in_row_stride = width * channels;
    const int out_row_stride = (*out_width) * channels;

    unsigned char *out_img = (unsigned char *)aligned_alloc(64, total_size);
    if (!out_img)
    {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    
    #pragma omp parallel for schedule(static)
    for (int y = 0; y < (*out_height); ++y)
    {
        const int in_y1 = 2 * y;
        const int in_y2 = in_y1 + 1;

        const unsigned char *in_row1 = img + in_y1 * in_row_stride;
        const unsigned char *in_row2 = img + in_y2 * in_row_stride;

        unsigned char *out_row = out_img + y * out_row_stride;

        for (int x = 0; x < (*out_width); ++x)
        {
            const int idx1 = 2 * x * channels;
            const int idx2 = (2 * x + 1) * channels;

            for (int channel = 0; channel < channels; ++channel)
            {
                int avg_pix = in_row1[idx1 + channel] +
                              in_row1[idx2 + channel] +
                              in_row2[idx1 + channel] +
                              in_row2[idx2 + channel];
                out_row[x * channels + channel] = (unsigned char)(avg_pix >> 2);
            }
        }
    }
    return out_img;
}