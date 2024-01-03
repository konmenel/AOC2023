#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdint>
#include <numeric>

using namespace std;

typedef std::vector<std::string> input_t;


void usage(const std::string &program_name) {
    std::cout << "Usage: " << program_name << " <input_file>\n";
}


std::string trim(const std::string& s) {
    const int l = (int)s.length();
    int a=0, b=l-1;
    char c;
    while(a<l && ((c=s[a])==' '||c=='\t'||c=='\n'||c=='\v'||c=='\f'||c=='\r'||c=='\0')) a++;
    while(b>a && ((c=s[b])==' '||c=='\t'||c=='\n'||c=='\v'||c=='\f'||c=='\r'||c=='\0')) b--;
    return s.substr(a, 1+b-a);
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

void get_card_nums(const string &line, vector<uint32_t> &winning_nums, vector<uint32_t> &my_nums) {
    string line_trimmed = trim(line);
    istringstream card(line);
    
    string winning_nums_str;
    string my_nums_str;

    getline(card, winning_nums_str, ':');
    getline(card, winning_nums_str, '|');
    getline(card, my_nums_str, '|');
    
    istringstream iss(winning_nums_str);
    uint32_t num;
    while (iss >> num) {
        winning_nums.push_back(num);
    }

    iss.str(my_nums_str);
    iss.clear();
    while (iss >> num) {
        my_nums.push_back(num);
    }
}

template<typename T>
bool contains(const vector<T> &vec, const T &val){
    for (const T &el : vec){
        if (el == val) return true;
    }
    return false;
}

int part1(const input_t &in) {
    int points = 0;
    for (const string &line : in){
        int matches = 0;
        vector<uint32_t> winning_nums;
        vector<uint32_t> my_nums;
        get_card_nums(line, winning_nums, my_nums);
        
        for (const uint32_t num : my_nums) {
            if (contains(winning_nums, num)) {
                matches++;
            }
        }

        if (matches){
            points += 1 << (matches-1); //< 2^(matches-1)
        }
    }

    return points;
}

int part2(const input_t &in) {
    int points = 0;
    const size_t ncards = in.size();
    vector<uint32_t> copies(ncards, 1);

    for (size_t i = 0; i < ncards; ++i) {
        const string &line = in[i];
        int matches = 0;
        vector<uint32_t> winning_nums;
        vector<uint32_t> my_nums;
        get_card_nums(line, winning_nums, my_nums);
        
        for (const uint32_t num : my_nums) {
            if (contains(winning_nums, num)) { matches++; }
        }

        for (size_t j = i+1; j < i+1+matches && j < ncards; ++j) { copies[j] += copies[i]; }
        
        #ifdef _DEBUG
        cout << "copies: ";
        for (auto c : copies) cout << c << ",";
        cout << endl;
        #endif
    }

    return accumulate(copies.begin(), copies.end(), 0);
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
    std::cout << "Points = " << res1 << std::endl;
    std::cout << "-----PART 2-----\n";
    std::cout << "Points = " << res2 << std::endl;

    return 0;
}