#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <vector>
#include <string>
#include <format>
#include <numeric>
#include <algorithm>
#include <unordered_map>
#include <filesystem>


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

std::string trim(const std::string& s) {
    const int l = (int)s.length();
    int a=0, b=l-1;
    char c;
    while(a<l && ((c=s[a])==' '||c=='\t'||c=='\n'||c=='\v'||c=='\f'||c=='\r'||c=='\0')) a++;
    while(b>a && ((c=s[b])==' '||c=='\t'||c=='\n'||c=='\v'||c=='\f'||c=='\r'||c=='\0')) b--;
    return s.substr(a, 1+b-a);
}

struct Node {
    std::string name;
    Node *left = nullptr;
    Node *right = nullptr;
};


std::unordered_map<std::string, Node> createNodeMap(const input_t &lines) {
    std::unordered_map<std::string, Node> nodes;
    
    for (const std::string &line : lines) {
        std::istringstream iss(line);
        std::string node_name;
        std::string lnode;
        std::string rnode;

        std::getline(iss, node_name, '=');
        node_name = trim(node_name);

        std::getline(iss, lnode, '=');
        lnode = trim(lnode);
        lnode = lnode.substr(1, lnode.size()-2);
        iss.str(lnode);
        iss.clear();
        std::getline(iss, lnode, ',');
        std::getline(iss, rnode, ',');
        rnode = trim(rnode);
        
        if (!nodes.contains(lnode)) nodes[lnode] = Node{lnode};
        if (!nodes.contains(rnode)) nodes[rnode] = Node{rnode};
        if (!nodes.contains(node_name)) {
            nodes[node_name] = Node{node_name, &nodes[lnode], &nodes[rnode]};
        } else {
            nodes[node_name].left  = &nodes[lnode];
            nodes[node_name].right = &nodes[rnode];
        }
    }
    return nodes;
}


int part1(const input_t &in) {
    std::string instructions = in[0];
    size_t len = instructions.size();

    std::unordered_map<std::string, Node> nodes = createNodeMap(std::vector(in.begin()+2, in.end()));
    #ifdef _DEBUG
    for (auto &kv : nodes) {
        std::cout << std::format("Node \"{}\", Left: {}, Right {}\n", kv.first, kv.second.left->name, kv.second.right->name);
    }
    #endif

    Node &curr_node = nodes["AAA"];
    size_t steps = 0;
    while (curr_node.name != "ZZZ") {
        char instr = instructions[steps++ % (len)];
        #ifdef _DEBUG
        std::cout << std::format("Instuction: \"{}\"\n", instr);
        #endif
        switch (instr)
        {
        case 'R':
            curr_node = *curr_node.right;
            break;
        case 'L':
            curr_node = *curr_node.left;
            break;
        default:
            std::cout << "Unreachable! Instuction: \"" << instr << "\"" << std::endl;
            exit(1);
        }
    }

    return steps;
}


std::vector<const Node *> getStartingNodes(std::unordered_map<std::string, Node> &nodes) {
    std::vector<const Node *> ret;
    for (const auto &kv : nodes) {
        const Node *node = &nodes[kv.first];
        const std::string &name = node->name;
        if (name.ends_with('A')) {
            ret.push_back(node);
        }
    }
    return ret;
}


void makeStep(const Node *&curr_node, char instr, std::unordered_map<std::string, Node> &nodes) {
    switch (instr)
    {
    case 'R':
        curr_node = curr_node->right;
        break;
    
    case 'L':
        curr_node = curr_node->left;
        break;

    default:
        std::cout << "Unreachable! Instuction: \"" << instr << "\"" << std::endl;
        exit(1);
    }
}

uint64_t lcm(const std::vector<uint64_t> &nums) {
    if (nums.size() == 2) {
        return nums[0]*nums[1] / std::gcd(nums[0], nums[1]);
    }
    return std::lcm(nums[0], lcm(std::vector(nums.begin()+1, nums.end())));
}


uint64_t part2(const input_t &in) {
    std::string instructions = in[0];
    size_t len = instructions.size();

    std::unordered_map<std::string, Node> nodes = createNodeMap(std::vector(in.begin()+2, in.end()));
    std::vector<const Node *> starting_nodes = getStartingNodes(nodes);
    #ifdef _DEBUG
    std::cout << "Starting positions: ";
    for (const auto &n : starting_nodes) {
        std::cout << std::format("{}, ", n->name);
    }
    std::cout << std::endl;
    #endif
    

    std::vector<uint64_t> node_steps;
    for (size_t i = 0; i < starting_nodes.size(); i++) {
        size_t steps = 0;
        const Node *curr_node = starting_nodes[i];
        while (!curr_node->name.ends_with('Z')) {
            makeStep(curr_node, instructions[steps % len], nodes);
            ++steps;
        }
        node_steps.push_back(steps);
    }

    return lcm(node_steps);
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }
    std::filesystem::path path(argv[1]);
    #ifdef _DEBUG
    std::string stem = path.stem();
    std::string ext = path.extension();
    std::string parent = path.parent_path();
    path = std::filesystem::path(parent + "/" + stem + "-1" + ext);
    #endif
    input_t lines1 = parse_inputs(path);

    auto res1 = part1(lines1);
    std::cout << "-----PART 1-----\n";
    std::cout << "number of steps: " << res1 << std::endl;
    
    #ifdef _DEBUG
    path = std::filesystem::path(parent + "/" + stem + "-2" + ext);
    #endif
    input_t lines2 = parse_inputs(path);
    auto res2 = part2(lines2);
    std::cout << "-----PART 2-----\n";
    std::cout << "number of steps: " << res2 << std::endl;

    return 0;
}