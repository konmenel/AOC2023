#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <format>
#include <vector>
#include <string>
#include <numeric>
#include <algorithm>
#include <optional>

#include "recycles.h"


#ifdef DEBUG
    #define debug_println(fmt, ...) std::cout << std::format((fmt), ##__VA_ARGS__) << std::endl
    #define debug_print(fmt, ...) std::cout << std::format((fmt), ##__VA_ARGS__)
#else
    #define debug_println(fmt, ...)
    #define debug_print(fmt, ...)
#endif


typedef std::vector<std::string> input_t;

using namespace utils;

void usage(const std::string &program_name) {
    std::cout << "Usage: " << program_name << " <input_file>\n";
}


input_t parseInputs(const std::string &filename) {
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



struct Point2 {
    float x;
    float y;

    Point2(float val=0.0f) : x(val), y(val) {}
    Point2(float x, float y) : x(x), y(y) {}

    inline float mag2() const {
        return x*x + y*y; 
    }

    inline float mag() const {
        return std::sqrt(mag2()); 
    }

    inline Point2 operator+(float val) const {
        return {x + val, y + val};
    }

    inline Point2 operator+(const Point2 &p) const {
        return {x + p.x, y + p.y};
    }

    inline Point2 operator*(float val) const {
        return {val * x, val * y};
    }

    inline Point2 operator/(float val) const {
        return {val / x, val / y};
    }

    inline bool operator==(const Point2 &rhs) const {
        return x == rhs.x && y == rhs.y;
    }

    std::string toString() const {
        return std::format("({},{})", x, y);
    }
    
    friend std::ostream& operator<<(std::ostream &os, const Point2 &p) {
        os << p.toString();
        return os;
    }
};


struct Point3 {
    float x;
    float y;
    float z;

    Point3(float val=0.0f) : x(val), y(val), z(val) {}
    Point3(float x, float y, float z) : x(x), y(y), z(z) {}

    inline float mag2() const {
        return x*x + y*y + z*z; 
    }

    inline float mag() const {
        return std::sqrt(mag2()); 
    }

    inline Point3 operator+(float val) const {
        return {x + val, y + val, z + val};
    }

    inline Point3 operator+(const Point3 &p) const {
        return {x + p.x, y + p.y, z + p.z};
    }

    inline Point3 operator*(float val) const {
        return {val * x, val * y, val * z};
    }

    inline Point3 operator/(float val) const {
        return {val / x, val / y, val / z};
    }

    inline bool operator==(const Point3 &rhs) const {
        return x == rhs.x && y == rhs.y && z == rhs.z;
    }

    std::string toString() const {
        return std::format("({},{},{})", x, y, z);
    }
    
    friend std::ostream& operator<<(std::ostream &os, const Point3 &p) {
        os << p.toString();
        return os;
    }
};



struct Matrix2d {
    float a11;
    float a12;
    float a21;
    float a22;

    Matrix2d(float val=0.0f) : a11(val), a12(val), a21(val), a22(val) {}
    Matrix2d(float a11, float a12, float a21, float a22) : a11(a11), a12(a12), a21(a21), a22(a22) {}
    
    inline Matrix2d operator*(float v) const {
        return {a11*v, a12*v, a21*v, a22*v};
    }

    inline Point2 operator*(const Point2 &vec) const {
        return {a11*vec.x + a12*vec.y, a21*vec.x + a22*vec.y};
    }

    float det() const {
        return a11*a22 - a12*a21;
    }

    std::optional<Matrix2d> inv() const {
        float d = det();
        if (!d) {
            return {};
        }

        return Matrix2d(a22, -a12, -a21, a11) * (1.0f / d);
    }
};


struct Line3d {
    Point3 x0;
    Point3 v;

    Line3d() : x0(), v() {}
    Line3d(const Point3 &x0, const Point3 &v) : x0(x0), v(v) {}

    static Line3d fromString(const std::string &str) {
        Line3d ret{};

        std::istringstream iss(str);
        std::string vec_str;

        std::getline(iss, vec_str, '@');
        std::istringstream iss2(vec_str);
        std::string num_str;
        std::getline(iss2, num_str, ',');
        ret.x0.x = std::stof(num_str);
        std::getline(iss2, num_str, ',');
        ret.x0.y = std::stof(num_str);
        std::getline(iss2, num_str, ',');
        ret.x0.z = std::stof(num_str);

        std::getline(iss, vec_str, '@');
        iss2.str(vec_str);
        iss2.clear();
        std::getline(iss2, num_str, ',');
        ret.v.x = std::stof(num_str);
        std::getline(iss2, num_str, ',');
        ret.v.y = std::stof(num_str);
        std::getline(iss2, num_str, ',');
        ret.v.z = std::stof(num_str);
        
        return ret;
    }

    bool isParallel(const Line3d &l2) const {
        return (v * (1.0f / v.mag())) == (l2.v * (1.0f / l2.v.mag()));
    }

    std::optional<Point3> intersection(const Line3d &l2) const {
        if (isParallel(l2)) {
            return {};
        }

        Matrix2d mat(v.x, -l2.v.x, v.y, -l2.v.y);
        Point2 b(l2.x0.x - x0.x, l2.x0.y - x0.y);
        std::optional<Matrix2d> inv = mat.inv();
        if (!inv) {
            return {};
        }

        Point2 sol = inv.value() * b;
        float t1 = sol.x;
        float t2 = sol.y;
        std::cout << sol << std::endl;
        std::cout << v.z*t1 - l2.v.z*t2 + x0.z - l2.x0.z << std::endl;

        if (v.z*t1 - l2.v.z*t2 + x0.z - l2.x0.z != 0) {
            return {};
        }
        return Point3{x0.x + v.x*t1, x0.y + v.y*t1, x0.z + v.z*t1};
    }

    inline bool operator==(const Line3d &rhs) const {
        return x0 == rhs.x0 && v == rhs.v;
    }
};


struct Line2d {
    float a;    // slope
    float b;    // y-intersect

    Line2d(float val=0.0f) : a(val), b(val) {}
    Line2d(float a, float b) : a(a), b(b) {}
    Line2d(const Line3d &l) {
        a = l.v.y / l.v.x;
        b = l.x0.y - l.x0.x * a;
    }


    bool isParallel(const Line2d &l2) const {
        return a == l2.a;
    }

    std::optional<Point2> intersection(const Line2d &l2) const {
        if (isParallel(l2)) {
            return {};
        }
        Point2 sol;
        sol.x = (l2.b - b) / (a - l2.a);
        sol.y = a * sol.x + b;
        return sol;
    }

    inline bool operator==(const Line2d &rhs) const {
        return a == rhs.a && b == rhs.b;
    }
};



i64 part1(const input_t &in) {
    #ifdef DEBUG
    const float MIN = 7;
    const float MAX = 27;
    #else
    const float MIN = 200000000000000;
    const float MAX = 400000000000000;
    #endif

    i64 total = 0;
    std::vector<Line3d> lines;
    for (const auto &line : in) {
        lines.emplace_back(Line3d::fromString(line));
    }

    for (usize i = 0; i < lines.size(); ++i) {
        for (usize j = i + 1; j < lines.size(); ++j) {
            const Line3d li = lines[i];
            const Line3d lj = lines[j];
            Line2d l1(lines[i]);
            Line2d l2(lines[j]);
            if (l1 == l2)
                continue;

            std::optional<Point2> pint = l1.intersection(l2);
            if (!pint)
                continue;
            if (li.v.y * (pint->y - lines[i].x0.y) < 0)
                continue;
            if (lj.v.y * (pint->y - lines[j].x0.y) < 0)
                continue;

            if (pint->x < MIN || pint->x > MAX || pint->y < MIN || pint->y > MAX)
                continue;

            total++;
        }
    }
    return total;
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }
    input_t lines = parseInputs(argv[1]);

    auto res1 = part1(lines);
    std::cout << "-----PART 1-----\n";
    std::cout << "Future intersects = " << res1 << std::endl;
    
    // auto res2 = part2(lines);
    // std::cout << "-----PART 2-----\n";
    // std::cout << "Future intersects = " << res2 << std::endl;

    return 0;
}