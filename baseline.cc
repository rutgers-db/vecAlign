#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <climits>
#include <unordered_map>
#include "io.hpp"

using namespace std;

struct MatchNode {
    int q_idx;      // Query frame index in q_frames
    int r_idx;      // Matched frame index in r_frames
    float score;    // Similarity score
    int prev;       // Previous node in path (for DP)
    float path_score; // Total score up to this node

    MatchNode(int q, int r, float s) : q_idx(q), r_idx(r), score(s), prev(-1), path_score(s) {}
};

struct MatchedSegment {
    vector<string> frames;  // 匹配的帧序列
    float total_score;     // 总匹配分数
    int start_idx;         // 参考视频中的起始索引
    int end_idx;          // 参考视频中的结束索引
    
    MatchedSegment(const vector<string>& f, float s, int start, int end) 
        : frames(f), total_score(s), start_idx(start), end_idx(end) {}
};
/*
vector<MatchedSegment> findAllMatchedSegments(
    vector<vector<int>>& r_match_indices,
    vector<vector<float>>& sim_scores,
    vector<string>& r_frames,
    int max_time_diff = 5,    
    int min_segment_length = 5, 
    float dis = 0.4,
    float overlap_threshold = 0.5  
) {
    // 1. 输入验证
    if (r_match_indices.empty() || sim_scores.empty() || r_frames.empty()) {
        return {};
    }

    // 2. 构建匹配节点
    const size_t MAX_NODES = 100000;
    vector<MatchNode> nodes;
    nodes.reserve(MAX_NODES);
    
    for (size_t q_idx = 0; q_idx < r_match_indices.size(); ++q_idx) {
        for (size_t k = 0; k < r_match_indices[q_idx].size(); ++k) {
            if (sim_scores[q_idx][k] <= dis) {
                if (nodes.size() >= MAX_NODES) {
                    std::cout << "Warning: Reached maximum number of nodes" << std::endl;
                    break;
                }
                nodes.emplace_back(q_idx, r_match_indices[q_idx][k], sim_scores[q_idx][k]);
            }
        }
    }

    if (nodes.empty()) return {};

    // 3. 按参考帧索引排序
    sort(nodes.begin(), nodes.end(), 
         [](const MatchNode& a, const MatchNode& b) { return a.r_idx < b.r_idx; });

    // 4. 迭代查找匹配片段
    vector<MatchedSegment> results;
    while (!nodes.empty()) {
        // 4.1 动态规划找最佳路径
        int best_end_node = 0;
        float max_score = nodes[0].score;

        for (size_t i = 1; i < nodes.size(); ++i) {
            size_t start_j = max(0, static_cast<int>(i) - max_time_diff * 2);
            
            for (size_t j = start_j; j < i; ++j) {
                // 检查是否满足匹配条件
                if (nodes[j].q_idx >= nodes[i].q_idx) continue;
                if (nodes[j].r_idx >= nodes[i].r_idx) continue;
                if (nodes[i].r_idx - nodes[j].r_idx > max_time_diff) continue;
                
                // 更新路径分数
                float new_score = nodes[j].path_score + nodes[i].score;
                if (new_score > nodes[i].path_score) {
                    nodes[i].path_score = new_score;
                    nodes[i].prev = j;
                }
            }

            if (nodes[i].path_score > max_score) {
                max_score = nodes[i].path_score;
                best_end_node = i;
            }
        }

        // 4.2 回溯构建路径
        vector<int> path_indices;
        path_indices.reserve(min(nodes.size(), static_cast<size_t>(max_time_diff * 2)));
        
        int current = best_end_node;
        while (current != -1) {
            path_indices.push_back(current);
            current = nodes[current].prev;
        }
        reverse(path_indices.begin(), path_indices.end());

        // 4.3 如果路径长度满足要求，添加到结果中
        if (path_indices.size() >= min_segment_length) {
            // 构建匹配片段
            vector<string> matched_frames;
            matched_frames.reserve(path_indices.size());
            int start_idx = nodes[path_indices.front()].r_idx;
            int end_idx = nodes[path_indices.back()].r_idx;
            
            for (int idx : path_indices) {
                matched_frames.push_back(r_frames[nodes[idx].r_idx]);
            }
            
            results.emplace_back(matched_frames, max_score, start_idx, end_idx);
            
            // 4.4 过滤重叠节点
            vector<MatchNode> new_nodes;
            new_nodes.reserve(nodes.size() / 2);
            
            for (const MatchNode& node : nodes) {
                bool overlaps = false;
                for (int idx : path_indices) {
                    if (abs(node.r_idx - nodes[idx].r_idx) < max_time_diff * overlap_threshold) {
                        overlaps = true;
                        break;
                    }
                }
                if (!overlaps) {
                    new_nodes.push_back(node);
                }
            }
            nodes = std::move(new_nodes);
        } else {
            break;
        }
    }

    return results;
} 
*/
/*
vector<MatchedSegment> findAllMatchedSegments(
    vector<vector<int>>& r_match_indices,
    vector<vector<float>>& sim_scores,
    vector<string>& r_frames,
    int max_time_diff = 5,    
    int min_segment_length = 5, 
    float dis = 0.4,
    float overlap_threshold = 0.5
) {
    
    // 1. 输入验证
    if (r_match_indices.empty() || sim_scores.empty() || r_frames.empty()) {
        return {};
    }

    // 2. 构建匹配节点
    const size_t MAX_NODES = 100000;
    vector<MatchNode> nodes;
    nodes.reserve(MAX_NODES);
    
    for (size_t q_idx = 0; q_idx < r_match_indices.size(); ++q_idx) {
        for (size_t k = 0; k < r_match_indices[q_idx].size(); ++k) {
            if (sim_scores[q_idx][k] <= dis) {
                if (nodes.size() >= MAX_NODES) {
                    std::cout << "Warning: Reached maximum number of nodes" << std::endl;
                    break;
                }
                nodes.emplace_back(q_idx, r_match_indices[q_idx][k], sim_scores[q_idx][k]);
            }
        }
    }

    if (nodes.empty()) return {};

    // 3. 按参考帧索引排序
    sort(nodes.begin(), nodes.end(), 
         [](const MatchNode& a, const MatchNode& b) { return a.r_idx < b.r_idx; });
    vector<MatchedSegment> results;
    
    // 添加调试信息
    std::cout << "Starting DP with nodes size: " << nodes.size() << std::endl;
    
    while (!nodes.empty()) {
        int best_end_node = 0;
        float max_score = nodes[0].score;

        // 优化动态规划过程，减少内存使用
        for (size_t i = 1; i < nodes.size(); ++i) {
            // 打印进度
            if (i % 50 == 0) {
                std::cout << "Processing node " << i << "/" << nodes.size() << std::endl;
            }
            
            // 限制向前查找的范围
            size_t start_j = max(0, static_cast<int>(i) - max_time_diff * 2);
            
            for (size_t j = start_j; j < i; ++j) {
                // 添加更严格的条件检查
                if (nodes[j].q_idx >= nodes[i].q_idx) continue;  // 跳过无效的前驱
                if (nodes[j].r_idx >= nodes[i].r_idx) continue;  // 跳过无效的前驱
                
                int time_diff = nodes[i].r_idx - nodes[j].r_idx;
                if (time_diff > max_time_diff) continue;  // 跳过时间差太大的
                
                float new_score = nodes[j].path_score + nodes[i].score;
                if (new_score > nodes[i].path_score) {
                    nodes[i].path_score = new_score;
                    nodes[i].prev = j;
                }
            }

            if (nodes[i].path_score > max_score) {
                max_score = nodes[i].path_score;
                best_end_node = i;
            }
        }

        // 优化路径回溯
        vector<int> path_indices;
        path_indices.reserve(min(nodes.size(), static_cast<size_t>(max_time_diff * 2)));
        
        int current = best_end_node;
        int path_length = 0;
        while (current != -1 && path_length < max_time_diff * 2) {
            path_indices.push_back(current);
            current = nodes[current].prev;
            path_length++;
        }
        
        if (path_indices.size() >= min_segment_length) {
            vector<string> matched_frames;
            matched_frames.reserve(path_indices.size());
            int start_idx = nodes[path_indices.back()].r_idx;
            int end_idx = nodes[path_indices.front()].r_idx;
            
            for (auto it = path_indices.rbegin(); it != path_indices.rend(); ++it) {
                matched_frames.push_back(r_frames[nodes[*it].r_idx]);
            }
            
            results.emplace_back(matched_frames, max_score, start_idx, end_idx);
            
            // 优化节点过滤
            vector<MatchNode> new_nodes;
            new_nodes.reserve(nodes.size() / 2);  // 预估过滤后的大小
            
            for (const MatchNode& node : nodes) {
                bool overlaps = false;
                for (int idx : path_indices) {
                    if (abs(node.r_idx - nodes[idx].r_idx) < max_time_diff * overlap_threshold) {
                        overlaps = true;
                        break;
                    }
                }
                if (!overlaps) {
                    new_nodes.push_back(node);
                }
            }
            
            if (new_nodes.size() == nodes.size()) {
                std::cout << "Warning: No nodes filtered out, breaking loop" << std::endl;
                break;
            }
            
            nodes = std::move(new_nodes);
        } else {
            break;
        }
        
        if (results.size() > 1000) {
            std::cout << "Warning: Reached maximum number of results" << std::endl;
            break;
        }
    }

    return results;
}
*/
vector<MatchedSegment> findAllMatchedSegments(
    vector<vector<int>>& r_match_indices,
    vector<vector<float>>& sim_scores,
    vector<string>& r_frames,
    int max_time_diff = 5,    
    int min_segment_length = 5, 
    float dis = 0.4,
    float overlap_threshold = 0.5  
) {
    // 1. 输入验证
    if (r_match_indices.empty() || sim_scores.empty() || r_frames.empty()) {
        return {};
    }

    // 2. 构建匹配节点
    vector<MatchNode> nodes;
    for (size_t q_idx = 0; q_idx < r_match_indices.size(); ++q_idx) {
        for (size_t k = 0; k < r_match_indices[q_idx].size(); ++k) {
            if (sim_scores[q_idx][k] <= dis) {
                // 添加安全检查
                if (r_match_indices[q_idx][k] < r_frames.size()) {
                    float sim_score = dis - sim_scores[q_idx][k];
                    nodes.emplace_back(q_idx, r_match_indices[q_idx][k], sim_score);
                }
            }
        }
    }

    if (nodes.empty()) return {};
    //std::cout << "Nodes size: " << nodes.size() << std::endl;

    // 3. 按参考帧索引排序
    sort(nodes.begin(), nodes.end(), 
         [](const MatchNode& a, const MatchNode& b) { return a.r_idx < b.r_idx; });

    // 4. 迭代查找匹配片段
    vector<MatchedSegment> results;
    int iteration = 0;
    while (!nodes.empty() && iteration < 100) {  // 添加迭代次数限制
        iteration++;
        
        // 4.1 动态规划找最佳路径
        int best_end_node = 0;
        float max_score = nodes[0].score;

        for (size_t i = 1; i < nodes.size(); ++i) {
            size_t start_j = max(0, static_cast<int>(i) - max_time_diff * 2);
            
            for (size_t j = start_j; j < i; ++j) {
                if (nodes[j].q_idx >= nodes[i].q_idx) continue;
                if (nodes[j].r_idx >= nodes[i].r_idx) continue;
                if (nodes[i].r_idx - nodes[j].r_idx > max_time_diff) continue;
                
                float new_score = nodes[j].path_score + nodes[i].score;
                if (new_score > nodes[i].path_score) {
                    nodes[i].path_score = new_score;
                    nodes[i].prev = j;
                }
            }

            if (nodes[i].path_score > max_score) {
                max_score = nodes[i].path_score;
                best_end_node = i;
            }
        }

        // 4.2 回溯构建路径
        //std::cout << "step 2 here " << std::endl;
        vector<int> path_indices;
        int current = best_end_node;
        int max_path_length = nodes.size();
        unordered_set<int> visited;
        int path_length = 0;
        while (current != -1 && current < nodes.size()) {  // 添加边界检查
            if (visited.find(current) != visited.end()) { // duplicte node 
                break;
            }
            visited.insert(current);
            path_indices.push_back(current);
            path_length++;
            if (path_length > max_path_length) {
                std::cout << "path_length > max_path_length, break" << std::endl;
                break;
            }
            current = nodes[current].prev;
        }
        reverse(path_indices.begin(), path_indices.end());

        // 4.3 如果路径长度满足要求，添加到结果中
        //std::cout << "step 3 here " << std::endl;
        if (path_indices.size() >= min_segment_length) {
            vector<string> matched_frames;
            matched_frames.reserve(path_indices.size());
            int start_idx = nodes[path_indices.front()].r_idx;
            int end_idx = nodes[path_indices.back()].r_idx;
            
            for (int idx : path_indices) {
                if (idx < nodes.size() && nodes[idx].r_idx < r_frames.size()) {  // 添加边界检查
                    matched_frames.push_back(r_frames[nodes[idx].r_idx]);
                }
            }
            
            if (!matched_frames.empty()) {  // 只有在成功构建matched_frames后才添加结果
                results.emplace_back(matched_frames, max_score, start_idx, end_idx);
            }
            
            // 4.4 过滤重叠节点
            //std::cout << "step 4 here " << std::endl;
            vector<MatchNode> new_nodes;
            for (const MatchNode& node : nodes) {
                bool overlaps = false;
                for (int idx : path_indices) {
                    if (abs(node.r_idx - nodes[idx].r_idx) < max_time_diff * overlap_threshold) {
                        overlaps = true;
                        break;
                    }
                }
                if (!overlaps) {
                    new_nodes.push_back(node);
                }
            }
            nodes = std::move(new_nodes);
        } else {
            break;
        }
    }
    return results;
}
// Helper function to print the result
void print_result(const vector<MatchedSegment>& result) {
    if (result.empty()) {
        cout << "No matching segment found that meets the criteria." << endl;
        return;
    }

    cout << "Found matching segments with " << result.size() << " frames:" << endl;
    for (const auto& ret: result) {
        std::cout << "--------------------------------" << std::endl;
        std::cout << "matched segment score: " << ret.total_score << std::endl;
        std::cout << "first frame of matched segment: " << ret.frames[0] << std::endl;
        std::cout << "last frame of matched segment: " << ret.frames[ret.frames.size() - 1] << std::endl;
        std::cout << "--------------------------------" << std::endl;
    }
    cout << endl;
}

