#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <format>
#include <vector>
#include <string>
#include <numeric>
#include <complex>
#include <algorithm>


typedef std::vector<std::string> input_t;
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;
typedef std::complex<i32> Pos;


const Pos NORTH = Pos( 0, -1);
const Pos SOUTH = Pos( 0,  1);
const Pos EAST  = Pos( 1,  0);
const Pos WEST  = Pos(-1,  0);



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


bool isAtEdge(const input_t &grid, const Pos &pos) {
    i32 x = pos.real();
    i32 y = pos.imag();
    return y == 0 || x == 0 || x == grid[0].size() - 1 || y == grid.size() - 1;
}

bool isOutOfBound(const input_t &grid, const Pos &pos) {
    i32 x = pos.real();
    i32 y = pos.imag();
    return y < 0 || x < 0 || x >= grid[0].size() || y >= grid.size();
}


void moveRock(input_t &grid, const Pos &pos, Pos dir = NORTH) {
    Pos tmp = pos; 
    i32 x = pos.real();
    i32 y = pos.imag();
    grid[y][x] = '.';

    while (y >= 0 && x >= 0 && x < grid[0].size() && y < grid.size()) {
        tmp += dir;
        if (isOutOfBound(grid, tmp)) {
            grid[y][x] = 'O';
            break;
        }
        if (grid[tmp.imag()][tmp.real()] != '.') {
            grid[y][x] = 'O';
            break;
        }

        x = tmp.real();
        y = tmp.imag();
    }
}


u64 part1(const input_t &in) {
    input_t grid = in;

    debug_println("Original grid:");
    for (const auto &line : grid) {
        debug_println("{}", line);
    }

    for (i32 y = 0; y < grid.size(); ++y) {
        for (i32 x = 0; x < grid[0].size(); ++x){
            if (grid[y][x] == 'O') moveRock(grid, Pos(x, y));
        }
    }

    debug_println("");
    debug_println("Moved grid:");
    for (const auto &line : grid) {
        debug_println("{}", line);
    }

    u64 score = 0;
    for (i32 y = 0; y < grid.size(); ++y) {
        for (i32 x = 0; x < grid[0].size(); ++x){
            if (grid[y][x] == 'O') score += (grid.size() - y); 
        }
    }

    return score;
}


void performCycle(input_t &grid) {
    // North tilt
    for (i32 y = 0; y < grid.size(); ++y) {
        for (i32 x = 0; x < grid[0].size(); ++x){
            if (grid[y][x] == 'O') moveRock(grid, Pos(x, y));
        }
    }

    // West tilt
    for (i32 y = 0; y < grid.size(); ++y) {
        for (i32 x = 0; x < grid[0].size(); ++x){
            if (grid[y][x] == 'O') moveRock(grid, Pos(x, y), WEST);
        }
    }

    // South tilt
    for (i32 y = grid.size()-1; y >= 0; --y) {
        for (i32 x = grid[0].size()-1; x >= 0; --x){
            if (grid[y][x] == 'O') moveRock(grid, Pos(x, y), SOUTH);
        }
    }

    // East tilt
    for (i32 y = grid.size()-1; y >= 0; --y) {
        for (i32 x = grid[0].size()-1; x >= 0; --x){
            if (grid[y][x] == 'O') moveRock(grid, Pos(x, y), EAST);
        }
    }
}


bool operator ==(const input_t &lhs, const input_t &rhs) {
    for (i32 y = 0; y < lhs.size(); ++y) {
        for (i32 x = 0; x < lhs[0].size(); ++x){
            if (lhs[y][x] == rhs[y][x]) return false; 
        }
    }
    return true;
}


bool operator !=(const input_t &lhs, const input_t &rhs) {
    for (i32 y = 0; y < lhs.size(); ++y) {
        for (i32 x = 0; x < lhs[0].size(); ++x){
            if (lhs[y][x] != rhs[y][x]) return true; 
        }
    }
    return false;
}

u64 part2(const input_t &in) {
    std::vector<input_t> grids = {in};
    input_t tmp = in;

    i32 cycle_start = -1;

    debug_println("Original grid:");
    for (const auto &line : grids[0]) {
        debug_println("{}", line);
    }
    
    #ifdef _DEBUG
    i32 i = 0;
    #endif
    while (true) {
        performCycle(tmp);
        
        #ifdef _DEBUG
        debug_println("");
        debug_println("Cycle {}:", ++i);
        for (const auto &line : tmp) {
            debug_println("{}", line);
        }
        #endif

        if (auto it = std::find(grids.begin(), grids.end(), tmp); it == grids.end()) {
            grids.push_back(tmp);
        } else {
            cycle_start = it - grids.begin();
            std::cout << cycle_start << std::endl;
            break;
        }
    }

    u64 no_cycles = 1000000000;
    u64 cycle_len = grids.size() - cycle_start;
    u64 non_repeat_cycles = cycle_start + 1;
    u64 repeat_cycles = no_cycles - non_repeat_cycles;
    const input_t &grid = grids[cycle_start + (repeat_cycles % cycle_len) + 1];

    u64 score = 0;
    for (i32 y = 0; y < grids.rbegin()->size(); ++y) {
        for (i32 x = 0; x <  grid[0].size(); ++x){
            if (grid[y][x] == 'O') score += grid.size() - y; 
        }
    }
    return score;
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }
    input_t lines = parse_inputs(argv[1]);

    auto res1 = part1(lines);
    std::cout << "-----PART 1-----\n";
    std::cout << "Score = " << res1 << std::endl;
    
    auto res2 = part2(lines);
    std::cout << "-----PART 2-----\n";
    std::cout << "Score = " << res2 << std::endl;

    return 0;
}