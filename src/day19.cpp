#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <format>
#include <vector>
#include <string>
#include <numeric>
#include <algorithm>


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


struct Part {
    u32 x;
    u32 m;
    u32 a;
    u32 s;

    Part() : x(0), m(0), a(0), s(0) {}
    Part(u32 val) : x(val), m(val), a(val), s(val) {}
    Part(u32 x_, u32 m_, u32 a_, u32 s_) : x(x_), m(m_), a(a_), s(s_) {}

    std::string printToString() const {
        std::ostringstream oss;
        oss << *this;
        return oss.str();
    }

    // a bit unsafe but will do. I guess I should wrap this in `unsafe{}`
    const u32& operator[](usize i) const {
        return *(reinterpret_cast<const u32*>(this) + i);
    }

    u32& operator[](usize i) {
        return *(reinterpret_cast<u32*>(this) + i);
    }

    inline bool operator==(const Part &rhs) {
        return a == rhs.a && m == rhs.m && x == rhs.x && s == rhs.s;
    }

    friend std::ostream& operator<<(std::ostream& os, const Part& p);
};

std::ostream& operator<<(std::ostream& os, const Part& p) {
    os << "{x=" << p.x << ", m=" << p.m << ", a=" << p.a << ", s=" << p.s << '}';
    return os;
}


enum class Rating {
    None = 0,
    X,
    M,
    A,
    S
};

std::ostream& operator<<(std::ostream& os, const Rating& r) {
    switch (r) {
    case Rating::X:
        os << "x";
        break;
    case Rating::M:
        os << "m";
        break;
    case Rating::S:
        os << "s";
        break;
    case Rating::A:
        os << "a";
        break;
    case Rating::None:
    default:
        os << "None";
        break;
    }

    return os;
}


Rating ratingFromChar(char c) {
    switch (c) {
    case 'x':
        return Rating::X;
    case 'm':
        return Rating::M;
    case 'a':
        return Rating::A;
    case 's':
        return Rating::S;

    default:
        debug_println("Unknown rating \"{}\"", c);
        return Rating::None;
    }
}


enum class Cmp {
    None = 0,
    Greater,
    Less
};

std::ostream& operator<<(std::ostream& os, const Cmp& c) {
    switch (c) {
    case Cmp::Less:
        os << '<';
        break;
    case Cmp::Greater:
        os << '>';
        break;
    
    default:
        debug_println("Unreachable. Unknown Cmp value {}", static_cast<i32>(c));
        break;
    }
    return os;
}


struct Rule{
    Rating rating;
    Cmp cmp;
    u32 limit;
    std::string dest;

    Rule(Rating r, Cmp c, u32 l, const std::string &d) 
        : rating(r), cmp(c), limit(l), dest(d) {}
};

std::ostream& operator<<(std::ostream& os, const Rule& r) {
    if (r.rating == Rating::None) {
        os << "Rule{" << r.dest << '}';
        return os;
    }

    os << "Rule{" << r.rating << r.cmp << r.limit << "->" << r.dest << '}';
    return os;
}


struct Workflow {
    std::string name;
    std::vector<Rule> rules;

    Workflow(const std::string &n) : name(n), rules({}) {}
    Workflow(const std::string &n, const std::vector<Rule> &rl) : name(n), rules(rl) {}

    std::string printToString() const {
        std::ostringstream oss;
        oss << *this;
        return oss.str();
    }

    friend std::ostream& operator<<(std::ostream& os, const Workflow& wf);
};

std::string ruleListToString(const std::vector<Rule> &rl) {
    std::ostringstream os;
    for (usize i = 0; i < rl.size(); ++i) {
        os << rl[i] << (i == rl.size()-1 ? "" : ", ");
    }
    return os.str();
}

std::ostream& operator<<(std::ostream& os, const std::vector<Rule>& rl) {
    os << ruleListToString(rl);
    return os;
}

std::ostream& operator<<(std::ostream& os, const Workflow& wf) {
    os << "Workflow: \"" << wf.name << "\", [" << wf.rules << ']';
    return os;
}


Cmp cmpFromChar(char c) {
    switch (c) {
    case '<':
        return Cmp::Less;
    case '>':
        return Cmp::Greater;

    default:
        debug_println("Unknown cmp \"{}\"", c);
        return Cmp::None;
    }
}


