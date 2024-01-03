#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <format>
#include <vector>
#include <string>
#include <complex>


typedef std::vector<std::string> input_t;
typedef std::complex<int64_t> Position;


constexpr Position NORTH( 0,-1);
constexpr Position SOUTH( 0, 1);
constexpr Position EAST ( 1, 0);
constexpr Position WEST (-1, 0);
constexpr Position NORTHEAST( 1,-1);
constexpr Position NORTHWEST(-1,-1);
constexpr Position SOUTHEAST( 1, 1);
constexpr Position SOUTHWEST(-1, 1);


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


char atPos(const input_t &grid, Position pos) {
    if (pos.real() < 0)               return 0;
    if (pos.real() >= grid[0].size())    return 0;
    if (pos.imag() < 0)               return 0;
    if (pos.imag() >= grid.size()) return 0;
    return grid[pos.imag()][pos.real()];
}


std::complex<int64_t> getStart(const input_t &grid) {
    for (auto ity = grid.begin(); ity < grid.end(); ity++) {
        std::string line = *ity;
        for (auto itx = line.begin(); itx < line.end(); itx++) {
            if ((*itx) == 'S') return std::complex(itx - line.begin(), ity - grid.begin());
        }
    }
    #ifdef _DEBUG
    std::cout << "Unreachable! Couln't find \'S\'!" << std::endl;
    exit(1);
    #endif
    return 0;
}


std::vector<Position> getValidNeighbors(const input_t &grid, Position pos) {
    std::vector<Position> ret;
    Position n;
    char c;

    n = NORTH + pos;
    c = atPos(grid, n);
    if (c && std::string("|7F").find(c) != std::string::npos) ret.push_back(n);

    n = SOUTH + pos;
    c = atPos(grid, n);
    if (c && std::string("|LJ").find(c) != std::string::npos) ret.push_back(n);

    n = EAST + pos;
    c = atPos(grid, n);
    if (c && std::string("-7J").find(c) != std::string::npos) ret.push_back(n);

    n = WEST + pos;
    c = atPos(grid, n);
    if (c && std::string("-FL").find(c) != std::string::npos) ret.push_back(n);
    return ret;
}


Position getDirection(char c, Position d) {
    // d= 0-1j: traveling south->north
    // d= 0+1j: traveling north->south
    // d= 1+0j: traveling west->east
    // d=-1+0j: traveling east->west
    switch (c)
    {
    case '|':
    case '-':
        return Position(1, 0) * d;
    case 'L':
        if (d == SOUTH) return Position( 1, 0);
        if (d == WEST)  return Position( 0,-1);
    case 'F':
        if (d == NORTH) return Position( 1, 0);
        if (d == WEST)  return Position( 0, 1);
    case 'J':
        if (d == EAST)  return Position( 0,-1);
        if (d == SOUTH) return Position(-1, 0);
    case '7':
        if (d == EAST)  return Position( 0, 1);
        if (d == NORTH) return Position(-1, 0);

    default:
        #ifdef _DEBUG
        std::cout << std::format("Unreachable! Symbol \'{}\'!", c) << std::endl;
        exit(1);
        #endif
        return 0;
    }
}


uint64_t part1(const input_t &in) {
    Position pos = getStart(in);
    std::vector<Position> neighbors = getValidNeighbors(in ,pos);
    std::vector<size_t> loop{};

    size_t steps = 0;
    Position dir = neighbors[0] - pos;
    pos += dir;
    char c = atPos(in, pos);
    while (c != 'S') {
        #ifdef _DEBUG
        std::cout << c << dir << pos;
        #endif

        dir = getDirection(c, dir);
        pos += dir;
        c = atPos(in, pos);
        loop.push_back(++steps);

        #ifdef _DEBUG
        std::cout << " -> " << c << dir << pos << std::endl;
        #endif
    }

    return loop[loop.size()/2];
}


enum class Tile{
    Vertical    = 0,
    Corner      = 2,
    None        = 3,
    Horizontal  = 4,
    NotMainLoop = 5,
    Unknown     = 6,
};


Tile getTile(char c) {
    switch (c)
    {
    case '.':
        return Tile::None;
    case '|':
        return Tile::Vertical;
    case '-':
        return Tile::Horizontal;
    case 'S':
    case 'F':
    case '7':
    case 'L':
    case 'J':
        return Tile::Corner;

    default:
        #ifdef _DEBUG
        std::cout << std::format("Unreachable! Symbol \'{}\'!", c) << std::endl;
        exit(1);
        #endif
        return Tile::Unknown;
    }
}


size_t raytrace(const std::vector<std::vector<Tile>> &loop, Position point, const input_t &grid) {
    size_t intersections = 0;
    size_t y = point.imag();

    bool wasempty = true;
    for (int x = point.real(); x < loop[0].size(); x++) {
        if (loop[y][x] == Tile::Vertical){
            intersections++;
        } else if (loop[y][x] == Tile::Corner) {
            char c = atPos(grid, Position(x, y));
            int i = x+1;
            for (; i < loop[0].size() && loop[y][i] == Tile::Horizontal; i++) {}
            if (c == 'F') {
                if (atPos(grid, Position(i, y)) != 'J') intersections++;
            } else if (c == 'L') {
                if (atPos(grid, Position(i, y)) != '7') intersections++;
            } else {
                intersections++;
            }
        }
    }
    return intersections;
}

uint64_t part2(const input_t &in) {
    using namespace std::complex_literals;
    Position pos = getStart(in);
    std::vector<Position> neighbors = getValidNeighbors(in ,pos);

    std::vector<std::vector<Tile>> loop;
    for (size_t i = 0; i < in.size(); i++) {
        loop.push_back(std::vector(in[0].size(), Tile::None));
    }

    Position n = neighbors[0];
    Position dir = n - pos;
    pos += dir;
    char c = atPos(in, pos);
    loop[pos.imag()][pos.real()] = getTile(c);
    while (c != 'S') {
        dir = getDirection(c, dir);
        pos += dir;
        c = atPos(in, pos);
        loop[pos.imag()][pos.real()] = getTile(c);
    }

    uint64_t numPoints = 0;
    for (int64_t y = 0; y < in.size(); y++) {
        for (int64_t x = 0; x < in[0].size(); x++) {
            size_t inter = 0;
            if (loop[y][x] == Tile::None)
                inter = raytrace(loop, Position(x, y), in);
            
            if (inter % 2 != 0) numPoints++;
            #ifdef _DEBUG
            if (loop[y][x] != Tile::None) {
                std::cout << in[y][x];
            } else {
                std::cout << (inter % 2 != 0);
            }
            #endif 
        }
        #ifdef _DEBUG
        std::cout << std::endl;
        #endif 
    }


    #ifdef DEBUG_
    for(auto line : loop) {
        for (auto j : line) {
            std::cout << j;
        }
        std::cout << std::endl;
    }
    #endif
    return numPoints;
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }
    input_t lines = parse_inputs(argv[1]);

    auto res1 = part1(lines);
    std::cout << "-----PART 1-----\n";
    std::cout << "Max steps = " << res1 << std::endl;

    auto res2 = part2(lines);
    std::cout << "-----PART 2-----\n";
    std::cout << "Enclosed area = " << res2 << std::endl;

    return 0;
}