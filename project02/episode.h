#pragma once
#include <list>
#include <vector>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <chrono>
#include <numeric>
#include "board.h"
#include "action.h"
#include "agent.h"

class Statistics;

class Episode {

friend class statistic;

public:
	Episode() : ep_state(initial_state()), ep_score(0), ep_time(0) {
	    ep_boards.push_back(initial_state());
	    ep_moves.reserve(10000);
	}

public:
	Board& state() { return ep_state; }
	const Board& state() const { return ep_state; }
	Board::Reward score() const { return ep_score; }

	void open_episode(const std::string& tag) {
		ep_open = { tag, millisec() };
	}
	void close_episode(const std::string& tag) {
		ep_close = { tag, millisec() };
	}
	bool apply_action(Action move) {
		Board::Reward reward = move.apply(state());
		if (reward == -1) return false;
		ep_moves.emplace_back(move, reward, millisec() - ep_time);
		ep_boards.push_back(state());
		ep_score += reward;
		return true;
	}
	Agent& take_turns(Agent& player, Agent& evil) {
		ep_time = millisec();
		return (std::max(step() + 1, size_t(9)) % 2) ? evil : player;
	}
	Agent& last_turns(Agent& play, Agent& evil) {
		return take_turns(evil, play);
	}

public:
	size_t step(unsigned action_type = -1u) const {
		int size = ep_moves.size(); // 'int' is important for handling 0
		switch (action_type) {
		case Action::Slide::type: return (size - 8) / 2;
		case Action::Place::type: return size - (size - 8) / 2;
		default:                  return size;
		}
	}

	time_t time(unsigned who = -1u) const {
		time_t time = 0;
		switch (who) {
		case Action::Place::type:
            for(int i = 0; i < 9 && i < ep_moves.size(); i++) time += ep_moves[i].time;  // 9 first steps (0-8)
            for(int i = 10; i < ep_moves.size(); i++) time += ep_moves[i].time;
            break;
		case Action::Slide::type:
            for(int i = 9; i < ep_moves.size(); i++) time += ep_moves[i].time;  // start from 9
			break;
		default:
			time = ep_close.when - ep_open.when;
			break;
		}
		return time;
	}

	std::vector<Action> actions(unsigned action_type = -1u) const {
		std::vector<Action> res;
		switch (action_type) {
            case Action::Place::type:
                for(int i = 0; i < 9 && i < ep_moves.size(); i++) res.push_back(ep_moves[i].action);  // 9 first steps (0-8)
                for(int i = 10; i < ep_moves.size(); i++) res.push_back(ep_moves[i].action);
                break;
            case Action::Slide::type:
                for(int i = 9; i < ep_moves.size(); i++) res.push_back(ep_moves[i].action);  // start from 9
                break;
            default:
                for(int i = 0; i < ep_moves.size(); i++) res.push_back(ep_moves[i].action);
                break;
		}
		return res;
	}

	const std::vector<Board>& states() {
	    return ep_boards;
	}

public:

	friend std::ostream& operator <<(std::ostream& out, const Episode& ep) {
		out << ep.ep_open << '|';
		for (const move& mv : ep.ep_moves) out << mv;
		out << '|' << ep.ep_close;
		return out;
	}
	friend std::istream& operator >>(std::istream& in, Episode& ep) {
		ep = {};
		std::string token;
		std::getline(in, token, '|');
		std::stringstream(token) >> ep.ep_open;
		std::getline(in, token, '|');
		for (std::stringstream moves(token); !moves.eof(); moves.peek()) {
			ep.ep_moves.emplace_back();
			moves >> ep.ep_moves.back();
			ep.ep_score += Action(ep.ep_moves.back()).apply(ep.ep_state);
		}
		std::getline(in, token, '|');
		std::stringstream(token) >> ep.ep_close;
		return in;
	}

protected:

	struct move {
		Action action;
		Board::Reward reward;
		time_t time;
		move(Action code = {}, Board::Reward reward = 0, time_t time = 0) : action(code), reward(reward), time(time) {}

		explicit operator Action() const { return action; }
		friend std::ostream& operator <<(std::ostream& out, const move& m) {
			out << m.action;
			if (m.reward) out << '[' << std::dec << m.reward << ']';
			if (m.time) out << '(' << std::dec << m.time << ')';
			return out;
		}
		friend std::istream& operator >>(std::istream& in, move& m) {
			in >> m.action;
			m.reward = 0;
			m.time = 0;
			if (in.peek() == '[') {
				in.ignore(1);
				in >> std::dec >> m.reward;
				in.ignore(1);
			}
			if (in.peek() == '(') {
				in.ignore(1);
				in >> std::dec >> m.time;
				in.ignore(1);
			}
			return in;
		}
	};

	struct meta {
		std::string tag;
		time_t when;
		meta(const std::string& tag = "N/A", time_t when = 0) : tag(tag), when(when) {}

		friend std::ostream& operator <<(std::ostream& out, const meta& m) {
			return out << m.tag << "@" << std::dec << m.when;
		}
		friend std::istream& operator >>(std::istream& in, meta& m) {
			return std::getline(in, m.tag, '@') >> std::dec >> m.when;
		}
	};

	/**
	 * Zero board
	 * @return Board()
	 */
	static Board initial_state() {
		return {};
	}
	static time_t millisec() {
		auto now = std::chrono::system_clock::now().time_since_epoch();
		return std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
	}

private:
	Board ep_state;
	Board::Reward ep_score;
	std::vector<move> ep_moves;
	std::vector<Board> ep_boards;
	time_t ep_time;

	meta ep_open;
	meta ep_close;
};
