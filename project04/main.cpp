#include <iostream>
#include <cassert>
#include <vector>
#include <array>
#include <sstream>
#include <iterator>
#include <cmath>
#include <iomanip>

#define debug(a) std::cout << #a << " = " << a << std::endl
#define ull unsigned long long
#define ui  unsigned int

/** ----------------- Board & Region ----------------------- */
const int BOARD_SIZE = 9;
const int NUM_CELL = BOARD_SIZE * BOARD_SIZE;

const int NONE = 0;
const int BLACK = 1;
const int WHITE = 2;

int bsft[64];

/**
 *   a b c d e f g h i
 * 1 + + + + + + + + +
 * 2 + + + + + + + + +
 * 3 + + + + + + + + +
 * 4 + + + + + + + + +
 * 5 + + + + + + + + +
 * 6 + + + + + + + + +
 * 7 + + + + + + + + +
 * 8 + + + + + + + + +
 * 9 + + + + + + + + +
 *
 *             i7 <---------------------------------------------------------------------- a1
 * first_seg: 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000
 *
 *              i9 <------------------------------ a8
 * second_seg: 0000 0000 0000 0000 0000 0000 0000 0000
 *
 * first_seg, second_seg:
 *  - 0: BLACK
 *  - 1: WHITE
 *
 * first_flag & second_flag:
 *  - 0: empty
 *  - 1: not empty
 *
 */

void init_bsf_table() {
    for (int i = 0; i < 64; i++)
        bsft[((1ULL << i) * 0x218A392CD3D5DBFULL) >> 58] = i;
}

ull count_on_bit(ull i) {
    i = i - ((i >> 1ULL) & 0x5555555555555555ULL);
    i = (i & 0x3333333333333333ULL) + ((i >> 2ULL) & 0x3333333333333333ULL);
    return (((i + (i >> 4ULL)) & 0xF0F0F0F0F0F0F0FULL) * 0x101010101010101ULL) >> 56;
}

void get_adj_cells(int pos, std::vector<int> &res) {
    assert(pos >= 0 && pos <= NUM_CELL - 1);

    int d[4] = {-BOARD_SIZE, +1, +BOARD_SIZE, -1};
    res.clear();

    for (int i = 0; i < 4; i++) {
        int new_pos = pos + d[i];
        if (new_pos >= 0 && new_pos <= NUM_CELL - 1) {
            if (pos % BOARD_SIZE == 0 && d[i] == -1) continue;
            if (pos % BOARD_SIZE == 8 && d[i] == 1) continue;
            res.push_back(new_pos);
        }
    }
}

int get_oppo_color(int color) {
    assert(color != NONE);
    return (color == BLACK) ? WHITE : BLACK;
}

struct Region {

    // region info
    ull first_seg = 0;
    ui second_seg = 0;
    ull first_flag = 0;
    ui second_flag = 0;

    // liberty info
    ull first_lib = 0;
    ui second_lib = 0;

};

struct Board {
    ull first_seg = 0;
    ui second_seg = 0;
    ull first_flag = 0;
    ui second_flag = 0;

    std::vector<Region> regions;
    int cell_to_region[NUM_CELL];
    std::vector<int> valid_cells;
};

void reset_region(Region &region) {
    region.first_seg = region.second_seg = region.first_flag = region.second_flag = 0;
    region.first_lib = region.second_lib = 0;
}

void reset_board(Board &board) {
    board.first_seg = board.second_seg = board.first_flag = board.second_flag = 0;
    board.regions.clear();
    for (int &cell : board.cell_to_region) cell = -1;
    for (int i = 0; i < NUM_CELL; i++) board.valid_cells.push_back(i);
}

/**
 * - Change the segment to BLACK (0)
 * - Change the flag to 1
 */
void add_black_bit_region(Region &region, int i) {
    assert(i >= 0 && i <= NUM_CELL - 1);
    if (i < 64) {
        region.first_seg &= ~(1ULL << i);
        region.first_flag |= (1ULL << i);
    } else {
        region.second_seg &= ~(1ULL << i);
        region.second_flag |= (1ULL << i);
    }
}

/**
 * - Change the segment to WHITE (1)
 * - Change the flag to 1
 */
void add_white_bit_region(Region &region, int i) {
    assert(i >= 0 && i <= NUM_CELL - 1);
    if (i < 64) {
        region.first_seg |= (1ULL << i);
        region.first_flag |= (1ULL << i);
    } else {
        region.second_seg |= (1ULL << i);
        region.second_flag |= (1ULL << i);
    }
}

/**
 * - Change the segment to BLACK (0)
 * - Change the flag to 1
 */
void add_black_bit_board(Board &board, int i) {
    assert(i >= 0 && i <= NUM_CELL - 1);
    if (i < 64) {
        board.first_seg &= ~(1ULL << i);
        board.first_flag |= (1ULL << i);
    } else {
        board.second_seg &= ~(1ULL << i);
        board.second_flag |= (1ULL << i);
    }
}

