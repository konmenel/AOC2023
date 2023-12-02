#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <numeric>


typedef std::vector<std::string> input_t;

const uint32_t DAY = 1; 

const char* NUMBER_NAMES[10] = {"zero", "one", "two", "three", "four",
                                "five", "six", "seven", "eight", "nine"};
const char* NUMBER_DIGITS = "0123456789";

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
    for (size_t i = 1; i <= 9; ++i) {
        if (substr.find(NUMBER_NAMES[i]) != std::string_view::npos) {
            return i;
        }
    }
    return -1;
}


int part1() {
    input_t inputs = parse_inputs();

    std::vector<int32_t> numbers;
    char first_digit;
    char last_digit;

    for (size_t i = 0; i < inputs.size(); ++i) {
        first_digit = 0;
        last_digit = 0;
        for (char c : inputs[i]) {
            if (std::isdigit(c)) {
                first_digit = c;
                break;
            }
        }

         for (size_t j = inputs[i].size() - 1; j >= 0; --j) {
            char c = inputs[i][j];
            if (std::isdigit(c)) {
                last_digit = c;
                break;
            }
        }

        const char number[2] = {first_digit, last_digit};
        #ifdef DEBUG
        std::cout << number << std::endl;
        #endif
        if (first_digit && last_digit) numbers.emplace_back(std::stoi(number));
    }

    const int sum = std::accumulate(numbers.begin(), numbers.end(), 0);
    return sum;
}


int part2() {
    input_t inputs = parse_inputs();

    std::vector<int32_t> numbers;
    char first_digit;
    char last_digit;;

    for (size_t i = 0; i < inputs.size(); ++i) {
        first_digit = 0;
        last_digit = 0;
        for (size_t j = 0; j < inputs[i].size(); ++j) {
            char c = inputs[i][j];
            if (std::isdigit(c)) {
                first_digit = c;
                break;
            }

            std::string substr = inputs[i].substr(0, j+1);
            int number = get_number_from_name(substr);
            if (number != -1) {
                first_digit = NUMBER_DIGITS[number];
                break;
            }
        }

        for (size_t j = inputs[i].size()-1; j >= 0; --j) {
            char c = inputs[i][j];
            if (std::isdigit(c)) {
                last_digit = c;
                break;
            }

            std::string substr = inputs[i].substr(j, inputs[i].size());
            int number = get_number_from_name(substr);
            if (number != -1) {
                last_digit = NUMBER_DIGITS[number];
                break;
            }
        }

        const char number[2] = {first_digit, last_digit};
        #ifdef DEBUG
        std::cout << inputs[i] << ": " << number << std::endl;
        #endif
        if (first_digit && last_digit) numbers.emplace_back(std::stoi(number));
    }


    const int sum = std::accumulate(numbers.begin(), numbers.end(), 0);

    return sum;
}


int main() 
{
    int res1 = part1();
    std::cout << std::endl;
    int res2 = part2();

    std::cout << "---PART 1---\n";
    std::cout << "Sum = " << res1 << std::endl;
    std::cout << "---PART 2---\n";
    std::cout << "Sum = " << res2 << std::endl;

    return 0;
}