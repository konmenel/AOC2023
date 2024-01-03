#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <numeric>
#include <math.h>
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

inline double dist(double t, double record) {
    return (record - t) * t;
}

int part1(const input_t &in) {
    std::vector<std::pair<double, double>> time_dist;

    double t, x;
    std::string without_label = in[0].substr(6); //< Remove "Time:"
    std::istringstream iss_time(without_label);
    without_label = in[1].substr(10); //< Remove "Distance:"
    std::istringstream iss_dist(without_label);
    while (iss_time >> t && iss_dist >> x) {
        time_dist.emplace_back(t, x);
    }

    int prod = 1;
    for (auto &t_x : time_dist){
        // Some high school level math...
        double _max_dist = t_x.first * t_x.first * 0.25; // Not needed but I did the math anyways
        double best_time = t_x.first * 0.5;
        
        int sum = 0;
        // Forward loop
        int t = static_cast<int>(std::round(best_time));
        while (dist(t, t_x.first) > t_x.second) {
            t++;
            sum++;
        }

        // The function is symmetric (quadratic) so the solution is however
        // if the optimal time is int we need to subtract one because we count
        // the peak twice
        sum *= 2;
        double tmp;
        if (std::modf(best_time, &tmp) == 0) {
            sum--;
        }

        prod *= sum;
        #ifdef _DEBUG 
        std::cout << sum << std::endl;
        #endif
    }

    return prod;
}



int part2(const input_t &in) {
    double time_race;
    double record_dist;
    // Parse strings
    std::string time_str = in[0].substr(6); //< Remove "Time:"
    time_str.erase(std::remove_if(time_str.begin(), time_str.end(), ::isspace), time_str.end());
    std::istringstream iss_time(time_str);
    iss_time >> time_race;

    std::string dist_str = in[1].substr(10); //< Remove "Distance:"
    dist_str.erase(std::remove_if(dist_str.begin(), dist_str.end(), ::isspace), dist_str.end());
    std::istringstream iss_dist(dist_str);
    iss_dist >> record_dist;

    // Some high school level math...
    double _max_dist = time_race * time_race * 0.25; // Not needed but I did the math anyways
    double best_time = time_race * 0.5;
    
    int sum = 0;
    // Forward loop
    int t = static_cast<int>(std::round(best_time));
    while (dist(t, time_race) > record_dist) {
        t++;
        sum++;
    }

    // The function is symmetric (quadratic) so the solution is however
    // if the optimal time is int we need to subtract one because we count
    // the peak twice
    sum *= 2;
    double tmp;
    if (std::modf(best_time, &tmp) == 0) {
        sum--;
    }
    return sum;
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
    std::cout << "sol = " << res1 << std::endl;
    std::cout << "-----PART 2-----\n";
    std::cout << "sol = " << res2 << std::endl;

    return 0;
}