/**
 * - Change the segment to BLACK (0)
 * - Change the flag to 1
 */
void add_while_bit_board(Board &board, int i) {
    assert(i >= 0 && i <= NUM_CELL - 1);
    if (i < 64) {
        board.first_seg |= (1ULL << i);
        board.first_flag |= (1ULL << i);
    } else {
        board.second_seg |= (1ULL << i);
        board.second_flag |= (1ULL << i);
    }
}

/**
 * - Change the segment to corresponding color
 * - Change the flag to 1
 * O(1)
 */
void add_bit_region(Region &region, int pos, int color) {
    assert(color == BLACK || color == WHITE);
    assert(pos >= 0 && pos <= NUM_CELL - 1);

    if (color == BLACK) add_black_bit_region(region, pos);
    else add_white_bit_region(region, pos);
}

/**
 * - Change the segment to corresponding color
 * - Change the flag to 1
 * O(1)
 */
void add_bit_board(Board &board, int pos, int color) {
    assert(color == BLACK || color == WHITE);
    assert(pos >= 0 && pos <= NUM_CELL - 1);

    if (color == BLACK) add_black_bit_board(board, pos);
    else add_while_bit_board(board, pos);
}

int get_region_id_by_cell(const Board &board, int pos) {
    return board.cell_to_region[pos];
}

/**
 * - Change the segment to 0
 * - Change the flag to 0
 * O(1)
 */
void remove_cell_on_board(Board &board, int i) {
    assert(i >= 0 && i <= NUM_CELL - 1);
    if (i < 64) {
        board.first_seg &= ~(1ULL << i);
        board.first_flag &= ~(1ULL << i);
    } else {
        board.second_seg &= ~(1ULL << i);
        board.second_flag &= ~(1ULL << i);
    }
}

/**
 * Return color at the position of a Region
 * O(1)
 */
int get_region_cell(const Region &region, int pos) {
    assert(pos >= 0 && pos <= NUM_CELL - 1);
    if (pos < 64) {
        int is_filled = (region.first_flag & (1ULL << pos)) >> pos;
        if (!is_filled) return NONE;

        int bit = (region.first_seg & (1ULL << pos)) >> pos;
        return (bit) ? WHITE : BLACK;

    } else {
        int is_filled = (region.second_flag & (1ULL << pos)) >> pos;
        if (!is_filled) return NONE;

        int bit = (region.second_seg & (1ULL << pos)) >> pos;
        return (bit) ? WHITE : BLACK;
    }
}

/**
 * Return color at the position of a Board
 * O(1)
 */
int get_board_cell(const Board &board, int pos) {
    assert(pos >= 0 && pos <= NUM_CELL - 1);
    if (pos < 64) {
        int is_filled = (board.first_flag & (1ULL << pos)) >> pos;
        if (!is_filled) return NONE;

        int bit = (board.first_seg & (1ULL << pos)) >> pos;
        return (bit) ? WHITE : BLACK;

    } else {
        int is_filled = (board.second_flag & (1ULL << pos)) >> pos;
        if (!is_filled) return NONE;

        int bit = (board.second_seg & (1ULL << pos)) >> pos;
        return (bit) ? WHITE : BLACK;
    }
}

/**
 * O(1)
 */
ull get_num_liberties(const Board &board, int region_id) {
    assert(region_id >= 0);
    ull res = count_on_bit(board.regions[region_id].first_lib) + count_on_bit(board.regions[region_id].second_lib);
    return res;
}

/**
 * O(1)
 */
ull get_num_empty(const Board &board) {
    ull res = count_on_bit(board.first_flag) + count_on_bit(board.second_flag);
    return NUM_CELL - res;
}

void get_all_on_bit(const Region &region, std::vector<int> &res) {

    res.clear();

    ull tmp = region.first_flag;
    int mem = 0;
    while (tmp != 0) {
        int pos = bsft[((tmp & -tmp) * 0x218A392CD3D5DBFULL) >> 58];
        res.push_back(pos + mem);
        tmp = tmp >> (pos + 1);
        mem += pos + 1;
    }

    tmp = region.second_flag;
    mem = 64;
    while (tmp != 0) {
        int pos = bsft[((tmp & -tmp) * 0x218A392CD3D5DBFULL) >> 58];
        res.push_back(pos + mem);
        tmp = tmp >> (pos + 1);
        mem += pos + 1;
    }
}

void print_board(const Board &board) {
    std::cout << " ";
    for (int i = 0; i < BOARD_SIZE; i++) std::cout << " " << (char) (i + 'a');
    for (int i = 0; i < NUM_CELL; i++) {
        if (i % BOARD_SIZE == 0) std::cout << std::endl << i / BOARD_SIZE + 1;
        if (get_board_cell(board, i) == BLACK) std::cout << "\033[32m" << " ●" << "\033[0m";
        else if (get_board_cell(board, i) == WHITE) std::cout << "\033[31m" << " ●" << "\033[0m";
        else std::cout << " +";
    }
    std::cout << std::endl << std::endl;
}

