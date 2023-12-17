#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <format>
#include <vector>
#include <string>
#include <numeric>
#include <algorithm>


typedef std::vector<std::string> input_t;
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


u64 part1(const input_t &in) {

    return -1;
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }
    input_t lines = parse_inputs(argv[1]);

    auto res1 = part1(lines);
    // std::cout << "-----PART 1-----\n";
    // std::cout << "*TEXT 1 HERE*" << res1 << std::endl;
    
    // auto res2 = part2(lines);
    // std::cout << "-----PART 2-----\n";
    // std::cout << "*TEXT 2 HERE*" << res2 << std::endl;

    return 0;
}