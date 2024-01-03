#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <format>
#include <vector>
#include <string>
#include <complex>
#include <numeric>
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

typedef std::complex<i64> Pos;
typedef std::complex<i64> Dir;
typedef std::complex<i64> Rot;

constexpr Dir NORTH( 0,-1);
constexpr Dir SOUTH( 0, 1);
constexpr Dir WEST (-1, 0);
constexpr Dir EAST ( 1, 0);

const std::array<Dir, 4> DIRS = {NORTH, SOUTH, EAST, WEST};

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



Dir dirFromChar(char c) {
    switch (c)
    {
    case 'U':
        return NORTH;
    case 'D':
        return SOUTH;
    case 'L':
        return WEST;
    case 'R':
        return EAST;

    default:
        debug_println("Unknown character \"{}\"", c);
        return 0;
    }
}


std::vector<Pos> digOutline(const input_t &instrs) {
    Pos pos(1000, 1000);
    std::vector<Pos> vertices{pos};


    for (const std::string & instr : instrs) {
        std::istringstream iss(instr);
        i64 steps;
        char direction;
        iss >> direction;
        iss >> steps;

        pos += steps * dirFromChar(direction);
        vertices.push_back(pos);
    }
    return vertices;
}


i64 perimeter(const std::vector<Pos> &outline) {
    i64 peri = 0;
    for (usize i = 0; i < outline.size()-1; ++i) {
        i64 x1 = outline[i].real();
        i64 y1 = outline[i].imag();
        i64 x2 = outline[i+1].real();
        i64 y2 = outline[i+1].imag();
        peri += std::abs(x2 - x1 + y2 - y1);
    }
    return peri;
}

i64 areaDouble(const std::vector<Pos> &outline) {
    // https://en.wikipedia.org/wiki/Shoelace_formula
    // https://en.wikipedia.org/wiki/Pick%27s_theorem
    i64 area = 0;

    for (usize i = 0; i < outline.size()-1; ++i) {
        i64 x1 = outline[i].real();
        i64 y1 = outline[i].imag();
        i64 x2 = outline[i+1].real();
        i64 y2 = outline[i+1].imag();
        area += (y1 + y2)*(x1 - x2);
    }
    return area;
}


u64 part1(const input_t &in) {
    std::vector<Pos> vertices = digOutline(in);
    return (areaDouble(vertices) + perimeter(vertices))/2 + 1;
}


input_t fixInstructions(const input_t &in) {
    input_t ret;
    ret.reserve(in.size());

    for (const auto &line : in) {
        std::stringstream ss(line);
        std::string hex;
        i32 steps;
        char direction;
        std::getline(ss, hex, '#');
        std::getline(ss, hex, ')');

        ss.str(hex.substr(0, 5));
        ss.clear();
        ss << std::hex;
        ss >> steps;

        if (hex[5] == '0') direction = 'R';
        else if (hex[5] == '1') direction = 'D';
        else if (hex[5] == '2') direction = 'L';
        else direction = 'U';
        
        ss.str("");
        ss.clear();
        ss << std::dec << direction << ' ' << steps;
        debug_println("{}", ss.str());
        ret.push_back(ss.str());
    }
    return ret;
}


u64 part2(const input_t &in) {
    input_t instrs = fixInstructions(in);

    std::vector<Pos> vertices = digOutline(instrs);
    return (areaDouble(vertices) + perimeter(vertices))/2 + 1;
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }
    input_t lines = parse_inputs(argv[1]);

    auto res1 = part1(lines);
    std::cout << "-----PART 1-----\n";
    std::cout << "Cubic meters of lava = " << res1 << std::endl;
    
    auto res2 = part2(lines);
    std::cout << "-----PART 2-----\n";
    std::cout << "Cubic meters of lava = " << res2 << std::endl;

    return 0;
}