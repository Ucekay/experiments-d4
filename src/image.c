#include "../include/image.h"
#include <math.h>
#include <string.h>

void init_image(image_t *pt_image, int width, int height, int max_value) {
  pt_image->width = width;
  pt_image->height = height;
  pt_image->max_value = max_value;

  /* メモリ領域の確保 */
  pt_image->data = (unsigned char *)malloc((size_t)(width * height));

  if (pt_image->data == NULL) /* メモリ確保ができなかった時はエラー */
  {
    fputs("out of memory\n", stderr);
    exit(1);
  }
}

void filtering_image(image_t *result_image, image_t *original_image) {
  int x, y;
  int width, height;

  /* originalImage と resultImage のサイズが違う場合は、共通部分のみ */
  /* を処理する。*/
  width = min(original_image->width, result_image->width);
  height = min(original_image->height, result_image->height);

  for (y = 0; y < height; y++) {
    for (x = 0; x < width; x++) {
      result_image->data[x + result_image->width * y] =
          (original_image->max_value -
           original_image->data[x + original_image->width * y]) *
          result_image->max_value / original_image->max_value;
    }
  }
}

void get_neighborhood(const image_t *padded_image, int x, int y, int width,
                      int neighborhood[3][3]) {
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      neighborhood[i][j] =
          padded_image->data[(x + j - 1) + (y + i - 1) * (width + 2)];
    }
  }
}

int matrix_dot_product(const int matrix1[3][3], const int matrix2[3][3]) {
  int sum = 0;
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      sum += matrix1[i][j] * matrix2[i][j];
    }
  }
  return sum;
}

int apply_filter(int neighborhood[3][3], const int filter_x[3][3],
                 const int filter_y[3][3]) {
  int derivative_x = matrix_dot_product(neighborhood, filter_x);
  int derivative_y = matrix_dot_product(neighborhood, filter_y);
  return (int)sqrt(derivative_x * derivative_x + derivative_y * derivative_y);
}

void apply_prewitt_filter(image_t *result_image, image_t *original_image) {
  int x, y;
  int width, height;
  int filter_x[3][3] = {{-1, 0, 1}, {-1, 0, 1}, {-1, 0, 1}};
  int filter_y[3][3] = {{-1, -1, -1}, {0, 0, 0}, {1, 1, 1}};
  float scale_factor;

  width = min(original_image->width, result_image->width);
  height = min(original_image->height, result_image->height);

  image_t padded_image;
  init_image(&padded_image, width + 2, height + 2, original_image->max_value);
  memset(padded_image.data, 0, (width + 2) * (height + 2));

  for (y = 0; y < height; y++) {
    for (x = 0; x < width; x++) {
      padded_image.data[(x + 1) + (y + 1) * (width + 2)] =
          original_image->data[x + y * width];
    }
  }

  // Store raw magnitudes in result_image and find maximum
  int max_magnitude = 0;
  int *temp_data = (int *)malloc(width * height * sizeof(int));

  for (y = 1; y <= height; y++) {
    for (x = 1; x <= width; x++) {
      int neighborhood[3][3];
      get_neighborhood(&padded_image, x, y, width, neighborhood);
      int magnitude = apply_filter(neighborhood, filter_x, filter_y);
      temp_data[(x - 1) + (y - 1) * width] = magnitude;
      if (magnitude > max_magnitude) {
        max_magnitude = magnitude;
      }
    }
  }

  // Scale and convert to unsigned char in one pass
  scale_factor =
      max_magnitude > 0 ? (float)result_image->max_value / max_magnitude : 1.0f;
  for (y = 0; y < height; y++) {
    for (x = 0; x < width; x++) {
      int scaled_magnitude = (int)(temp_data[x + y * width] * scale_factor);
      result_image->data[x + y * width] =
          (unsigned char)max(0, min(result_image->max_value, scaled_magnitude));
    }
  }

  free(temp_data);

  free_image(&padded_image);
}

