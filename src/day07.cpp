#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <numeric>
#include <algorithm>
#include <unordered_map>


typedef std::vector<std::string> input_t;

const char CARD_ORDER[] = {'A', 'K', 'Q', 'J', 'T', '9', '8', '7', '6', '5', '4', '3', '2'};

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

enum class HandType {
    HighCard = 1,
    OnePair = 2,
    TwoPair = 3,
    ThreeOfAKind = 4,
    FullHouse = 5,
    FourOfAKind = 6,
    FiveOfAKind = 7,
};    



HandType getHand(const std::string &hand) {
    HandType ret = HandType::HighCard;

    std::unordered_map<char, uint32_t> hand{};

    for (char c : hand) {

    }

    return ret;
}


int part1(const input_t &in) {

    return -1;
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    input_t lines = parse_inputs(argv[1]);


    // std::cout << "-----PART 1-----\n";
    // std::cout << "*TEXT 1 HERE*" << var_name_1 << std::endl;
    // std::cout << "-----PART 2-----\n";
    // std::cout << "*TEXT 2 HERE*" << var_name_2 << std::endl;

    return 0;
}