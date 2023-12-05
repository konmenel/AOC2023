#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <numeric>
#include <algorithm>


using std::vector;
using std::string;
using std::istringstream;
using std::cout;
using std::endl;
using std::getline;

typedef std::vector<std::string> input_t;


void usage(const std::string &program_name) {
    std::cout << "Usage: " << program_name << " <input_file>\n";
}


input_t parse_inputs(const std::string &filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Unable to open file \"" << filename << "\"!" << std::endl;
        exit(EXIT_FAILURE);
    }

    input_t lines;
    std::string line;

    while (std::getline(file, line)) {
        lines.emplace_back(std::move(line));
    }
    
    file.close();
    return lines;
}


vector<uint64_t> get_seeds(const string &line) {
    vector<uint64_t> ret;
    uint64_t num;

    istringstream iss(line.substr(7, line.size()));
    string seed_nums;
    while (iss >> num){ ret.push_back(num); }

    return ret;
} 

struct Range {
    uint64_t start;
    uint64_t count;

    Range(uint64_t s, uint64_t c): start(s), count(c) {}

    void print() {
        cout << "start: " << start << ", count: " << count << endl; 
    }
};

uint64_t map_num(uint64_t num, vector<Range> src_ranges, vector<Range> dst_ranges) {
    for (size_t i = 0; i < src_ranges.size(); i++) {
        uint64_t start = src_ranges[i].start;
        uint64_t end = start + src_ranges[i].count;

        if (num >= start && num < end) {
            return dst_ranges[i].start + (num - start);
        }
    }
    return num;
}

uint64_t part1(const input_t &in) {
    vector<uint64_t> src = get_seeds(in[0]);
    vector<uint64_t> dst(src.size(), 0);

    for (size_t i = 3; i < in.size(); i++){
        vector<Range> src_ranges;
        vector<Range> dst_ranges;
        while(!in[i].empty()){
            istringstream iss(in[i]);
            uint64_t dst_start;
            uint64_t src_start;
            uint64_t count;
            iss >> dst_start;
            iss >> src_start;
            iss >> count;

            src_ranges.emplace_back(src_start, count);
            dst_ranges.emplace_back(dst_start, count);
            i++;
        } 

        for (size_t j = 0; j < src.size(); j++) {
            dst[j] = map_num(src[j], src_ranges, dst_ranges);
            #ifdef DEBUG
            cout << src[j] << " => " << dst[j] << endl;
            #endif
        }
        src = dst;
        i++;
        #ifdef DEBUG
        cout << endl;
        #endif
    }

    uint64_t min = UINT64_MAX;
    for (uint64_t n : src){
        if(min > n) min = n;
    }
    return min;
}

vector<uint64_t> get_seeds2(const string &line) {
    vector<uint64_t> ret;
    uint64_t start;
    uint64_t count;

    istringstream iss(line.substr(7, line.size()));
    string seed_nums;
    while (iss >> start){ 
        iss >> count;
        for (uint64_t n = start; n < start+count; n++) {
            ret.push_back(n);
        }
    }
    return ret;
} 

uint64_t part2(const input_t &in) {
    vector<uint64_t> src = get_seeds2(in[0]);
    vector<uint64_t> dst(src.size(), 0);

    for (size_t i = 3; i < in.size(); i++){
        vector<Range> src_ranges;
        vector<Range> dst_ranges;
        while(!in[i].empty()){
            istringstream iss(in[i]);
            uint64_t dst_start;
            uint64_t src_start;
            uint64_t count;
            iss >> dst_start;
            iss >> src_start;
            iss >> count;

            src_ranges.emplace_back(src_start, count);
            dst_ranges.emplace_back(dst_start, count);
            i++;
        } 

        for (size_t j = 0; j < src.size(); j++) {
            dst[j] = map_num(src[j], src_ranges, dst_ranges);
            #ifdef DEBUG
            cout << src[j] << " => " << dst[j] << endl;
            #endif
        }
        src = dst;
        i++;
        #ifdef DEBUG
        cout << endl;
        #endif
    }

    uint64_t min = UINT64_MAX;
    for (uint64_t n : src){
        if(min > n) min = n;
    }
    return min;
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    input_t lines = parse_inputs(argv[1]);
    uint64_t res1 = part1(lines);
    // uint64_t res2 = part2(lines);

    std::cout << "-----PART 1-----\n";
    std::cout << "Lowest Location = " << res1 << std::endl;
    // std::cout << "-----PART 2-----\n";
    // std::cout << "Lowest Location = " << res2 << std::endl;

    return 0;
}