#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "io_image.h"
#include "types.h"

#include <CL/cl.h>

#define KERNEL_FILE "conv_resize.cl"

static double get_time_ms(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1000000.0;
}

int main(int argc, char *argv[])
{
    double t_total = 0, t_load = 0, t_init = 0, t_build = 0, t_filter = 0, t_resize = 0, t_read = 0, t_save = 0;
    t_total = get_time_ms();

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
        if (!has_extension(input_name, ".jpg") && !has_extension(input_name, ".png"))
        {
            printf("Input pic should be in png or jpg\n");
            return 1;
        }
        output_name = construct_output_name(input_name);
    }
    else
    {
        printf("wrong num of arguments: %d\n", argc);
        return 1;
    }

    t_load = get_time_ms();
    image_t *input_img = load_image(input_name);
    t_load = get_time_ms() - t_load;
    if (!input_img)
    {
        fprintf(stderr, "Load error\n");
        return -1;
    }

    printf("Image info:\n"
           "\twidth: %d\n\theight: %d\n\tchannels: %d\n",
           input_img->width, input_img->height, input_img->channels);

    t_init = get_time_ms();
    cl_context_t *ctx = create_cl_context();
    t_init = get_time_ms() - t_init;
    if (!ctx)
    {
        free_image(input_img);
        fprintf(stderr, "cl context creation error");
        return -1;
    }

    cl_int err;

    size_t kernel_size;
    char *kernel_source = load_kernel_source(KERNEL_FILE, &kernel_size);
    if (!kernel_source)
    {
        free_cl_context(ctx);
        free_image(input_img);
        return -1;
    }

    cl_program program = clCreateProgramWithSource(ctx->context, 1, (const char **)&kernel_source, &kernel_size, &err);
    free(kernel_source);
    if (err != CL_SUCCESS)
    {
        fprintf(stderr, "clCreateProgramWithSource\n");
        free_cl_context(ctx);
        free_image(input_img);
        return -1;
    }

    t_build = get_time_ms();
    err = clBuildProgram(program, 1, &ctx->device, NULL, NULL, NULL);
    t_build = get_time_ms() - t_build;
    if (err != CL_SUCCESS)
    {
        size_t log_size;
        clGetProgramBuildInfo(program, ctx->device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
        char *log = malloc(log_size);
        clGetProgramBuildInfo(program, ctx->device, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
        fprintf(stderr, "Build error:\n%s\n", log);
        free(log);
        free_image(input_img);
        return -1;
    }

    cl_kernel kernel = clCreateKernel(program, "relief_filter", &err);
    if (err != CL_SUCCESS)
    {
        free_cl_context(ctx);
        fprintf(stderr, "clCreateKernel\n");
        free_image(input_img);
        return -1;
    }

    size_t image_size = input_img->width * input_img->height * input_img->channels;
    cl_mem input_buffer = clCreateBuffer(ctx->context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                         image_size, input_img->data, &err);
    cl_mem output_buffer = clCreateBuffer(ctx->context, CL_MEM_WRITE_ONLY, image_size, NULL, &err);
    if (err != CL_SUCCESS)
    {
        free_cl_context(ctx);
        fprintf(stderr, "clCreateBuffer\n");
        free_image(input_img);
        return -1;
    }

    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &input_buffer);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &output_buffer);
    err |= clSetKernelArg(kernel, 2, sizeof(cl_int), &input_img->width);
    err |= clSetKernelArg(kernel, 3, sizeof(cl_int), &input_img->height);
    err |= clSetKernelArg(kernel, 4, sizeof(cl_int), &input_img->channels);
    if (err != CL_SUCCESS)
    {
        free_cl_context(ctx);
        fprintf(stderr, "clSetKernelArg\n");
        free_image(input_img);
        return -1;
    }

    size_t local_size[2] = {16, 16};
    size_t global_size[2] = {
        ((input_img->width + local_size[0] - 1) / local_size[0]) * local_size[0],
        ((input_img->height + local_size[1] - 1) / local_size[1]) * local_size[1]};

    t_filter = get_time_ms();
    err = clEnqueueNDRangeKernel(ctx->queue, kernel, 2, NULL, global_size, local_size, 0, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        free_cl_context(ctx);
        fprintf(stderr, "clEnqueueNDRangeKernel\n");
        free_image(input_img);
        return -1;
    }

    err = clFinish(ctx->queue);
    if (err != CL_SUCCESS)
    {
        free_cl_context(ctx);
        fprintf(stderr, "clFinish\n");
        free_image(input_img);
        return -1;
    }
    t_filter = get_time_ms() - t_filter;

    cl_kernel resize_kernel = clCreateKernel(program, "resize_twice_low", &err);
    if (err != CL_SUCCESS)
    {
        free_cl_context(ctx);
        fprintf(stderr, "clCreateKernel resize\n");
        free_image(input_img);
        return -1;
    }

    int out_width = input_img->width / 2;
    int out_height = input_img->height / 2;
    size_t image_size_out = out_width * out_height * input_img->channels;

    cl_mem resized_buffer = clCreateBuffer(ctx->context, CL_MEM_WRITE_ONLY, image_size_out, NULL, &err);
    if (err != CL_SUCCESS)
    {
        free_cl_context(ctx);
        fprintf(stderr, "clCreateBuffer resize\n");
        free_image(input_img);
        return -1;
    }

    err = clSetKernelArg(resize_kernel, 0, sizeof(cl_mem), &output_buffer);
    err |= clSetKernelArg(resize_kernel, 1, sizeof(cl_mem), &resized_buffer);
    err |= clSetKernelArg(resize_kernel, 2, sizeof(cl_int), &input_img->width);
    err |= clSetKernelArg(resize_kernel, 3, sizeof(cl_int), &input_img->height);
    err |= clSetKernelArg(resize_kernel, 4, sizeof(cl_int), &input_img->channels);
    if (err != CL_SUCCESS)
    {
        free_cl_context(ctx);
        fprintf(stderr, "clSetKernelArg resize\n");
        free_image(input_img);
        return -1;
    }

    size_t local_size_resize[2] = {16, 16};
    size_t global_size_resize[2] = {
        ((out_width + local_size_resize[0] - 1) / local_size_resize[0]) * local_size_resize[0],
        ((out_height + local_size_resize[1] - 1) / local_size_resize[1]) * local_size_resize[1]};

    t_resize = get_time_ms();
    err = clEnqueueNDRangeKernel(ctx->queue, resize_kernel, 2, NULL, global_size_resize, local_size_resize, 0, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        free_cl_context(ctx);
        fprintf(stderr, "clEnqueueNDRangeKernel resize\n");
        free_image(input_img);
        return -1;
    }

    err = clFinish(ctx->queue);
    if (err != CL_SUCCESS)
    {
        free_cl_context(ctx);
        fprintf(stderr, "clFinish resize\n");
        free_image(input_img);
        return -1;
    }
    t_resize = get_time_ms() - t_resize;

    t_read = get_time_ms();
    unsigned char *output_data = malloc(image_size_out);
    err = clEnqueueReadBuffer(ctx->queue, resized_buffer, CL_TRUE, 0, image_size_out, output_data, 0, NULL, NULL);
    t_read = get_time_ms() - t_read;
    if (err != CL_SUCCESS)
    {
        free_cl_context(ctx);
        fprintf(stderr, "clEnqueueReadBuffer\n");
        free(output_data);
        free_image(input_img);
        return -1;
    }

    image_t *output_img = malloc(sizeof(image_t));
    output_img->width = out_width;
    output_img->height = out_height;
    output_img->channels = input_img->channels;
    output_img->data = output_data;

    t_save = get_time_ms();
    save_img(output_img, output_name);
    t_save = get_time_ms() - t_save;
    free_image(output_img);
    free_image(input_img);

    clReleaseKernel(kernel);
    clReleaseKernel(resize_kernel);
    clReleaseProgram(program);
    clReleaseMemObject(input_buffer);
    clReleaseMemObject(output_buffer);
    clReleaseMemObject(resized_buffer);
    free_cl_context(ctx);
    free(output_name);

    t_total = get_time_ms() - t_total;
    printf("Timings:\n");
    printf("\timage load:      %8.2f ms\n", t_load);
    printf("\tCL init:         %8.2f ms\n", t_init);
    printf("\tkernel build:    %8.2f ms\n", t_build);
    printf("\trelief filter:   %8.2f ms\n", t_filter);
    printf("\tresize:          %8.2f ms\n", t_resize);
    printf("\tread GPU buffer: %8.2f ms\n", t_read);
    printf("\tsave image:      %8.2f ms\n", t_save);
    printf("\t------------------------\n");
    printf("\ttotal:           %8.2f ms\n", t_total);
    return 0;
}