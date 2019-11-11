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

    virtual Action take_action(const Board &board, const Action &opponent_action) { return Action(); }

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
        num_moves = 0;
    }

    virtual void close_episode(const std::string &flag = "") {
        bag.assign({1, 2, 3});
    }

    unsigned opponent_type() override {
        return Action::Slide::type;
    }

    virtual Action take_action(const Board &board, const Action &opponent_action) override {

        std::vector<int> *tiles = &space;
        if (num_moves < 9) { tiles = &space; }  // first 9-moves
        else if (opponent_action.event() == 0) { tiles = &bottom; }  // up
        else if (opponent_action.event() == 1) { tiles = &left;  }   // right
        else if (opponent_action.event() == 2) { tiles = &top;  }    // down
        else if (opponent_action.event() == 3) { tiles = &right;  }  // left

        if (bag.empty()) bag.assign({1, 2, 3});
        std::shuffle(bag.begin(), bag.end(), engine);
        std::shuffle(tiles->begin(), tiles->end(), engine);

        num_moves++;

        for (auto it = tiles->begin(); it < tiles->end(); it++) {
            if (board.get_cell(*it) != 0) continue;
            Board::Cell tile = bag.back();   bag.pop_back();
            return Action::Place(*it, tile);
        }
        return Action();
}

private:
    std::vector<int> space;
    std::vector<int> top, bottom, left, right;
    std::vector<int> bag;
    int num_moves;
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

    virtual Action take_action(const Board &board, const Action &opponent_action) {
        std::shuffle(opcode.begin(), opcode.end(), engine);
        for (int op : opcode) {
            Board::Reward reward = Board(board).slide(op);
            if (reward != -1) return Action::Slide(op);
        }
        return Action();
    }

private:
    std::array<int, 4> opcode;
};

class TDPlayer : public WeightAgent {

public:
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
    int tree_depth;

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

    int play_mode;
    std::map<unsigned long long, int> v_map;

public:
    TDPlayer(const std::string &args = "") : WeightAgent(args) {
        num_tuple = 4;  tuple_len = 6;  num_tile = 15;
        num_player_action = 4;   num_evil_action = 12;  tree_depth = 2;
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

        if (play_mode == 1) generate_tree(root, tree_depth);
    }

    virtual void open_episode(const std::string &flag = "") {
        ep.clear();
    }

    int get_LUT_index(const Board& s, int index) {
        int res = 0, base = 1;
        for (int i = 0; i < tuple_len; i++) {
            res += int(s.get_cell(tuple_index[index][i])) * base;
            base *= num_tile;
        }
        return res;
    }

    float get_v(const Board& s) {
//        float res = 0;
//        for(int i = 0; i < num_tuple; i++) {
//            int LUT_id = get_LUT_index(s, i);
//            res += net[i][LUT_id];
//        }
//        return res;
        float res = 0;
        unsigned int id = 0;
        auto t = s.get_tile();

       // debug(s);

        id = t & 0xffffffull; // debug(id);  debug(Board::pre_id[id]);
        res += net[0][Board::pre_id[id]];
        id = (t >> 16ull) & 0xffffffull;  // debug(id);  debug(Board::pre_id[id]);
        res += net[1][Board::pre_id[id]];
        id = ((t >> 20ull) & 0xfffull) | ((t >> 24ull) & 0xfff000ull);  // debug(id);  debug(Board::pre_id[id]);
        res += net[2][Board::pre_id[id]];
        id = ((t >> 36ull) & 0xfffull) | ((t >> 40ull) & 0xfff000ull);  // debug(id);  debug(Board::pre_id[id]);
        res += net[3][Board::pre_id[id]];

        return res;
    }

    void update_v(const Board& s, float value) {
        for(int i = 0; i < num_tuple; i++) {
            int LUT_id = get_LUT_index(s, i);
            net[i][LUT_id] += value;
        }
    }

