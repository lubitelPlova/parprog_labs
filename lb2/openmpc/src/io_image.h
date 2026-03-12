#ifndef IO_IMAGE_H
#define IO_IMAGE_H

int has_extension(const char *filename, const char *ext);

char *construct_output_name(const char *input_name);

int relief_parallel_row(unsigned char* img, const int width, const int height, const int channels);

unsigned char* resize_2lower_row(unsigned char* img, const int width, const int height, const int channels, 
                 int* out_width, int* out_height);

int relief_parallel_tiling(unsigned char* img, const int width, const int height, const int channels);
unsigned char* resize_2lower_tiling(unsigned char* img, const int width, const int height, const int channels, 
                 int* out_width, int* out_height);

#endif // IO_IMAGE_H