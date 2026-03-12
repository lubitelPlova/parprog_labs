#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <omp.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "io_image.h"

int main(int argc, char *argv[])
{

    char *input_name;
    char *output_name;
    if (argc == 1)
    {
        printf("Usage: %s [INPUT_IMAGE_NAME]\nNote that input must be jpg or png\n", argv[0]);
        return 0;
    }
    else if (argc == 2)
    {
        input_name = argv[1];
        output_name = construct_output_name(input_name);
    }
    else
    {
        printf("wrong num of arguments: %d\n", argc);
        return 1;
    }

    if (!has_extension(input_name, ".jpg") && !has_extension(input_name, ".png"))
    {
        printf("Input pic should be in png or jpg\n");
        return 1;
    }
    printf("Используется потоков: %d\n", omp_get_num_threads());

    int width, height, channels;
    unsigned char *img = stbi_load(input_name, &width, &height, &channels, 0);
    if (!img)
    {
        fprintf(stderr, "Load error\n");
        return -1;
    }

    printf("Image info:\n"
           "\twidth: %d\n\theight: %d\n\tchannels: %d\n",
           width, height, channels);

    double start = omp_get_wtime();
    if (relief_parallel_tiling(img, width, height, channels) != 0)
    {
        fprintf(stderr, "Relief error\n");
        return -1;
    }
    double end = omp_get_wtime();
    double elapsed = (end - start)*1000.0;

    printf(" - Image reliefed %f ms\n", elapsed);

    int out_width, out_height;

    start = omp_get_wtime();
    unsigned char *out_img = resize_2lower_tiling(img, width, height, channels, &out_width, &out_height);
    if (!out_img)
    {
        fprintf(stderr, "Resize error\n");
        return -1;
    }
    end = omp_get_wtime();
    elapsed = (end - start)*1000.0;
    printf(" - Image resized %f ms\n", elapsed);

    printf("Save image: %s\n", output_name);
    stbi_write_png(output_name, out_width, out_height, channels, out_img, out_width * channels);

    stbi_image_free(img);
    free(out_img);
    return 0;
}