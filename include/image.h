#ifndef IMAGE_H
#define IMAGE_H

#include <stdio.h>
#include <stdlib.h>

/* マクロ定義 */
#define min(A, B) ((A) < (B) ? (A) : (B))
#define max(A, B) ((A) > (B) ? (A) : (B))

/* 画像構造体の定義 */
typedef struct {
  int width;           /* 画像の横方向の画素数 */
  int height;          /* 画像の縦方向の画素数 */
  int max_value;       /* 画素の値(明るさ)の最大値 */
  unsigned char *data; /* 画像の画素値データを格納する領域を指すポインタ */
} image_t;

/* 関数プロトタイプ宣言 */
void parse_arg(int argc, char **argv, FILE **infp, FILE **outfp);
void init_image(image_t *pt_image, int width, int height, int max_value);
char *read_one_line(char *buf, int n, FILE *fp);
void read_pgm_raw_header(FILE *fp, image_t *pt_image);
void read_pgm_paw_bitmap_data(FILE *fp, image_t *pt_image);
void filtering_image(image_t *result_image, image_t *original_image);
void apply_prewitt_filter(image_t *result_image, image_t *original_image);
void apply_soebel_filter(image_t *result_image, image_t *original_image);
void apply_laplacian_filter(image_t *result_image, image_t *original_image);
void apply_8_laplacian_filter(image_t *result_image, image_t *original_image);
void apply_forsen_filter(image_t *result_image, image_t *original_image);
void write_pgm_raw_header(FILE *fp, image_t *pt_image);
void write_pgm_raw_bitmap_data(FILE *fp, image_t *pt_image);
void free_image(image_t *pt_image);
int calculate_otsu_threshold(const image_t *result_image,
                             const image_t *original_image);
void apply_thresholding(image_t *result_image, image_t *original_image,
                        int threshold);
void close_files(FILE *infp, FILE *outfp);

typedef enum {
  FILTER_NEGATIVE,
  FILTER_PREWITT,
  FILTER_SOBEL,
  FILTER_LAPLACIAN,
  FILTER_FORSEN,
  FILTER_OTSU
} filter_type_t;

#endif