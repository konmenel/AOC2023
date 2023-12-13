#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <format>
#include <vector>
#include <string>
#include <numeric>
#include <complex>
#include <unordered_map>
#include <deque>
#include <algorithm>


typedef std::vector<std::string> input_t;
typedef std::complex<uint16_t> Position;

#ifdef DEBUG
    #define debug_println(fmt, ...) std::cout << std::format((fmt), ##__VA_ARGS__) << std::endl
    #define debug_print(fmt, ...) std::cout << std::format((fmt), ##__VA_ARGS__)
#else
    #define debug_println(fmt, ...)
    #define debug_print(fmt, ...)
#endif

#ifdef DEBUG
    #define EXPANSION_FACTOR 10
#else
    #define EXPANSION_FACTOR 1000000
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


enum class Space {
    Empty = 0,
    Galaxy
};

Space getSpaceType(char c) {
    if (c == '.') return Space::Empty;
    else          return Space::Galaxy;
}

char getSpaceChar(Space s) {
    if (s == Space::Empty) return '.';
    else                   return '#';
}

template<typename T = Space> 
class Universe {
public:
    size_t rows;
    size_t cols;

    Universe(size_t rows_, size_t cols_, const T &value) : rows(rows_), cols(cols_), m_data(rows*cols, value) {}


    T& operator()(size_t x, size_t y) {
        #ifdef DEBUG
        if (x >= cols) {
            debug_println("x {} exceeds total number of columns({})!", x, cols-1);
        } else if (y >= rows) {
            debug_println("y {} exceeds total number of rows({})!", y, rows-1);
        }
        #endif
        return m_data[y*cols + x];
    }

    const T& operator()(size_t x, size_t y) const {
        #ifdef DEBUG
        if (x >= cols) {
            debug_println("x {} exceeds total number of columns({})!", x, cols-1);
        } else if (y >= rows) {
            debug_println("y {} exceeds total number of rows({})!", y, rows-1);
        }
        #endif
        return m_data[y*cols + x];
    }

    std::string print_to_string() const {
        std::stringstream ss;
        for (size_t y = 0; y < rows; y++) {
            for (size_t x = 0; x < cols; x++) {
                ss << getSpaceChar((*this)(x, y));
            }
            ss << "\n";
        }
        return ss.str();
    }

private:
    std::vector<T> m_data;
};


template<> 
class Universe<bool> {
public:
    size_t rows;
    size_t cols;

    Universe(size_t rows_, size_t cols_, bool value) : rows(rows_), cols(cols_), m_data(rows*cols, value) {}


    std::vector<bool>::reference operator()(size_t x, size_t y) {
        #ifdef DEBUG
        if (x >= cols) {
            debug_println("x {} exceeds total number of columns({})!", x, cols-1);
        } else if (y >= rows) {
            debug_println("y {} exceeds total number of rows({})!", y, rows-1);
        }
        #endif
        return m_data[y*cols + x];
    }

    std::vector<bool>::const_reference operator()(size_t x, size_t y) const {
        #ifdef DEBUG
        if (x >= cols) {
            debug_println("x {} exceeds total number of columns({})!", x, cols-1);
        } else if (y >= rows) {
            debug_println("y {} exceeds total number of rows({})!", y, rows-1);
        }
        #endif
        return m_data[y*cols + x];
    }

    std::string print_to_string() const {
        std::stringstream ss;
        for (size_t y = 0; y < rows; y++) {
            for (size_t x = 0; x < cols; x++) {
                ss << (*this)(x, y);
            }
            ss << "\n";
        }
        return ss.str();
    }

private:
    std::vector<bool> m_data;
};


int64_t nchoosek(int64_t n, int64_t k) {
    int64_t num = n;
    int64_t den = 1;
    for (int64_t i = 2; i <= k; den *= i, num *= n+1-i, i++) {}
    return num / den;
}


std::vector<size_t> findEmptyRows(const input_t &in) {
    std::vector<size_t> empty_rows;
    debug_print("Empty rows=");
    //Find expanted rows
    for (auto it = in.begin(); it < in.end(); it++) {
        if ((*it) == std::string(in[0].size(), '.')) {
            empty_rows.emplace_back(it - in.begin());
            debug_print("{},", it - in.begin());
        }
    }
    return empty_rows; 
}


std::vector<size_t> findEmptyCols(const input_t &in) {
    std::vector<size_t> empty_cols;
    debug_print("\nEmpty cols=");
    for (size_t y = 0; y < in[0].size(); y++) {
        bool isempty = true;
        for (size_t x = 0; x < in.size() && isempty; x++) {
            isempty = isempty && in[x][y] == '.';
        }
        if (isempty) {
            empty_cols.emplace_back(y);
            debug_print("{},", y);
        }
    }
    debug_println("");
    return empty_cols;
}