void apply_soebel_filter(image_t *result_image, image_t *original_image) {
  int x, y;
  int width, height;
  const int filter_x[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
  const int filter_y[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};
  float scale_factor;

  width = min(original_image->width, result_image->width);
  height = min(original_image->height, result_image->height);

  image_t padded_image;
  init_image(&padded_image, width + 2, height + 2, original_image->max_value);
  memset(padded_image.data, 0, (width + 2) * (height + 2));

  for (y = 0; y < height; y++) {
    for (x = 0; x < width; x++) {
      padded_image.data[(x + 1) + (y + 1) * (width + 2)] =
          original_image->data[x + y * width];
    }
  }

  int max_magnitude = 0;
  int *temp_data = (int *)malloc(width * height * sizeof(int));

  for (y = 1; y <= height; y++) {
    for (x = 1; x <= width; x++) {
      int neighborhood[3][3];
      get_neighborhood(&padded_image, x, y, width, neighborhood);
      int magnitude = apply_filter(neighborhood, filter_x, filter_y);
      temp_data[(x - 1) + (y - 1) * width] = magnitude;
      if (magnitude > max_magnitude) {
        max_magnitude = magnitude;
      }
    }
  }

  scale_factor =
      max_magnitude > 0 ? (float)result_image->max_value / max_magnitude : 1.0f;

  for (y = 0; y < height; y++) {
    for (x = 0; x < width; x++) {
      int scaled_magnitude = (int)(temp_data[x + y * width] * scale_factor);
      result_image->data[x + y * width] =
          (unsigned char)max(0, min(result_image->max_value, scaled_magnitude));
    }
  }

  free(temp_data);

  free_image(&padded_image);
}

void apply_laplacian_filter(image_t *result_image, image_t *original_image) {
  int x, y;
  int width, height;
  const int filter[3][3] = {{0, 1, 0}, {1, -4, 1}, {0, 1, 0}};
  float scale_factor;

  width = min(original_image->width, result_image->width);
  height = min(original_image->height, result_image->height);

  image_t padded_image;
  init_image(&padded_image, width + 2, height + 2, original_image->max_value);
  memset(padded_image.data, 0, (width + 2) * (height + 2));

  for (y = 0; y < height; y++) {
    for (x = 0; x < width; x++) {
      padded_image.data[(x + 1) + (y + 1) * (width + 2)] =
          original_image->data[x + y * width];
    }
  }

  int max_magnitude = 0;
  int *temp_data = (int *)malloc(width * height * sizeof(int));

  for (y = 1; y <= height; y++) {
    for (x = 1; x <= width; x++) {
      int neighborhood[3][3];
      get_neighborhood(&padded_image, x, y, width, neighborhood);
      int magnitude = apply_filter(neighborhood, filter, filter);
      temp_data[(x - 1) + (y - 1) * width] = magnitude;
      if (magnitude > max_magnitude) {
        max_magnitude = magnitude;
      }
    }
  }

  scale_factor =
      max_magnitude > 0 ? (float)result_image->max_value / max_magnitude : 1.0f;

  for (y = 0; y < height; y++) {
    for (x = 0; x < width; x++) {
      int scaled_magnitude = (int)(temp_data[x + y * width] * scale_factor);
      result_image->data[x + y * width] =
          (unsigned char)max(0, min(result_image->max_value, scaled_magnitude));
    }
  }

  free(temp_data);

  free_image(&padded_image);
}

void apply_forsen_filter(image_t *result_image, image_t *original_image) {
  int x, y;
  int width, height;

  width = min(original_image->width, result_image->width);
  height = min(original_image->height, result_image->height);

  image_t padded_image;
  init_image(&padded_image, width + 2, height + 2, original_image->max_value);
  memset(padded_image.data, 0, (width + 2) * (height + 2));

  for (y = 0; y < height; y++) {
    for (x = 0; x < width; x++) {
      padded_image.data[(x + 1) + (y + 1) * (width + 2)] =
          original_image->data[x + y * width];
    }
  }

  int max_magnitude = 0;
  int *temp_data = (int *)malloc(original_image->width *
                                 original_image->height * sizeof(int));

  for (y = 1; y <= height; y++) {
    for (x = 1; x <= width; x++) {
      int neighborhood[3][3];
      get_neighborhood(&padded_image, x, y, width, neighborhood);
      int magnitude = abs(neighborhood[1][1] - neighborhood[2][2]) +
                      abs(neighborhood[1][2] - neighborhood[2][1]);
      temp_data[(x - 1) + (y - 1) * width] = magnitude;
      if (magnitude > max_magnitude) {
        max_magnitude = magnitude;
      }
    }
  }

  float scale_factor = max_magnitude > 0 ? 256.0f / max_magnitude : 1.0f;

  for (y = 0; y < height; y++) {
    for (x = 0; x < width; x++) {
      int scaled_magnitude = (int)(temp_data[x + y * width] * scale_factor);
      result_image->data[x + y * width] =
          (unsigned char)max(0, min(result_image->max_value, scaled_magnitude));
    }
  }

  free(temp_data);

  free_image(&padded_image);
}

int calculate_otsu_threshold(const image_t *result_image,
                             const image_t *original_image) {
  int x, y;
  int width, height;
  int histogram[256] = {0};
  int total = 0;  // Total number of pixels
  float sum_B = 0;
  float varMax = 0;
  int threshold = 0;

  float omega_0[256] = {0};       // Sum of p_i, i < k
  float weighted_sum[256] = {0};  // Sum of i * p_i, i < k

  width = min(original_image->width, result_image->width);
  height = min(original_image->height, result_image->height);

  for (y = 0; y < height; y++) {
    for (x = 0; x < width; x++) {
      histogram[original_image->data[x + y * width]]++;
      total++;
    }
  }

  for (int i = 0; i < 256; i++) {
    float p_i = (float)histogram[i] / total;
    if (i == 0) {
      omega_0[i] = p_i;
      weighted_sum[i] = 0;
    } else {
      omega_0[i] = omega_0[i - 1] + p_i;
      weighted_sum[i] = weighted_sum[i - 1] + i * p_i;
    }
  }

  float mu_T = weighted_sum[256 - 1];

  for (int i = 0; i < 256; i++) {
    float omega_1 = omega_0[256 - 1] - omega_0[i];
    float mu_0 = (omega_0[i] != 0) ? weighted_sum[i] / omega_0[i]
                                   : weighted_sum[i] / 0.0001;
    float mu_1 = (omega_1 != 0) ? (mu_T - weighted_sum[i]) / omega_1
                                : (mu_T - weighted_sum[i]) / 0.0001;
    sum_B = omega_0[i] * (mu_0 - mu_T) * (mu_0 - mu_T) +
            omega_1 * (mu_1 - mu_T) * (mu_1 - mu_T);
    if (sum_B > varMax) {
      varMax = sum_B;
      threshold = i;
    }
  }
  return threshold;
}

void apply_thresholding(image_t *result_image, image_t *original_image,
                        int threshold) {
  int x, y;
  int width, height;

  width = min(original_image->width, result_image->width);
  height = min(original_image->height, result_image->height);

  for (y = 0; y < height; y++) {
    for (x = 0; x < width; x++) {
      result_image->data[x + y * width] =
          (original_image->data[x + y * width] > threshold)
              ? result_image->max_value
              : 0;
    }
  }
}

void free_image(image_t *pt_image) {
  if (pt_image->data != NULL) {
    free(pt_image->data);
    pt_image->data = NULL;
  }
}
