#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <format>
#include <vector>
#include <string>
#include <numeric>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>

#include "robin_hood.h"


#ifdef DEBUG
    #define debug_println(fmt, ...) std::cout << std::format((fmt), ##__VA_ARGS__) << std::endl
    #define debug_print(fmt, ...) std::cout << std::format((fmt), ##__VA_ARGS__)
#else
    #define debug_println(fmt, ...)
    #define debug_print(fmt, ...)
#endif


typedef std::vector<std::string> input_t;
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;
typedef size_t   usize;


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


enum class Axis {
    X = 0,
    Y,
    Z,
    NoOrient,
};


struct Point {
    i32 x;
    i32 y;
    i32 z;

    Point(i32 val=0) : x(val), y(val), z(val) {}
    Point(i32 x, i32 y, i32 z) : x(x), y(y), z(z) {}
    Point(const std::string &str) {
        std::istringstream point_iss(str);
        std::string stmp;

        std::getline(point_iss, stmp, ',');
        x = std::atoi(stmp.c_str());
        std::getline(point_iss, stmp, ',');
        y = std::atoi(stmp.c_str());
        std::getline(point_iss, stmp, ',');
        z = std::atoi(stmp.c_str());
    }

    Point operator+(const Point &rhs) const noexcept { return Point(x + rhs.x, y + rhs.y, z + rhs.z); }
    Point operator+(i32 val) const noexcept { return Point(x + val, y + val, z + val); }

    inline bool operator==(const Point &rhs) const noexcept { return x == rhs.x && y == rhs.y && z == rhs.z; }
};


struct Brick {
    Point start;
    Point end;

    Brick() : start(0), end(0) {}
    Brick(const Point &s, const Point &e) : start(s), end(e) {}
    Brick(const std::string &str) {
        std::istringstream iss(str);
        std::string point_str;

        std::array<Point, 2> points{Point(), Point()};

        
        for (usize i = 0; i < 2; ++i) {
            std::getline(iss, point_str, '~');
            points[i] = Point(point_str);
        }
        start = points[0];
        end = points[1];
    }

    bool isOverlap(const Point &p) const {
        if (orientation() == Axis::X) {
            if (start.y != p.y || start.z != p.z) {
                return false;
            }
            for (i32 x = start.x; x <= end.x; x++) {
                if (x == p.x) return true;
            }
            return false;
        } else if (orientation() == Axis::Y) {
            if (start.x != p.x || start.z != p.z) {
                return false;
            }
            for (i32 y = start.y; y <= end.y; y++) {
                if (y == p.y) return true;
            }
            return false;
        } else if (orientation() == Axis::Z) {
            if (start.x != p.x || start.y != p.y) {
                return false;
            }
            for (i32 z = start.z; z <= end.z; z++) {
                if (z == p.z) return true;
            }
            return false;
        }
        return start.x == p.x && start.y == p.y && start.z == p.z;
    }

    bool isOverlap(const Brick &b) const {
        if (orientation() == Axis::X) {
            for (i32 x = start.x; x <= end.x; x++) {
                if (b.isOverlap(Point(x, start.y, start.z)))
                    return true;
            }
            return false;
        } else if (orientation() == Axis::Y) {
            for (i32 y = start.y; y <= end.y; y++) {
                if (b.isOverlap(Point(start.x, y, start.z)))
                    return true;
            }
            return false;
        } else if (orientation() == Axis::Z) {
            for (i32 z = start.z; z <= end.z; z++) {
                if (b.isOverlap(Point(start.x, start.y, z)))
                    return true;
            }
            return false;
        }
        return b.isOverlap(start);
    }

    bool isOverlapProjection(const Brick &other) {
        return std::max(start.x, other.start.x) <= std::min(end.x, other.end.x)
            && std::max(start.y, other.start.y) <= std::min(end.y, other.end.y);
    }

    inline i32 height() const { return end.z - start.z + 1; }

    Axis orientation() const {
        if (start.x != end.x)
            return Axis::X;
        if (start.y != end.y)
            return Axis::Y;
        if (start.z != end.z)
            return Axis::Z;
        return Axis::NoOrient;
    }

    inline bool operator<(const Brick &rhs) const noexcept { return start.z < rhs.start.z; }

    friend std::ostream& operator<<(std::ostream &os, const Brick &b) {
        os << std::format("{{{},{},{}}}-{{{},{},{}}}",
            b.start.x, b.start.y, b.start.z, b.end.x, b.end.y, b.end.z);
        return os;
    }

    inline bool operator==(const Brick &rhs) const noexcept { return start == rhs.start && end == rhs.end; }
};


