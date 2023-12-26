#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <format>
#include <vector>
#include <string>
#include <numeric>
#include <algorithm>
#include <array>
#include <assert.h>

#include "robin_hood.h"


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
typedef size_t usize;

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


u64 countValidPerm(const std::string &condition, const std::vector<u32> &counts) {
    u32 sum = 0;
    const u32 unknowns = countUknowns(condition);
    std::string tmp = condition;

    debug_println("Unknown condition {}:", condition);
    for (u64 perm = 0; perm < (1U << unknowns); perm++) {
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


struct Record {
    usize i;    // index in condition
    usize ci;   // index in count vector
    usize bs;   // current broken block size

    Record(usize val=0) : i(val), ci(val), bs(val) {}
    Record(usize i, usize ci, usize bs) : i(i), ci(ci), bs(bs) {}

    inline bool operator==(const Record &rhs) const {
        return i == rhs.i && ci == rhs.ci && bs == rhs.bs;
    }
};


template<>
struct std::hash<Record> {
    inline std::size_t operator()(const Record &rec) const noexcept {
        return (rec.i << 32) + (rec.ci << 16) + rec.bs;
    }
};


robin_hood::unordered_flat_map<Record, u64> cache;


u64 countValidPermCached(const std::string &cond, const std::vector<u32> &counts, Record rec) {
    if (cache.contains(rec))
        return cache[rec];

    if (rec.i == cond.size()) {
        if (rec.ci == counts.size() && rec.bs == 0) {
            return 1;
        }
        if (rec.ci == counts.size()-1 && rec.bs == counts[rec.ci]) {
            return 1;
        }
        return 0;
    }

    u64 ret = 0;
    for (char c : std::array<char,2>{'.', '#'}) {
        if (cond[rec.i] == '?' || cond[rec.i] == c) {
            if (c == '.' && rec.bs > 0 && rec.ci < counts.size() && counts[rec.ci] == rec.bs) {
                ret += countValidPermCached(cond, counts, Record(rec.i+1, rec.ci+1, 0));
            } else if (c == '.' && rec.bs == 0) {
                 ret += countValidPermCached(cond, counts, Record(rec.i+1, rec.ci, 0));
            } else if (c == '#') {
                ret += countValidPermCached(cond, counts, Record(rec.i+1, rec.ci, rec.bs+1));
            }
        }
    }
    cache[rec] = ret;
    return ret;
}


u64 part1(const input_t &in) {
    u64 totalArrangements = 0;
    for (const auto &line : in) {
        std::string condition;
        std::vector<u32> counts;
        getRecordAndCounts(line, condition, counts);
        // totalArrangements += countValidPermCached(condition, counts, Record(0, 0, 0));
        totalArrangements += countValidPerm(condition, counts);
        
        cache.clear();
        assert(countValidPerm(condition, counts) == countValidPermCached(condition, counts, Record()));
    }

    return totalArrangements;
}


u64 part2(const input_t &in) {
    u64 totalArrangements = 0;
    for (const auto &line : in) {
        cache.clear();
        std::string condition;
        std::vector<u32> counts;
        getRecordAndCounts(line, condition, counts);

        std::string condition_unfolded = condition;
        std::vector<u32> counts_unfolded = counts;
        for (usize i = 0; i < 4; ++i) {
            condition_unfolded += '?' + condition;
            for (u32 el : counts) counts_unfolded.emplace_back(el);
        }

        u64 arr = countValidPermCached(condition_unfolded, counts_unfolded, Record(0, 0, 0));
        totalArrangements += arr;
        debug_println("{} - {}", line, arr);
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
    
    auto res2 = part2(lines);
    std::cout << "-----PART 2-----\n";
    std::cout << "Total arrangements = " << res2 << std::endl;

    return 0;
}