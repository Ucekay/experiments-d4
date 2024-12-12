#include "../include/image.h"

void parse_arg(int argc, char **argv, FILE **infp, FILE **outfp) {
  /* 引数の個数をチェック */
  if (argc != 3) {
    goto usage;
  }

  *infp = fopen(argv[1], "rb");
  if (*infp == NULL) {
    fputs("Opening the input file was failend\n", stderr);
    goto usage;
  }

  *outfp = fopen(argv[2], "wb");
  if (*outfp == NULL) {
    fputs("Opening the output file was failend\n", stderr);
    goto usage;
  }

  return;

usage:
  fprintf(stderr, "usage : %s <input pgm file> <output pgm file>\n", argv[0]);
  exit(1);
}

char *read_one_line(char *buf, int n, FILE *fp) {
  char *fgets_result;

  do {
    fgets_result = fgets(buf, n, fp);
  } while (fgets_result != NULL && buf[0] == '#');

  return fgets_result;
}

void read_pgm_raw_header(FILE *fp, image_t *pt_image) {
  int width, height, max_value;
  char buf[128];

  /* マジックナンバー(P5) の確認 */
  if (read_one_line(buf, 128, fp) == NULL) {
    goto error;
  }
  if (buf[0] != 'P' || buf[1] != '5') {
    goto error;
  }

  /* 画像サイズの読み込み */
  if (read_one_line(buf, 128, fp) == NULL) {
    goto error;
  }
  if (sscanf(buf, "%d %d", &width, &height) != 2) {
    goto error;
  }
  if (width <= 0 || height <= 0) {
    goto error;
  }

  /* 最大画素値の読み込み */
  if (read_one_line(buf, 128, fp) == NULL) {
    goto error;
  }
  if (sscanf(buf, "%d", &max_value) != 1) {
    goto error;
  }
  if (max_value <= 0 || max_value >= 256) {
    goto error;
  }

  /* 画像構造体の初期化 */
  init_image(pt_image, width, height, max_value);
  return;

error:
  fputs("Reading PGM-RAW header was failed\n", stderr);
  exit(1);
}

void read_pgm_paw_bitmap_data(FILE *fp, image_t *pt_image) {
  if (fread(pt_image->data, sizeof(unsigned char),
            pt_image->width * pt_image->height,
            fp) != (size_t)(pt_image->width * pt_image->height)) {
    fputs("Reading PGM-RAW bitmap data was failed\n", stderr);
    exit(1);
  }
}

void write_pgm_raw_header(FILE *fp, image_t *pt_image) {
  /* マジックナンバー(P5) の書き込み */
  if (fputs("P5\n", fp) == EOF) {
    goto error;
  }

  /* 画像サイズの書き込み */
  if (fprintf(fp, "%d %d\n", pt_image->width, pt_image->height) == EOF) {
    goto error;
  }

  /* 画素値の最大値を書き込む */
  if (fprintf(fp, "%d\n", pt_image->max_value) == EOF) {
    goto error;
  }

  return;

error:
  fputs("Writing PGM-RAW header was failed\n", stderr);
  exit(1);
}

void write_pgm_raw_bitmap_data(FILE *fp, image_t *pt_image) {
  if (fwrite(pt_image->data, sizeof(unsigned char),
             pt_image->width * pt_image->height,
             fp) != (size_t)(pt_image->width * pt_image->height)) {
    fputs("Writing PGM-RAW bitmap data was failed\n", stderr);
    exit(1);
  }
}

void close_files(FILE *infp, FILE *outfp) {
  if (infp != NULL) {
    fclose(infp);
  }
  if (outfp != NULL) {
    fclose(outfp);
  }
}
