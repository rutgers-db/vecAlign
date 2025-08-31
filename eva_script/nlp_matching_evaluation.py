import argparse
import re
from datetime import datetime
from collections import defaultdict

def load_ground_truth(gt_file):
    gt_matches = []
    total_gt_length = 0
    gt_dict = defaultdict(list)
    with open(gt_file, 'r') as f:
        for line in f:
            parts = line.strip().split()
            if len(parts) == 6:
                # 提取6个主要元素
                susp_file = parts[0]
                susp_offset = parts[1]
                susp_len = parts[2]
                src_file = parts[3]
                src_offset = parts[4]
                src_len = parts[5]
                total_gt_length += int(src_len)
                gt_matches.append({
                    'susp_file': susp_file,
                    'susp_start': int(susp_offset),
                    'susp_end': int(susp_offset) + int(susp_len) - 1,
                    'src_file': src_file,
                    'src_start': int(src_offset),
                    'src_end': int(src_len) + int(src_offset) - 1
                })
            else:
                print(f"Error: ground truth file format error, line: {line}")
    for gt in gt_matches:
        key = (gt['susp_file'], gt['susp_start'], gt['susp_end'])
        value = (gt['src_file'], gt['src_start'], gt['src_end'])
        gt_dict[key].append(value)
    return gt_dict, total_gt_length

def load_predictions(pred_file):
    """加载预测结果数据"""
    predictions = []
    total_pred_length = 0
    with open(pred_file, 'r') as f:
        for line in f:
            parts = line.strip().split()
            if len(parts) == 6:
                # 提取6个主要元素
                susp_file = parts[0]
                susp_offset = parts[1]
                susp_len = parts[2]
                src_file = parts[3]
                src_offset = parts[4]
                src_len = parts[5]
                total_pred_length += int(src_len)
                predictions.append({
                    'susp_file': susp_file,
                    'susp_start': int(susp_offset),
                    'susp_end': int(susp_offset) + int(susp_len) - 1,
                    'src_file': src_file,
                    'src_start': int(src_offset),
                    'src_end': int(src_len) + int(src_offset) - 1
                })
            else:
                print(f"Error: prediction file format error, line: {line}")
    return predictions, total_pred_length

def calculate_overlap(pred, gt):
    gt_doc = gt[0]
    gt_start = gt[1]
    gt_end = gt[2]
    #print(pred['match_video'], gt_video)
    if pred['src_file'] != gt_doc:
        return 0
    match_overlap_start = max(pred['src_start'], gt_start)
    match_overlap_end = min(pred['src_end'], gt_end)
    
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
    print(f"Ground Truth total length: {total_gt_frames}")
    print(f"Predictions total length: {total_pred_frames}")

    total_matched_frames = 0
    for pred in predictions:
        overlap_frames = 0
        key = (pred['susp_file'], pred['susp_start'], pred['susp_end'])
        if key not in gt_dict:
            print(f"Error: susp doc {pred['susp_file']} not in gt file")
        for gt_item in gt_dict[key]:
            overlap_frames += calculate_overlap(pred, gt_item)
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
    print(f"Ground Truth file is: {args.gt}")
    print(f"Pred file is: {args.pred}")
    precision, recall = calculate_metrics(args.gt, args.pred)
    print(f"Experiment Info: {cmt}")
    print(f"Precision: {precision:.4f}")
    print(f"Recall: {recall:.4f}")
    
    # 计算F1分数
    if precision + recall > 0:
        f1 = 2 * precision * recall / (precision + recall)
        print(f"F1 Score: {f1:.4f}")
                
if __name__ == "__main__":
    main()
    #generate_query_gt("./vector_sequence_alignment/embeddings/ret/query.txt", "./vector_sequence_alignment/embeddings/all_query/valid_gt.txt", "./vector_sequence_alignment/embeddings/ret/query2gt_info.txt")