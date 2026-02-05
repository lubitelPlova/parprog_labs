#ifndef IO_IMAGE_H
#define IO_IMAGE_H

int has_extension(const char *filename, const char *ext);
int relief_parallel_by_row(unsigned char* img, const int width, const int height, const int channels);
unsigned char* resize_2lower(unsigned char* img, const int width, const int height, const int channels, 
                 int* out_width, int* out_height);
#endif // IO_IMAGE_H