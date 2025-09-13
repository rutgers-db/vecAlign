#!/bin/bash

# 检查参数数量
if [ $# -ne 4 ]; then
    echo "usage: $0 <radius> <threshold> <max_time_diff> <k>"
    echo "example: $0 0.6 0.7 3 5"
    exit 1
fi

# 获取参数
RADIUS=$1
THRESHOLD=$2
MAX_TIME_DIFF=$3
K=$4

# 验证参数类型
if ! [[ $RADIUS =~ ^[-+]?[0-9]*\.?[0-9]+$ ]]; then
    echo "Error: radius must be a float (like: 0.6)"
    exit 1
fi

if ! [[ $THRESHOLD =~ ^[-+]?[0-9]*\.?[0-9]+$ ]]; then
    echo "Error: threshold must be a float number (like: 0.7)"
    exit 1
fi

if ! [[ $MAX_TIME_DIFF =~ ^[0-9]+$ ]]; then
    echo "Error: max_time_diff must be an integer (like: 3)"
    exit 1
fi

if ! [[ $K =~ ^[0-9]+$ ]]; then
    echo "Error: k must be an integer (like: 5)"
    exit 1
fi

# 输出参数信息
echo "Parameters:"
echo "  radius = $RADIUS"
echo "  threshold = $THRESHOLD"
echo "  max_time_diff = $MAX_TIME_DIFF"
echo "  k = $K"
# 执行程序 (替换为您的实际程序命令)
# 例如: ./your_program -radius $RADIUS -threshold $THRESHOLD -max_time_diff $MAX_TIME_DIFF -k $K
./baseline -hnsw_file_name "./embeddings/img_emb/M_256_ef_200_hnsw.bin" -radius $RADIUS -threshold $THRESHOLD -max_time_diff $MAX_TIME_DIFF -k $K

# 检查程序是否成功执行
if [ $? -ne 0 ]; then
    echo "Error: program execution failed"
    exit 1
fi

# 检查输出文件是否存在
OUTPUT_FILE="./embeddings/ret/baseline_pred.txt"
if [ ! -f "$OUTPUT_FILE" ]; then
    echo "Error: output file $OUTPUT_FILE doesnt exist"
    exit 1
fi

# 构建新文件名
NEW_FILENAME="r_${RADIUS}_thres_${THRESHOLD}_mtd_${MAX_TIME_DIFF}_k_${K}_baseline_pred.txt"
NEW_FILE_PATH="./embeddings/ret/$NEW_FILENAME"
# 重命名文件
mv "$OUTPUT_FILE" "$NEW_FILE_PATH"
python /common/home/zw393/wzz/Spring2024/video_matching_evaluation.py --gt ./embeddings/ret/query2gt_info.txt --pred $NEW_FILE_PATH --comment "test"
echo "Done!"
exit 0