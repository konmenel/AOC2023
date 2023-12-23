#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <format>
#include <vector>
#include <string>
#include <numeric>
#include <complex>
#include <queue>
#include <unordered_set>
#include <algorithm>
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

typedef std::complex<i64> Pos;
typedef std::complex<i64> Dir;
typedef std::complex<i64> Rot;

const Dir NORTH( 0,-1);
const Dir SOUTH( 0, 1);
const Dir WEST (-1, 0);
const Dir EAST ( 1, 0);

const std::array<Dir, 4> DIRS = {NORTH, SOUTH, EAST, WEST};


enum Direction {
    NoDir = 0,
    North,
    South,
    West,
    East,
    Unknown
};


Direction dirToInt(const Dir &dir) {
    if (dir == NORTH) return North;
    else if (dir == SOUTH) return South;
    else if (dir == WEST) return West;
    else if (dir == EAST) return East;
    else if (dir == Dir(0,0)) return NoDir;
    debug_println("Unknown Direction ({},{})", dir.real(), dir.imag());
    exit(1);
}


enum class TimerTypes{
    hasher = 0,

};


class Timer {
    public:


};


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


template<typename T = u32>
class Grid {
public:
    usize rows;
    usize cols;

    Grid(usize rows_, usize cols_, const T &value) : rows(rows_), cols(cols_), m_data(rows*cols, value) {}

    Grid(const input_t &in) : rows(in.size()), cols(in[0].size()), m_data(rows*cols, 0) {
        for (usize y = 0; y < in.size(); ++y) {
            for (usize x = 0; x < in[0].size(); ++x) {
                (*this)(x, y) = in[y][x] - '0';
            }
        }
    }

    inline T& operator()(usize x, usize y) {
        if (x >= cols) {
            debug_println("x {} exceeds total number of columns({})!", x, cols-1);
        } else if (y >= rows) {
            debug_println("y {} exceeds total number of rows({})!", y, rows-1);
        }
        return m_data[y*cols + x];
    }

    inline T& operator()(std::complex<usize> pos) {
        return (*this)(pos.real(), pos.imag());
    }

    inline const T& operator()(usize x, usize y) const {
        if (x >= cols) {
            debug_println("x {} exceeds total number of columns({})!", x, cols-1);
        } else if (y >= rows) {
            debug_println("y {} exceeds total number of rows({})!", y, rows-1);
        }
        return m_data[y*cols + x];
    }

    inline const T& operator()(std::complex<usize> pos) const {
        return (*this)(pos.real(), pos.imag());
    }

    std::string printToString() const {
        std::stringstream ss;
        for (size_t y = 0; y < rows; y++) {
            for (size_t x = 0; x < cols; x++) {
                ss << (*this)(x, y) << ' ';
            }
            ss << "\n";
        }
        return ss.str();
    }

    bool isOutOfBound(i32 x, i32 y) const {
        return x < 0 || y < 0 || x >= cols || y >= rows;
    }

    bool isOutOfBound(const Pos &pos) const {
        return isOutOfBound(pos.real(), pos.imag());
    }

private:
    std::vector<T> m_data;
};


template<typename T>
std::ostream& operator<<(std::ostream& os, const Grid<T>& grid) {
    os << grid.printToString();
    return os;
}


struct State {
    u32 hl;   // Heat-loss from start
    Pos pos;
    Dir dir;
    u32 steps;  // number of straight steps

    State() : hl(0), pos(0), dir(0), steps(0) {}
    State(u32 hl_,Pos pos_, Dir dir_, u8 steps_) : hl(hl_), pos(pos_), dir(dir_), steps(steps_) {}

    inline std::size_t operator()(const State &s) const {
        return ((size_t)s.pos.real() << 32) +
               ((size_t)s.pos.imag() << 16) +
               ((size_t)s.steps << 8) +
                (size_t)dirToInt(s.dir);
    }

    inline bool operator>(const State &rhs) const {
        return hl > rhs.hl;
    }

    inline bool operator<(const State &rhs) const {
        return hl < rhs.hl;
    }

    inline bool operator==(const State &rhs) const {
        return pos==rhs.pos && dir==rhs.dir && steps==rhs.steps;
    }
};


u32 shortestPath(Pos start, const Pos &end, const Grid<> &grid, const u32 min_steps=0, const u32 max_steps=3) {
    std::priority_queue<State, std::vector<State>, std::greater<State>> pq;
    pq.emplace(0, start, Dir(0, 0), 0);
    robin_hood::unordered_flat_set<State, State> seen;
    seen.reserve(10 * 1024);
    State state;

    while (!pq.empty()) {
        state = pq.top();
        pq.pop();
        if (state.pos == end && state.steps >= min_steps) return state.hl;

        if (seen.contains(state)) continue;
        seen.emplace(state);
        
        if (state.steps < max_steps && state.dir != Dir(0, 0)) {
            Pos npos = state.pos + state.dir;
            if (!grid.isOutOfBound(npos))
                pq.emplace(state.hl + grid(npos), npos, state.dir, state.steps+1);
        }

        if (state.steps >= min_steps || state.dir == Dir(0, 0)) {
            for (const Dir &ndir : DIRS) {
                if (ndir != state.dir && ndir != -state.dir) {
                    Pos npos = state.pos + ndir;
                    if (!grid.isOutOfBound(npos))
                        pq.emplace(state.hl + grid(npos), npos, ndir, 1);
                }
            }
        }
        // std::cout << pq.size() << ", " << seen.size() << std::endl;
    }
    return UINT32_MAX;
}



u64 part1(const input_t &in) {
    Grid<> grid(in);
    Pos start(0, 0);
    Pos end(grid.cols-1, grid.rows-1);
    return shortestPath(start, end, grid);
}


u64 part2(const input_t &in) {
    Grid<> grid(in);
    Pos start(0, 0);
    Pos end(grid.cols-1, grid.rows-1);
    return shortestPath(start, end, grid, 4, 10);
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }
    input_t lines = parse_inputs(argv[1]);

    auto res1 = part1(lines);
    std::cout << "-----PART 1-----\n";
    std::cout << "Total heat loss = " << res1 << std::endl;
    
    auto res2 = part2(lines);
    std::cout << "-----PART 2-----\n";
    std::cout << "Total heat loss = " << res2 << std::endl;

    return 0;
}