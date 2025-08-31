import argparse
import re
from datetime import datetime
from collections import defaultdict

def parse_time(time_str):
    """将时间字符串解析为秒数"""
    try:
        t = datetime.strptime(time_str, "%H:%M:%S")
        return t.hour * 3600 + t.minute * 60 + t.second
    except ValueError:
        # 尝试解析没有小时的格式
        try:
            t = datetime.strptime(time_str, "%M:%S")
            return t.minute * 60 + t.second
        except ValueError:
            print(f"无法解析时间格式: {time_str}")
            return 0

def normalize_video_id(video_id):
    return video_id.split('.')[0]
# ground truth video name contains .flv or .mp4 remove it 
def load_ground_truth(gt_file):
    gt_matches = []
    total_gt_frames = 0
    gt_dict = defaultdict(list)
    with open(gt_file, 'r') as f:
        for line in f:
            parts = line.strip().split()
            if len(parts) >= 6:
                # 提取6个主要元素
                video_a = normalize_video_id(parts[0])
                start_a = parse_time(parts[1])
                end_a = parse_time(parts[2])
                video_b = normalize_video_id(parts[3])
                start_b = parse_time(parts[4])
                end_b = parse_time(parts[5])
                total_gt_frames += end_b - start_b + 1
                gt_matches.append({
                    'query_video': video_a,
                    'query_start': start_a,
                    'query_end': end_a,
                    'match_video': video_b,
                    'match_start': start_b,
                    'match_end': end_b
                })
    for gt in gt_matches:
        key = (gt['query_video'], gt['query_start'], gt['query_end'])
        value = (gt['match_video'], gt['match_start'], gt['match_end'])
        gt_dict[key].append(value)
    return gt_dict, total_gt_frames

def load_predictions(pred_file):
    """加载预测结果数据"""
    predictions = []
    total_pred_frames = 0
    with open(pred_file, 'r') as f:
        for line in f:
            parts = line.strip().split()
            if len(parts) >= 6:
                # 提取6个主要元素
                video_a = normalize_video_id(parts[0])
                start_a = parse_time(parts[1])
                end_a = parse_time(parts[2])
                video_b = normalize_video_id(parts[3])
                start_b = parse_time(parts[4])
                end_b = parse_time(parts[5])
                total_pred_frames += end_b - start_b + 1
                predictions.append({
                    'query_video': video_a,
                    'query_start': start_a,
                    'query_end': end_a,
                    'match_video': video_b,
                    'match_start': start_b,
                    'match_end': end_b
                })
    return predictions, total_pred_frames

def calculate_frame_overlap(pred, gt):
    gt_video = gt[0]
    gt_start = gt[1]
    gt_end = gt[2]
    #print(pred['match_video'], gt_video)
    if pred['match_video'] != gt[0]:
        return 0
    match_overlap_start = max(pred['match_start'], gt_start)
    match_overlap_end = min(pred['match_end'], gt_end)
    
    # 计算重叠的帧数
    match_overlap_frames = max(0, match_overlap_end - match_overlap_start + 1)
    
    # 返回两个片段中较小的重叠帧数
    return match_overlap_frames

def calculate_metrics(gt_path, pred_path):
    """precision and recall"""
    if not gt_path or not pred_path:
        return 0.0, 0.0
    gt_dict, total_gt_frames = load_ground_truth(gt_path)
    predictions, total_pred_frames = load_predictions(pred_path)
    #print(predictions)
    print(f"Ground Truth frame number: {total_gt_frames}")
    print(f"Predictions frame number: {total_pred_frames}")

    total_matched_frames = 0
    for pred in predictions:
        overlap_frames = 0
        key = (pred['query_video'], pred['query_start'], pred['query_end'])
        if key not in gt_dict:
            print(f"Error: query video {pred['query_video']} not in ground truth")
        for gt_item in gt_dict[key]:
            overlap_frames += calculate_frame_overlap(pred, gt_item)
        total_matched_frames += overlap_frames
    
    # calculate precision and recall
    precision = total_matched_frames / total_pred_frames if total_pred_frames > 0 else 0
    recall = total_matched_frames / total_gt_frames if total_gt_frames > 0 else 0
    
    return precision, recall

def main():
    parser = argparse.ArgumentParser(description='Video Matching Evaluation')
    parser.add_argument('--gt', required=True, help='Ground truth file path')
    parser.add_argument('--pred', required=True, help='Predictions file path')
    parser.add_argument('--comment', required=True, help='show basic infomation')
    
    args = parser.parse_args()
    cmt = args.comment
    print(f"Pred file is: {args.pred}")
    precision, recall = calculate_metrics(args.gt, args.pred)
    print(f"Experiment Info: {cmt}")
    print(f"Precision: {precision:.4f}")
    print(f"Recall: {recall:.4f}")
    
    # 计算F1分数
    if precision + recall > 0:
        f1 = 2 * precision * recall / (precision + recall)
        print(f"F1 Score: {f1:.4f}")
def loadGT(gt_file):
    gt_matches = []
    gt_dict = defaultdict(list)
    with open(gt_file, 'r') as f:
        for line in f:
            parts = line.strip().split()
            if len(parts) >= 6:
                # 提取6个主要元素
                video_a = parts[0]
                start_a = parts[1]
                end_a = parts[2]
                video_b = parts[3]
                start_b = parts[4]
                end_b = parts[5]
                gt_matches.append({
                    'query_video': video_a,
                    'query_start': start_a,
                    'query_end': end_a,
                    'match_video': video_b,
                    'match_start': start_b,
                    'match_end': end_b
                })
    for gt in gt_matches:
        key = (gt['query_video'], gt['query_start'], gt['query_end'])
        value = (gt['match_video'], gt['match_start'], gt['match_end'])
        gt_dict[key].append(value)
    return gt_dict
def load_query_info(query_file):
    query_info = []
    with open(query_file, 'r') as f:
        for line in f:
            parts = line.strip().split()
            if len(parts) == 3:
                query_info.append(parts)
            else:
                print(f"Error: query file format error, line: {line}")
    return query_info
def generate_query_gt(query_file, gt_file, query2gt_file):
    gt_dict = loadGT(gt_file)
    query_info = load_query_info(query_file)[:100]
    query2gt_info = []
    for query in query_info:
        query_key = (query[0], query[1], query[2])
        if query_key not in gt_dict:
            print(f"Error: query video {query[0]} not in ground truth")
        else:
            gt_infos = gt_dict[query_key]
            for gt_info in gt_infos:
                query2gt_info.append((query[0], query[1], query[2], gt_info[0], gt_info[1], gt_info[2]))
    with open(query2gt_file, 'w') as f:
        for query2gt in query2gt_info:
            f.write(f"{query2gt[0]} {query2gt[1]} {query2gt[2]} {query2gt[3]} {query2gt[4]} {query2gt[5]}\n")
                
if __name__ == "__main__":
    main()
    #generate_query_gt("./vector_sequence_alignment/embeddings/ret/query.txt", "./vector_sequence_alignment/embeddings/all_query/valid_gt.txt", "./vector_sequence_alignment/embeddings/ret/query2gt_info.txt")