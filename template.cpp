#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <format>
#include <vector>
#include <string>
#include <numeric>
#include <algorithm>

#include "recycles.h"


#ifdef _DEBUG
    #define debug_println(fmt, ...) std::cout << std::format((fmt), ##__VA_ARGS__) << std::endl
    #define debug_print(fmt, ...) std::cout << std::format((fmt), ##__VA_ARGS__)
#else
    #define debug_println(fmt, ...)
    #define debug_print(fmt, ...)
#endif


typedef std::vector<std::string> input_t;

using namespace utils;

void usage(const std::string &program_name) {
    std::cout << "Usage: " << program_name << " <input_file>\n";
}


input_t parseInputs(const std::string &filename) {
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


i64 part1(const input_t &in) {

    return -1;
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }
    input_t lines = parseInputs(argv[1]);

    auto res1 = part1(lines);
    // std::cout << "-----PART 1-----\n";
    // std::cout << "*TEXT HERE*" << res1 << std::endl;
    
    // auto res2 = part2(lines);
    // std::cout << "-----PART 2-----\n";
    // std::cout << "*TEXT HERE*" << res2 << std::endl;

    return 0;
}