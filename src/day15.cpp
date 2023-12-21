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
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;
typedef size_t   usize;


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


i32 hash(const std::string &step) {
    i32 current_value = 0;
    for (char c : step) {
        current_value += c;
        current_value *= 17;
        current_value %= 256;
    }
    return current_value;
}


u64 part1(const input_t &in) {
    std::istringstream iss(in[0]);
    std::string step;

    u64 sum = 0;
    while (std::getline(iss, step, ',')) {
        i32 current_value = hash(step);
        sum += current_value;
        debug_println("{} becomes {}", step, current_value);
    }

    return sum;
}


enum class Operation {
    Remove = 0,
    Add,
};


struct Lens {
    std::string label;
    u32 focal_length;
};


bool operator ==(const Lens &lhs, const Lens &rhs) {
    return lhs.label == rhs.label && lhs.focal_length == rhs.focal_length;
}


bool operator !=(const Lens &lhs, const Lens &rhs) {
    return lhs.label != rhs.label || lhs.focal_length != rhs.focal_length;
}


std::vector<Lens>::iterator findLens(std::vector<Lens> &box, const std::string &label) {
    for (auto it = box.begin(); it < box.end(); ++it) {
        if (it->label == label) return it;
    }
    return box.end();
}


u64 part2(const input_t &in) {
    std::istringstream iss(in[0]);
    std::string step;
   std::vector<std::vector<Lens>> hashmap(256, std::vector<Lens>());

    u64 sum = 0;
    while (std::getline(iss, step, ',')) {
        std::istringstream iss_step(step);
        Lens lens;
        Operation op;

        if (isdigit(*step.rbegin())) {
            op = Operation::Add;
            getline(iss_step, lens.label, '=');
            iss_step >> lens.focal_length;
        } else {
            op = Operation::Remove;
            lens.focal_length = 0;
            getline(iss_step, lens.label, '-');
        }

        i32 box = hash(lens.label);

        switch (op) {
        case Operation::Remove:
            if (!hashmap[box].empty()) {
                auto &vec = hashmap[box];
                auto it = findLens(vec, lens.label);
                if (it != vec.end()) vec.erase(it);
            }
            break;
        
        case Operation::Add:
            if (!hashmap[box].empty()) {
                auto &vec = hashmap[box];
                auto it = findLens(vec, lens.label);
                if (it != vec.end()) it->focal_length = lens.focal_length;
                else vec.push_back(lens);
            } else {
                hashmap[box] = std::vector<Lens>{lens};
            }
            break;

        default:
            debug_println("Unreachable Oparation {}", static_cast<i32>(op));
            break;
        }

        debug_println("After \"{}\":", step);
        for (usize i = 0; i < 256; ++i) {
            if (hashmap[i].empty()) continue;
            debug_print("Box {}: ", i);
            for (const auto &lens : hashmap[i]) {
                debug_print("[{} {}] ", lens.label, lens.focal_length);
            }
            debug_println("");
        }
        debug_println("");
    }

    u64 focusing_power = 0;
    for (usize i = 0; i < 256; ++i) {
        if (hashmap[i].empty()) continue;
        
        for (usize j = 0; j < hashmap[i].size(); ++j) {
            focusing_power += (i + 1) * (j + 1) * hashmap[i][j].focal_length;
        }
    }

    return focusing_power;
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }
    input_t lines = parse_inputs(argv[1]);

    auto res1 = part1(lines);
    std::cout << "-----PART 1-----\n";
    std::cout << "Sum = " << res1 << std::endl;
    
    auto res2 = part2(lines);
    std::cout << "-----PART 2-----\n";
    std::cout << "Sum = " << res2 << std::endl;

    return 0;
}