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
typedef bool bit;
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef  int8_t  i8;
typedef  int16_t i16;
typedef  int32_t i32;
typedef  int64_t i64;

#ifdef DEBUG
    #define debug_println(fmt, ...) std::cout << std::format((fmt), ##__VA_ARGS__) << std::endl
    #define debug_print(fmt, ...) std::cout << std::format((fmt), ##__VA_ARGS__)
#else
    #define debug_println(fmt, ...)
    #define debug_print(fmt, ...)
#endif




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


std::vector<u32> countContiguous(const std::string &condition) {
    std::vector<u32> ret;
    u32 sum = 0;
    for (char c : condition) {
        if (c == '.') {
            if(sum) ret.push_back(sum);
            sum = 0;
        } else if (c == '#') sum++;
    }
    if(sum) ret.push_back(sum);
    return ret;
} 


void getRecordAndCounts(const std::string &line, std::string &condition, std::vector<u32> &counts) {
    std::istringstream iss(line);
    iss >> condition;
    std::string tmp;
    u32 num;
    while(std::getline(iss, tmp, ',')) {
        std::istringstream(tmp) >> num;
        counts.push_back(num);
    }
}


u32 countUknowns(const std::string &condition) {
    u32 sum = 0;
    for (char c : condition) {
        if (c == '?') sum++;
    }
    return sum;
}


inline bit indexPerm(u64 perm, size_t i) { return ((perm & (1 << i)) >> i); }


void applyPerm(const std::string &condition, u64 perm, std::string &permuted_str) {
    #ifdef DEBUG
    if (condition.size() != permuted_str.size())
        debug_println("Strings must have the same length.");
    #endif
    u64 j = 0;
    for (size_t i = 0; i < condition.size(); i++) {
        if (condition[i] == '?') {
            permuted_str[i] = indexPerm(perm, j++)*11 + 35;
        }
    }
}


u32 countValidPerm(const std::string &condition, const std::vector<u32> &counts) {
    u32 sum = 0;
    const u32 unknowns = countUknowns(condition);
    std::string tmp = condition;

    debug_println("Unknown condition {}:", condition);
    for (u64 perm = 0; perm < (1 << unknowns); perm++) {
        applyPerm(condition, perm, tmp);
        std::vector<u32> perm_counts = countContiguous(tmp);
        if (perm_counts == counts) {
            sum++;

            debug_print("\t{:3}) perm={}, applied={} ", sum, perm, tmp);
            #ifdef DEBUG
            for (const u32 &c : perm_counts)
                debug_print("{},", c);
            debug_println("");
            #endif
        }
    }
    return sum;
}


u64 part1(const input_t &in) {
    u64 totalArrangements = 0;
    for (const auto &line : in) {
        std::string condition;
        std::vector<u32> counts;
        getRecordAndCounts(line, condition, counts);
        totalArrangements += countValidPerm(condition, counts);
    }

    return totalArrangements;
}


u64 part2(const input_t &in) {
    u64 totalArrangements = 0;
    for (const auto &line : in) {
        std::string condition1;
        std::vector<u32> counts;
        getRecordAndCounts(line, condition1, counts);
        std::string condition2 = "?" + condition1;
        std::string condition3 = condition1 + "?";
        std::string condition4 = "?" + condition1 + "?";
        
        const uint n1 = countValidPerm(condition1, counts);
        const uint n2 = countValidPerm(condition2, counts);
        const uint n3 = countValidPerm(condition3, counts);
        const uint n4 = countValidPerm(condition4, counts);
        totalArrangements += 0;
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
    // std::cout << "Total arrangements = " << res2 << std::endl;

    return 0;
}