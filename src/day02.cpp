#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <numeric>


typedef std::vector<std::string> input_t;


void usage(const std::string &program_name) {
    std::cout << "Usage: " << program_name << " <input_file>\n";
}


input_t parse_inputs(const std::string &filename){
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


int part1(const input_t& in) {
    std::vector<int> possible_games;
    for (int i = 0; i < in.size(); ++i) {
        std::stringstream game_ss(in[i]);

        bool possible=true;
        int game = i+1;
        const int MAX_RED  =12;
        const int MAX_GREEN=13;
        const int MAX_BLUE =14;

        std::string set;
        std::getline(game_ss, set, ':');
        while (std::getline(game_ss, set, ';') && possible) {
            std::stringstream set_ss(set);
            std::string color;
            while (std::getline(set_ss, color, ',') && possible) {
                color.erase(0, 1);
                std::stringstream color_ss(color);
                std::string color_value_str;
                std::getline(color_ss, color_value_str, ' ');
                int color_val = std::stoi(color_value_str);
                std::string color_name;
                std::getline(color_ss, color_name, ' ');

                if      (color_name == "red"   && color_val > MAX_RED)   possible=false;
                else if (color_name == "green" && color_val > MAX_GREEN) possible=false;
                else if (color_name == "blue"  && color_val > MAX_BLUE)  possible=false;
            }
        }

        if (possible) possible_games.push_back(game);
    }

    return std::accumulate(possible_games.begin(), possible_games.end(), 0);
}

int part2(const input_t& in) {
    std::vector<int> powers;
    for (int i = 0; i < in.size(); ++i) {
        int32_t max_red   = 0;
        int32_t max_green = 0;
        int32_t max_blue  = 0;

        std::stringstream game_ss(in[i]);
        std::string set;
        std::getline(game_ss, set, ':');
        while (std::getline(game_ss, set, ';')) {
            std::stringstream set_ss(set);
            std::string color;
            while (std::getline(set_ss, color, ',')) {
                color.erase(0, 1);
                std::stringstream color_ss(color);
                std::string color_value_str;
                std::getline(color_ss, color_value_str, ' ');
                int color_val = std::stoi(color_value_str);
                std::string color_name;
                std::getline(color_ss, color_name, ' ');

                if      (color_name == "red")   max_red   = std::max(max_red,   color_val);
                else if (color_name == "green") max_green = std::max(max_green, color_val);
                else if (color_name == "blue")  max_blue  = std::max(max_blue,   color_val);
            }
        }
        powers.push_back(max_blue * max_green * max_red);
#ifdef DEBUG
        std::cout << "Game " << i+1 << ": " << max_red << "," << max_green << "," << max_blue << "; "
            << max_red*max_blue*max_green << std::endl;
#endif
    }
    return std::accumulate(powers.begin(), powers.end(), 0);
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