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

    Universe(size_t rows_, size_t cols_) : rows(rows_), cols(cols_), m_data(rows*cols, Space::Empty) {}


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


int64_t nchoosek(int64_t n, int64_t k) {
    int64_t num = n;
    int64_t den = 1;
    for (int64_t i = 2; i <= k; den *= i, num *= n+1-i, i++) {}
    return num / den;
}


Universe<> expandUniverse(const input_t &in) {
    std::vector<size_t> empty_rows;
    debug_print("Empty rows=");
    //Find expanted rows
    for (auto it = in.begin(); it < in.end(); it++) {
        if ((*it) == std::string(in[0].size(), '.')) {
            empty_rows.emplace_back(it - in.begin());
            debug_print("{},", it - in.begin());
        }
    }

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

    Universe<> uni(in.size() + empty_rows.size(), in[0].size() + empty_cols.size());

    debug_println("{}, {}", uni.rows, uni.cols);

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
            debug_println("{}, {}, {}", ux, uy, in[j][i]);
            uni(ux, uy) = getSpaceType(in[j][i]);
        }
    }
    debug_print("{}", uni.print_to_string());
    return uni;
}


uint64_t part1(const input_t &in) {
    Universe uni = expandUniverse(in);

    return nchoosek(9, 5);
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }
    input_t lines = parse_inputs(argv[1]);

    auto res1 = part1(lines);
    std::cout << "-----PART 1-----\n";
    std::cout << "*TEXT 1 HERE*" << res1 << std::endl;
    
    // auto res2 = part2(lines);
    // std::cout << "-----PART 2-----\n";
    // std::cout << "*TEXT 2 HERE*" << res2 << std::endl;

    return 0;
}