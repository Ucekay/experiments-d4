# コンパイラの設定
CC = gcc
CFLAGS = -Wall -O2 -I./include

# ディレクトリ設定
SRC_DIR = src
DIST_DIR = dist

# ソースファイルとオブジェクトファイルの設定
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(DIST_DIR)/%.o)
TARGET = $(DIST_DIR)/image_processor

# エッジ検出フィルタのデフォルト設定
FILTER ?= forsen

# デフォルトターゲット
all: $(DIST_DIR) $(TARGET)

# distディレクトリの作成
$(DIST_DIR):
	mkdir -p $(DIST_DIR)

# オブジェクトファイルの生成規則
$(DIST_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# 実行ファイルの生成規則
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

# 各フィルタ用のターゲット
prewitt: $(TARGET)
	./$(TARGET) prewitt

sobel: $(TARGET)
	./$(TARGET) sobel

laplacian: $(TARGET)
	./$(TARGET) laplacian

8_laplacian: $(TARGET)
	./$(TARGET) 8_laplacian

forsen: $(TARGET)
	./$(TARGET) forsen

# 実行（デフォルトまたは指定されたフィルタを使用）
run: $(TARGET)
	./$(TARGET) $(FILTER)

# 処理結果の表示
show:
	@echo "Opening filtered images..."
	@open ./filtering_out/*.pgm 2>/dev/null || true
	@echo "Opening thresholded images..."
	@open ./thresholding_out/*.pgm 2>/dev/null || true

# クリーン
clean:
	rm -rf $(DIST_DIR)
	rm -rf ./filtering_out
	rm -rf ./thresholding_out
	rm -f threshold_log.txt

.PHONY: all clean run prewitt sobel laplacian forsen show
