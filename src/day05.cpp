#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdint>
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
    public:
    uint64_t start;
    uint64_t count;

    Range(uint64_t s, uint64_t c): start(s), count(c) {}

    uint64_t end() const { return start + count - 1; }
    void set_start(uint64_t val) { start = val; }
    void set_count(uint64_t val) { count = val; }
    void set_end(uint64_t val) { 
        if(val >= start) {
            count = val - start + 1; 
        } else {
            printf("[WARNING] End value is smaller start of range\n");
            exit(1);
        }
    }

    void print(const char *tail="\n") const {
        cout << "start: " << start << ", count: " << count << tail; 
    }
    
    void print_start_end(const char *tail="\n") const {
        cout << "start: " << start << ", end: " << this->end() << tail; 
    }

    bool is_overlap(const Range &other) const {
        if (is_overlap_at_lower(other) || is_overlap_at_upper(other) || is_contained_in(other)) return true; 
        return false;
    }

    //      [o o o]         < this
    // [- - -o o o - - - -] < other
    bool is_contained_in(const Range &other) const {
        if (other.start <= start && end() <= other.end()) return true;
        return false;
    }

    // [- - - - - - - o o o]         < this
    //               [o o o - - - -] < other
    bool is_overlap_at_upper(const Range &other) const {
        if (start < other.start && other.end() > end() && other.start <= end()) return true; 
        return false;
    }

    //               [o o o - - - -] < this
    // [- - - - - - - o o o]         < other
    bool is_overlap_at_lower(const Range &other) const {
        if (start > other.start && other.end() < end() && start <= other.end()) return true; 
        return false;
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

vector<Range> get_seeds2(const string &line) {
    vector<Range> ret;
    uint64_t start;
    uint64_t count;

    istringstream iss(line.substr(7, line.size()));
    string seed_nums;
    while (iss >> start){ 
        iss >> count;
        ret.emplace_back(start, count);
    }
    return ret;
} 

uint64_t part2(const input_t &in) {
    vector<Range> src = get_seeds2(in[0]);
    vector<Range> dst = src;

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

        #ifdef DEBUG
        std::cout << "================================================================\n";
        std::cout << "map src_ranges -> dst_ranges \n";
        for (size_t ii = 0; ii < src_ranges.size(); ii++) {
            std::cout << ii << ") ";
            src_ranges[ii].print_start_end("");
            cout << " -> ";
            dst_ranges[ii].print_start_end("\n");
        }
        #endif

        for (size_t j = 0; j < dst.size(); j++) {
            for (size_t k = 0; k < src_ranges.size(); k++) {
                if (dst[j].is_contained_in(src_ranges[k])){
                    //      [o o o]         < dst[j]
                    // [- - -o o o - - - -] < src_ranges[k]
                    dst[j].start = dst_ranges[k].start + (dst[j].start - src_ranges[k].start);
                    break;
                }
                else if (dst[j].is_overlap_at_upper(src_ranges[k])) {
                    Range nr(dst[j].start, 0);
                    nr.set_end(src_ranges[k].start-1);
                    dst.push_back(nr);
                    // [- - - - - - - A A A] dst[j]
                    //               [A A A - - - -] src_range
                    int64_t e_old = dst[j].end();
                    dst[j].start = dst_ranges[k].start;
                    dst[j].set_end(dst_ranges[k].start + (e_old - src_ranges[k].start));
                    break;
                }
                else if (dst[j].is_overlap_at_lower(src_ranges[k])) {
                    Range nr(src_ranges[k].end() + 1, 0);
                    nr.set_end(dst[j].end());
                    dst.push_back(nr);
                    //               [A A A - - - -] dst[j]
                    // [- - - - - - - A A A] src_range
                    int64_t s_old = dst[j].start;
                    dst[j].start = dst_ranges[k].start + (src_ranges[k].end() - s_old);
                    dst[j].set_end(dst_ranges[k].end());
                    break;
                }
            }
        }
        #ifdef DEBUG 
        {
        // std::cout << "================================================================\n";
        std::cout << "src = \n";
        int ii=1;
        for (auto el : src) {
            std::cout << ii++ << ") ";
            el.print_start_end();;
        }
        std::cout << "dst = \n";
        ii=1;
        for (auto el : dst) {
            std::cout << ii++ << ") ";
            el.print_start_end();
        }}
        #endif

        src = dst;
        i++;
    }

    uint64_t min = UINT64_MAX;
    for (const Range &n : src){
        if(min > n.start) min = n.start;
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
    uint64_t res2 = part2(lines);

    std::cout << "-----PART 1-----\n";
    std::cout << "Lowest Location = " << res1 << std::endl;
    std::cout << "-----PART 2-----\n";
    std::cout << "Lowest Location = " << res2 << std::endl;

    return 0;
}