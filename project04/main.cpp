#include <iostream>
#include <cassert>
#include <vector>
#include <array>
#include <sstream>
#include <iterator>

#define ull unsigned long long
#define ui  unsigned int

/** ----------------- Board & Region ----------------------- */
const int BOARD_SIZE = 9;
const int NUM_CELL = BOARD_SIZE * BOARD_SIZE;

const int NONE = 0;
const int BLACK = 1;
const int WHITE = 2;

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

ull count_on_bit(ull i) {
    i = i - ((i >> 1ULL) & 0x5555555555555555ULL);
    i = (i & 0x3333333333333333ULL) + ((i >> 2ULL) & 0x3333333333333333ULL);
    return (((i + (i >> 4ULL)) & 0xF0F0F0F0F0F0F0FULL) * 0x101010101010101ULL) >> 56;
}

void get_adj_cells(int pos, std::vector<int> &res) {
    assert(pos >= 0 && pos <= NUM_CELL - 1);

    int d[4] = {-BOARD_SIZE, +1, +BOARD_SIZE, -1};

    for (int i = 0; i < 4; i++) {
        int new_pos = pos + d[i];
        if (new_pos >= 0 && new_pos <= NUM_CELL - 1) res.push_back(new_pos);
    }
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
};

void reset_board(Board &board) {
    board.first_seg = board.second_seg = board.first_flag = board.second_flag = 0;
    board.regions.clear();
    for(int & cell : board.cell_to_region) cell = -1;
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
 * Check and add new liberties in list of adj_cells to region
 * @param region
 * @param board
 * @param adj_cells
 */
void update_liberty_region(Region &region, const Board &board, int pos, const std::vector<int> &adj_cells) {
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
 * - Add new region to list of regions
 * - Update cell information of board which is inside the region
 */
void add_region_to_board(Region region, Board &board, int color) {

    board.regions.push_back(region);
    int region_id = board.regions.size() - 1;

    for (int pos = 0; pos < NUM_CELL; pos++) {
        int cell_color = get_region_cell(region, pos);
        if (cell_color == color)
            board.cell_to_region[pos] = region_id;
    }
}

/**
 * Merge regions
 *  - Update cell information of second region into first region
 *  - Update mapping of cell from second region to first region
 *  - Remove second region from board information
 *
 * @param fr_id id of the first region
 * @param sr_id id of the second region
 * @param board the board that contains two regions
 */
void merge_region(int fr_id, int sr_id, Board &board, int color) {

    Region &fr = board.regions[fr_id];
    Region &sr = board.regions[sr_id];

    // one cell can not in 2 different regions
    assert((fr.first_flag & sr.first_flag) != 0);
    assert((fr.second_flag & sr.second_flag) != 0);

    // merging region
    fr.first_seg |= sr.first_seg;
    fr.second_seg |= sr.second_seg;
    fr.first_flag |= sr.first_flag;
    fr.second_flag |= sr.second_flag;
    fr.first_lib |= sr.first_lib;
    fr.second_lib |= sr.second_lib;

    // update mapping cell to region
    // for each cell in second region, change mapping to first region
    for (int i = 0; i < NUM_CELL; i++) {
        int fr_color = get_region_cell(fr, i);
        int sr_color = get_region_cell(sr, i);

        assert(fr_color != WHITE);
        assert(sr_color != WHITE);
        if (sr_color != color) continue;
        board.cell_to_region[i] = fr_id;
    }
}

/**
 * - Update board state (turn on bit in board)
 * - Update regions state
 *      - Update new cell in region
 *      - Update mapping from cell to region
 *      - Update liberties in region
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

    if (pivot_id == -1) {  // no merging region, new region
        Region region;
        add_bit_region(region, pos, color);
        update_liberty_region(region, board, pos, adj_cells);
        add_region_to_board(region, board, color);

    } else {  // merging region

        for (int &adj: adj_cells) {
            if (get_board_cell(board, adj) != color) continue;
            int next_id = get_region_id_by_cell(board, adj);
            if (pivot_id == next_id) continue;
            merge_region(pivot_id, next_id, board, color);
        }

        add_bit_region(board.regions[pivot_id], pos, color);
        update_liberty_region(board.regions[pivot_id], board, pos, adj_cells);
    }
}

ull get_num_liberties(const Board &board, int region_id) {
    assert(region_id >= 0);
    ull res = count_on_bit(board.regions[region_id].first_lib) + count_on_bit(board.regions[region_id].second_lib);
    return res;
}

bool is_suicide(const Board &board, int pos) {
    assert(pos >= 0 && pos <= NUM_CELL - 1);

    int region_id = get_region_id_by_cell(board, pos);
    return get_num_liberties(board, region_id) == 0;
}

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
 */
int set_board_cell(Board &board, int pos, int color) {
    assert(pos >= 0 && pos <= NUM_CELL - 1);
    assert(color == BLACK || color == WHITE);

    if (get_board_cell(board, pos) != NONE) return -1;

    Board tmp = board;  // copy value
    update_board_info(tmp, pos, color);

    if (is_suicide(tmp, pos)) return -1;
    if (is_capture(tmp, pos)) return -1;

    board = tmp;
    return 1;
}

void print_board(const Board &board) {
    std::cout << " ";
    for (int i = 0; i < BOARD_SIZE; i++) std::cout << " " << (char) (i + 'a');
    for (int i = 0; i < NUM_CELL; i++) {
        if (i % BOARD_SIZE == 0) std::cout << std::endl << i / BOARD_SIZE + 1;
        if (get_board_cell(board, i) == BLACK) std::cout << " ○";
        else if (get_board_cell(board, i) == WHITE) std::cout << " ●";
        else std::cout << " +";
    }
    std::cout << std::endl << std::endl;
}

/** ----------------- Node ------------------------- */
/** ----------------- MCTS ----------------------- */
/** ----------------- Agent ----------------------- */

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

    std::string get_string() {
        std::string s = std::to_string(id).append(" ").append(command);
        for (auto &arg: arguments) s.append(" ").append(arg);
        return s;
    }
};

void init_command(Command &command, int _id, const std::string &_command, const std::string &_arg) {
    command.id = _id;
    command.command = _command;
    command.arguments.push_back(_arg);
}

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

    Command res;
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
//        auto color = parse_color_helper(args[0]);
//        int pos = ai.make_move(mainboard, color);
//        response = get_response(pos != -1, command, pos != -1 ? "" : "resign");

    } else {
        response = get_response(false, command, "unknown command");
    }

    history.push_back({command, response});
    std::cout << history.back().response;
}

int main() {

    is_quit = false;
    reset_board(mainboard);

    std::string raw_command;
    while (!is_quit) {
        getline(std::cin, raw_command);
        exec_command(raw_command);
        print_board(mainboard);
    }

    return 0;
}