#pragma once

#include <string>
#include <random>
#include <sstream>
#include <map>
#include <type_traits>
#include <algorithm>
#include <fstream>
#include <chrono>

#include "board.h"
#include "action.h"
#include "weight.h"

#define debug(a) std::cout << #a << " = " << a << std::endl

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

    virtual Action take_action(const Board &board, const std::vector<Action> &opponent_actions) { return Action(); }

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
    virtual void init_weights(const std::string &info) {
        net.emplace_back(65536); // create an empty weight table with size 65536
        net.emplace_back(65536); // create an empty weight table with size 65536
        // now net.size() == 2; net[0].size() == 65536; net[1].size() == 65536
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
                                              right({3, 7, 11, 15}) {
        engine.seed(std::chrono::system_clock::now().time_since_epoch().count());
    }

    virtual void close_episode(const std::string &flag = "") {
        bag.assign({1, 2, 3});
    }

    unsigned opponent_type() override {
        return Action::Slide::type;
    }

    Action take_action(const Board &board, const std::vector<Action> &opponent_actions) override {

        // Choose tiles to generate
        std::vector<int> *tiles = &space;
        if (opponent_actions.empty()) { tiles = &space; }  // first 9-moves
        else if (opponent_actions.back().event() == 0) { tiles = &bottom; }  // up
        else if (opponent_actions.back().event() == 1) { tiles = &left;  }   // right
        else if (opponent_actions.back().event() == 2) { tiles = &top;  }    // down
        else if (opponent_actions.back().event() == 3) { tiles = &right;  }  // left

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

class TDPlayer : public WeightAgent {

private:
    int num_tuple;
    int tuple_len;
    int num_tile;

    struct State {
        Board board;
        Board::Reward reward;
    };

    std::vector<State> ep;


    float learning_rate;
    int tuple_index[4][6] = {
            { 0,  1,  2,  3,  4,  5},
            { 4,  5,  6,  7,  8,  9},
            { 5,  6,  7,  9, 10, 11},
            { 9, 10, 11, 13, 14, 15},
    };

    int num_player_action;
    int num_evil_action;

    struct Node {
        double value;
        Board board;
        int last_op;
        std::vector<Node*> children;

        Node() {
            value = 0;
            last_op = -1;
            board = Board();
        }
    };

    Node root;

    int place_pos[4][4] = {
            {12, 13, 14, 15},
            {0,  4,  8,  12},
            {0,  1,  2,  3},
            {3,  7,  11, 15}
    };

public:
    void generate_tree(Node& currNode, int d) {
        if (d == 0) {  // leaf
        } else {
            if (d % 2 == 1) {  // max node
                for(int i = 0; i < num_player_action; i++) {
                    Node* child = new Node();
                    currNode.children.push_back(child);
                    generate_tree(*child, d - 1);
                }
            } else {  // chance node
                for(int i = 0; i < num_evil_action; i++) {
                    Node* child = new Node();
                    currNode.children.push_back(child);
                    generate_tree(*child, d - 1);
                }
            }
        }
    }

    Board::Reward expectminimax_search(Node currNode, int d) {
        if (d == 0) {
            debug(currNode.board);
            return get_v(currNode.board);
        } else {
            if (d % 2 == 1) {  // max node
                Board::Reward max_value = -1000000000;
                for(int op = 0; op < 4; op++) {
                    Node nextNode = *(currNode.children[op]);
                    nextNode.board = currNode.board;
                    nextNode.last_op = op;
                    int valid = Action::Slide(op).apply(nextNode.board);
                    if (valid == -1) continue;

                    Board::Reward search_value = expectminimax_search(nextNode, d - 1);
                    max_value = std::max(max_value, search_value);
                }
                currNode.value = max_value;
                return max_value;
            } else {    // chance node
                Board::Reward expect_value = 0;
                for(int tile = 1; tile <= 3; tile++) {
                    for(int pos_id = 0; pos_id < 4; pos_id++) {
                        Node nextNode = *(currNode.children[4 * (tile - 1) + pos_id]);
                        int pos = place_pos[currNode.last_op][pos_id];
                        int valid = nextNode.board.place(pos, tile);
                        if (valid == -1) continue;

                        Board::Reward search_value = expectminimax_search(nextNode, d - 1);
                        expect_value = expect_value + (1.0 / 12) * search_value;
                    }
                }
                currNode.value = expect_value;
                return expect_value;
            }
        }
    }

    int play_mode;
public:
    TDPlayer(const std::string &args = "") : WeightAgent(args) {
        num_tuple = 4;  tuple_len = 6;  num_tile = 15;
        num_player_action = 4;   num_evil_action = 12;
        learning_rate = 0.00025;

        if (meta.find("mode") != meta.end()) {
            if (meta["mode"].value == "train") play_mode = 0;
            else play_mode = 1;
        }

        int num_element = 1;
        for(int i = 0; i < tuple_len; i++) num_element *= num_tile;
        if (net.empty()) {
            for (int i = 0; i < num_tuple; i++)
                net.emplace_back(num_element, 0);  // create 4 tables for 6-tuple network
        }

        generate_tree(root, 3);
    }

    virtual void open_episode(const std::string &flag = "") {
        ep.clear();
    }

    int get_LUT_index(const Board& s, int index) {
        int res = 0, base = 1;
        for (int i = 0; i < tuple_len; i++) {
            res += s(tuple_index[index][i]) * base;
            base *= num_tile;
        }
        return res;
    }

    float get_v(const Board& s) {
        float res = 0;
        for(int i = 0; i < net.size(); i++) {
            int LUT_id = get_LUT_index(s, i);
            res += net[i][LUT_id];
        }
        return res;
    }

    void update_v(const Board& s, float value) {
        for(int i = 0; i < net.size(); i++) {
            int LUT_id = get_LUT_index(s, i);
            net[i][LUT_id] += value;
        }
    }

    Board::Reward get_reward(Board::Reward before_action, Board::Reward after_action) {
        return after_action - before_action;
    }

    Board::Reward compute_afterstate(Board& s, const Action::Slide& a) {
        Board::Reward score01 = s.get_curr_score();
        a.apply(s);
        Board::Reward score02 = s.get_curr_score();
        return get_reward(score01, score02);
    }

    Board::Reward evaluation(const Board& s, const Action::Slide& a) {
        Board s_prime(s);
        Board::Reward r = compute_afterstate(s_prime, a);
        return r + get_v(s_prime);
    }

    /**
     * Return -1 if there is no available move.
     * Otherwise return op (0, 1, 2, 3)
     */
    int get_max_op(const Board &board) {
        int max_op = -1;
        float max_reward = 0;

        for(int op = 0; op < 4; op++) {

            // check if this action is valid
            Board tmp(board);
            Action::Slide a(op);
            if (a.apply(tmp) == -1) continue;

            // if valid, get the evaluation of 'board' after taking action 'a'
            float curr_reward = evaluation(board, a);
            if (max_op == -1) {
                max_reward = curr_reward;
                max_op = op;
            } else if (max_reward < curr_reward) {
                max_reward = curr_reward;
                max_op  = op;
            }
        }
        return max_op;
    }

    void learn_evaluation(const Board& s_prime, Board::Reward r_prime, const Board& s_prime_next) {

        //        std::cout << "s_prime: " << std::endl << s_prime << std::endl;
        //        std::cout << "s_double_prime: " << std::endl << s_double_prime << std::endl;

        float update_value = learning_rate * (r_prime + get_v(s_prime_next) - get_v(s_prime));
        update_v(s_prime, update_value);
    }

    void td_training() {

        // terminal state
        float update_value = learning_rate * (0 - get_v(ep.back().board));
        update_v(ep.back().board, update_value);

        // other states

        while (ep.size() > 1) {
            int last_id = ep.size() - 1;
            learn_evaluation(ep[last_id - 1].board, ep[last_id].reward, ep[last_id].board);

//            ep[last_id - 1].board.rotate_right();  ep[last_id].board.rotate_right();
//            learn_evaluation(ep[last_id - 1].board, ep[last_id].reward, ep[last_id].board);
//
//            ep[last_id - 1].board.rotate_right();  ep[last_id].board.rotate_right();
//            learn_evaluation(ep[last_id - 1].board, ep[last_id].reward, ep[last_id].board);
//
//            ep[last_id - 1].board.rotate_right();  ep[last_id].board.rotate_right();
//            learn_evaluation(ep[last_id - 1].board, ep[last_id].reward, ep[last_id].board);
//
//            ep[last_id - 1].board.rotate_right();  ep[last_id].board.rotate_right();
//
//            // new round
//            ep[last_id - 1].board.reflect_vertical();  ep[last_id].board.reflect_vertical();
//            learn_evaluation(ep[last_id - 1].board, ep[last_id].reward, ep[last_id].board);
//
//            ep[last_id - 1].board.rotate_right();  ep[last_id].board.rotate_right();
//            learn_evaluation(ep[last_id - 1].board, ep[last_id].reward, ep[last_id].board);
//
//            ep[last_id - 1].board.rotate_right();  ep[last_id].board.rotate_right();
//            learn_evaluation(ep[last_id - 1].board, ep[last_id].reward, ep[last_id].board);
//
//            ep[last_id - 1].board.rotate_right();  ep[last_id].board.rotate_right();
//            learn_evaluation(ep[last_id - 1].board, ep[last_id].reward, ep[last_id].board);
//
//            ep[last_id - 1].board.rotate_right();      ep[last_id].board.rotate_right();
//            ep[last_id - 1].board.reflect_vertical();  ep[last_id].board.reflect_vertical();

            ep.pop_back();
        }
    }

    virtual Action take_action(const Board &board, const std::vector<Action> &actions) {

        if (play_mode == 0) {  // training mode
            int max_op = get_max_op(board);
            if (max_op == -1) return Action();

            Board afterstate(board);
            Action::Slide max_action(max_op);
            Board::Reward r = compute_afterstate(afterstate, max_action);

            State state;
            state.board = afterstate;
            state.reward = r;
            ep.push_back(state);

            return max_action;

        } else {  // playing mode
            root.board = board;
            expectminimax_search(root, 3);

            int max_op = -1;
            double max_score = -1;
            for(int op = 0; op < num_player_action; op++) {
                if (root.children[op]->value > max_score || max_score == -1) {
                    max_op = op;
                    max_score = root.children[op]->value;
                }
            }

            Board afterstate(board);
            Action::Slide max_action(max_op);
            return max_action.apply(afterstate) == -1 ? Action() : max_action;
        }
    }
};