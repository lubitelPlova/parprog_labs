#include "io_image.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

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

char *construct_output_name(const char *input_name)
{
    char *output_name;
    char *tmp;

    output_name = malloc(strlen(input_name) + 8 + 1);
    tmp = malloc(strlen(input_name));

    if (output_name == NULL)
    {
        return NULL;
    }

    char *dot = strrchr(input_name, '.');

    if (dot != NULL)
    {

        strncpy(tmp, input_name, dot - input_name);
        tmp[dot - input_name] = '\0';

        sprintf(output_name, "out_%s.png", tmp);
    }
    else
    {
        sprintf(output_name, "out_%s.png", input_name);
    }

    return output_name;
}

image_t *load_image(const char *input_name)
{
    int width, height, channels;
    unsigned char *img = stbi_load(input_name, &width, &height, &channels, 0);
    if (!img)
    {
        fprintf(stderr, "Load error\n");
        return NULL;
    }

    image_t *new_image = malloc(sizeof(image_t));
    if (!new_image)
    {
        stbi_image_free(img);
        fprintf(stderr, "Memory allocation error\n");
        return NULL;
    }

    new_image->width = width;
    new_image->height = height;
    new_image->channels = channels;
    new_image->data = img;
    return new_image;
}

void free_image(image_t *img)
{
    if (img)
    {
        if (img->data)
        {
            stbi_image_free(img->data);
        }
        free(img);
    }
}

void save_img(image_t *img, const char *output_name)
{
    stbi_write_png(output_name, img->width, img->height, img->channels, img->data, img->width * img->channels);
}

char* load_kernel_source(const char* filename, size_t* size) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Failed to open kernel file\n");
        return NULL;
    }
    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* source = malloc(*size + 1);
    fread(source, 1, *size, file);
    source[*size] = '\0';
    fclose(file);
    return source;
}