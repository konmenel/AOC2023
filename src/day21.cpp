#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <format>
#include <vector>
#include <string>
#include <numeric>
#include <algorithm>
#include <complex>
#include <array>
#include <deque>

#include "robin_hood.h"


#ifdef DEBUG
    #define debug_println(fmt, ...) std::cout << std::format((fmt), ##__VA_ARGS__) << std::endl
    #define debug_print(fmt, ...) std::cout << std::format((fmt), ##__VA_ARGS__)
#else
    #define debug_println(fmt, ...)
    #define debug_print(fmt, ...)
#endif


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


using Pos=std::complex<i32>;
using Dir=std::complex<i32>;
constexpr Dir NORTH( 0,-1);
constexpr Dir SOUTH( 0, 1);
constexpr Dir EAST ( 1, 0);
constexpr Dir WEST (-1, 0);
constexpr std::array<Dir, 4> ALL_DIRS{NORTH, SOUTH, EAST, WEST};


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


enum class Tile {
    Unreachable = 0,
    Reachable,
    Rock
};


Tile tileFromChar(char c) {
    switch (c)
    {
    case '.':
        return  Tile::Unreachable;
    case '#':
        return Tile::Rock;
    
    case 'O':
    case 'S':
        return Tile::Reachable;

    default:
        debug_println("{}:{} Unsupported character: {}", __FILE__, __LINE__, c);
        return Tile::Unreachable;
    }
}


char tileToChar(Tile t) {
    switch (t)
    {
    case Tile::Unreachable:
        return '.';
    case Tile::Reachable:
        return 'O';
    case Tile::Rock:
        return '#';

    default:
        debug_println("{}:{} Unhandle tile: {}", __FILE__, __LINE__, static_cast<i32>(t));
        return 0;
    }
}


std::ostream& operator<<(std::ostream &os, Tile t) {
    os << tileToChar(t);
    return os;
}


template<typename T = Tile> 
class Grid {
public:
    usize rows;
    usize cols;

    Grid(usize rows_, usize cols_, const T &value) : rows(rows_), cols(cols_), m_data(rows*cols, value) {}
    Grid(usize rows_, usize cols_, i32 value) : rows(rows_), cols(cols_), m_data(rows*cols, static_cast<Tile>(value)) {}

    Grid<T>& operator=(const Grid<T> &g) {
        rows = g.rows;
        cols = g.cols;
        m_data = g.m_data;
        return *this;
    }

    std::vector<Pos> getPointsAtRadius(Pos center,i32 radius) {
        if (radius == 0)
            return {center};

        std::vector<Pos> ret;
        for (i32 x = radius, y = 0; x >= 0; --x, ++y) {
            if (y == 0) {
                if (!isOutOfBound(center + Pos(x, 0))) ret.emplace_back(center + Pos(x, 0));
                if (!isOutOfBound(center - Pos(x, 0))) ret.emplace_back(center - Pos(x, 0));
                continue;
            }

            if (x == 0) {
                if (!isOutOfBound(center + Pos(0, y))) ret.emplace_back(center + Pos(0, y));
                if (!isOutOfBound(center - Pos(0, y))) ret.emplace_back(center - Pos(0, y));
                continue;
            }
            
            const std::array<Pos,4> all_points{ center + Pos(x, y),
                                                center + Pos(-x, y),
                                                center + Pos(x, -y),
                                                center + Pos(-x, -y)};
            for (const Pos &p : all_points) {
                if (!isOutOfBound(p)) ret.emplace_back(p);
            }
        }
        return ret;
    }

    i32 count(const T& t) const {
        i32 count = 0;
        for (usize y = 0; y < rows; y++) {
            for (usize x = 0; x < cols; x++) {
                if (at(x, y) == t) count++;
            }
        }
        return count;
    }

    T& at(usize x, usize y) {
        if (x >= cols) {
            debug_println("x {} exceeds total number of columns({})!", x, cols-1);
        } else if (y >= rows) {
            debug_println("y {} exceeds total number of rows({})!", y, rows-1);
        }
        return m_data[y*cols + x];
    }

    const T& at(usize x, usize y) const {
        if (x >= cols) {
            debug_println("x {} exceeds total number of columns({})!", x, cols-1);
        } else if (y >= rows) {
            debug_println("y {} exceeds total number of rows({})!", y, rows-1);
        }
        return m_data[y*cols + x];
    }

    T& operator()(usize x, usize y) { return at(x, y); }

    T& operator()(Pos pos) { return at(pos.real(), pos.imag()); }

    const T& operator()(usize x, usize y) const { return at(x, y); }

    const T& operator()(Pos pos) const { return at(pos.real(), pos.imag()); }

    bool operator==(const Grid<T> &rhs) const {
        if (rhs.cols != cols || rhs.rows != rows)
            return false;

        for (usize y = 0; y < rows; y++) {
            for (usize x = 0; x < cols; x++) {
                if (at(x, y) != rhs(x, y))
                    return false;
            }
        }
        return true;
    }

    inline bool isOutOfBound(i32 x, i32 y) const {
        return x < 0 || y < 0 || x >= (i32)cols || y >= (i32)rows;
    }

    bool isOutOfBound(const Pos &pos) const {
        return isOutOfBound(pos.real(), pos.imag());
    }

    std::string toString() const {
        std::stringstream ss;
        for (size_t y = 0; y < rows; y++) {
            for (size_t x = 0; x < cols; x++) {
                ss << at(x, y);
            }
            ss << "\n";
        }
        return ss.str();
    }

    friend std::ostream& operator<<(std::ostream& os, const Grid<T>& grid) {
        os << grid.toString();
        return os;
    }

private:
    std::vector<T> m_data;
};