void print_region(const Region &region) {
    std::cout << " ";
    for (int i = 0; i < BOARD_SIZE; i++) std::cout << " " << (char) (i + 'a');
    for (int i = 0; i < NUM_CELL; i++) {
        if (i % BOARD_SIZE == 0) std::cout << std::endl << i / BOARD_SIZE + 1;
        if (get_region_cell(region, i) == BLACK) std::cout << " ○";
        else if (get_region_cell(region, i) == WHITE) std::cout << " ●";
        else std::cout << " +";
    }
    std::cout << std::endl << std::endl;
}

/**
 * - Check adj_cell whether a new liberty
 * - Remove the liberty in 'pos'
 * O(1) * 4
 *
 * @param region
 * @param board
 * @param adj_cells
 */
void update_player_liberty(Region &region, const Board &board, int pos, const std::vector<int> &adj_cells) {
    for (int adj : adj_cells) {
        assert(adj >= 0 && adj <= NUM_CELL - 1);
        if (get_board_cell(board, adj) != NONE) continue;
        if (adj < 64) region.first_lib |= (1 << adj);
        else region.second_lib |= (1 << adj);
    }
    if (pos < 64) region.first_lib &= ~(1ULL << pos);
    else region.second_lib &= ~(1ULL << pos);
}

/**
 * O(1) * 4
 */
void update_evil_liberty(Board &board, int oppo_color, int pos, std::vector<int> adj_cells) {
    for (int adj : adj_cells) {
        if (get_board_cell(board, adj) != oppo_color) continue;
        int region_id = get_region_id_by_cell(board, adj);
        if (adj < 64) board.regions[region_id].first_lib &= ~(1ULL << pos);
        else board.regions[region_id].second_lib &= ~(1ULL << pos);
    }
}

/**
 * - Add new region to list of regions
 * - Update cell information of board which is inside the region
 * - O(1)
 * TODO: is there anyway to reduce the number of cells I need to check
 * TODO: like jumping directly into each bit 1 position
 */
void add_region_to_board(Region region, Board &board, int pos, int color) {

    std::cout << " inside\n";
    board.regions.push_back(region);
    int region_id = board.regions.size() - 1;

    std::cout << " in inside\n";

    std::vector<int> on_bit_pos;
    get_all_on_bit(region, on_bit_pos);
    debug(on_bit_pos.size());
    assert(on_bit_pos.size() == 1);   // otherwise, it is merging regions

    for (int i: on_bit_pos) {
        debug(i);
        int cell_color = get_region_cell(region, i);
        assert(cell_color == color);  // all stone in region suppose to have same color
        board.cell_to_region[i] = region_id;
    }

    board.cell_to_region[pos] = region_id;
}

/**
 * Merge regions
 *  - Update cell information of second region into first region
 *  - Update mapping of cell from second region to first region
 *  - Remove second region from board information
 *
 * Worst case: O(81)
 * Best case: O(1)
 * @param fr_id id of the first region
 * @param sr_id id of the second region
 * @param board the board that contains two regions
 */
void merge_region(int fr_id, int sr_id, Board &board) {

    Region &fr = board.regions[fr_id];
    Region &sr = board.regions[sr_id];

    // one cell can not in 2 different regions
    assert((fr.first_flag & sr.first_flag) == 0);
    assert((fr.second_flag & sr.second_flag) == 0);

    // merging region
    fr.first_flag |= sr.first_flag;
    fr.second_flag |= sr.second_flag;
    fr.first_seg |= sr.first_seg;
    fr.second_seg |= sr.second_seg;
    fr.first_lib |= sr.first_lib;
    fr.second_lib |= sr.second_lib;

    // update mapping cell to region
    // for each cell in second region, change mapping to first region
    std::vector<int> on_bit_pos;
    get_all_on_bit(sr, on_bit_pos);
    for (int pos: on_bit_pos) {
        board.cell_to_region[pos] = fr_id;
    }
}

/**
 * - Update board state (turn on bit in board)
 * - Update regions state
 *      - Update new cell in region
 *      - Update mapping from cell to region
 *      - Update liberties in region
 *      - Update liberties of adjacent regions
 *
 *  New region : O(1)
 *  Merged region:
 *      - Worst case: O(81) * 4 (two regions contains the whole board)
 *      - Best case: O(1) * 4 (two pieces)
 */
