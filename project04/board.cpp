//
// Created by Thong Nguyen on 11/19/19.
//

#include <queue>
#include <iostream>
#include <algorithm>
#include "board.h"
#include "helper.h"

int Board::BOARD_SIZE = 9;
int Board::NUM_CELL = 81;
int Board::NONE = 0;
int Board::BLACK = 1;
int Board::WHITE = 2;


Board::Board() {
    for (int i = 0; i < NUM_CELL; i++) {
        board[i] = 0;
        empty_random_cells.push_back(i);
    }
    shuffle(empty_random_cells.begin(), empty_random_cells.end(), Helper::get_random_number(NUM_CELL));

    num_empty_cell = NUM_CELL;
}

std::vector<Board::Cell> Board::get_liberties(Cell pos, Board::Color color, Cell checking_pos) {

    std::vector<Board::Cell> res;
    std::queue<Board::Cell> q;
    bool checked[NUM_CELL];

    board[pos] = color;
    if (board[checking_pos] == NONE) {
        Helper::print("Warning: checking liberties of empty cell regions");
    }

    for (int i = 0; i < NUM_CELL; i++) checked[i] = false;
    q.push(checking_pos);
    checked[checking_pos] = true;
    Color checking_color = board[checking_pos];

    while (!q.empty()) {
        Board::Cell cell = q.front();
        q.pop();
        auto adj_cells = get_adj_cells(cell);
        for (Cell new_cell : adj_cells) {
            if (checked[new_cell]) continue;
            if (board[new_cell] == checking_color) {
                checked[new_cell] = true;
                q.push(new_cell);
            } else if (board[new_cell] == NONE) {
                checked[new_cell] = true;
                res.push_back(new_cell);
            }
        }
    }

    board[pos] = NONE;

    return res;
}

bool Board::is_capturing(Cell pos, Board::Color color) {
    auto other_color = get_opponent_color(color);
    auto adj_cells = get_adj_cells(pos);

    for (Cell adj_cell: adj_cells) {
        if (board[adj_cell] != other_color) continue;
        if (get_liberties(pos, color, adj_cell).empty()) {
            Helper::print("Capture position: " + std::to_string(adj_cell));
            return true;
        }
    }

    return false;
}

bool Board::is_suiciding(Cell pos, Board::Color color) {
    auto liberties = get_liberties(pos, color, pos);
    if (liberties.empty()) Helper::print("Suicide at: " + std::to_string(pos));
    return liberties.empty();
}

/**
 * Place a stone in location 'pos' with 'color' on the board
 * @return 1 if success, -1 if fail
 */
int Board::place(Cell pos, Color color) {
    if (pos < 0 || pos >= NUM_CELL) return -1;
    if (board[pos] != 0) return -1;
    if (is_capturing(pos, color)) return -1;
    if (is_suiciding(pos, color)) return -1;
    board[pos] = color;
    num_empty_cell--;
    return 1;
}

int Board::place(int x, int y, Board::Color color) {
    return place(x * BOARD_SIZE + y, color);
}

Board::Color Board::get_opponent_color(Board::Color color) {
    if (color == WHITE) return BLACK;
    if (color == BLACK) return WHITE;
    return NONE;
}

int Board::get_boardsize() {
    return BOARD_SIZE;
}

void Board::set_boardsize(int new_size) {
    BOARD_SIZE = new_size;
    NUM_CELL = new_size * new_size;
}

void Board::clear_board() {
    for (int i = 0; i < NUM_CELL; i++) board[i] = 0;
}

void Board::print() {
    std::cout << " ";
    for (int i = 0; i < BOARD_SIZE; i++) std::cout << " " << (char) (i + 'a');
    for (int i = 0; i < NUM_CELL; i++) {
        if (i % BOARD_SIZE == 0) std::cout << std::endl << i / BOARD_SIZE + 1;
        if (board[i] == BLACK) std::cout << " ●";
        else if (board[i] == WHITE) std::cout << " ○";
        else std::cout << " +";
    }
    std::cout << std::endl << std::endl;
}

std::vector<Board::Cell> Board::get_adj_cells(Board::Cell pos) {
    if (pos == 0)
        return {1, BOARD_SIZE};
    if (pos == BOARD_SIZE)
        return {BOARD_SIZE - 2, BOARD_SIZE + BOARD_SIZE - 1};
    if (pos == BOARD_SIZE * (BOARD_SIZE - 1))
        return {BOARD_SIZE * (BOARD_SIZE - 2), BOARD_SIZE * (BOARD_SIZE - 1) + 1};
    if (pos == BOARD_SIZE * BOARD_SIZE - 1)
        return {BOARD_SIZE * (BOARD_SIZE - 1) - 1, BOARD_SIZE * BOARD_SIZE - 2};

    if (pos > 0 && pos < BOARD_SIZE - 1)
        return {pos - 1, pos + BOARD_SIZE, pos + 1};
    if (pos > BOARD_SIZE * (BOARD_SIZE - 1) && pos < BOARD_SIZE * BOARD_SIZE - 1)
        return {pos - 1, pos - BOARD_SIZE, pos + 1};
    if (pos > 0 && pos < BOARD_SIZE * (BOARD_SIZE - 1) && pos % BOARD_SIZE == 0)
        return {pos - BOARD_SIZE, pos + 1, pos + BOARD_SIZE};
    if (pos > 8 && pos < BOARD_SIZE * BOARD_SIZE - 1 && pos % BOARD_SIZE == BOARD_SIZE - 1)
        return {pos - BOARD_SIZE, pos - 1, pos + BOARD_SIZE};

    if (pos > BOARD_SIZE && pos < BOARD_SIZE * (BOARD_SIZE - 1) - 1)
        return {pos - BOARD_SIZE, pos + 1, pos + BOARD_SIZE, pos - 1};

    return {};
}

bool Board::is_terminated() {
    return num_empty_cell == 0;
}

/**
 * @return -1 if there is no empty cell, otherwise the position of the Cell
 */
Board::Cell Board::get_random_empty_cell() {  // TODO: can improve this?
    while (!empty_random_cells.empty() && board[empty_random_cells.back()] != NONE) empty_random_cells.pop_back();
    return (empty_random_cells.empty()) ? -1 : empty_random_cells.back();
}

bool Board::equal(Board other_board) {
    for (int pos = 0; pos < NUM_CELL; pos++)
        if (other_board.board[pos] != board[pos]) return false;
    return true;
}
