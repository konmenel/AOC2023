#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <format>
#include <vector>
#include <string>
#include <numeric>
#include <complex>
#include <deque>
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
typedef size_t   usize;

typedef std::complex<i32> Pos;
typedef std::complex<i32> Rot;
typedef std::complex<i32> Dir;

const Dir NORTH( 0,-1);
const Dir SOUTH( 0, 1);
const Dir WEST (-1, 0);
const Dir EAST ( 1, 0);

const Rot ACLOCK_ROT(0,  1);        // Counter-clockwise rotation
const Rot CLOCK_ROT (0, -1);        // Clockwise rotation


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


enum class Tile {
    Empty = 0,
    SplitHor,
    SplitVer,
    MirrorFront,    // Forward=slash
    MirrorBack,     // Back-slash
};


char tileToChar(Tile tile) {
    switch (tile) {
    case Tile::SplitVer:
        return '|';
    case Tile::SplitHor:
        return '-';
    case Tile::MirrorFront:
        return '/';
    case Tile::MirrorBack:
        return '\\';
    case Tile::Empty:
        return '.';
    
    default:
        debug_println("Unreachable tile type \"{}\"", static_cast<i32>(tile));
        return 0;
    }
}


std::ostream& operator<<(std::ostream& os, const Tile& tile) {
    os << tileToChar(tile);
    return os;
}


template<typename T = Tile> 
class Grid {
public:
    usize rows;
    usize cols;

    Grid(usize rows_, usize cols_, const T &value) : rows(rows_), cols(cols_), m_data(rows*cols, value) {}


    T& operator()(usize x, usize y) {
        if (x >= cols) {
            debug_println("x {} exceeds total number of columns({})!", x, cols-1);
        } else if (y >= rows) {
            debug_println("y {} exceeds total number of rows({})!", y, rows-1);
        }
        return m_data[y*cols + x];
    }

    T& operator()(std::complex<usize> pos) {
        return (*this)(pos.real(), pos.imag());
    }

    const T& operator()(usize x, usize y) const {
        if (x >= cols) {
            debug_println("x {} exceeds total number of columns({})!", x, cols-1);
        } else if (y >= rows) {
            debug_println("y {} exceeds total number of rows({})!", y, rows-1);
        }
        return m_data[y*cols + x];
    }

    const T& operator()(std::complex<usize> pos) const {
        return (*this)(pos.real(), pos.imag());
    }

