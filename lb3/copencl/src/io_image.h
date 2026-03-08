#ifndef IO_IMAGE_H
#define IO_IMAGE_H

#include "types.h"
#include <stddef.h>

int has_extension(const char *filename, const char *ext);

char *construct_output_name(const char *input_name);

image_t* load_image(const char* input_name);

void free_image(image_t* img);

void save_img(image_t* img, const char* output_name);

char* load_kernel_source(const char* filename, size_t* size);

cl_context_t* create_cl_context(void);

void free_cl_context(cl_context_t* ctx);

#endif // IO_IMAGE_H