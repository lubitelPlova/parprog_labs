#pragma once

void gpuResize(const unsigned char* src, unsigned char* dst, 
               int srcW, int srcH, int srcStep,
               int channels);

void gpuConvertBgrToGray(const unsigned char* input, unsigned char* output, 
                         int width, int height, int step);

void gpuFuseGrayscale(const unsigned char* img1, int w1, int h1, int step1,
                       const unsigned char* img2, int w2, int h2, int step2,
                       const unsigned char* img3, int w3, int h3, int step3,
                       unsigned char* result);
