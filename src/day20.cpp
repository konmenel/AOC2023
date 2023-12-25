#include <cstdint>
#include <iostream>
#include <fstream>
#include <sstream>
#include <format>
#include <vector>
#include <string>
#include <numeric>
#include <algorithm>
#include <deque>
#include <memory>

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


enum class Pulse {
    NoPulse = 0,
    Low,
    High
};


class Module {
public:
    std::string name;
    std::vector<std::string> outputs;
    Pulse last;

    Module(const std::string &n, const std::vector<std::string> &o) 
        : name(n), outputs(o), last(Pulse::Low) {}
    ~Module()=default;

    virtual Pulse broadcast(Pulse p)=0;

    virtual std::string toString()=0;

    friend std::ostream& operator<<(std::ostream &os, const std::unique_ptr<Module> &m) {
        os << m->toString();
        return os;
    }
};


using Universe=std::vector<std::unique_ptr<Module>>;
Universe universe{};


Universe::iterator findInUniverse(const std::string &name) {
    for (auto it = universe.begin(); it < universe.end(); ++it) {
        if ((*it)->name == name) return it;
    }
    return universe.end();
}


class FlipFlop : public Module {
public:
    bool on;

    FlipFlop(const std::string &n, const std::vector<std::string> &o) : Module(n, o), on(false) {}

    virtual Pulse broadcast(Pulse p) override {
        if (p == Pulse::Low) {
            on = !on;
            if (on)
                last = Pulse::High;
            else
                last = Pulse::Low;
            return last;
        }
        return Pulse::NoPulse;
    }

    virtual std::string toString() override {
        std::ostringstream oss;
        oss << "FlipFlop{name: " << name << ", state: " << (on ? "on" : "off")
            << ", out: [";
        for (const auto & o : outputs) {
            oss << o << (o == *outputs.rbegin() ? "]}" : ", ");
        }
        return oss.str();
    }
};


class Conjunction : public Module {
public:
    std::vector<std::string> inputs;

    Conjunction(const std::string &n, const std::vector<std::string> &out, 
        const std::vector<std::string> &in = {}) 
        : Module(n, out), inputs(in) {}

    virtual Pulse broadcast(Pulse p) override {
        for (const auto &in : inputs) {
            auto it = findInUniverse(in);
            if (it == universe.end()) {
                debug_println("Unknown module with name \"{}\"", in);
            }
            if ((*it)->last == Pulse::Low) {
                last = Pulse::High;
                return last;
            }
        }
        last = Pulse::Low;
        return last;
    }

    virtual std::string toString() override {
        std::ostringstream oss;
        oss << "Conjunction{name: " << name << ", out: [";
        for (const auto & o : outputs) {
            oss << o << (o == *outputs.rbegin() ? "]" : ", ");
        }
        oss << ", in: [";
        for (const auto & i : inputs) {
            oss << i << (i == *inputs.rbegin() ? "]}" : ", ");
        }
        return oss.str();
    }
};


class Broadcaster : public Module {
public:
    Broadcaster(const std::string &n, const std::vector<std::string> &out) : Module(n, out) {}

    virtual Pulse broadcast(Pulse p) override {
        return Pulse::Low;
    }

    virtual std::string toString() override {
        std::ostringstream oss;
        oss << "Broadcaster{name: " << name << ", out: [";
        for (const auto & o : outputs) {
            oss << o << (o == *outputs.rbegin() ? "]}" : ", ");
        }
        return oss.str();
    }
};


class Final : public Module {
public:
    bool on;

    Final(const std::string &n) : Module(n, {}), on(false) {last=Pulse::NoPulse;}

    virtual Pulse broadcast(Pulse p) override {
        if (p == Pulse::Low && !on) on = true; 
        return Pulse::NoPulse;
    }

    virtual std::string toString() override {
        std::ostringstream oss;
        oss << "Final Machine{name: " << name << "}";
        return oss.str();
    }
};


void createUniverse(const input_t &in) {
    for (const auto &line : in) {
        std::istringstream iss;
        usize idx = line.find(" -> ");
        iss.str(line.substr(idx+4));
        iss.clear();
        std::string out_name;
        std::vector<std::string> out;
        while(std::getline(iss, out_name, ' ')) {
            if (out_name.ends_with(',')) 
                out_name.resize(out_name.size()-1);
            out.emplace_back(out_name);
        }

        if (line.starts_with('&')) {
            std::string name;
            iss.str(line.substr(1));
            iss.clear();
            std::getline(iss, name, ' ');
            universe.emplace_back(std::make_unique<Conjunction>(name, out));
        } else if (line.starts_with('%')) {
            std::string name;
            iss.str(line.substr(1));
            iss.clear();
            std::getline(iss, name, ' ');
            universe.emplace_back(std::make_unique<FlipFlop>(name, out));
        } else {
            std::string name;
            iss.str(line);
            iss.clear();
            std::getline(iss, name, ' ');
            universe.emplace_back(std::make_unique<Broadcaster>(name, out));
        }
    }

    // Add inputs for Conjunction
    for (const auto & mod : universe) {
        const auto &out = mod->outputs;
        for (const auto &o : out) {
            auto it =  findInUniverse(o);
            if (it == universe.end()) {
                debug_println("Unknown module with name \"{}\"", o);
                continue;
            }
            Conjunction *p = dynamic_cast<Conjunction*>(it->get());
            if (p) {
                p->inputs.emplace_back(mod->name);
            }
        }
    }
}