    Board::Reward get_reward(Board::Reward before_action, Board::Reward after_action) {
        return after_action - before_action;
    }

    Board::Reward compute_afterstate(Board& s, const Action::Slide& a) {
        Board::Reward before = s.get_curr_score();
        a.apply(s);
        Board::Reward after = s.get_curr_score();
        return get_reward(before, after);
    }

    /**
     * Return -1 if there is no available move.
     * Otherwise return op (0, 1, 2, 3)
     */
    std::pair<int, Board::Reward> get_best_move(const Board &before) {
        int max_op = -1;
        float max_reward = 0;

        for(int op = 0; op < 4; op++) {

            // check if this action is valid
            Board after(before);
            Action::Slide a(op);
            if (a.apply(after) == -1) continue;

            // if valid, get the evaluation of 'before' after taking action 'a'
            float curr_reward = get_reward(before.get_curr_score(), after.get_curr_score()) + get_v(after);
            if (max_op == -1) {
                max_reward = curr_reward;
                max_op = op;
            } else if (max_reward < curr_reward) {
                max_reward = curr_reward;
                max_op  = op;
            }
        }
        return {max_op, max_reward};
    }

    void learn_evaluation(const Board& s_prime, Board::Reward r_prime, const Board& s_prime_next) {
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
            ep.pop_back();
        }
    }

    void generate_tree(Node& currNode, int d) {
        if (d == 0) return;

        if (d % 2 == 0) {  // max node
            for(int i = 0; i < num_player_action; i++) {
                Node* child = new Node();
                currNode.children.push_back(child);
                generate_tree(*child, d - 1);
            }
            return;
        }

        if (d % 2 == 1) {  // min node
            for(int i = 0; i < num_evil_action; i++) {
                Node* child = new Node();
                currNode.children.push_back(child);
                generate_tree(*child, d - 1);
            }
            return;
        }
    }

    int expectminimax_search(Node* curr_node, int d) {
 //       debug(curr_node->board);
        if (d == 0) {  // leaf node (max node)
            auto best_move = get_best_move(curr_node->board);
            curr_node->value = best_move.second;
            return best_move.first;
        }

        if (d % 2 == 0) {  //  max node
            double curr_value = 0;
            int curr_op = -1;

            for(int op = 0; op < 4; op++) {

                Node* next_node = curr_node->children[op];
                next_node->board = curr_node->board;
                next_node->last_op = op;
                Action::Slide slide(op);

                int valid = slide.apply(next_node->board);
                if (valid == -1) continue;

                Board::Reward r = get_reward(curr_node->board.get_curr_score(), next_node->board.get_curr_score());
                expectminimax_search(next_node, d - 1);
                if (next_node->value + r > curr_value || curr_op == -1) {
                    curr_value = next_node->value + r;
                    curr_op = op;
                }
            }

            curr_node->value = curr_value;
            return curr_op;
        }

        if (d % 2 == 1) {  // chance node
            double expect_value = 0.0;
            int num_valid = 0;

            for(int tile = 1; tile <= 3; tile++) {
                for(int pos_id = 0; pos_id < 4; pos_id++) {

                    Node* next_node = curr_node->children[4 * (tile - 1) + pos_id];
                    next_node->board = curr_node->board;
                    int pos = place_pos[curr_node->last_op][pos_id];

                    int valid = next_node->board.place(pos, tile);
                    if (valid == -1) continue;

                    expectminimax_search(next_node, d - 1);
                    expect_value = expect_value + next_node->value;
                    num_valid++;
                }
            }

            curr_node->value = expect_value / num_valid;
            return -1;
        }
    }

    virtual Action take_action(const Board &board, const Action &opponent_action) {

        if (play_mode == 0) {  // training mode
            int max_op = get_best_move(board).first;
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
            int max_op = expectminimax_search(&root, tree_depth);
            if (max_op == -1) return Action();

            Action::Slide max_action(max_op);
            return max_action;
        }
    }
};