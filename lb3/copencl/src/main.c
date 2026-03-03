#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "io_image.h"
#include "types.h"

#include <CL/cl.h>


#define KERNEL_FILE "conv_resize.cl"

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

    image_t *input_img = load_image(input_name);
    if (!input_img)
    {
        fprintf(stderr, "Load error\n");
        return -1;
    }

    printf("Image info:\n"
           "\twidth: %d\n\theight: %d\n\tchannels: %d\n",
           input_img->width, input_img->height, input_img->channels);

    cl_platform_id platform; // OpenCL platform
    cl_device_id device;     // device ID
    cl_context context;      // context
    cl_command_queue queue;  // command queue
    cl_program program;      // program
    cl_kernel kernel;        // kernel
    cl_int err;

    err = clGetPlatformIDs(1, &platform, NULL);
    if (err != CL_SUCCESS){
        fprintf(stderr, "clGetPlatformIDs\n");
        free_image(input_img);
        return -1;
    }

    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &device, NULL);
    if (err != CL_SUCCESS){
        fprintf(stderr, "clGetDeviceIDs\n");
        free_image(input_img);
        return -1;
    }

    context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    if (err != CL_SUCCESS){
        fprintf(stderr, "clCreateContext\n");
        free_image(input_img);
        return -1;
    }

    queue = clCreateCommandQueue(context, device, 0, &err);
    if (err != CL_SUCCESS){
        fprintf(stderr, "clCreateCommandQueue\n");
        free_image(input_img);
        return -1;
    }

    size_t kernel_size;
    char *kernel_source = load_kernel_source(KERNEL_FILE, &kernel_size);
    if (!kernel_source)
    {
        free_image(input_img);
        return -1;
    }

    program = clCreateProgramWithSource(context, 1, (const char**)&kernel_source, &kernel_size, &err);
    free(kernel_source);
    if (err != CL_SUCCESS){
        fprintf(stderr, "clCreateProgramWithSource\n");
        free_image(input_img);
        return -1;
    }

    err = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
    if (err != CL_SUCCESS) {
        size_t log_size;
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
        char* log = malloc(log_size);
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
        fprintf(stderr, "Build error:\n%s\n", log);
        free(log);
        free_image(input_img);
        return -1;
    }

    kernel = clCreateKernel(program, "emboss_filter", &err);
    if (err != CL_SUCCESS){
        fprintf(stderr, "clCreateKernel\n");
        free_image(input_img);
        return -1;
    }

    size_t image_size = input_img->width * input_img->height * input_img->channels;
    cl_mem input_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                         image_size, input_img->data, &err);
    cl_mem output_buffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, image_size, NULL, &err);
    if (err != CL_SUCCESS){
        fprintf(stderr, "clCreateBuffer\n");
        free_image(input_img);
        return -1;
    }

    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &input_buffer);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &output_buffer);
    err |= clSetKernelArg(kernel, 2, sizeof(cl_int), &input_img->width);
    err |= clSetKernelArg(kernel, 3, sizeof(cl_int), &input_img->height);
    err |= clSetKernelArg(kernel, 4, sizeof(cl_int), &input_img->channels);
    if (err != CL_SUCCESS){
        fprintf(stderr, "clSetKernelArg\n");
        free_image(input_img);
        return -1;
    }

    size_t global_size[2] = { input_img->width, input_img->width };
    size_t local_size[2] = { 16, 16 };

    err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, global_size, local_size, 0, NULL, NULL);
    if (err != CL_SUCCESS){
        fprintf(stderr, "clEnqueueNDRangeKernel\n");
        free_image(input_img);
        return -1;
    }

    err = clFinish(queue);
    if (err != CL_SUCCESS){
        fprintf(stderr, "clFinish\n");
        free_image(input_img);
        return -1;
    }

    unsigned char* output_data = malloc(image_size);
    err = clEnqueueReadBuffer(queue, output_buffer, CL_TRUE, 0, image_size, output_data, 0, NULL, NULL);
    if (err != CL_SUCCESS){
        fprintf(stderr, "clEnqueueReadBuffer\n");
        free_image(input_img);
        return -1;
    }
    
    image_t* output_img;
    output_img->data = output_data;
    output_img->width = input_img->width;
    output_img->height = input_img->height;    
    output_img->channels = input_img->channels;    

    free_image(input_img);

    save_img(output_img, output_name);

    return 0;
}