void update_board_info(Board &board, int pos, int color) {

    assert(color == BLACK || color == WHITE);
    assert(pos >= 0 && pos <= NUM_CELL - 1);

    add_bit_board(board, pos, color);

    std::vector<int> adj_cells;
    int pivot_id = -1;

    // get pivot region for merging other into
    get_adj_cells(pos, adj_cells);
    for (int &adj: adj_cells) {
        if (get_board_cell(board, adj) != color) continue;
        pivot_id = get_region_id_by_cell(board, adj);
        break;
    }

    debug(pivot_id);

    if (pivot_id == -1) {  // no merging region, new region
        Region region;
        add_bit_region(region, pos, color);
        update_player_liberty(region, board, pos, adj_cells);
        debug(region.first_lib);
        add_region_to_board(region, board, pos, color);

    } else {  // merging region

        for (int &adj: adj_cells) {
            if (get_board_cell(board, adj) != color) continue;
            int next_id = get_region_id_by_cell(board, adj);
            if (pivot_id == next_id) continue;
            merge_region(pivot_id, next_id, board);
        }

        add_bit_region(board.regions[pivot_id], pos, color);
        update_player_liberty(board.regions[pivot_id], board, pos, adj_cells);
        board.cell_to_region[pos] = pivot_id;
    }

    int oppo_color = get_oppo_color(color);
    update_evil_liberty(board, oppo_color, pos, adj_cells);
}

/**
 * Check whether the position 'pos' at board is suicide.
 * O(1)
 */
bool is_suicide(const Board &board, int pos) {
    assert(pos >= 0 && pos <= NUM_CELL - 1);

    int region_id = get_region_id_by_cell(board, pos);
    return get_num_liberties(board, region_id) == 0;
}

/**
 * O(1) * 4
 */
bool is_capture(const Board &board, int pos) {
    assert(pos >= 0 && pos <= NUM_CELL - 1);

    std::vector<int> adj_cells;
    get_adj_cells(pos, adj_cells);

    for (int adj: adj_cells) {
        if (get_board_cell(board, adj) == NONE) continue;
        int region_id = get_region_id_by_cell(board, adj);
        if (get_num_liberties(board, region_id) == 0) return true;
    }

    return false;
}

/**
 * Set a cell of board require:
 *  - Empty cell
 *  - No suicide
 *  - No capture
 *
 * Return:
 *  - 1: success
 *  - 2: fail, board not change
 *
 *  O(81) * 4
 */
int set_board_cell(Board &board, int pos, int color) {
    assert(pos >= 0 && pos <= NUM_CELL - 1);
    assert(color == BLACK || color == WHITE);

    if (get_board_cell(board, pos) != NONE) return -1;

    debug(color);

    Board tmp = board;  // copy value
    update_board_info(tmp, pos, color);

    debug(pos);

    if (is_suicide(tmp, pos)) return -1;
    if (is_capture(tmp, pos)) return -1;

    board = tmp;
    return 1;
}

void remove_valid_cell(Board &board, int id) {
    std::swap(board.valid_cells.back(), board.valid_cells[id]);
    board.valid_cells.pop_back();
}

int get_random_valid_pos(Board &board, int color) {
    srand(time(NULL));
    Board tmp = board;
    while (!board.valid_cells.empty()) {
        int id = std::rand() % board.valid_cells.size();
        int cell = board.valid_cells[id];
        debug(cell);
        if (set_board_cell(tmp, cell, color) != -1) return cell;
        remove_valid_cell(board, id);
    }

    return -1;
}

/** ----------------- Node ------------------------- */
struct Node {
    Board board;
    Node *parent;
    int pos, color;
    int count, num_win;

    std::vector<Node *> children;

    Node(Board board, int pos, int color) {
        this->board = board;
        this->pos = pos;
        this->color = color;
        this->children.clear();
        this->parent = nullptr;
        this->count = this->num_win = 0;
    }
};

bool is_fully_expanded(Node *node) {
    int num_empty = get_num_empty(node->board);
    assert(num_empty > node->children.size());
    return num_empty == node->children.size();
}

double get_score(Node *node) {
    return (double) node->num_win / node->count + sqrt(log(node->parent->count) / node->count);
}

Node *get_best_uct_child(Node *parent) {

    if (parent->children.empty()) return nullptr;

    double curr_score = get_score(parent->children[0]);
    std::vector<Node *> chosen(1, parent->children[0]);

    for (Node *child: parent->children) {
        double score = get_score(child);
        if (curr_score - score > 0.0001) {  /** curr_score > score */
            chosen.clear();
            chosen.push_back(child);
            curr_score = score;
        } else if (curr_score - score > -0.0001) {  /** curr_score >= score */
            chosen.push_back(child);
        }
    }

    srand(time(NULL));
    return chosen[std::rand() % chosen.size()];
}

void update_node_value(Node *node, int value) {
    node->num_win += value;
    node->count++;
}


/** ----------------- MCTS ----------------------- */
struct MCTS {
    Node *root;
    int depth = 0;
};

void init_tree(MCTS &tree, Node *node, int _depth) {
    tree.root = node;
    tree.depth = _depth;
}

Node *selection(Node *node) {
    while (is_fully_expanded(node)) {
        node = get_best_uct_child(node);
    }
    return node;
}

Node *expansion(Node *parent) {

    int oppo_color = get_oppo_color(parent->color);
    int random_pos = get_random_valid_pos(parent->board, oppo_color);
    if (random_pos == -1) return parent;  // terminated

    Node *child = new Node(parent->board, random_pos, oppo_color);
    set_board_cell(child->board, child->pos, child->color);
    parent->children.push_back(child);
    child->parent = parent;

    return get_best_uct_child(parent);
}

