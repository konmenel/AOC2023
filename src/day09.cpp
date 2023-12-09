#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <vector>
#include <string>
#include <format>
#include <numeric>
#include <algorithm>


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
        lines.emplace_back(std::move(line));
    }
    
    file.close();
    return lines;
}


typedef std::vector<int64_t> sequence;


bool isAllZero(const sequence &seq) {
    for (auto n : seq) {
        if (n != 0) return false; 
    }
    return true;
}


int64_t part1(const input_t &in) {
    int64_t sum = 0;
    for (const std::string &line : in) {
        std::vector<sequence> seqs;
        
        std::istringstream iss(line);
        int64_t n;
        seqs.emplace_back();
        sequence &seq = *seqs.rbegin();
        while (iss >> n) seq.push_back(n);

        // Walk down
        while (!isAllZero(*seqs.rbegin())) {
            seqs.emplace_back();
            sequence &seq_old = *(seqs.rbegin()+1);
            sequence &seq_new = *seqs.rbegin();
            

            for (auto it = seq_old.begin()+1; it < seq_old.end(); it++) {
                seq_new.emplace_back((*it) - (*(it-1)));
            }
        }

        // Walk up (extrapolate)
        for (auto it = seqs.rbegin()+1; it < seqs.rend(); it++) {
            it->push_back(*(it-1)->rbegin() + *it->rbegin());
        }

        sum += *seqs[0].rbegin();

        #ifdef DEBUG
        for (const auto &s : seqs) {
            for (auto n : s) {
                std::cout << n << ", ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
        #endif
    }

    return sum;
}

int64_t part2(const input_t &in) {
    int64_t sum = 0;
    for (const std::string &line : in) {
        std::vector<sequence> seqs;
        
        std::istringstream iss(line);
        int64_t n;
        seqs.emplace_back();
        sequence &seq = *seqs.rbegin();
        while (iss >> n) seq.push_back(n);

        // Walk down
        while (!isAllZero(*seqs.rbegin())) {
            seqs.emplace_back();
            sequence &seq_old = *(seqs.rbegin()+1);
            sequence &seq_new = *seqs.rbegin();
            

            for (auto it = seq_old.begin()+1; it < seq_old.end(); it++) {
                seq_new.emplace_back((*it) - (*(it-1)));
            }
        }

        // Walk up (extrapolate backwards)
        for (auto it = seqs.rbegin()+1; it < seqs.rend(); it++) {
            it->insert(it->begin(), *it->begin() - *(it-1)->begin());
        }

        sum += *seqs[0].begin();

        #ifdef DEBUG
        for (const auto &s : seqs) {
            for (auto n : s) {
                std::cout << n << ", ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
        #endif
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