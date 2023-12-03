#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstring>
#include <numeric>
#include <optional>
#include <complex>


typedef std::vector<std::string> input_t;


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
        if (line[line.size()] == '\r') line.pop_back();
        lines.emplace_back(std::move(line));
    }
    
    file.close();
    std::cout << std::endl;
    return lines;
}


bool check_neighbors(int px, int py, const input_t &in) {
    for (int i = std::max(px-1, 0); i <= px + 1 && i < in.size() ; i++) {
        for (int j = std::max(py-1, 0); j <= py + 1 && j < in[i].size(); j++) {
            char c = in[i][j];
            if (c != '.' && !std::isdigit(c)) {
                return true;
            } 
        }
    }
    return false;
}


int part1(const input_t &in) {
    std::vector<int> valid_parts;
    char buf[64];
    size_t idx = 0;
    bool valid_part = false;
    for (int i = 0; i < in.size(); ++i) {
        const std::string &line = in[i];

        if (idx != 0 && valid_part) {
            buf[idx] = '\0';
            valid_parts.emplace_back(std::stoi(buf));
            valid_part = false;
        }
        memset(buf, 0xff, 64);
        idx = 0;
        for (int j = 0; j < line.size(); ++j) {
            char c = line[j];
            if (std::isdigit(c)) {
                buf[idx++] = c;
                if (!valid_part) 
                    valid_part = check_neighbors(i, j, in);
                continue;
            }
            if (idx != 0 && valid_part) {
                buf[idx] = '\0';
                valid_parts.emplace_back(std::stoi(buf));
                valid_part = false;
            }
            idx = 0;
            memset(buf, 0xff, 64);
        }
    }

    return std::accumulate(valid_parts.begin(), valid_parts.end(), 0);
}


int get_num(int py, int &px, const input_t &in) {
    char num[4] = {0x30, 0x30, 0x30, 0};
    int p1 = px;
    int p2 = px;

    for (;isdigit(in[py][p1]) && p1 >= 0; p1--){}
    for (;isdigit(in[py][p2]) && p2 < in[0].size(); p2++){}
    
    for (int i = p1+1; i < p2; i++) {
        num[i-p1-1] = in[py][i];
    }

    px = p2;
    num[p2-p1-1] = '\0';
    return std::stoi(num);
}

int get_gear_ratio(int py, int px, const input_t &in) {
    int neighbors[2] = {0, 0};
    size_t nneighbors = 0;

    for (int i = std::max(py-1, 0); i <= py + 1 && i < in.size() ; i++) {
        for (int j = std::max(px-1, 0); j <= px + 1 && j < in[0].size(); j++) {
            char c = in[i][j];
            if (std::isdigit(c)) {
                neighbors[nneighbors++] = get_num(i, j, in);
            } 
        }
    }
    return neighbors[0]*neighbors[1];
}


int part2(const input_t &in) {
    std::vector<int> gear_ratios;
    for (int i = 0; i < in.size(); ++i) {
        const std::string &line = in[i];
        for (int j = 0; j < line.size(); ++j) {
            char c = line[j];
            if (c == '*') {
                int gr = get_gear_ratio(i, j, in);
                if (gr) {
                    gear_ratios.push_back(gr);
                }
            }
        }
    }

    return std::accumulate(gear_ratios.begin(), gear_ratios.end(), 0);
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    input_t lines = parse_inputs(argv[1]);

    int res1 = part1(lines);
    int res2 = part2(lines);

    std::cout << "-----PART 1-----\n";
    std::cout << "Sum = " << res1 << std::endl;
    std::cout << "-----PART 2-----\n";
    std::cout << "Sum = " << res2 << std::endl;

    return 0;
}