Grid<Tile> parseGrid(const input_t &in) {
    Grid<> grid(in.size(), in[0].size(), Tile::Unreachable);
    for (usize y = 0; y < grid.rows; ++y) {
        for (usize x = 0; x < grid.cols; ++x) {
            grid(x, y) = tileFromChar(in[y][x]);
        }
    }
    return grid;
}


void walkGrid(Grid<> &grid, const i32 max_steps, Pos pos) {
    std::deque<Pos> queue{pos};
    for (i32 step = 0; step < max_steps; step++) {
        usize old_size = queue.size();
        for (usize i = 0; i < old_size; i++) {
            Pos p = queue.front();
            queue.pop_front();
            grid(p) = Tile::Unreachable;

            for (const auto &n : ALL_DIRS) {
                Pos np = p + n;
                if (grid.isOutOfBound(np) || grid(np) != Tile::Unreachable) {
                    continue;
                }
                queue.push_back(np);
                grid(np) = Tile::Reachable;
            }
        }
    }
}


i64 part1(const input_t &in) {
    #ifdef DEBUG
    const i32 N = 6;
    #else
    const i32 N = 64;
    #endif
    Grid<> grid = parseGrid(in);
    debug_println("Starting grid:\n{}", grid.toString());
    Pos start(0);
    for (usize y = 0, run = 1; run && y < grid.rows; ++y) {
        for (usize x = 0; x < grid.cols; ++x) {
            if (grid(x, y) == Tile::Reachable) {
                start = Pos(x, y);
                run = 0;
                break;
            }
        }
    }
    walkGrid(grid, N, start);
    debug_println("After {} steps:\n{}", N, grid.toString());
    return grid.count(Tile::Reachable);
}


// struct State{
//     Pos p;
//     bool isDiv2;

//     bool operator==(const State &rhs) const {
//         return p == rhs.p && isDiv2 == rhs.isDiv2;
//     }
// };


// template<>
// struct std::hash<State> {
//     inline usize operator()(const State &s) const noexcept {
//         return ((usize)s.p.real() << 48) + ((usize)s.p.imag() << 16) + (usize)s.isDiv2;
//     }
// };


// robin_hood::unordered_map<State, i64> cache{};


// i64 walkGridCached(Grid<> &grid, const i64 max_steps, Pos pos, const Grid<> &repeating_grid, i64 step=0) {
//     if (cache.contains(State{pos, max_steps % 2 == step % 2})
//      && cache[State{pos, max_steps % 2 == step % 2}] + step < max_steps) {
//         return cache[State{pos, max_steps % 2 == step % 2}];
//     }

//     i64 ret = 0;
//     Pos start = pos;
//     Grid<> gridold = grid;
//     Grid<> gridoldold = grid;

//     std::deque<Pos> queue{pos};
//     for (; step < max_steps; step++) {
//         usize old_size = queue.size();
//         for (usize i = 0; i < old_size; i++) {
//             Pos p = queue.front();
//             queue.pop_front();
//             grid(p) = Tile::Unreachable;

//             for (const auto &n : ALL_DIRS) {
//                 Pos np = p + n;
//                 if (grid.isOutOfBound(np)) {
//                     Grid<> grid2 = repeating_grid;
//                     i32 x = np.real();
//                     i32 y = np.imag();
//                     if      (x < 0)               x = grid.cols - 1;
//                     else if (x >= (i32)grid.cols) x = 0;
//                     if      (y < 0)               y = grid.rows - 1;
//                     else if (y >= (i32)grid.rows) y = 0;
//                     np = Pos(x, y);
//                     grid2(np) = Tile::Reachable;
//                     ret += walkGridCached(grid2, max_steps, np, repeating_grid, step);
//                     continue;
//                 } else if (grid(np) != Tile::Unreachable) {
//                     continue;
//                 }

//                 queue.push_back(np);
//                 grid(np) = Tile::Reachable;
//             }
//         }
//         if (grid == gridoldold && (max_steps % 2 == step % 2) && step > 2) {
//             std::cout << step << std::endl;
//             break;
//         }
//         gridoldold = gridold;
//         gridold = grid;
//     }
//     i64 count = grid.count(Tile::Reachable);
//     cache[State{start, (max_steps % 2 == step % 2)}] = count;
//     ret += count;
//     return ret;
// }


// i64 part2(const input_t &in) {
//     #ifdef DEBUG
//     const i64 N = 500;
//     #else
//     const i64 N = 26501365;
//     #endif
//     Grid<> grid = parseGrid(in);
//     Pos start(0);
//     for (usize y = 0, run = 1; run && y < grid.rows; ++y) {
//         for (usize x = 0; x < grid.cols; ++x) {
//             if (grid(x, y) == Tile::Reachable) {
//                 start = Pos(x, y);
//                 run = 0;
//                 break;
//             }
//         }
//     }
//     Grid<> repeating_grid = grid;
//     repeating_grid(start) = Tile::Unreachable;
//     walkGridCached(grid, N, start, repeating_grid);
//     debug_println("After {} steps:\n{}", N, grid.toString());

//     return grid.count(Tile::Reachable);
// }


int main(int argc, char *argv[]) {
    if (argc < 2) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }
    input_t lines = parseInputs(argv[1]);

    auto res1 = part1(lines);
    std::cout << "-----PART 1-----\n";
    std::cout << "Number of reachable plots = " << res1 << std::endl;
    
    // auto res2 = part2(lines);
    // std::cout << "-----PART 2-----\n";
    // std::cout << "Number of reachable plots = " << res2 << std::endl;

    return 0;
}