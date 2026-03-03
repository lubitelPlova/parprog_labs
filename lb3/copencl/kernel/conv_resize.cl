__kernel void emboss_filter(__global const uchar* input,
                            __global uchar* output,
                            const int width,
                            const int height,
                            const int channels)
{
    int x = get_global_id(0);
    int y = get_global_id(1);

    if (x >= width || y >= height) return;

    const int kernel[9] = {
        -2, -1,  0,
        -1,  1,  1,
         0,  1,  2
    };

    int base_idx = (y * width + x) * channels;
    float result[4] = {0, 0, 0, 0};

    for (int c = 0; c < channels; c++) {
        float sum = 0.0f;
        for (int ky = -1; ky <= 1; ky++) {
            for (int kx = -1; kx <= 1; kx++) {
                int nx = x + kx;
                int ny = y + ky;
                if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                    int idx = (ny * width + nx) * channels + c;
                    int k_idx = (ky + 1) * 3 + (kx + 1);
                    sum += input[idx] * kernel[k_idx];
                }
            }
        }
        sum += 128.0f; // Смещение для яркости
        result[c] = fmax(0.0f, fmin(255.0f, sum));
    }

    for (int c = 0; c < channels; c++) {
        output[base_idx + c] = (uchar)result[c];
    }
}