#include "types.h"
#include <stddef.h>


#ifndef IO_IMAGE_H
#define IO_IMAGE_H


int has_extension(const char *filename, const char *ext);

char *construct_output_name(const char *input_name);

image_t* load_image(const char* input_name);

void free_image(image_t* img);

void save_img(image_t* img, const char* output_name);

char* load_kernel_source(const char* filename, size_t* size);

#endif // IO_IMAGE_H