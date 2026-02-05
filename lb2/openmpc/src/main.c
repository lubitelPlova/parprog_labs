#include <stdio.h>
#include <stdlib.h>

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
        printf("Usage: %s [INPUT_IMAGE_NAME] [OUTPUT_IMAGE_NAME]\nNote that input and output must be jpg or png\n", argv[0]);
        return 0;
    }
    else if (argc == 3)
    {
        input_name = argv[1];
        output_name = argv[2];
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
    if (!has_extension(output_name, ".png"))
    {
        printf("Output pic should be in png\n");
        return 1;
    }

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

    if (relief_parallel_by_row(img, width, height, channels) != 0)
    {
        fprintf(stderr, "Relief error\n");
        return -1;
    }
    int out_width, out_height;
    unsigned char *out_img = resize_2lower(img, width, height, channels, &out_width, &out_height);
    if (!out_img)
    {
        fprintf(stderr, "Resize error\n");
        return -1;
    }

    stbi_write_png(output_name, out_width, out_height, channels, out_img, out_width * channels);
    // stbi_write_png(output_name, width, height, channels, img, width * channels);


    stbi_image_free(img);
    free(out_img);
    return 0;
}