void simulateFalling(std::vector<Brick> &bricks) {
    for (usize i = 0; i < bricks.size(); ++i) {
        Brick &brick = bricks[i];
        i32 maxz = 1;
        for (usize j = 0; j < i; j++) {
            const Brick &other = bricks[j];
            if (brick.isOverlapProjection(other)) {
                maxz = std::max(maxz, other.end.z + 1);
            }
        }
        brick.end.z += maxz - brick.start.z;
        brick.start.z = maxz;
    }
}


i64 countRemoveable(const std::vector<Brick> &bricks) {
    using Map = std::unordered_map<usize, std::unordered_set<usize>>;
    Map supports;
    Map supported;
    for (usize i = 0; i < bricks.size(); ++i) {
        const Brick &brick = bricks[i];
        Brick brick_cp = brick;
        brick_cp.start.z--;
        brick_cp.end.z--;
        if (!supports.contains(i)) {
            supports[i] = {};
            supported[i] = {};
        }
        if (brick_cp.start.z == 0)
            continue;
        
        for (usize j = 0; j < i; ++j) {
            const Brick &other = bricks[j];

            if (brick_cp.isOverlap(other)) {
                supported[i].insert(j);
                supports[j].insert(i);
            }
        }
    }
    i32 count = 0;
    for (usize i = 0; i < bricks.size(); ++i) {
        const auto &lam = [&](usize j){ return supported[j].size() > 1; };
        if (std::all_of(supports[i].cbegin(), supports[i].cend(), lam)) {
            count++;
        }
    }
    return count;
}

i64 part1(const input_t &in) {
    std::vector<Brick> bricks;
    for (const auto &line : in) {
        bricks.push_back(Brick(line));
    }
    std::sort(bricks.begin(), bricks.end());

    simulateFalling(bricks);
    std::sort(bricks.begin(), bricks.end());
    i32 level = INT32_MAX;
    i32 i = 1;
    for (auto it = bricks.rbegin(); it < bricks.rend(); ++it, ++i) {
        if (level != it->start.z) {
            i = 1;
            level = it->start.z;
            debug_println("\nLevel {}:", level);
        }
        debug_print("{}:({},{},{})-({},{},{}) ", i, it->start.x, it->start.y, it->start.z, it->end.x, it->end.y, it->end.z);
    }
    debug_println("");
    return countRemoveable(bricks);
}


i64 countChainReactions(const std::vector<Brick> &bricks) {
    using Map = std::unordered_map<usize, std::unordered_set<usize>>;
    Map supports;
    Map supported;
    for (usize i = 0; i < bricks.size(); ++i) {
        const Brick &brick = bricks[i];
        Brick brick_cp = brick;
        brick_cp.start.z--;
        brick_cp.end.z--;
        if (!supports.contains(i)) {
            supports[i] = {};
            supported[i] = {};
        }
        if (brick_cp.start.z == 0)
            continue;
        
        for (usize j = 0; j < i; ++j) {
            const Brick &other = bricks[j];

            if (brick_cp.isOverlap(other)) {
                supported[i].insert(j);
                supports[j].insert(i);
            }
        }
    }
    
    i32 count = 0;
    for (usize i = 0; i < bricks.size(); ++i) {
        for  (usize j = 0; j < supports[i].size(); ++j) {
            if (supported[j].size() == 1) {
                while()
            }
        }
    }
    return count;
}


i64 part2(const input_t &in) {
    std::vector<Brick> bricks;
    for (const auto &line : in) {
        bricks.push_back(Brick(line));
    }
    std::sort(bricks.begin(), bricks.end());

    simulateFalling(bricks);
    std::sort(bricks.begin(), bricks.end());
    i32 level = INT32_MAX;
    i32 i = 1;
    for (auto it = bricks.rbegin(); it < bricks.rend(); ++it, ++i) {
        if (level != it->start.z) {
            i = 1;
            level = it->start.z;
            debug_println("\nLevel {}:", level);
        }
        debug_print("{}:({},{},{})-({},{},{}) ", i, it->start.x, it->start.y, it->start.z, it->end.x, it->end.y, it->end.z);
    }
    debug_println("");
    return countChainReactions(bricks);
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }
    input_t lines = parseInputs(argv[1]);

    auto res1 = part1(lines);
    std::cout << "-----PART 1-----\n";
    std::cout << "Number of bricks that can be disintegrated = " << res1 << std::endl;
    
    auto res2 = part2(lines);
    std::cout << "-----PART 2-----\n";
    std::cout << "Chain reaction power = " << res2 << std::endl;

    return 0;
}