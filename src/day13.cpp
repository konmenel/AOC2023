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
typedef std::vector<std::string> Grid;
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;
typedef size_t   usize;


#ifdef _DEBUG
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


enum class Axis {
    X=0,
    Y
};


bool isReflection(const Grid &grid, usize row_col, Axis axis) {
    if (axis == Axis::X) {
        if (row_col < 0 || row_col >= grid[0].size() - 1) return false;
        
        for (i32 left = row_col, right = row_col + 1; left >= 0 && right < grid[0].size(); --left, ++right) {
            for (const auto &line : grid) {
                if (line[left] != line[right]) return false;
            }
        }
        return true;
    } else {
        if (row_col < 0 || row_col >= grid.size() - 1) return false;
        
        for (i32 top = row_col, bot = row_col + 1; top >= 0 && bot < grid.size(); --top, ++bot) {
            for (usize i = 0; i < grid[0].size(); ++i) {
                if (grid[top][i] != grid[bot][i]) return false;
            }
        }
        return true;
    }
}


usize getReflectionLineVer(const Grid &grid, i32 skip=-1) {
    for (usize i = 0; i < grid[0].size() - 1; ++i) {
        if (isReflection(grid, i, Axis::X) && i != skip) {
            return i;
        }
    }

    return -1;
}


usize getReflectionLineHor(const Grid &grid, i32 skip=-1) {
    for (usize i = 0; i < grid.size() - 1; ++i) {
        if (isReflection(grid, i, Axis::Y) && i != skip) {
            return i;
        }
    }
    
    return -1;
}


std::vector<Grid> parseGrids(const input_t &in) {
    std::vector<Grid> ret;
    ret.emplace_back();

    for (const auto &line : in) {
        if (line.empty()) {
            ret.emplace_back();
            continue;
        }
        ret.rbegin()->emplace_back(line);
    }
    return ret;
}


u64 part1(const input_t &in) {
    std::vector<Grid> grids = parseGrids(in);
    
    u64 sum = 0;
    for (const auto &grid : grids) {
        i32 line;
        line = getReflectionLineVer(grid);
        if (line >= 0) { 
            debug_println("Vertical line between {}-{}", line+1, line+2);
            sum += line + 1; 
            continue;
        }

        line = getReflectionLineHor(grid);
        if (line >= 0)  {
            debug_println("Horizontal line between {}-{}", line+1, line+2);
            sum += 100 * (line + 1); 
        }
    }

    return sum;
}


char swapChar(char c) { return ('#' + '.') - c; }


u64 part2(const input_t &in) {
    std::vector<Grid> grids = parseGrids(in);
    
    u64 sum = 0;
    for (auto &grid : grids) {
        i32 line_orig;
        Axis ax = Axis::X;
        i32 line_new;

        line_orig = getReflectionLineVer(grid);
        if (line_orig < 0) {
            line_orig = getReflectionLineHor(grid);
            ax = Axis::Y;
        }
        
        usize x = 0, y = 0;
        while (true) {
            if (x > grid[0].size()) {
                x = 0;
                ++y;
            }
            if (y >= grid.size()) break;
            
            grid[y][x] = swapChar(grid[y][x]);
            
            line_new = getReflectionLineVer(grid, ax == Axis::X ? line_orig : -1);
            if (line_new >= 0) { 
                debug_println("Vertical line between {}-{}", line_new+1, line_new+2);
                sum += line_new + 1;
                break;
            }
            line_new = getReflectionLineHor(grid, ax == Axis::Y ? line_orig : -1);
            if (line_new >= 0)  {
                debug_println("Horizontal line between {}-{}", line_new+1, line_new+2);
                sum += 100 * (line_new + 1);
                break; 
            }

            grid[y][x] = swapChar(grid[y][x]);
            ++x;
        }

    }

    return sum;
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