void getTopKMatchedFrames(std::vector<std::vector<float>>& query_seq_vec, 
    hnswlib::HierarchicalNSW<float>* &alg_hnsw,
    int k, std::string hnsw_file_name, int dim,
    vector<vector<int>>& r_match_indices,
    vector<vector<float>>& sim_scores
){
    r_match_indices.clear();
    sim_scores.clear();
    size_t query_num = query_seq_vec.size();
    r_match_indices.resize(query_num);
    sim_scores.resize(query_num);
    int num_threads = 20;
    
    ParallelFor(0, query_num, num_threads, [&](size_t query_idx, size_t threadId) {
        
        auto result = alg_hnsw->searchKnn(query_seq_vec[query_idx].data(), k);
        while (!result.empty()) {
            r_match_indices[query_idx].push_back(result.top().second);  // Store approximate neighbors
            sim_scores[query_idx].push_back(result.top().first);
            result.pop();
        }
    });
    /*
    for (int i = 0; i < query_seq_vec.size(); i++){
        std::vector<float> q_frame = query_seq_vec[i];
        std::vector<int> r_match_index;
        std::vector<float> sim_score;
        auto result = alg_hnsw->searchKnn(q_frame.data(), k);
        while (!result.empty()){
            sim_score.push_back(result.top().first);
            r_match_index.push_back(result.top().second);
            result.pop();
        }
        r_match_indices.push_back(r_match_index);
        sim_scores.push_back(sim_score);
    }
    */
    return;
}

