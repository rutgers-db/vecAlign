#!/bin/bash

# 检查参数数量
if [ $# -ne 3 ]; then
    echo "usage: $0 <radius> <threshold> <frame_dis>"
    echo "example: $0 0.6 0.7 0.35"
    exit 1
fi

# 获取参数
RADIUS=$1
THRESHOLD=$2
FRAME_DIS=$3
if ! [[ $RADIUS =~ ^[-+]?[0-9]*\.?[0-9]+$ ]]; then
    echo "Error: radius must be a float (like: 0.6)"
    exit 1
fi

if ! [[ $THRESHOLD =~ ^[-+]?[0-9]*\.?[0-9]+$ ]]; then
    echo "Error: threshold must be a float number (like: 0.7)"
    exit 1
fi

if ! [[ $FRAME_DIS =~ ^[-+]?[0-9]*\.?[0-9]+$ ]]; then
    echo "Error: frame_dis must be a float number (like: 0.35)"
    exit 1
fi


# 输出参数信息
echo "Parameters:"
echo "  radius = $RADIUS"
echo "  threshold = $THRESHOLD"
echo "  frame_dis = $FRAME_DIS"
# 执行程序 (替换为您的实际程序命令)
# 例如: ./your_program -radius $RADIUS -threshold $THRESHOLD -max_time_diff $MAX_TIME_DIFF -k $K
./vec_video -radius $RADIUS -threshold $THRESHOLD -frame_dis $FRAME_DIS

# 检查程序是否成功执行
if [ $? -ne 0 ]; then
    echo "Error: program execution failed"
    exit 1
fi

# 检查输出文件是否存在
OUTPUT_FILE="./embeddings/ret/vec_pred.txt"
if [ ! -f "$OUTPUT_FILE" ]; then
    echo "Error: output file $OUTPUT_FILE doesnt exist"
    exit 1
fi

# 构建新文件名
NEW_FILENAME="r_${RADIUS}_thres_${THRESHOLD}_fd_${FRAME_DIS}_vec_pred.txt"
NEW_FILE_PATH="./embeddings/ret/$NEW_FILENAME"
# 重命名文件
mv "$OUTPUT_FILE" "$NEW_FILE_PATH"
python /common/home/zw393/wzz/Spring2024/video_matching_evaluation.py --gt ./embeddings/ret/query2gt_info.txt --pred $NEW_FILE_PATH --comment "test" >> "video_ret.txt"
echo "Done!"
exit 0