/**
 * Play until end game (or other conditions meet)
 *  Return:
 *  - win: 1
 *  - lose: 0
 */
int simulation(Node *node, int player_color) {

    int curr_color = node->color;
    Board tmp = node->board;
    while (true) {
        curr_color = get_oppo_color(curr_color);
        int random_pos = get_random_valid_pos(tmp, curr_color);
        if (random_pos == -1) break;
        set_board_cell(tmp, random_pos, curr_color);
        print_board(tmp);
    }

    print_board(tmp);

    // if we can not move, then we lose
    return curr_color == player_color ? -1 : 0;
}

void backpropagation(Node *node, int value) {
    while (node != nullptr) {
        update_node_value(node, value);
        node = node->parent;
    }
}

void run_once(Node *node, int player_color) {
    Node *leaf = selection(node);
    leaf = expansion(leaf);
    int end_game = simulation(leaf, player_color);
    backpropagation(node, end_game);
}

void free_all_child(Node *node) {
    if (node->children.empty()) {
        free(node);
        return;
    }

    for (Node *child: node->children) {
        free_all_child(child);
    }
}

/** ----------------- Agent ----------------------- */
struct Agent {
    MCTS tree;
};

int make_move_by_AI(Agent &agent, Board &board, int player_color) {

    int oppo_color = get_oppo_color(player_color);
    Node *root = new Node(board, -1, oppo_color);
    init_tree(agent.tree, root, 0);

    for (int i = 0; i < 10; i++) run_once(agent.tree.root, player_color);

    Node *best_child = get_best_uct_child(agent.tree.root);
    int res = (best_child == nullptr) ? -1 : best_child->pos;
    free_all_child(agent.tree.root);

    return res;
}

/** ----------------- Coordinator ----------------------- */

int get_int_helper(const std::string &s) {
    if (s.find_first_not_of("01234566789") != std::string::npos) return -1;
    return strtol(s.c_str(), nullptr, 0);
}

std::string to_lowercase_helper(const std::string &s) {
    std::string res;
    for (auto x: s) {
        if (x >= 'A' && x <= 'Z') res.append(1, x + 32);
        else res.append(1, x);
    }
    return res;
}

int parse_color_helper(const std::string &arg) {
    auto token = to_lowercase_helper(arg);
    return (token[0] == 'b') ? BLACK : WHITE;
}

int parse_pos_helper(const std::string &arg) {
    auto token = to_lowercase_helper(arg);
    int row = token[1] - '1';
    int col = token[0] - 'a';
    return row * BOARD_SIZE + col;
}

struct Command {
    int id = -1;
    std::string command = "";
    std::vector<std::string> arguments;
};

std::string preprocess_command(const std::string &raw_command) {

    std::string res;
    bool in_comment = false;
    for (int code : raw_command) {
        if (code < 32 && code != 9 && code != 10) continue;  // control char not HT or LF

        if (code == 35) in_comment = true;   // # char
        if (code == 10) in_comment = false;  // LF char
        if (in_comment) continue;  // after # char

        if (code == 9) res.append(1, (char) 32);  // HT to SPACE
        else res.append(1, (char) code);
    }

    return res;
}

Command parse_command(const std::string &command) {
    std::istringstream iss(command);
    std::vector<std::string> tokens(std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>());

    Command res = Command();
    if (tokens.size() == 0) return res;  // NOLINT(readability-container-size-empty)

    int id = get_int_helper(tokens[0]);
    if (id >= 0) {
        res.id = id;
        if (tokens.size() > 1) res.command = tokens[1];
        if (tokens.size() > 2) res.arguments.assign(tokens.begin() + 2, tokens.end());
    } else {
        res.command = tokens[0];
        if (tokens.size() > 1) res.arguments.assign(tokens.begin() + 1, tokens.end());
    }
    return res;
}

std::string get_response(bool is_success, Command &command, const std::string &mess) {
    std::string res;
    if (is_success) res.append("="); else res.append("?");
    if (command.id != -1) res.append(std::to_string(command.id));
    if (!mess.empty()) res.append(" ").append(mess);
    res.append("\n\n");
    return res;
}

bool is_known_command(const std::string &c, const std::array<std::string, 11> &known_coms) {
    for (const auto &command : known_coms)
        if (command == c) return true;
    return false;
}

bool make_move_by_input(Board &board, const std::vector<std::string> &args) {
    if (args.size() < 2) return false;
    int color = parse_color_helper(args[0]);
    int pos = parse_pos_helper(args[1]);
    return set_board_cell(board, pos, color) != -1;
}

/** ---------------------- MAIN --------------------------- */

struct Log {
    Command command;
    std::string response;
};

std::array<std::string, 11> known_commands = {
        "protocol_version", "name", "version", "known_command", "list_commands", "quit", "boardsize", "clear_board",
        "komi", "play", "genmove"
};

Agent ai;
Board mainboard;
bool is_quit;
std::vector<Log> history;

