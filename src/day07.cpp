#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <numeric>
#include <cstdint>
#include <algorithm>
#include <unordered_map>


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

enum class HandType {
    HighCard = 1,
    OnePair = 2,
    TwoPair = 3,
    ThreeOfAKind = 4,
    FullHouse = 5,
    FourOfAKind = 6,
    FiveOfAKind = 7,
};

std::pair<char,uint32_t> getMostFreqCard(const std::unordered_map<char, uint32_t> &handmap) {
    return *std::max_element(handmap.begin(), handmap.end(),
        [] (
            const std::pair<char, int>& a,
            const std::pair<char, int>& b) -> 
            bool{ return a.second < b.second; }
    );
}

HandType getHandType(const std::string &hand) {
    std::unordered_map<char, uint32_t> hand_map{};

    for (char c : hand) {
        if(hand_map.contains(c)) {
           hand_map[c]++; 
        } else {
            hand_map[c] = 1;
        }
    }

    switch (hand_map.size()) {
    case 1:
        return HandType::FiveOfAKind;
    case 2:
        if (getMostFreqCard(hand_map).second == 4) {
            return HandType::FourOfAKind;
        }
        
        return HandType::FullHouse;
    case 3:
        if (getMostFreqCard(hand_map).second == 3) {
            return HandType::ThreeOfAKind;
        }
        
        return HandType::TwoPair;
    case 4:
        return HandType::OnePair;
    case 5:
        return HandType::HighCard;
    default:
        std::cout << "Unreachable!" << std::endl;
        exit(1);
        break;
    }
}


struct Hand {
    std::string hand;
    HandType type;
    int bet;
    std::string cardsorder;
};


std::string getHandTypeName(HandType hand) {
    switch (hand) {
    case HandType::HighCard:
        return "High Card";
    case HandType::OnePair:
        return "One Pair";
    case HandType::TwoPair:
        return "Two Pair";
    case HandType::ThreeOfAKind:
        return "Three of a Kind";
    case HandType::FullHouse:
        return "Full House";
    case HandType::FourOfAKind:
        return "Four of a Kind";
    case HandType::FiveOfAKind:
        return "Five of a Kind";
    
    default:
        std::cout << "Unreachable!" << std::endl;
        exit(1);
    }
}

bool operator<(Hand a, Hand b) {
    if (a.type != b.type) {
        return a.type < b.type;
    }
    
    const std::string &order = a.cardsorder;
    for (size_t i = 0; i < 5; ++i) {
        if (a.hand[i] == b.hand[i]) continue;
        const char ac = a.hand[i]; 
        const char bc = b.hand[i]; 
        return order.find(ac) < order.find(bc);
    }

    // Unreachable
    std::cout << "Unreachable in less than reached!\n";
    return true;
}

int part1(const input_t &in) {
    const std::string CARDS_ORDER = "23456789TJQKA";
    std::vector<Hand> hands{};
    #ifdef DEBUG
    std::cout << "Unsorted:\n";
    #endif
    for (const std::string &line : in) {
        std::istringstream iss(line);
        std::string hand;
        int bet;
        std::getline(iss, hand, ' ');
        iss >> bet;
        HandType type = getHandType(hand);
        
        #ifdef DEBUG
        std::cout << "Hand: " <<  hand << ", Bet: " << bet << ", Type: " << getHandTypeName(type) << "\n";
        #endif
        hands.emplace_back(std::move(hand), type, bet, CARDS_ORDER);
    }

    // Money printing part
    int winnings = 0;
    std::sort(hands.begin(), hands.end());
    #ifdef DEBUG
    std::cout << "\nSorted:\n";
    #endif
    for (size_t i = 0; i < hands.size(); i++) {
        const Hand &h = hands[i];
        winnings += h.bet * (i + 1);
        #ifdef DEBUG
        std::cout << "Hand: " <<  h.hand << ", Bet: " << h.bet << ", Type: " << getHandTypeName(h.type) << "\n";
        #endif
    }

    return winnings;
}

HandType getHandType2(const std::string &hand) {
    std::unordered_map<char, uint32_t> hand_map{};

    for (char c : hand) {
        if(hand_map.contains(c)) {
           hand_map[c]++; 
        } else {
            hand_map[c] = 1;
        }
    }

    switch (hand_map.size()) {
    case 1:
        return HandType::FiveOfAKind;
    case 2:
        if (getMostFreqCard(hand_map).second == 4) {
            if (hand_map.contains('J'))
                return HandType::FiveOfAKind;
            return HandType::FourOfAKind;
        }

        if (hand_map.contains('J'))
            return HandType::FiveOfAKind;
        return HandType::FullHouse;
    case 3:
        if (getMostFreqCard(hand_map).second == 3) {
            if (hand_map.contains('J'))
                return HandType::FourOfAKind;
            return HandType::ThreeOfAKind;
        }
        
        if (hand_map.contains('J')) { 
            if (hand_map['J'] == 1)
                return HandType::FullHouse;
            if (hand_map['J'] == 2)
                return HandType::FourOfAKind ;
        }
        return HandType::TwoPair;
    case 4:
        if (hand_map.contains('J')) 
            return HandType::ThreeOfAKind;

        return HandType::OnePair;
    case 5:
        if (hand_map.contains('J'))
            return HandType::OnePair;
        return HandType::HighCard;
    default:
        std::cout << "Unreachable!" << std::endl;
        exit(1);
        break;
    }
}

int part2(const input_t &in) {
    const std::string CARDS_ORDER = "J23456789TQKA";
    std::vector<Hand> hands{};
    #ifdef DEBUG
    std::cout << "Unsorted:\n";
    #endif
    for (const std::string &line : in) {
        std::istringstream iss(line);
        std::string hand;
        int bet;
        std::getline(iss, hand, ' ');
        iss >> bet;
        HandType type = getHandType2(hand);
        
        #ifdef DEBUG
        std::cout << "Hand: " <<  hand << ", Bet: " << bet << ", Type: " << getHandTypeName(type) << "\n";
        #endif
        hands.emplace_back(std::move(hand), type, bet, CARDS_ORDER);
    }

    // Money printing part
    int winnings = 0;
    std::sort(hands.begin(), hands.end());
    #ifdef DEBUG
    std::cout << "\nSorted:\n";
    #endif
    for (size_t i = 0; i < hands.size(); i++) {
        const Hand &h = hands[i];
        winnings += h.bet * (i + 1);
        #ifdef DEBUG
        std::cout << "Hand: " <<  h.hand << ", Bet: " << h.bet << ", Type: " << getHandTypeName(h.type) << "\n";
        #endif
    }

    return winnings;
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    input_t lines = parse_inputs(argv[1]);
    // Apparently we are printing money in this challenge. Just how I like it.
    int res1 = part1(lines);
    int res2 = part2(lines);

    std::cout << "-----PART 1-----\n";
    std::cout << "Total winnings = " << res1 << std::endl;
    std::cout << "-----PART 2-----\n";
    std::cout << "Total winnings = " << res2 << std::endl;

    return 0;
}