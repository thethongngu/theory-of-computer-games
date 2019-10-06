#!/usr/bin/env python3


class node:

    def __init__(self, parent, curr_board, last_action):
        self.parent = parent
        self.curr_board = curr_board
        self.last_action = last_action

    def get_score(self):
        return self.curr_board.curr_score()

