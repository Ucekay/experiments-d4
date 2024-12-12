# エッジ検出プログラムの使用方法

## Makefileでの操作方法

### 基本的なコマンド

- `make` または `make all`: プログラムをビルドします（`make`は`make all`の省略形です）
- `make clean`: ビルド成果物と出力ファイルを削除します
- `make run`: デフォルトフィルタ（Forsenフィルタ）でエッジ検出を実行します

### フィルタの種類と実行方法

以下のフィルタが利用可能です：

- `make prewitt`: Prewittフィルタでエッジ検出を実行
- `make sobel`: Sobelフィルタでエッジ検出を実行
- `make laplacian`: Laplacianフィルタでエッジ検出を実行
- `make forsen`: Forsenフィルタでエッジ検出を実行

### 出力結果の確認

- `make show`: 処理結果の画像を表示します
  - フィルタリング結果: `filtering_out/*.pgm`
  - 閾値処理結果: `thresholding_out/*.pgm`

### 使用例

```bash
# ビルドと実行
make

# 特定のフィルタでの実行
make sobel
make show  # 結果の確認

# クリーンアップ
make clean
```

出力ファイルは以下のディレクトリに保存されます：
- フィルタリング結果: `./filtering_out/`
- 閾値処理結果: `./thresholding_out/`
- 閾値処理ログ: `threshold_log.txt`