void exec_command(const std::string &raw_command) {

    auto command_string = preprocess_command(raw_command);
    Command command = parse_command(command_string);
    auto head = command.command;
    auto args = command.arguments;

    std::string response;

    if (head == "protocol_version") {
        response = get_response(true, command, "2");

    } else if (head == "name") {
        response = get_response(true, command, "0860832");

    } else if (head == "version") {
        response = get_response(true, command, "0.0");

    } else if (head == "known_command") {
        std::string res = is_known_command(args[0], known_commands) ? "true" : "false";
        response = get_response(true, command, res);

    } else if (head == "list_commands") {
        std::string res;
        for (const auto &x: known_commands) {
            res.append(x);
            res.append("\n");
        }
        response = get_response(true, command, res);

    } else if (head == "quit") {
        is_quit = true;
        response = get_response(true, command, "");

    } else if (head == "clear_board") {
        reset_board(mainboard);
        response = get_response(true, command, "");

    } else if (head == "play") {
        bool can_move = make_move_by_input(mainboard, args);
        response = get_response(can_move, command, can_move ? "" : "illegal move");

    } else if (head == "genmove") {
        // TODO: assume that args always true because the protocol don't specify this
        int color = parse_color_helper(args[0]);
        int pos = make_move_by_AI(ai, mainboard, color);
        response = get_response(pos != -1, command, pos != -1 ? "" : "resign");

    } else {
        response = get_response(false, command, "unknown command");
    }

    history.push_back({command, response});
    std::cout << history.back().response;
}

void init_program() {
    is_quit = false;
    reset_board(mainboard);
    init_bsf_table();
}

/** ------------------- TESTING ----------------- */
void test_count_on_bit() {
    assert(count_on_bit(0b01010101) == 4);
    assert(count_on_bit(0b0000000000000000000000000000000000000000000000000000000000000000) == 0);
    assert(count_on_bit(0b0000000000000000000000000000000000000000000000000000011000000000) == 2);
    assert(count_on_bit(0b0000000000000000000000000000000000000000000000000000011000000010) == 3);
    assert(count_on_bit(0b1111111111111111111111111111111111111111111111111111111111111111) == 64);
    assert(count_on_bit(0b1111111111111111111111111111111110000000000000000000011000000000) == 35);
    assert(count_on_bit(0b0001000110000111001011001000000001100011010101000000000110000000) == 19);
}

void test_get_adj_cells() {
    std::vector<int> res;

    res.clear();
    get_adj_cells(0, res);
    assert(res.size() == 2);
    assert(res[0] == 1);
    assert(res[1] == 9);

    res.clear();
    get_adj_cells(1, res);
    assert(res.size() == 3);
    assert(res[0] == 2);
    assert(res[1] == 10);
    assert(res[2] == 0);

    res.clear();
    get_adj_cells(5, res);
    assert(res.size() == 3);
    assert(res[0] == 6);
    assert(res[1] == 14);
    assert(res[2] == 4);

    res.clear();
    get_adj_cells(8, res);
    assert(res.size() == 2);
    assert(res[0] == 17);
    assert(res[1] == 7);

    res.clear();
    get_adj_cells(9, res);
    assert(res.size() == 3);
    assert(res[0] == 0);
    assert(res[1] == 10);
    assert(res[2] == 18);

    res.clear();
    get_adj_cells(10, res);
    assert(res.size() == 4);
    assert(res[0] == 1);
    assert(res[1] == 11);
    assert(res[2] == 19);
    assert(res[3] == 9);

    res.clear();
    get_adj_cells(22, res);
    assert(res.size() == 4);
    assert(res[0] == 13);
    assert(res[1] == 23);
    assert(res[2] == 31);
    assert(res[3] == 21);

    res.clear();
    get_adj_cells(72, res);
    assert(res.size() == 2);
    assert(res[0] == 63);
    assert(res[1] == 73);

    res.clear();
    get_adj_cells(76, res);
    assert(res.size() == 3);
    assert(res[0] == 67);
    assert(res[1] == 77);
    assert(res[2] == 75);

    res.clear();
    get_adj_cells(80, res);
    assert(res.size() == 2);
    assert(res[0] == 71);
    assert(res[1] == 79);
}

void test_get_oppo_color() {
    assert(get_oppo_color(BLACK) == WHITE);
    assert(get_oppo_color(WHITE) == BLACK);
}

