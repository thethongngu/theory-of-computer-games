#!/usr/bin/env python3

"""
Basic framework for developing 2048 programs in Python

Author: Hung Guei (moporgic)
        Computer Games and Intelligence (CGI) Lab, NCTU, Taiwan
        http://www.aigames.nctu.edu.tw
"""

from board import board
from action import action
import random


class agent:
    """ base agent """

    def __init__(self, options=""):
        self.info = {}
        options = "name=unknown role=unknown " + options
        for option in options.split():
            data = option.split("=", 1) + [True]
            self.info[data[0]] = data[1]
        return

    def open_episode(self, flag=""):
        return

    def close_episode(self, flag=""):
        return

    def take_action(self, state, actions):
        return action()

    def check_for_win(self, state):
        return False

    def property(self, key):
        return self.info[key] if key in self.info else None

    def notify(self, message):
        data = message.split("=", 1) + [True]
        self.info[data[0]] = data[1]
        return

    def name(self):
        return self.property("name")

    def role(self):
        return self.property("role")


class random_agent(agent):
    """ base agent for agents with random behavior """

    def __init__(self, options=""):
        super().__init__(options)
        seed = self.property("seed")
        if seed is not None:
            random.seed(int(seed))
        self.rstate = random.getstate()
        return

    def choice(self, seq):
        random.setstate(self.rstate)
        target = random.choice(seq)
        self.rstate = random.getstate()
        return target

    def shuffle(self, seq):
        random.setstate(self.rstate)
        random.shuffle(seq)
        self.rstate = random.getstate()
        return


class rndenv(random_agent):
    """
    random environment
    add a new random tile to an empty cell
    1-tile: 33.33%
    2-tile: 33.33%
    3-tile: 33.33%
    """

    def __init__(self, options=""):
        super().__init__("name=random role=environment " + options)
        self.bag = []
        return

    def take_action(self, input_state, actions):

        side_tile = []
        if actions:
            last_move = actions[-1].event()
            if last_move == 0:
                side_tile = [12, 13, 14, 15]
            if last_move == 1:
                side_tile = [0, 4, 8, 12]
            if last_move == 2:
                side_tile = [0, 1, 2, 3]
            if last_move == 3:
                side_tile = [3, 7, 11, 15]
        else:
            side_tile = [i for i in range(16)]

        empty_cell = [pos for pos, tile in enumerate(input_state.state) if not tile and pos in side_tile]

        if not self.bag:  # refill the bag if empty
            self.bag = [1, 2, 3]

        if empty_cell:
            pos = self.choice(empty_cell)
            self.shuffle(self.bag)
            tile = self.bag[-1]
            self.bag.pop()
            return action.place(pos, tile)
        else:
            return action()


class player(random_agent):
    """
    dummy player
    select a legal action randomly
    """

    def __init__(self, options=""):
        super().__init__("name=dummy role=player " + options)
        return

    def take_action(self, state, actions):
        legal = [op for op in range(4) if board(state).slide(op) != -1]
        if legal:
            op = self.choice(legal)
            return action.slide(op)
        else:
            return action()


if __name__ == '__main__':
    print('2048 Demo: agent.py\n')