int main(int argc, char** argv) {
    auto start_time = std::chrono::high_resolution_clock::now();
    int M = 256;                 // Tightly connected with internal dimensionality of the data
                                // strongly affects the memory consumption
    int ef_construction = 200;  // Controls index search speed/build speed tradeoff
    int num_threads = 20;       // Number of threads for operations with index
    int k = 10;                 // number of nearest neighbors
    float radius = 0.1;         // a fixed radius //threshold for Euclidean distance
    int count_max = 100;
    int dim = 512;
    float threshold = 0.8; //query sequence length * threshold = min segment length
    float overlap_threshold = 0.5; //overlap threshold for matched segments
    int max_time_diff = 5; //max time difference for matched segments
    std::string hnsw_file_name = "";
    for (int i = 0; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-M")                   M              = atoi(argv[i+1]);
        if (arg == "-k")                   k              = atoi(argv[i+1]);
        if (arg == "-radius")              radius         = atof(argv[i+1]);
        if (arg == "-ef")              ef_construction    = atoi(argv[i+1]);
        if (arg == "-hnsw_file_name")  hnsw_file_name     = std::string(argv[i+1]);
        if (arg == "-count_max")          count_max       = atoi(argv[i+1]);
        if (arg == "-threshold")          threshold       = atof(argv[i+1]);
        if (arg == "-overlap_threshold")  overlap_threshold       = atof(argv[i+1]);
        if (arg == "-max_time_diff")      max_time_diff       = atoi(argv[i+1]);
    }
    
    std::cout << "parameter k: " << k << std::endl;
    std::cout << "parameter radius: " << radius << std::endl;
    std::cout << "parameter threshold(min segment length = query sequence length * threshold): " << threshold << std::endl;
    std::cout << "parameter overlap_threshold: " << overlap_threshold << std::endl;
    std::cout << "parameter max_time_diff: " << max_time_diff << std::endl;
    std::cout << "parameter dim: " << dim << std::endl;
    std::cout << "parameter count_max: " << count_max << std::endl;
    std::cout << "parameter hnsw_file_name: " << hnsw_file_name << std::endl;
    std::string query_file_path = "./embeddings/ret/query.txt";
    std::vector<std::string> query_info;
    std::string video_folder = "/common/users/zw393/core_dataset/frames/uniform/";
    std::string folderPath = "/common/users/zw393/core_dataset/frames/uniform_embedding/"; //embedding folder path
    std::string pred_file_path = "./embeddings/ret/baseline_pred.txt";
    std::vector<std::vector<std::string>> all_query_paths;
    loadQueryInfo(query_file_path, query_info, folderPath, all_query_paths);

    std::vector<std::vector<float>> embeddings;
    float* img_emb_data = nullptr;
    std::vector<std::string> binFiles;
    loadAllImgEmb(folderPath, binFiles, embeddings, img_emb_data);
    std::vector<int> video_indices; //video 1: [0, 12], video 2: [13, 37], video 3: [38, 99], video 4: [100, 151]
    getVideoIndices(binFiles, video_indices);

    std::vector<std::vector<std::vector<float>>> all_test_seq_vec;
    
    int count = 0;
    for(auto img_paths: all_query_paths){
        std::vector<std::vector<float>> test_seq_vec;
        for (auto& img_path: img_paths){
            std::vector<float> emb(dim);
            loadImgEmb(img_path, emb);
            test_seq_vec.push_back(emb);
        }
        all_test_seq_vec.push_back(test_seq_vec);
        count++;
        if (count >= count_max) break;
    }
    std::cout << "current all_test_seq_vec size is: " << all_test_seq_vec.size() << std::endl;
    std::cout << "current search number is: " << count_max << std::endl;
    if (fileExists(hnsw_file_name)) {
        std::cout << "File exists." << std::endl;
    } else {
        std::cout << "File does not exist." << std::endl;
        return 0;
    }
    hnswlib::HierarchicalNSW<float>* alg_hnsw;
    
    hnswlib::L2Space space(dim);
    alg_hnsw = new hnswlib::HierarchicalNSW<float>(&space, hnsw_file_name);
    std::cout << "finish loading hnsw index and embedding" << std::endl;

    std::vector<std::string> video_a_vec;
    std::vector<std::string> start_a_vec;
    std::vector<std::string> end_a_vec;
    std::vector<std::string> video_b_vec;
    std::vector<std::string> start_b_vec;
    std::vector<std::string> end_b_vec;
    // test for one query sequence
    for (int i = 0; i < all_test_seq_vec.size(); i++){
        //int i = 33;
        //std::cout << "current working on query:" << i << std::endl;
        //std::cout << "current query sequence length is: " << all_test_seq_vec[i].size() << std::endl;
        std::vector<std::vector<float>> query_seq_vec = all_test_seq_vec[i];
        std::string video_a = "";
        std::string start_a = "";
        std::string end_a = "";
        
        convertBinPathToTimeFormat(all_query_paths[i][0], all_query_paths[i].back(), video_a, start_a, end_a);
        std::vector<std::vector<int>> r_match_indices;
        std::vector<std::vector<float>> sim_scores;
        getTopKMatchedFrames(query_seq_vec, alg_hnsw, k, hnsw_file_name, dim, r_match_indices, sim_scores);
        //std::cout << "finish getTopKMatchedFrames for index:" << i << std::endl;
        //std::cout << "query_seq_vec size: " << query_seq_vec.size() << std::endl;
        /*
        std::cout << "r_match_indices size: " << r_match_indices.size() << std::endl;
        std::cout << "sim_scores size: " << sim_scores.size() << std::endl;
        for (int i = 0; i < r_match_indices.size(); i++){
            if (r_match_indices[i].size() !=  k || sim_scores[i].size() != k){
                std::cout << "r_match_indices or sim_scores size error" << std::endl;
            }
            for (int j = 0; j < r_match_indices[i].size(); j++){
                std::cout << "r_match_indices[" << i << "][" << j << "]: " << r_match_indices[i][j] << std::endl;
                std::cout << "sim_scores[" << i << "][" << j << "]: " << sim_scores[i][j] << std::endl;
            }
            std::cout << "--------------------------------" << std::endl;
        }
        */

        int min_segment_length = query_seq_vec.size() * threshold;
        std::vector<MatchedSegment> results = findAllMatchedSegments(r_match_indices, sim_scores, binFiles, max_time_diff, min_segment_length, radius, overlap_threshold);
        //std::cout << "finish generate Results for index:" << i << std::endl;
        for (auto& ret: results){
            std::string video_b = "";
            std::string start_b = "";
            std::string end_b = "";
            convertBinPathToTimeFormat(ret.frames[0], ret.frames.back(), video_b, start_b, end_b);
            if (start_b != "" && end_b != ""){
                video_a_vec.push_back(video_a);
                start_a_vec.push_back(start_a);
                end_a_vec.push_back(end_a);
                video_b_vec.push_back(video_b);
                start_b_vec.push_back(start_b);
                end_b_vec.push_back(end_b);
            }
        }
    }
    writeVideoSegmentsToFile(pred_file_path, video_a_vec, start_a_vec, end_a_vec, video_b_vec, start_b_vec, end_b_vec);
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto d_total = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    std::cout << "Time taken in total: " << d_total.count() << " milliseconds" << std::endl;
    /*
    // Example usage
    vector<string> q_frames = {"q1", "q2", "q3", "q4", "q5"};
    vector<string> r_frames = {"r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10"};

    // For each q_frame, top-k matches in r_frames (indices), k = 2
    vector<vector<int>> r_match_indices = {
        {1, 5},  // matches for q1
        {2, 6},  // matches for q2
        {3, 7},  // matches for q3
        {4, 8},  // matches for q4
        {5, 9}   // matches for q5
    };

    // Similarity scores for each match
    vector<vector<float>> sim_scores = {
        {0.9f, 0.8f},  // scores for q1's matches
        {0.85f, 0.7f}, // scores for q2's matches
        {0.95f, 0.6f}, // scores for q3's matches
        {0.8f, 0.75f}, // scores for q4's matches
        {0.9f, 0.65f}  // scores for q5's matches
    };

    // Find the longest matching segment
    vector<string> result = findMacthedSegment(
        q_frames, r_match_indices, sim_scores, r_frames,
        5,  // max_time_diff
        3,  // min_segment_length
        0.7 // min_similarity
    );

    print_result(result);
    */
    return 0;
}