Universe<> expandUniverse(const input_t &in) {
    std::vector<size_t> empty_rows = findEmptyRows(in);

    std::vector<size_t> empty_cols = findEmptyCols(in);

    Universe<> uni(in.size() + empty_rows.size(), in[0].size() + empty_cols.size(), Space::Empty);

    for (size_t uy = 0, j = 0; uy < uni.rows; uy++, j++) {
        for (size_t ux = 0, i = 0; ux < uni.cols; ux++, i++) {
            if (std::find(empty_cols.begin(), empty_cols.end(), i) != empty_cols.end()) {
                ux++;
                continue;
            }
            if (std::find(empty_rows.begin(), empty_rows.end(), j) != empty_rows.end()) {
                uy++;
                break;
            }
            uni(ux, uy) = getSpaceType(in[j][i]);
        }
    }
    debug_print("{}", uni.print_to_string());
    return uni;
}


uint32_t complex2uint(Position c) {
    return (static_cast<uint32_t>(c.real()) << 16) | c.imag();
}


Position uint2complex(uint32_t n) {
    return Position(n >> 16, n & 0xffff);
}


uint64_t encodePair(uint32_t a, uint32_t b) {
    if (a > b) {
        return (static_cast<uint64_t>(a) << 32) | b;
    }

    return (static_cast<uint64_t>(b) << 32) | a;
}


inline uint64_t encodePair(const Position &a, const Position &b) { 
    return encodePair(complex2uint(a), complex2uint(b));
}


void decodePair(uint64_t pair, Position &a, Position &b) {
    const uint32_t paira = pair >> 32;
    const uint32_t pairb = pair & 0xffffffff;
    a = uint2complex(paira);
    b = uint2complex(pairb);
}


std::vector<Position> findNeighbors(Position pos, const Universe<> &uni) {
    const uint16_t x = pos.real();
    const uint16_t y = pos.imag();
    std:: vector<Position> ret;
    ret.reserve(4);

    const Position NORTH( 0,-1);
    const Position SOUTH( 0, 1);
    const Position WEST (-1, 0);
    const Position EAST ( 1, 0);

    if (x > 0)          ret.emplace_back(pos + WEST);
    if (x < uni.cols-1) ret.emplace_back(pos + EAST);
    if (y > 0)          ret.emplace_back(pos + NORTH);
    if (y < uni.rows-1) ret.emplace_back(pos + SOUTH);
    return ret;
}


Position minDistNode(const Universe<uint32_t> &dist, const Universe<bool> &visited) {
    uint32_t min = UINT32_MAX;
    Position p = UINT16_MAX;
    for  (uint32_t y = 0; y < dist.rows; y++) {
        for (uint32_t x = 0; x < dist.cols; x++) {
            if (!visited(x, y) && dist(x, y) < min) {
                min = dist(x, y);
                p = Position(x, y);
            }
        }
    }
    return p;
}


uint32_t dijkstra(Position start, Position dest, const Universe<> &uni) {
    Position pos = start;
    Universe<bool> visited(uni.rows, uni.cols, false);
    Universe<uint32_t> dist(uni.rows, uni.cols, UINT32_MAX);

    dist(start.real(), start.imag()) = 0;

    while (!visited(dest.real(), dest.imag())) {
        Position pos = minDistNode(dist, visited);
        const uint16_t &px = pos.real();
        const uint16_t &py = pos.imag();
        visited(px, py) = true;

        std::vector<Position> neighbors = findNeighbors(pos, uni);
        for (const auto& n : neighbors) {
            const int16_t &nx = n.real();
            const int16_t &ny = n.imag();
            if (!visited(nx, ny) || dist(nx, ny) < dist(px, py) + 1) {
                dist(nx, ny) = dist(px, py) + 1;
            } 
        }
    }
    return dist(dest.real(), dest.imag());
}


uint32_t calculateDistance(uint64_t pair, const Universe<> &uni) {
    Position start;
    Position dest;
    decodePair(pair, start, dest);
    std::unordered_map<uint32_t, int64_t> visited{};
    return dijkstra(start, dest, uni);
}


void findPairsAndDistance(uint32_t galaxy, const std::vector<uint32_t> &galaxies, std::unordered_map<uint64_t, int64_t>& distances) {
    /* I AM AN IDIOT YOU JUST NEED TO ADD THE dx AND dy BETWEEN THE PAIRS!!!
    dijkstra implementation for nothing!!!

    int64_t npairs = nchoosek(galaxies.size(), 2);
    for (const auto &other_galaxy : galaxies) {
        if (galaxy == other_galaxy) continue;
        uint64_t pair = encodePair(galaxy, other_galaxy);
        
        if (distances.contains(pair)) continue;
        distances[pair] = calculateDistance(pair, uni);
        
        const float percent = static_cast<float>(distances.size()) / npairs * 100;
        std::cout << std::format("\r{0:5}/{1} -- {2:2.1f}%", distances.size(), npairs, percent) << std::flush;
    }
    std::cout << std::endl;
    */

    for (const auto &other_galaxy : galaxies) {
        const uint64_t pair = encodePair(galaxy, other_galaxy);
        if (galaxy == other_galaxy) continue;        
        if (distances.contains(pair)) continue;
        
        const Position pos1 = uint2complex(galaxy);
        const Position pos2 = uint2complex(other_galaxy);
        const int32_t dx = std::abs(static_cast<int32_t>(pos1.real() - static_cast<int32_t>(pos2.real())));
        const int32_t dy = std::abs(static_cast<int32_t>(pos1.imag() - static_cast<int32_t>(pos2.imag())));
        
        distances[pair] = dx + dy;
    }
}


