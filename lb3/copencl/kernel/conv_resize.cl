__kernel void relief_filter(__global const uchar *input, __global uchar *output,
                            const int width, const int height,
                            const int channels) {
  int x = get_global_id(0);
  int y = get_global_id(1);

  if (x < 1 || x >= (width - 1) || y < 1 || y >= (height - 1))
    return;

  const int convkernel[9] = {-2, -1, 0, -1, 1, 1, 0, 1, 2};

  int base_idx = (y * width + x) * channels;
  float result[3] = {0, 0, 0};

  for (int c = 0; c < channels; c++) {

    float sum =
        input[((y - 1) * width + (x - 1)) * channels + c] * convkernel[0];
    sum += input[((y - 1) * width + (x)) * channels + c] * convkernel[1];
    sum += input[((y - 1) * width + (x + 1)) * channels + c] * convkernel[2];
    sum += input[((y)*width + (x - 1)) * channels + c] * convkernel[3];
    sum += input[(base_idx + c] * convkernel[4];
    sum += input[((y)*width + (x + 1)) * channels + c] * convkernel[5];
    sum += input[((y + 1) * width + (x - 1)) * channels + c] * convkernel[6];
    sum += input[((y + 1) * width + (x)) * channels + c] * convkernel[7];
    sum += input[((y + 1) * width + (x + 1)) * channels + c] * convkernel[8];

    result[c] = fmax(0.0f, fmin(255.0f, sum));
  }

  for (int c = 0; c < channels; c++) {
    output[base_idx + c] = (uchar)result[c];
  }
}

__kernel void resize_twice_low(__global const uchar *input,
                               __global uchar output, const int width,
                               const int height, const int channels) {
  int x = get_global_id(0);
  int y = get_global_id(1);
  int base_idx = (y * width + x) * channels;
  for (int c = 0; c < channels; c++) {
    int sum = input[(y * width + x) * channels + c];
    sum += input[(y * width + (x+1)) * channels + c];
    sum += input[((y+1) * width + (x)) * channels + c];
    sum += input[((y+1) * width + (x+1)) * channels + c];
    output[base_idx] = sum / 4;

  }
}