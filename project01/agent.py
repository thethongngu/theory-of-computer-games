#!/usr/bin/env python3

"""
Basic framework for developing 2048 programs in Python

Author: Hung Guei (moporgic)
        Computer Games and Intelligence (CGI) Lab, NCTU, Taiwan
        http://www.aigames.nctu.edu.tw
"""

from board import board
from action import action
from tree import node
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

    def open_episode(self, flag=""):
        self.bag = []

    @staticmethod
    def get_tile_id_by_action(last_move):
        side_tile = []
        if last_move == 0:
            side_tile = [12, 13, 14, 15]
        if last_move == 1:
            side_tile = [0, 4, 8, 12]
        if last_move == 2:
            side_tile = [0, 1, 2, 3]
        if last_move == 3:
            side_tile = [3, 7, 11, 15]
        return side_tile

    def take_action(self, input_state, actions):

        side_tile = self.get_tile_id_by_action(actions[-1].event()) if actions else [i for i in range(16)]
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

    def generate_tree(self, curr_state, last_action, parent, depth):
        """ Generate tree from current board """

        curr_node = node(parent, curr_state, last_action)
        if depth == 0:
            return curr_node.get_score()
        else:
            if last_action == action.place.type:  # parent node is environment
                max_score, max_op = 0
                for op in range(4):
                    curr_board = board(curr_state)
                    if curr_board.slide(op):
                        curr_score, _ = self.generate_tree(curr_board, op, curr_node, depth - 1)
                        if curr_score > max_score:
                            max_score = curr_score
                            max_op = op
                return max_score, max_op

            else:  # parent node is player
                side_tiles = rndenv.get_tile_id_by_action(last_action)
                min_score = 0
                for pos in side_tiles:
                    for tile_id in [1, 2, 3]:
                        curr_board = board(curr_state)
                        curr_board.place(pos, tile_id)
                        curr_score, _ = self.generate_tree(curr_board, action.place.type, curr_node, depth - 1)
                        min_score = min(min_score, curr_score)
                return min_score, None

    def take_action(self, state, actions):
        max_score, max_op = self.generate_tree(state, actions[-1], None, 3)
        return action() if max_op is None else action.slide(max_op)


if __name__ == '__main__':
    print('2048 Demo: agent.py\n')