Rule parseRule(const std::string &rule_str) {
    // expected input "m<1234:asi", "R", "A", "wiqe", "s>1234:R", etc
    if (rule_str.size() == 1 || (rule_str[1] != '<' && rule_str[1] != '>')) {
        return Rule(Rating::None, Cmp::None, 0, rule_str);
    }


    Rating r = ratingFromChar(rule_str[0]);
    Cmp c = cmpFromChar(rule_str[1]);
    
    std::istringstream iss(rule_str.substr(2));
    u32 l;
    iss >> l;
    
    std::string d;
    std::getline(iss, d, ':');
    std::getline(iss, d);
    return Rule(r, c, l, d);
}


std::vector<Rule> parseRuleList(const std::string &rule_list_str) {
    // expected input "{rule1,rule2,rule3}"
    std::istringstream iss(rule_list_str.substr(1, rule_list_str.size()-2));
    std::string rule_str;
    std::vector<Rule> rules;

    while (std::getline(iss, rule_str, ',')) {
        rules.emplace_back(parseRule(rule_str));
    }
    return rules;
}


Workflow parseWorkflow(const std::string &workflow_str) {
    usize rulename_end = workflow_str.find('{');
    if (rulename_end == std::string::npos) {
        debug_println("Error parsing workflow string. '{{' not found in \"{}\"", workflow_str);
    }

    std::string workflow_name = workflow_str.substr(0, rulename_end);
    std::vector<Rule> rl = parseRuleList(workflow_str.substr(rulename_end));
    return Workflow{workflow_name, rl};
}


Part parsePart(const std::string &part_str) {
    std::istringstream iss(part_str.substr(1, part_str.size()-2));
    std::string rating;
    Part p{0};

    while (std::getline(iss, rating, ',')) {
        switch (rating[0]) {
        case 'x':
            p.x = std::atoi(rating.substr(2).c_str());
            break;
        case 'a':
            p.a = std::atoi(rating.substr(2).c_str());
            break;
        case 'm':
            p.m = std::atoi(rating.substr(2).c_str());
            break;
        case 's':
            p.s = std::atoi(rating.substr(2).c_str());
            break;
        
        default:
            debug_println("Unknown rating char in \"{}\"", rating);
            break;
        }
    }
    return p;
}


std::vector<Workflow>::const_iterator findWorkflowByName(const std::string &name, const std::vector<Workflow> &wfs) {
    for (auto it = wfs.begin(); it < wfs.end(); ++it) {
        if (it->name == name) return it;
    }

    return wfs.end();
}


bool applyRule(const Part &part, const Rule &rule) {
    u32 val = 0;
    switch (rule.rating){
    case Rating::X:
        val = part.x;
        break;
    case Rating::A:
        val = part.a;
        break;
    case Rating::M:
        val = part.m;
        break;
    case Rating::S:
        val = part.s;
        break;
    
    default:
        debug_println("Unreachable");
        break;
    }

    if (rule.cmp == Cmp::Less)
        return val < rule.limit;
    
    return val > rule.limit;
}


std::string followPartWorkflow(const Part &part, const std::string &start, const std::vector<Workflow> &wfs) {
    auto wf = findWorkflowByName(start, wfs);
    if (wf == wfs.end()) {
        debug_println("Starting workflow {} not found in Workflows", start);
    }
    
    debug_print("{} ", part.printToString());
    while (wf->name != "A" && wf->name != "R") {
        debug_print("{}->", wf->name);
        for (const auto &rule : wf->rules) {
            if (rule.rating == Rating::None) {
                wf = findWorkflowByName(rule.dest, wfs);
                break;
            }
            
            if (applyRule(part, rule)) {
                wf = findWorkflowByName(rule.dest, wfs);
                break;
            }
        }
    }

    debug_println("{}", wf->name);
    return wf->name;
}


u64 part1(const input_t &in) {
    std::vector<Workflow> wfs;
    std::vector<Part> parts;

    usize i = 0;
    for (; !in[i].empty(); ++i) {
        wfs.emplace_back(parseWorkflow(in[i]));
        debug_println("{}", wfs.rbegin()->printToString());
    }
    wfs.emplace_back("A");
    wfs.emplace_back("R");
    for (; ++i < in.size();) {
        parts.emplace_back(parsePart(in[i]));
        debug_println("{}", parts.rbegin()->printToString());
    }
    debug_println("");

    u64 total_rating = 0;
    for (const auto &part : parts) {
        if (followPartWorkflow(part, "in", wfs) == "A")
            total_rating += part.a + part.m + part.s + part.x;
    }

    return total_rating;
}