uint64_t part1(const input_t &in) {
    Universe uni = expandUniverse(in);

    std::vector<uint32_t> galaxies;
    for (uint16_t y = 0; y < uni.rows; y++) {
        for (uint16_t x = 0; x < uni.cols; x++) {
            if (uni(x, y) != Space::Galaxy) {
                continue;
            }
            galaxies.emplace_back(complex2uint(std::complex(x, y)));
        }
    }
    std::cout << "Found " << galaxies.size() << " galaxies. Number of pairs: " << nchoosek(galaxies.size(), 2) << std::endl;

    // A pair is combined into a long and used as key. Value is the distance to be calculated.
    std::unordered_map<uint64_t, int64_t> distances;
    for (const auto& galaxy : galaxies) {
        findPairsAndDistance(galaxy, galaxies, distances);
    }
    int64_t sum = 0;
    for (const auto &kv : distances) {
        Position a;
        Position b;
        decodePair(kv.first, a, b);
        debug_println("Pair ({}, {}) ({}, {}): distance = {}", a.real(), a.imag(), b.real(), b.imag(), kv.second);
        sum += kv.second;
    }

    return sum;
}


int64_t numOfEmptyBetween(size_t col_or_row_min, size_t col_or_row_max, std::vector<size_t> empty_cols_or_rows) {
    int64_t sum = 0;
    for (const auto &rc : empty_cols_or_rows) {
        if (rc > col_or_row_max) break;
        if (rc > col_or_row_min) sum++;
    }
    return sum;
}

void expandAndGetDist(uint32_t galaxy, const std::vector<uint32_t> &galaxies,
    std::unordered_map<uint64_t, int64_t>& distances,
    const std::vector<size_t> &empty_rows, const std::vector<size_t> &empty_cols) {
    for (const auto &other_galaxy : galaxies) {
        const uint64_t pair = encodePair(galaxy, other_galaxy);
        if (galaxy == other_galaxy) continue;        
        if (distances.contains(pair)) continue;
        
        const Position pos1 = uint2complex(galaxy);
        const Position pos2 = uint2complex(other_galaxy);
        uint16_t min_col = std::min(pos1.real(), pos2.real());
        uint16_t max_col = std::max(pos1.real(), pos2.real());
        uint16_t min_row = std::min(pos1.imag(), pos2.imag());
        uint16_t max_row = std::max(pos1.imag(), pos2.imag());
        const int64_t dx = std::abs(static_cast<int64_t>(pos1.real() - static_cast<int64_t>(pos2.real()))) 
             + numOfEmptyBetween(min_col, max_col, empty_cols) * (EXPANSION_FACTOR - 1);
        const int64_t dy = std::abs(static_cast<int64_t>(pos1.imag() - static_cast<int64_t>(pos2.imag())))
             + numOfEmptyBetween(min_row, max_row, empty_rows) * (EXPANSION_FACTOR - 1);

        distances[pair] = dx + dy;
    }
}


uint64_t part2(const input_t &in) {
    const size_t rows = in.size();
    const size_t cols = in[0].size();
    std::vector<size_t> empty_rows = findEmptyRows(in);
    std::vector<size_t> empty_cols = findEmptyCols(in);

    std::vector<uint32_t> galaxies;
    for (uint16_t y = 0; y < rows; y++) {
        for (uint16_t x = 0; x < cols; x++) {
            if (in[y][x] != '#') continue;
            galaxies.emplace_back(complex2uint(std::complex(x, y)));
        }
    }

    // A pair is combined into a long and used as key. Value is the distance to be calculated.
    std::unordered_map<uint64_t, int64_t> distances;
    for (const auto& galaxy : galaxies) {
        expandAndGetDist(galaxy, galaxies, distances, empty_rows, empty_cols);
    }

    int64_t sum = 0;
    for (const auto &kv : distances) {
        Position a;
        Position b;
        decodePair(kv.first, a, b);
        debug_println("Pair ({}, {}) ({}, {}): distance = {}", a.real(), a.imag(), b.real(), b.imag(), kv.second);
        sum += kv.second;
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
    std::cout << "Sum of distances = " << res1 << std::endl;
    
    auto res2 = part2(lines);
    std::cout << "-----PART 2-----\n";
    std::cout << "Sum of distances = " << res2 << std::endl;

    return 0;
}