void test_add_black_bit_region() {
    Region region;

    add_black_bit_region(region, 0);
    assert(region.first_flag == 1);
    assert(region.first_seg == 0);
    assert(region.second_flag == 0);
    assert(region.second_seg == 0);

    reset_region(region);
    add_black_bit_region(region, 1);
    assert(region.first_flag == 2);
    assert(region.first_seg == 0);
    assert(region.second_flag == 0);
    assert(region.second_seg == 0);

    reset_region(region);
    add_black_bit_region(region, 1);
    add_black_bit_region(region, 0);
    assert(region.first_flag == 3);
    assert(region.first_seg == 0);
    assert(region.second_flag == 0);
    assert(region.second_seg == 0);

    reset_region(region);
    add_black_bit_region(region, 63);
    add_black_bit_region(region, 0);
    assert(region.first_flag == (1ULL << 63) + 1);
    assert(region.first_seg == 0);
    assert(region.second_flag == 0);
    assert(region.second_seg == 0);

    reset_region(region);
    add_black_bit_region(region, 64);
    add_black_bit_region(region, 0);
    assert(region.first_flag == 1);
    assert(region.first_seg == 0);
    assert(region.second_flag == 1);
    assert(region.second_seg == 0);

    reset_region(region);
    add_black_bit_region(region, 80);
    add_black_bit_region(region, 3);
    assert(region.first_flag == 8);
    assert(region.first_seg == 0);
    assert(region.second_flag == (1U << 16));
    assert(region.second_seg == 0);

    reset_region(region);
    add_black_bit_region(region, 0);
    add_black_bit_region(region, 10);
    add_black_bit_region(region, 0);
    add_black_bit_region(region, 78);
    add_black_bit_region(region, 80);
    assert(region.first_flag == (1 << 10) + 1);
    assert(region.first_seg == 0);
    assert(region.second_flag == (1U << 16) + (1U << 14));
    assert(region.second_seg == 0);
}

void test_add_white_to_region() {
    Region region;

    add_white_bit_region(region, 0);
    assert(region.first_flag == 1);
    assert(region.first_seg == 1);
    assert(region.second_flag == 0);
    assert(region.second_seg == 0);

    reset_region(region);
    add_white_bit_region(region, 1);
    assert(region.first_flag == 2);
    assert(region.first_seg == 2);
    assert(region.second_flag == 0);
    assert(region.second_seg == 0);

    reset_region(region);
    add_white_bit_region(region, 1);
    add_white_bit_region(region, 0);
    assert(region.first_flag == 3);
    assert(region.first_seg == 3);
    assert(region.second_flag == 0);
    assert(region.second_seg == 0);

    reset_region(region);
    add_white_bit_region(region, 63);
    add_white_bit_region(region, 0);
    assert(region.first_flag == (1ULL << 63) + 1);
    assert(region.first_seg == (1ULL << 63) + 1);
    assert(region.second_flag == 0);
    assert(region.second_seg == 0);

    reset_region(region);
    add_white_bit_region(region, 64);
    add_white_bit_region(region, 0);
    assert(region.first_flag == 1);
    assert(region.first_seg == 1);
    assert(region.second_flag == 1);
    assert(region.second_seg == 1);

    reset_region(region);
    add_white_bit_region(region, 80);
    add_white_bit_region(region, 3);
    assert(region.first_flag == 8);
    assert(region.first_seg == 8);
    assert(region.second_flag == (1U << 16));
    assert(region.second_seg == (1U << 16));

    reset_region(region);
    add_white_bit_region(region, 0);
    add_white_bit_region(region, 10);
    add_white_bit_region(region, 0);
    add_white_bit_region(region, 78);
    add_black_bit_region(region, 80);
    assert(region.first_flag == (1 << 10) + 1);
    assert(region.first_seg == (1 << 10) + 1);
    assert(region.second_flag == (1U << 16) + (1U << 14));
    assert(region.second_seg == (1U << 14));
}

void test_get_region_cell() {
    Region region;
    reset_region(region);

    add_bit_region(region, 2, BLACK);
    assert(get_region_cell(region, 2) == BLACK);

    add_bit_region(region, 10, BLACK);
    assert(get_region_cell(region, 10) == BLACK);

    add_bit_region(region, 11, WHITE);
    assert(get_region_cell(region, 11) == WHITE);

    assert(get_region_cell(region, 12) == NONE);
    assert(get_region_cell(region, 80) == NONE);

    add_bit_region(region, 2, WHITE);
    assert(get_region_cell(region, 2) == WHITE);
}

void test_get_all_on_bit() {
    Region region;
    reset_region(region);
    region.first_flag = 0b0111100000000000000000000000000000000000000000010001000000110110;
    region.second_flag = 0b00000000000000010110000000000110;

    std::vector<int> pos;
    get_all_on_bit(region, pos);
    assert(pos.size() == 15);
    assert(pos[0] == 1);
    assert(pos[1] == 2);
    assert(pos[2] == 4);
    assert(pos[3] == 5);
    assert(pos[4] == 12);
    assert(pos[5] == 16);
    assert(pos[6] == 59);
    assert(pos[7] == 60);
    assert(pos[8] == 61);
    assert(pos[9] == 62);
    assert(pos[10] == 65);
    assert(pos[11] == 66);
    assert(pos[12] == 77);
    assert(pos[13] == 78);
    assert(pos[14] == 80);
}

