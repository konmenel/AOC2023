#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <format>
#include <vector>
#include <string>
#include <numeric>
#include <algorithm>
#include <unordered_map>


typedef std::vector<std::string> input_t;

#ifdef DEBUG
    #define debug_println(fmt, ...) std::cout << std::format((fmt), ##__VA_ARGS__) << std::endl
    #define debug_print(fmt, ...) std::cout << std::format((fmt), ##__VA_ARGS__)
#else
    #define debug_println(fmt, ...)
    #define debug_print(fmt, ...)
#endif


#define indexPerm(perm, i) (((perm) & (1 << (i))) >> (i))


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


std::vector<uint32_t> countContiguous(const std::string &condition) {
    std::vector<uint32_t> ret;
    uint32_t sum = 0;
    for (char c : condition) {
        if (c == '.') {
            if(sum) ret.push_back(sum);
            sum = 0;
        } else if (c == '#') sum++;
    }
    if(sum) ret.push_back(sum);
    return ret;
} 


void getRecordAndCounts(const std::string &line, std::string &condition, std::vector<uint32_t> &counts) {
    std::istringstream iss(line);
    iss >> condition;
    std::string tmp;
    uint32_t num;
    while(std::getline(iss, tmp, ',')) {
        std::istringstream(tmp) >> num;
        counts.push_back(num);
    }
}


uint32_t countUknowns(const std::string &condition) {
    uint32_t sum = 0;
    for (char c : condition) {
        if (c == '?') sum++;
    }
    return sum;
}


void applyPerm(const std::string &condition, uint64_t perm, std::string &permuted_str) {
    #ifdef DEBUG
    if (condition.size() != permuted_str.size())
        debug_println("Strings must have the same length.");
    #endif
    uint64_t j = 0;
    for (size_t i = 0; i < condition.size(); i++) {
        if (condition[i] == '?') {
            permuted_str[i] = indexPerm(perm, j)*11 + 35;
            j++;
        }
    }
}


uint32_t countValidPerm(const std::string &condition, const std::vector<uint32_t> &counts) {
    uint32_t sum = 0;
    const uint32_t unknowns = countUknowns(condition);
    std::string tmp = condition;

    debug_println("Unknown condition {}:", condition);
    for (uint64_t perm = 0; perm < (1 << unknowns); perm++) {
        applyPerm(condition, perm, tmp);
        std::vector<uint32_t> perm_counts = countContiguous(tmp);
        if (perm_counts == counts) {
            sum++;

            debug_print("\t{}) perm={}, applied={} ", sum, perm, tmp);
            #ifdef DEBUG
            for (const uint32_t &c : perm_counts)
                debug_print("{},", c);
            debug_println("");
        }
        
        #endif
    }
    return sum;
}


uint64_t part1(const input_t &in) {
    uint64_t totalArrangements = 0;
    for (const auto &line : in) {
        std::string condition;
        std::vector<uint32_t> counts;
        getRecordAndCounts(line, condition, counts);
        totalArrangements += countValidPerm(condition, counts);
    }

    return totalArrangements;
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }
    input_t lines = parse_inputs(argv[1]);

    auto res1 = part1(lines);
    std::cout << "-----PART 1-----\n";
    std::cout << "Total arrangements = " << res1 << std::endl;
    
    // auto res2 = part2(lines);
    // std::cout << "-----PART 2-----\n";
    // std::cout << "*TEXT 2 HERE*" << res2 << std::endl;

    return 0;
}