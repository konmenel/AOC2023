#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <format>
#include <vector>
#include <string>
#include <numeric>
#include <algorithm>
#include <complex>
#include <ranges>
#include <queue>

#include "recycles.h"
#include "robin_hood.h"

#ifdef _DEBUG
    #define debug_println(fmt, ...) std::cout << std::format((fmt), ##__VA_ARGS__) << std::endl
    #define debug_print(fmt, ...) std::cout << std::format((fmt), ##__VA_ARGS__)
#else
    #define debug_println(fmt, ...)
    #define debug_print(fmt, ...)
#endif

using namespace utils;

typedef std::vector<std::string> input_t;

const std::array<Dir, 4> DIRS = {NORTH, SOUTH, EAST, WEST};

using Graph = robin_hood::unordered_flat_map<Pos, std::vector<std::pair<Pos, i32>>>;
using Set = robin_hood::unordered_flat_set<Pos>;

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


enum class Tile {
    Empty = 0,
    Forest,
    SNorth,
    SSouth,
    SEast,
    SWest
};


Dir tileToDir(Tile t) {
    switch (t) {
    case Tile::SNorth:
        return NORTH;
    case Tile::SSouth:
        return SOUTH;
    case Tile::SEast:
        return EAST;
    case Tile::SWest:
        return WEST;
    case Tile::Empty:
    case Tile::Forest:
        return 0;

    default:
        debug_println("{}:{}: Unreachable. Unknown Tile type {}", __FILE__, __LINE__, (i32)t);
        return 0;
    }
}


char tileToChar(Tile t) {
    switch (t) {
    case Tile::Empty:
        return '.';
    case Tile::Forest:
        return '#';
    case Tile::SNorth:
        return '^';
    case Tile::SSouth:
        return 'v';
    case Tile::SEast:
        return '>';
    case Tile::SWest:
        return '<';

    default:
        debug_println("Unreachable. Unknown Tile type {}", (i32)t);
        return 0;
    }
}


Tile tileFromChar(char c) {
    switch (c) {
    case '.':
        return Tile::Empty;
    case '#':
        return Tile::Forest;
    case '^':
        return Tile::SNorth;
    case 'v':
        return Tile::SSouth;
    case '>':
        return Tile::SEast;
    case '<':
        return Tile::SWest;

    default:
        debug_println("Unreachable. Unknown Tile char {}", c);
        exit(1);
    }
}


std::ostream& operator<<(std::ostream &os, Tile t) {
    os << tileToChar(t);
    return os;
}


Grid<Tile> parseGrid(const input_t &in) {
    Grid<Tile> grid(in.size(), in[0].size(), Tile::Empty);
    for (const auto &[y, line] : std::views::enumerate(in)) {
        for (const auto [x, c] : std::views::enumerate(line)) {
            grid(x, y) = tileFromChar(c);
        }
    }
    return grid;
}


std::vector<Pos> findNeighbors(const Pos &pos, const Grid<Tile> &grid) {
    switch (grid(pos)) {
    case Tile::SNorth:
        return {pos + NORTH};
    case Tile::SSouth:
        return {pos + SOUTH};
    case Tile::SEast:
        return {pos + EAST};
    case Tile::SWest:
        return {pos + WEST};

    case Tile::Forest:
    case Tile::Empty:
    default:
    std::vector<Pos> ret;
    ret.reserve(4);
    for (const auto &dir : DIRS) {
        Pos np = pos + dir;
        if (!grid.isOutOfBound(np) && grid(np) != Tile::Forest)
            ret.emplace_back(np);
        
    }
    return ret;
    }
}



template<>
struct std::hash<Pos> {
    std::size_t operator()(const Pos &p) const noexcept {
        return ((i64)p.real() << 32) + (i64)p.imag();
    }
};


struct State {
    Pos pos;
    i64 dist;

    State(i64 val=0) : pos(static_cast<i32>(val)), dist(val) {}
    State(const Pos &p, i64 d) : pos(p), dist(d) {}

    inline bool operator<(const State &rhs) const noexcept {
        return dist < rhs.dist;
    }

    inline bool operator>(const State &rhs) const noexcept {
        return dist > rhs.dist;
    }

