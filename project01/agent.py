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

    slide_up_tile = [12, 13, 14, 15]
    slide_right_tile = [0, 4, 8, 12]
    slide_down_tile = [0, 1, 2, 3]
    slide_left_tile = [3, 7, 11, 15]
    whole_board = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15]

    @staticmethod
    def get_tile_id_by_action(last_move):
        if last_move == 0:
            return rndenv.slide_up_tile
        elif last_move == 1:
            return rndenv.slide_right_tile
        elif last_move == 2:
            return rndenv.slide_down_tile
        elif last_move == 3:
            return rndenv.slide_left_tile
        else:
            return rndenv.whole_board

    def take_action(self, input_state, actions):

        side_tile = self.get_tile_id_by_action(actions[-1].event()) if actions else rndenv.whole_board
        empty_cell = [tile for tile in side_tile if not input_state.state[tile]]

        if not self.bag:  # refill the bag if empty
            self.bag = [1, 2, 3]
            self.shuffle(self.bag)

        if empty_cell:
            pos = self.choice(empty_cell)
            tile = self.bag.pop()
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

    def generate_tree(self, curr_state, last_action, avai_env_tile, depth, alpha, beta):
        """ Generate tree from current board """

        if depth == 0:
            return curr_state.curr_score(), last_action

        elif last_action == action.place.type:  # player
            max_score, max_op = 0, -1
            for op in range(4):
                curr_board = board(curr_state)
                if curr_board.slide(op) != -1:
                    curr_score, _ = self.generate_tree(
                        curr_board, op, avai_env_tile, depth - 1, alpha, beta
                    )
                    if curr_score > max_score:
                        max_score = curr_score
                        max_op = op
                        alpha = max_score
                    if beta <= alpha:
                        break
            return max_score, max_op

        else:  # environment
            min_score = 1000000000
            side_tiles = rndenv.get_tile_id_by_action(last_action)
            stop = False
            if not avai_env_tile:
                avai_env_tile = [1, 2, 3]

            for pos in side_tiles:
                if curr_state[pos]:
                    continue
                for tile_id in avai_env_tile:

                    curr_board = board(curr_state)
                    curr_board.place(pos, tile_id)
                    avai_env_tile.remove(tile_id)

                    curr_score, _ = self.generate_tree(
                        curr_board, action.place.type, avai_env_tile, depth - 1, alpha, beta
                    )

                    avai_env_tile.append(tile_id)
                    min_score = min(min_score, curr_score)
                    beta = min(beta, min_score)
                    if beta <= alpha:
                        stop = True
                        break
                if stop:
                    break

            return min_score, None

    def take_action(self, state, actions):
        avai_env_tiles = [1, 2, 3]
        for i in range(-1, -1 * (len(actions) % 3), -1):
            avai_env_tiles.remove(actions[-1 * i].tile())

        max_score, max_op = self.generate_tree(state, actions[-1].type, avai_env_tiles, 3, 0, 1000000000)
        if max_op == -1:
            legal = []
            for op in range(4):
                score = board(state).slide(op)
                if score != -1:
                    legal.append((score, op))

            if legal:
                sorted(legal, key=lambda t: t[0])
                return action.slide(legal[0][1])
            else:
                return action()
        else:
            return action.slide(max_op)


if __name__ == '__main__':
    print('2048 Demo: agent.py\n')
