#include "../include/image.h"
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

#define PATH_MAX_LENGTH 1024

void create_directory(const char *path) {
  struct stat st;
  memset(&st, 0, sizeof(struct stat));
  if (stat(path, &st) == -1) {
    mkdir(path, 0700);
  }
}

const char *get_file_extension(const char *filename) {
  const char *dot = strrchr(filename, '.');
  if (!dot || dot == filename) return "";
  return dot + 1;
}

void print_usage(const char *program_name) {
  fprintf(stderr, "Usage: %s <filter_type>\n", program_name);
  fprintf(stderr, "Filter types:\n");
  fprintf(stderr, "  prewitt    - Prewitt edge detection\n");
  fprintf(stderr, "  sobel      - Sobel edge detection\n");
  fprintf(stderr, "  laplacian  - Laplacian edge detection\n");
  fprintf(stderr, "  forsen     - Forsen edge detection\n");
  exit(1);
}

void apply_edge_filter(image_t *result_image, image_t *original_image,
                       const char *filter_type) {
  if (strcmp(filter_type, "prewitt") == 0) {
    apply_prewitt_filter(result_image, original_image);
  } else if (strcmp(filter_type, "sobel") == 0) {
    apply_soebel_filter(result_image, original_image);
  } else if (strcmp(filter_type, "laplacian") == 0) {
    apply_laplacian_filter(result_image, original_image);
  } else if (strcmp(filter_type, "forsen") == 0) {
    apply_forsen_filter(result_image, original_image);
  }
}

int main(int argc, char **argv) {
  DIR *dir;
  struct dirent *ent;
  char input_path[PATH_MAX_LENGTH];
  char filtering_path[PATH_MAX_LENGTH];
  char thresholding_path[PATH_MAX_LENGTH];
  FILE *log_fp;

  if (argc != 2) {
    print_usage(argv[0]);
  }

  // フィルタータイプの検証
  const char *filter_type = argv[1];
  if (strcmp(filter_type, "prewitt") != 0 &&
      strcmp(filter_type, "sobel") != 0 &&
      strcmp(filter_type, "laplacian") != 0 &&
      strcmp(filter_type, "forsen") != 0) {
    fprintf(stderr, "Invalid filter type: %s\n", filter_type);
    print_usage(argv[0]);
  }

  create_directory("./filtering_out");
  create_directory("./thresholding_out");

  log_fp = fopen("threshold_log.txt", "w");
  if (log_fp == NULL) {
    fprintf(stderr, "Failed to create log file\n");
    return 1;
  }

  fprintf(log_fp, "Threshold Log\n");
  fprintf(log_fp, "=============\n\n");

  dir = opendir("./assets");
  if (dir == NULL) {
    fprintf(stderr, "Failed to open ./assets directory\n");
    fclose(log_fp);
    return 1;
  }

  while ((ent = readdir(dir)) != NULL) {
    if (strcmp(get_file_extension(ent->d_name), "pgm") != 0) {
      continue;
    }

    if (strlen(ent->d_name) + strlen("./assets/") >= PATH_MAX_LENGTH ||
        strlen(ent->d_name) + strlen("./filtering_out/") >= PATH_MAX_LENGTH ||
        strlen(ent->d_name) + strlen("./thresholding_out/") >=
            PATH_MAX_LENGTH) {
      fprintf(stderr, "File name too long: %s\n", ent->d_name);
      continue;
    }

    image_t original_image, result_image;
    FILE *infp, *outfp_filtering, *outfp_thresholding;

    if (snprintf(input_path, PATH_MAX_LENGTH, "./assets/%s", ent->d_name) >=
        PATH_MAX_LENGTH) {
      fprintf(stderr, "Input path too long\n");
      continue;
    }

    if (snprintf(filtering_path, PATH_MAX_LENGTH, "./filtering_out/%s",
                 ent->d_name) >= PATH_MAX_LENGTH) {
      fprintf(stderr, "Filtering path too long\n");
      continue;
    }

    if (snprintf(thresholding_path, PATH_MAX_LENGTH, "./thresholding_out/%s",
                 ent->d_name) >= PATH_MAX_LENGTH) {
      fprintf(stderr, "Thresholding path too long\n");
      continue;
    }

    infp = fopen(input_path, "rb");
    if (infp == NULL) {
      fprintf(stderr, "Failed to open input file: %s\n", input_path);
      continue;
    }

    read_pgm_raw_header(infp, &original_image);
    read_pgm_paw_bitmap_data(infp, &original_image);

    init_image(&result_image, original_image.width, original_image.height,
               original_image.max_value);

    // 指定されたエッジ検出フィルタを適用
    apply_edge_filter(&result_image, &original_image, filter_type);

    outfp_filtering = fopen(filtering_path, "wb");
    if (outfp_filtering != NULL) {
      write_pgm_raw_header(outfp_filtering, &result_image);
      write_pgm_raw_bitmap_data(outfp_filtering, &result_image);
      fclose(outfp_filtering);
    }

    image_t threshold_image;
    init_image(&threshold_image, result_image.width, result_image.height,
               result_image.max_value);

    int threshold = calculate_otsu_threshold(&threshold_image, &result_image);
    fprintf(log_fp, "Image: %s\nThreshold: %d\n\n", ent->d_name, threshold);

    apply_thresholding(&threshold_image, &result_image, threshold);

    outfp_thresholding = fopen(thresholding_path, "wb");
    if (outfp_thresholding != NULL) {
      write_pgm_raw_header(outfp_thresholding, &threshold_image);
      write_pgm_raw_bitmap_data(outfp_thresholding, &threshold_image);
      fclose(outfp_thresholding);
    }

    free_image(&original_image);
    free_image(&result_image);
    free_image(&threshold_image);
    fclose(infp);
  }

  fclose(log_fp);
  closedir(dir);
  return 0;
}
