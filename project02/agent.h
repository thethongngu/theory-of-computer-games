#pragma once

#include <string>
#include <random>
#include <sstream>
#include <map>
#include <type_traits>
#include <algorithm>
#include "board.h"
#include "action.h"
#include "weight.h"
#include <fstream>

class Agent {
public:
    Agent(const std::string &args = "") {
        std::stringstream ss("name=unknown role=unknown " + args);
        for (std::string pair; ss >> pair;) {
            std::string key = pair.substr(0, pair.find('='));
            std::string value = pair.substr(pair.find('=') + 1);
            meta[key] = {value};
        }
    }

    virtual ~Agent() {}

    virtual void open_episode(const std::string &flag = "") {}

    virtual void close_episode(const std::string &flag = "") {}

    virtual Action take_action(const Board &board, const std::vector<Action> &actions) { return Action(); }

    virtual bool check_for_win(const Board &b) { return false; }

    virtual unsigned opponent_type() {};

public:
    virtual std::string property(const std::string &key) const { return meta.at(key); }

    virtual void notify(const std::string &msg) {
        meta[msg.substr(0, msg.find('='))] = {msg.substr(msg.find('=') + 1)};
    }

    virtual std::string name() const { return property("name"); }

    virtual std::string role() const { return property("role"); }

protected:
    typedef std::string key;

    struct value {
        std::string value;

        operator std::string() const { return value; }

        template<typename numeric, typename = typename std::enable_if<std::is_arithmetic<numeric>::value, numeric>::type>
        operator numeric() const { return numeric(std::stod(value)); }
    };

    std::map<key, value> meta;
};

class RandomAgent : public Agent {
public:

    RandomAgent(const std::string &args = "") : Agent(args) {
        if (meta.find("seed") != meta.end())
            engine.seed(int(meta["seed"]));
    }

    virtual ~RandomAgent() {}

protected:
    std::default_random_engine engine;
};

/**
 * base agent for agents with weight tables
 */
class WeightAgent : public Agent {
public:

    virtual unsigned action_type() {
        return Action::Slide::type;
    }

    WeightAgent(const std::string &args = "") : Agent(args) {
        if (meta.find("init") != meta.end()) // pass init=... to initialize the weight
            init_weights(meta["init"]);
        if (meta.find("load") != meta.end()) // pass load=... to load from a specific file
            load_weights(meta["load"]);
    }

    virtual ~WeightAgent() {
        if (meta.find("save") != meta.end()) // pass save=... to save to a specific file
            save_weights(meta["save"]);
    }

protected:
    virtual void init_weights(const std::string &info) {
        net.emplace_back(65536); // create an empty weight table with size 65536
        net.emplace_back(65536); // create an empty weight table with size 65536
        // now net.size() == 2; net[0].size() == 65536; net[1].size() == 65536
    }

    virtual void load_weights(const std::string &path) {
        std::ifstream in(path, std::ios::in | std::ios::binary);
        if (!in.is_open()) std::exit(-1);
        uint32_t size;
        in.read(reinterpret_cast<char *>(&size), sizeof(size));
        net.resize(size);
        for (weight &w : net) in >> w;
        in.close();
    }

    virtual void save_weights(const std::string &path) {
        std::ofstream out(path, std::ios::out | std::ios::binary | std::ios::trunc);
        if (!out.is_open()) std::exit(-1);
        uint32_t size = net.size();
        out.write(reinterpret_cast<char *>(&size), sizeof(size));
        for (weight &w : net) out << w;
        out.close();
    }

protected:
    std::vector<weight> net;
};

/**
 * base agent for agents with a learning rate
 */
class LearningAgent : public Agent {
public:

    virtual unsigned action_type() {
        return Action::Slide::type;
    }

    LearningAgent(const std::string &args = "") : Agent(args), alpha(0.1f) {
        if (meta.find("alpha") != meta.end())
            alpha = float(meta["alpha"]);
    }

    virtual ~LearningAgent() {}

protected:
    float alpha;
};

/**
 * random environment
 * add a new random tile to an empty cell
 * 2-tile: 90%
 * 4-tile: 10%
 */
class RandomEnv : public RandomAgent {
public:
    RandomEnv(const std::string &args = "") : RandomAgent("name=random role=environment " + args),
                                              space({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}),
                                              bag({1, 2, 3}),
                                              top({0, 1, 2, 3}),
                                              bottom({12, 13, 14, 15}),
                                              left({0, 4, 8, 12}),
                                              right({3, 7, 11, 15}),
                                              popup(1, 3) {}

    virtual void close_episode(const std::string &flag = "") {
        bag.assign({1, 2, 3});
    }

    unsigned opponent_type() override {
        return Action::Slide::type;
    }

    Action take_action(const Board &board, const std::vector<Action> &actions) override {

        // Choose tiles to generate
        std::vector<int> *tiles = &space;
        if (actions.empty()) {  tiles = &space; }  // first 9-moves
        else if (actions.back().event() == 0) { tiles = &bottom; }  // up
        else if (actions.back().event() == 1) { tiles = &left;  }   // right
        else if (actions.back().event() == 2) { tiles = &top;  }    // down
        else if (actions.back().event() == 3) { tiles = &right;  }  // left

        if (bag.empty()) bag.assign({1, 2, 3});
        std::shuffle(bag.begin(), bag.end(), engine);
        std::shuffle(tiles->begin(), tiles->end(), engine);

        for (auto it = tiles->begin(); it < tiles->end(); it++) {
            if (board(*it) != 0) continue;
            Board::Cell tile = bag.back();   bag.pop_back();
            return Action::Place(*it, tile);
        }
        return Action();
}

private:
    std::vector<int> space;
    std::vector<int> top, bottom, left, right;
    std::uniform_int_distribution<int> popup;
    std::vector<int> bag;
};

/**
 * dummy player
 * select a legal action randomly
 */
class Player : public RandomAgent {
public:
    Player(const std::string &args = "") : RandomAgent("name=dummy role=player " + args),
                                           opcode({0, 1, 2, 3}) {}

    unsigned opponent_type() override {
        return Action::Place::type;
    }

    virtual Action take_action(const Board &before, const std::vector<Action> &actions) {
        std::shuffle(opcode.begin(), opcode.end(), engine);
        for (int op : opcode) {
            Board::Reward reward = Board(before).slide(op);
            if (reward != -1) return Action::Slide(op);
        }
        return Action();
    }

private:
    std::array<int, 4> opcode;
};