void test_update_board_info() {
    Board board;
    Board tmp;
    reset_board(board);

    // first move
    update_board_info(board, 0, BLACK);
    assert(board.regions.size() == 1);
    assert(board.regions[0].first_lib == (1ULL << 9) + 2);
    assert(get_num_liberties(board, 0) == 2);
    assert(board.cell_to_region[0] == 0);
    assert(board.cell_to_region[1] == -1);
    assert(board.first_seg == 0);
    assert(board.first_flag == 1);

    // 2 regions
    update_board_info(board, 10, BLACK);
    assert(board.regions.size() == 2);
    assert(board.regions[0].first_lib == (1ULL << 9) + 2);
    assert(board.regions[1].first_lib == (1ULL << 9) + 2 + (1ULL << 11) + (1ULL << 19));
    assert(get_num_liberties(board, 1) == 4);
    assert(board.cell_to_region[0] == 0);
    assert(board.cell_to_region[1] == -1);
    assert(board.cell_to_region[10] == 1);
    assert(board.first_seg == 0);
    assert(board.first_flag == (1ULL << 10) + 1);

    // merging (1 regions)
    update_board_info(board, 1, BLACK);
    assert(board.regions.size() == 2);
    assert(board.regions[1].first_lib == (1ULL << 9) + (1ULL << 2) + (1ULL << 11) + (1ULL << 19));
    assert(get_num_liberties(board, 1) == 4);
    assert(board.cell_to_region[0] == 1);
    assert(board.cell_to_region[1] == 1);
    assert(board.cell_to_region[10] == 1);
    assert(board.first_seg == 0);
    assert(board.first_flag == (1ULL << 10) + 1 + 2);

    update_board_info(board, 9, WHITE);
    assert(board.regions.size() == 3);
    assert(board.regions[1].first_lib == (1ULL << 2) + (1ULL << 11) + (1ULL << 19));
    assert(board.regions[1].second_lib == 0);
    assert(board.regions[2].first_lib == (1ULL << 18));
    assert(board.regions[2].second_lib == 0);
    assert(get_num_liberties(board, 1) == 3);
    assert(get_num_liberties(board, 2) == 1);
    assert(board.cell_to_region[0] == 1);
    assert(board.cell_to_region[1] == 1);
    assert(board.cell_to_region[10] == 1);
    assert(board.cell_to_region[9] == 2);

    assert(board.first_seg == (1ULL << 9));
    assert(board.first_flag == (1ULL << 10) + 1 + 2 + (1ULL << 9));

    // cannot capture, suicide
    update_board_info(board, 18, BLACK);
    assert(is_capture(board, 18));
    assert(!is_suicide(board, 18));
    assert(is_suicide(board, 9));
    assert(board.regions.size() == 4);
    assert(board.regions[1].first_lib == (1ULL << 2) + (1ULL << 11) + (1ULL << 19));
    assert(board.regions[1].second_lib == 0);
    assert(board.regions[2].first_lib == 0);
    assert(board.regions[2].second_lib == 0);
    assert(board.regions[3].first_lib == (1ULL << 19) + (1ULL << 27));
    assert(board.regions[3].second_lib == 0);
    assert(get_num_liberties(board, 1) == 3);
    assert(get_num_liberties(board, 2) == 0);
    assert(get_num_liberties(board, 3) == 2);
    assert(board.cell_to_region[0] == 1);
    assert(board.cell_to_region[1] == 1);
    assert(board.cell_to_region[10] == 1);
    assert(board.cell_to_region[9] == 2);
    assert(board.cell_to_region[18] == 3);

    assert(board.first_seg == (1ULL << 9));
    assert(board.first_flag == (1ULL << 10) + 1 + 2 + (1ULL << 9) + (1ULL << 18));

    update_board_info(board, 20, BLACK);
    assert(board.regions.size() == 5);

    update_board_info(board, 28, BLACK);
    assert(board.regions.size() == 6);

    // merge 4 regions
    update_board_info(board, 19, BLACK);
    assert(board.regions.size() == 6);
    assert(get_num_liberties(board, 1) == 6);
    assert(board.cell_to_region[0] == 1);
    assert(board.cell_to_region[1] == 1);
    assert(board.cell_to_region[10] == 1);
    assert(board.cell_to_region[9] == 2);
    assert(board.cell_to_region[18] == 1);
    assert(board.cell_to_region[19] == 1);
    assert(board.cell_to_region[20] == 1);
    assert(board.cell_to_region[28] == 1);
    assert(get_num_empty(board) == 81 - 8);

}

void test_all() {
    test_count_on_bit();
    test_get_adj_cells();
    test_get_oppo_color();
    test_add_black_bit_region();
    test_add_white_to_region();
    test_get_region_cell();
    test_get_all_on_bit();
    test_update_board_info();
    std::cout << "Test OK!" << std::endl;
}


/** ------------------ ENTRY POINT ---------------- */
int main() {

    /** Should run test here
     * Board & Region:
     *  - Place in correct position
     *  - Merging region
     *  - Create new region
     *  - Occur suicide
     *  - Occur capture
     */

    init_program();
    test_all();

    std::string raw_command;
    while (!is_quit) {
        getline(std::cin, raw_command);
        exec_command(raw_command);
        print_board(mainboard);
    }

    return 0;
}