    inline bool operator==(const State &rhs) const noexcept {
        return dist == rhs.dist;
    }
    
};


Graph createGraph(const Grid<Tile> &grid, const Pos &start, const Pos &end) {
    std::vector<Pos> nodes = {start, end};
    for (usize y = 0; y < grid.rows; ++y) {
        for (usize x = 0; x < grid.cols; ++x) {
            const Pos p(x, y);
            if (grid(p) == Tile::Forest)
                continue;
            auto n = findNeighbors(p, grid);
            if (n.size() >= 3) 
                nodes.push_back(p);
        }
    }

    Graph graph;
    for (const auto &node : nodes) {
        if (!graph.contains(node))
            graph[node] = {};
        
        std::vector<Pos> neighs = findNeighbors(node, grid);
        for (const auto &n : neighs) {
            robin_hood::unordered_flat_set<Pos> seen{node};
            std::deque<std::pair<Pos, i32>> q = {{n, 1}};
            while (!q.empty()) {
                auto pr = q.front();
                q.pop_front();
                const Pos &p = pr.first;
                const i32 d = pr.second;
                auto it = std::find(nodes.begin(), nodes.end(), p);
                if (it != nodes.end()) {
                    graph[node].emplace_back(p, d);
                    q.clear();
                    break;
                }
                seen.insert(p);
                std::vector<Pos> neighs2 = findNeighbors(p, grid);
                for (const auto &n2 : neighs2) {
                    if (seen.contains(n2))
                        continue;
                    if (p - n2 == tileToDir(grid(n2)))
                        continue;
                    q.emplace_back(n2, d+1);
                }
            }
        }
    }
    return graph;
}


i64 longestPath(Pos pos, Set &seen, Graph &graph, const Pos &end) {
    if (pos == end)
        return 0;
    i64 max = INT64_MIN;
    seen.insert(pos);
    std::vector<std::pair<Pos, i32>> ns = graph[pos];
    for (const auto &n : ns) {
        const Pos &np = n.first;
        const i64 dist = n.second;
        if (!seen.contains(np))
            max = std::max(max,
                longestPath(np, seen, graph, end) + dist);
    }
    seen.erase(pos);
    return max;
}


i64 part1(const input_t &in) {
    Grid<Tile> grid = parseGrid(in);
    debug_println("{}", grid.toString());
    Pos start(1, 0);
    Pos end  (grid.cols-2, grid.rows-1);

    Graph graph = createGraph(grid, start, end);

    debug_println("{{");
    for ([[maybe_unused]]const auto &[key, val] : graph) {
        debug_print("  {}:[", key);
        for ([[maybe_unused]]const auto &el : val)  {
            debug_print("{{node:{}, dist:{}}},", el.first, el.second);
        }
        debug_println("]");
    }
    debug_println("}}");

    Set seen{};
    return longestPath(start, seen, graph, end);
}


i64 part2(const input_t &in) {
    Grid<Tile> grid = parseGrid(in);
    // Remove slopes
    for (usize y = 0; y < grid.rows; y++) {
        for (usize x = 0; x < grid.cols; x++) {
            if (grid(x, y) != Tile::Forest) 
                grid(x, y) = Tile::Empty;
        }
    }
    Pos start(1, 0);
    Pos end  (grid.cols-2, grid.rows-1);

    Graph graph = createGraph(grid, start, end);

    debug_println("{{");
    for ([[maybe_unused]]const auto &[key, val] : graph) {
        debug_print("  {}:[", key);
        for ([[maybe_unused]]const auto &el : val)  {
            debug_print("{{node:{}, dist:{}}},", el.first, el.second);
        }
        debug_println("]");
    }
    debug_println("}}");

    Set seen{};
    return longestPath(start, seen, graph, end);
}




int main(int argc, char *argv[]) {
    if (argc < 2) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }
    input_t lines = parseInputs(argv[1]);

    auto res1 = part1(lines);
    std::cout << "-----PART 1-----\n";
    std::cout << "Longest path = " << res1 << std::endl;
    
    auto res2 = part2(lines);
    std::cout << "-----PART 2-----\n";
    std::cout << "Longest path = " << res2 << std::endl;

    return 0;
}