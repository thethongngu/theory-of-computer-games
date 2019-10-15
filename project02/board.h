#pragma once
#include <array>
#include <iostream>
#include <iomanip>

/**
 * array-based board for 2048
 *
 * index (1-d form):
 *  (0)  (1)  (2)  (3)
 *  (4)  (5)  (6)  (7)
 *  (8)  (9) (10) (11)
 * (12) (13) (14) (15)
 *
 */
class Board {
public:
	typedef uint32_t Cell;
	typedef std::array<Cell, 4> Row;
	typedef std::array<Row, 4> Grid;
	typedef uint64_t Data;
	typedef int Reward;

public:
	Board() : tile(), attr(0) {}
	explicit Board(const Grid& b, Data v = 0) : tile(b), attr(v) {}
	Board(const Board& b) = default;
	Board& operator =(const Board& b) = default;

	explicit operator Grid&() { return tile; }
	explicit operator const Grid&() const { return tile; }
	Row& operator [](unsigned i) { return tile[i]; }
	const Row& operator [](unsigned i) const { return tile[i]; }
	Cell& operator ()(unsigned i) { return tile[i / 4][i % 4]; }
	const Cell& operator ()(unsigned i) const { return tile[i / 4][i % 4]; }

	Data info() const { return attr; }
	Data info(Data dat) { Data old = attr; attr = dat; return old; }

public:
	bool operator ==(const Board& b) const { return tile == b.tile; }
	bool operator < (const Board& b) const { return tile < b.tile; }
	bool operator !=(const Board& b) const { return !(*this == b); }
	bool operator > (const Board& b) const { return b < *this; }
	bool operator <=(const Board& b) const { return !(b < *this); }
	bool operator >=(const Board& b) const { return !(*this < b); }

public:

	/**
	 * place a tile (index value) to the specific position (1-d form index)
	 * return 0 if the action is valid, or -1 if not
	 */
	Reward place(unsigned pos, Cell tile) {
		if (pos >= 16) return -1;
		if (tile != 1 && tile != 2) return -1;
		operator()(pos) = tile;
		return 0;
	}

	/**
	 * apply an action to the board
	 * return the reward of the action, or -1 if the action is illegal
	 */
	Reward slide(unsigned opcode) {
		switch (opcode & 0b11) {
		case 0: return slide_up();
		case 1: return slide_right();
		case 2: return slide_down();
		case 3: return slide_left();
		default: return -1;
		}
	}

	Reward slide_left() {
		Board prev = *this;
		Reward score = 0;
		for (int r = 0; r < 4; r++) {
			auto& row = tile[r];
			int top = 0, hold = 0;
			for (int c = 0; c < 4; c++) {
				int tile = row[c];
				if (tile == 0) continue;
				row[c] = 0;
				if (hold) {
					if (tile == hold) {
						row[top++] = ++tile;
						score += (1 << tile);
						hold = 0;
					} else {
						row[top++] = hold;
						hold = tile;
					}
				} else {
					hold = tile;
				}
			}
			if (hold) tile[r][top] = hold;
		}
		return (*this != prev) ? score : -1;
	}
	Reward slide_right() {
		reflect_horizontal();
		Reward score = slide_left();
		reflect_horizontal();
		return score;
	}
	Reward slide_up() {
		rotate_right();
		Reward score = slide_right();
		rotate_left();
		return score;
	}
	Reward slide_down() {
		rotate_right();
		Reward score = slide_left();
		rotate_left();
		return score;
	}

	void transpose() {
		for (int r = 0; r < 4; r++) {
			for (int c = r + 1; c < 4; c++) {
				std::swap(tile[r][c], tile[c][r]);
			}
		}
	}

	void reflect_horizontal() {
		for (int r = 0; r < 4; r++) {
			std::swap(tile[r][0], tile[r][3]);
			std::swap(tile[r][1], tile[r][2]);
		}
	}

	void reflect_vertical() {
		for (int c = 0; c < 4; c++) {
			std::swap(tile[0][c], tile[3][c]);
			std::swap(tile[1][c], tile[2][c]);
		}
	}

	/**
	 * rotate the board clockwise by given times
	 */
	void rotate(int r = 1) {
		switch (((r % 4) + 4) % 4) {
		default:
		case 0: break;
		case 1: rotate_right(); break;
		case 2: reverse(); break;
		case 3: rotate_left(); break;
		}
	}

	void rotate_right() { transpose(); reflect_horizontal(); } // clockwise
	void rotate_left() { transpose(); reflect_vertical(); } // counterclockwise
	void reverse() { reflect_horizontal(); reflect_vertical(); }

public:
	friend std::ostream& operator <<(std::ostream& out, const Board& b) {
		out << "+------------------------+" << std::endl;
		for (auto& row : b.tile) {
			out << "|" << std::dec;
			for (auto t : row) out << std::setw(6) << ((1 << t) & -2u);
			out << "|" << std::endl;
		}
		out << "+------------------------+" << std::endl;
		return out;
	}

private:
	Grid tile;
	Data attr;
};