void findAcceptPaths(const std::string &start, const std::vector<Workflow> &wfs,
    std::vector<Rule> path, usize rule_idx, std::vector<std::vector<Rule>> &accept_paths) {
    auto wf = findWorkflowByName(start, wfs);
    if (wf == wfs.end()) {
        debug_println("Starting workflow {} not found in Workflows", start);
    }
    if (wf->name == "R") return;
    if (wf->name == "A") {
        accept_paths.push_back(path);
        return;
    }
    if (rule_idx >= wf->rules.size()) return;

    const Rule &rule = wf->rules[rule_idx];
    if(path.size() && path.rbegin()->cmp == Cmp::None) path.pop_back();
    path.push_back(rule);
    findAcceptPaths(rule.dest, wfs, path, 0, accept_paths);

    if (rule.cmp != Cmp::None) {
        path.pop_back();
        Rule reverse_rule = rule;
        reverse_rule.limit += rule.cmp==Cmp::Less ? - 1 : 1;
        reverse_rule.cmp = rule.cmp==Cmp::Less ? Cmp::Greater : Cmp::Less;
        reverse_rule.dest = "";
        path.push_back(reverse_rule);
        findAcceptPaths(start, wfs, path, rule_idx+1, accept_paths);
    }
}


struct Limits {
    Part min;
    Part max;
};

Limits findAcceptPathLimits(const std::vector<Rule> &accept_path) {
    Limits l{Part(1), Part(4000)};

    for (const Rule &rule : accept_path) {
        if (rule.cmp == Cmp::None) continue;
        
        if (rule.cmp == Cmp::Less    && rule.rating == Rating::A)
            l.max.a = std::min(l.max.a, rule.limit-1);
        else if (rule.cmp == Cmp::Less    && rule.rating == Rating::M)
            l.max.m = std::min(l.max.m, rule.limit-1);
        else if (rule.cmp == Cmp::Less    && rule.rating == Rating::X)
            l.max.x = std::min(l.max.x, rule.limit-1);
        else if (rule.cmp == Cmp::Less    && rule.rating == Rating::S)
            l.max.s = std::min(l.max.s, rule.limit-1);
        else if (rule.cmp == Cmp::Greater && rule.rating == Rating::A)
            l.min.a = std::max(l.min.a, rule.limit+1);
        else if (rule.cmp == Cmp::Greater && rule.rating == Rating::M)
            l.min.m = std::max(l.min.m, rule.limit+1);
        else if (rule.cmp == Cmp::Greater && rule.rating == Rating::X)
            l.min.x = std::max(l.min.x, rule.limit+1);
        else if (rule.cmp == Cmp::Greater && rule.rating == Rating::S)
            l.min.s = std::max(l.min.s, rule.limit+1);
        
        if (l.min.a > l.max.a || l.min.m > l.max.m || l.min.s > l.max.s || l.min.x > l.max.x) {
            l.min = Part(0);
            l.max = Part(0);
            return l;
        }
    }
    return l;
}


u64 part2(const input_t &in) {
    std::vector<Workflow> wfs;
    for (usize i = 0; !in[i].empty(); ++i) {
        wfs.emplace_back(parseWorkflow(in[i]));
    }
    wfs.emplace_back("A");
    wfs.emplace_back("R");

    std::vector<std::vector<Rule>> accept_paths{};
    findAcceptPaths("in", wfs, {}, 0, accept_paths);
    
    u64 total_combinations = 0;
    debug_println("Accept paths:");
    for (const auto &ap : accept_paths) {
        Limits l = findAcceptPathLimits(ap);
        i64 total = 1;
        for (usize i = 0; i < 4; ++i) {
            if (l.min[i] == l.max[i]) {
                total = 0;
                break;
            } 
            total *= l.max[i] - l.min[i] + 1;
        }
        total_combinations += total;
        debug_println("{} total = {}", ruleListToString(ap), total);
    }

    return total_combinations;
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }
    input_t lines = parseInputs(argv[1]);

    auto res1 = part1(lines);
    std::cout << "-----PART 1-----\n";
    std::cout << "Total ratings = " << res1 << std::endl;
    
    auto res2 = part2(lines);
    std::cout << "-----PART 2-----\n";
    std::cout << "Total distinct combinations = " << res2 << std::endl;

    return 0;
}