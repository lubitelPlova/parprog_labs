#include "io_image.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

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

int relief_by_channel(unsigned char *img, const int width, const int height, const int channels)
{
    const int total_size = width * height * channels;
    unsigned char *out_img = (unsigned char *)malloc(total_size);
    if (!out_img)
    {
        fprintf(stderr, "Memory allocation failed\n");
        return -1;
    }
    memcpy(out_img, img, total_size);

    const int relief_kernel[3][3] = {
        {-2, -1, 0},
        {-1, 1, 1},
        {0, 1, 2},
    };
    for (int y = 1; y < height - 1; y++)
    {
        for (int x = 1; x < width - 1; x++)
        {
            for (int channel = 0; channel < channels; channel++)
            {
                int sum_of_pix = 0;
                int raw_pixel;
                unsigned char *out_pixel = out_img + (y * width + x) * channels + channel;
                for (int ky = -1; ky <= 1; ky++)
                {
                    for (int kx = -1; kx <= 1; kx++)
                    {
                        unsigned char *in_pixel = img + ((y + ky) * width + (x + kx)) * channels + channel;
                        int raw_pixel = (int)*in_pixel * relief_kernel[ky + 1][kx + 1];
                        sum_of_pix += raw_pixel;
                    }
                }
                sum_of_pix = MAX(0, sum_of_pix);
                sum_of_pix = MIN(255, sum_of_pix);
                *out_pixel = (unsigned char)sum_of_pix;
            }
        }
    }
    memcpy(img, out_img, total_size);
    free(out_img);
    return 0;
}

unsigned char* resize_2lower(unsigned char* img, const int width, const int height, const int channels, 
                 int* out_width, int* out_height)
{
    *out_width = width / 2;
    *out_height = height / 2;

    const int total_size = (*out_width) * (*out_height) * channels;

    unsigned char* out_img = (unsigned char*)malloc(total_size); 
    if (!out_img)
    {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    for (int y = 0; y < (*out_height); ++y)
    {
        for (int x = 0; x < (*out_width); ++x)
        {
            for (int channel = 0; channel < channels; ++channel){
                unsigned char* out_pix = out_img + (y*(*out_width) + x) * channels + channel;
                int avg_pix = ((*(img + ((2*y*width + 2*x) * channels + channel)))
                            + (*(img + ((2*y*width + (2*x+1)) * channels + channel)))
                            + (*(img + (((2*y+1)*width + 2*x) * channels + channel)))
                            + (*(img + (((2*y+1)*width + (2*x+1)) * channels + channel))))/4;
                *out_pix = (unsigned char)avg_pix;
            }
        }
    }
    return out_img;
}