void pushButton(i64 &nlow, i64 &nhigh) {
    std::deque<std::pair<Pulse, std::unique_ptr<Module>&>> queue;
    auto it = findInUniverse("broadcaster");
    if (it == universe.end()) {
        debug_println("Broadcaster does not exists in Universe");
    }
    queue.emplace_back(Pulse::Low, *it);

    debug_println("button -low-> broadcaster");
    nlow++; // button push = 1 low
    while (!queue.empty()) {
        auto pair = queue.front();
        queue.pop_front();
        Pulse inPulse = pair.first;
        std::unique_ptr<Module> &mod = pair.second;

        Pulse outPulse = mod->broadcast(inPulse);
        if (outPulse != Pulse::NoPulse) {
            if (outPulse == Pulse::Low) nlow += mod->outputs.size();
            else                        nhigh += mod->outputs.size();
            
            for (const auto &o : mod->outputs) {
                debug_println("{} -{}-> {}", mod->name, outPulse==Pulse::Low?"low":"high", o);
                
                it = findInUniverse(o);
                if (it == universe.end()) {
                    debug_println("Output {} for {} does not exists in Universe", o, mod->toString());
                    continue;
                }
                queue.emplace_back(outPulse, *it);
            }
        }
    }
}


i64 part1(const input_t &in) {
    createUniverse(in);

    for ([[maybe_unused]]const auto &i : universe) {
        debug_println("{}", i->toString());
    }
    debug_println("");

    i64 nlow = 0;
    i64 nhigh = 0;

    #ifdef DEBUG
        #define N 3
    #else
        #define N 1000
    #endif

    for (usize i = 0; i < N; ++i) {
        pushButton(nlow, nhigh);
        debug_println("");
    }

    return nlow * nhigh;
}


i64 findHighCycles(const std::string &mod_name) {
    i64 seen = 0;
    for (u64 i = 1;; ++i) {
        std::deque<std::pair<Pulse, std::unique_ptr<Module>&>> queue;
        auto it = findInUniverse("broadcaster");
        if (it == universe.end()) {
            debug_println("Broadcaster does not exists in Universe");
        }
        queue.emplace_back(Pulse::Low, *it);

        while (!queue.empty()) {
            auto pair = queue.front();
            queue.pop_front();
            Pulse inPulse = pair.first;
            std::unique_ptr<Module> &mod = pair.second;

            Pulse outPulse = mod->broadcast(inPulse);
            
            if (mod_name == mod->name && outPulse == Pulse::High) {
                if (seen == 0)
                    seen = i;
                else
                    return i - seen;
            }
            
            if (outPulse != Pulse::NoPulse) {
                for (const auto &o : mod->outputs) {
                    it = findInUniverse(o);
                    if (it == universe.end()) {
                        continue;
                    }
                    queue.emplace_back(outPulse, *it);
                }
            }
        }
    }
}


i64 lcm(const std::vector<i64> &nums) {
    if (nums.size() == 0) 
        return 0;

    if (nums.size() == 1)
        return nums[0];

    return std::lcm(nums[0], lcm(std::vector(nums.begin()+1, nums.end())));
}


i64 part2(const input_t &in) {
    universe.resize(0);
    createUniverse(in);

    std::vector<std::string> rx_inputs;
    for (const auto &mod : universe) {
        for (const auto &out : mod->outputs) {
            if (out == "rx") {
                rx_inputs.emplace_back(mod->name);
                break;
            }
        }
    }

    #ifdef DEBUG
        rx_inputs = {"con"};
    #endif

    if (rx_inputs.size() != 1) {
        std::cout << "\"rx\" input is not one(found "
            << rx_inputs.size() << ")." << std::endl;
        exit(0);
    }

    std::vector<std::string> rx_inputs_depth2{};
    for (const auto &mod : universe) {
        for (const auto &out : mod->outputs) {
            if (out == rx_inputs[0]) {
                rx_inputs_depth2.emplace_back(mod->name);
                break;
            }
        }
    }

    std::vector<i64> cycles{};
    for (const auto &in : rx_inputs_depth2) {
        cycles.emplace_back(findHighCycles(in));
        debug_println("cycles of \"{}\": {}", in, *cycles.rbegin());
        std::cout << std::format("cycles of \"{}\": {}\n", in, *cycles.rbegin());
    }

    return lcm(cycles);
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }
    input_t lines = parseInputs(argv[1]);

    auto res1 = part1(lines);
    std::cout << "-----PART 1-----\n";
    std::cout << "Result = " << res1 << std::endl;
    
    auto res2 = part2(lines);
    std::cout << "-----PART 2-----\n";
    std::cout << "Result = " << res2 << std::endl;

    return 0;
}