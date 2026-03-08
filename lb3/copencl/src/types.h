#ifndef TYPES_H
#define TYPES_H

#include <CL/cl.h>

typedef struct
{
    int width;
    int height;
    int channels;
    unsigned char* data;
} image_t;

typedef struct 
{
    cl_platform_id platform; // OpenCL platform
    cl_device_id device;     // device ID
    cl_context context;      // context
    cl_command_queue queue;  // command queue
} cl_context_t;




#endif // TYPES_H