    std::string printToString() const {
        std::stringstream ss;
        for (size_t y = 0; y < rows; y++) {
            for (size_t x = 0; x < cols; x++) {
                ss << (*this)(x, y);
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


Grid<> parseGrid(const input_t &in) {
    Grid<> grid(in.size(), in[0].size(), Tile::Empty);
    for (usize y = 0; y < grid.rows; ++y) {
        for (usize x = 0; x < grid.cols; ++x) {
            char c = in[y][x];
            switch (c) {
            case '|':
                grid(x, y) = Tile::SplitVer;
                break;
            case '-':
                grid(x, y) = Tile::SplitHor;
                break;
            case '\\':
                grid(x, y) = Tile::MirrorBack;
                break;
            case '/':
                grid(x, y) = Tile::MirrorFront;
                break;
            
            case '.':
            default:
                break;
            }
        }
    }
    return grid;
}


struct Visits {
    usize times;
    std::vector<Dir> dirs;

    Visits() : times(0), dirs(std::vector<Dir>()) {}
};


void updateBeam(const Pos &pos, Dir &vel, std::deque<Pos> &beams, std::deque<Dir> &beams_vel, const  Grid<> &grid) {
    if (grid.isOutOfBound(pos)) return;

    switch (grid(pos)) {
    case Tile::SplitHor:
        if (vel != WEST && vel != EAST) {
            vel = WEST;
            beams.push_back(pos + EAST);
            beams_vel.push_back(EAST);
        }
        break;
    case Tile::SplitVer:
        if (vel != NORTH && vel != SOUTH) {
            vel = NORTH;
            beams.push_back(pos + SOUTH);
            beams_vel.push_back(SOUTH);
        }
        break;
    case Tile::MirrorBack:
        vel *= vel == WEST || vel == EAST ? ACLOCK_ROT : CLOCK_ROT;
        break;
    case Tile::MirrorFront:
        vel *= vel == WEST || vel == EAST ? CLOCK_ROT : ACLOCK_ROT;
        break;
    
    case Tile::Empty:
    default:
        break;
    }
}


bool isFinished(const Grid<> &grid, const Grid<Visits> &energized_grid, const Pos &pos, const Dir &vel) {
    return grid.isOutOfBound(pos) ||
        std::find(energized_grid(pos).dirs.begin(), energized_grid(pos).dirs.end(), vel) 
        != energized_grid(pos).dirs.end();
}


u64 part1(const input_t &in, const Pos &starting_pos=Pos(0,0), const Dir &starting_vel=EAST) {
    Grid<> grid = parseGrid(in);
    Grid<Visits> energized_grid(grid.rows, grid.cols, Visits());
    std::deque<Pos> beams{starting_pos};
    std::deque<Dir> beams_vel{starting_vel};

    while (!beams.empty()) {
        Pos pos = beams.front();
        beams.pop_front();
        Dir vel = beams_vel.front();
        beams_vel.pop_front();
        updateBeam(pos, vel, beams, beams_vel, grid); 

        while (!isFinished(grid, energized_grid, pos, vel)) {
            if (!grid.isOutOfBound(pos)) {
                energized_grid(pos).times++;
                energized_grid(pos).dirs.push_back(vel);
            }
            pos += vel;
            updateBeam(pos, vel, beams, beams_vel, grid);
        }

        for (usize y = 0; y < energized_grid.rows; ++y) {
            for (usize x = 0; x < energized_grid.cols; ++x) {
                char c;
                if (grid(x, y) != Tile::Empty) {
                    c = tileToChar(grid(x, y));
                } else if (energized_grid(x, y).times == 0) {
                    c = '.';
                } else if (energized_grid(x, y).times == 1) {
                    if (energized_grid(x, y).dirs[0] == WEST) {
                        c = '<';
                    } else if (energized_grid(x, y).dirs[0] == EAST) {
                        c = '>';
                    } else if (energized_grid(x, y).dirs[0] == NORTH) {
                        c = '^';
                    } else {
                        c = 'v';
                    }
                } else {
                    c = 0x30 + energized_grid(x, y).times;
                }
                debug_print("{}", c);
            }
            debug_println("");
        }
        debug_println("");
    }

    u64 sum = 0;
    for (usize y = 0; y < energized_grid.rows; ++y) {
        for (usize x = 0; x < energized_grid.cols; ++x) {
            if (energized_grid(x, y).times > 0) sum++;
        }
    }
    return sum;
}


u64 part2(const input_t &in) {
    u64 max = 0;
    for (usize x = 0; x < in[0].size(); ++x) {
        max = std::max(max, part1(in, Pos(x, 0), SOUTH));
        max = std::max(max, part1(in, Pos(x, in.size()-1), NORTH));
    }
    for (usize y = 0; y < in.size(); ++y) {
        max = std::max(max, part1(in, Pos(0, y), EAST));
        max = std::max(max, part1(in, Pos(in[0].size()-1, y), WEST));
    }
    return max; 
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }
    input_t lines = parse_inputs(argv[1]);

    auto res1 = part1(lines);
    std::cout << "-----PART 1-----\n";
    std::cout << "Total energized tiles = " << res1 << std::endl;
    
    auto res2 = part2(lines);
    std::cout << "-----PART 2-----\n";
    std::cout << "Total energized tiles = " << res2 << std::endl;

    return 0;
}