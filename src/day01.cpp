#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <numeric>


typedef std::vector<std::string> input_t;

const uint32_t DAY = 1; 

const char* NUMBER_NAMES[9] = {"one", "two", "three", "four",
                               "five", "six", "seven", "eight", "nine"};

#ifdef DEBUG
#define INPUT_FILE "data/examples/01.txt"
#else
#define INPUT_FILE "data/01.txt"
#endif

input_t parse_inputs(){
    std::ifstream file(INPUT_FILE);
    if (!file) {
        std::cerr << "Unable to open file \"" << INPUT_FILE << "\"!" << std::endl;
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


int get_number_from_name(std::string &substr) {
    for (size_t i = 0; i < 9; ++i) {
        if (substr.find(NUMBER_NAMES[i]) != std::string_view::npos) {
            return i+1;
        }
    }
    return -1;
}


int part1() {
    input_t inputs = parse_inputs();

    std::vector<int32_t> numbers;
    int num;

    for (size_t i = 0; i < inputs.size(); ++i) {
        num = 0;
        for (char c : inputs[i]) {
            if (std::isdigit(c)) {
                num += (c - 0x30) * 10;
                break;
            }
        }

         for (size_t j = inputs[i].size() - 1; j >= 0; --j) {
            char c = inputs[i][j];
            if (std::isdigit(c)) {
                num += c - 0x30;
                break;
            }
        }

        #ifdef DEBUG
            std::cout << num << std::endl;
        #endif
        numbers.emplace_back(num);
    }

    #ifdef DEBUG
        std::cout << "\n";
    #endif

    const int sum = std::accumulate(numbers.begin(), numbers.end(), 0);
    return sum;
}


int part2() {
    input_t inputs = parse_inputs();

    std::vector<int32_t> numbers;
    int num;

    for (size_t i = 0; i < inputs.size(); ++i) {
        num = 0;
        for (size_t j = 0; j < inputs[i].size(); ++j) {
            char c = inputs[i][j];
            if (std::isdigit(c)) {
                num += 10*(c - 0x30);
                break;
            }

            std::string substr = inputs[i].substr(0, j+1);
            int number = get_number_from_name(substr);
            if (number != -1) {
                num += 10*number; 
                break;
            }
        }

        for (size_t j = inputs[i].size()-1; j >= 0; --j) {
            char c = inputs[i][j];
            if (std::isdigit(c)) {
                num += c - 0x30;
                break;
            }

            std::string substr = inputs[i].substr(j, inputs[i].size());
            int number = get_number_from_name(substr);
            if (number != -1) {
                num += number;
                break;
            }
        }

        #ifdef DEBUG
            std::cout << inputs[i] << ": " << num << std::endl;
        #endif
        numbers.emplace_back(num);
    }
    #ifdef DEBUG
        std::cout << "\n";
    #endif

    const int sum = std::accumulate(numbers.begin(), numbers.end(), 0);

    return sum;
}


int main() 
{
    int res1 = part1();
    int res2 = part2();

    std::cout << "---PART 1---\n";
    std::cout << "Sum = " << res1 << std::endl;
    std::cout << "---PART 2---\n";
    std::cout << "Sum = " << res2 << std::endl;

    return 0;
}