#!/usr/bin/env python3

"""
Basic framework for developing 2048 programs in Python

Author: Hung Guei (moporgic)
        Computer Games and Intelligence (CGI) Lab, NCTU, Taiwan
        http://www.aigames.nctu.edu.tw
"""


class board:
    """ simple implementation of Threes puzzle """

    SCORES = [0, 0, 0, 3, 9, 27, 81, 243, 729, 2187, 6561, 19683, 59049, 177147, 531441]
    TILE_VALUES = [0, 1, 2, 3, 6, 12, 24, 48, 96, 192, 384, 768, 1536, 3072, 6144]

    def __init__(self, input_state=None):
        self.state = 0 if input_state is None else input_state.state
        return

    def __getitem__(self, pos):
        # print("pos: " + str(pos))
        # print(15 << (pos * 4))
        # print(self.state)
        # print()
        return (self.state & (15 << (pos * 4))) >> (pos * 4)

    def __setitem__(self, pos, tile):
        remove_mask = (1 << 64) - (1 << (pos + 1) * 4) + ((1 << (pos * 4)) - 1)
        add_mask = tile << (pos * 4)
        self.state = (self.state & remove_mask) | add_mask
        return

    def place(self, pos, tile):
        """
        place a tile (index value) to the specific position (1-d form index)
        return 0 if the action is valid, or -1 if not
        """
        if pos >= 16 or pos < 0:
            return -1
        if 1 > tile > 3:
            return -1
        self[pos] = tile
        return 0

    def slide(self, opcode):
        """
        apply an action to the board
        return the reward of the action, or -1 if the action is illegal
        """
        if opcode == 0:
            return self.slide_up()
        if opcode == 1:
            return self.slide_right()
        if opcode == 2:
            return self.slide_down()
        if opcode == 3:
            return self.slide_left()
        return -1

    def curr_score(self):
        res = 0
        for pos in range(0, 15):
            res += self.SCORES[self[pos]]
        return res

    def slide_left(self):
        new_state = board()
        score, r = 0, 0
        while r < 16:
            row = [self[r], self[r + 1], self[r + 2], self[r + 3]]
            for i in range(1, 4):
                if row[i] == 0:
                    continue
                if row[i - 1] == 0:
                    row[i - 1] = row[i]
                    row[i] = 0
                elif (row[i - 1] == row[i] and row[i - 1] >= 3 and row[i] >= 3) or \
                     (self.TILE_VALUES[row[i - 1]] + self.TILE_VALUES[row[i]] == 3):

                    row[i - 1] = max(row[i - 1], row[i]) + 1
                    row[i] = 0

            new_state[r] = row[0]
            new_state[r + 1] = row[1]
            new_state[r + 2] = row[2]
            new_state[r + 3] = row[3]
            r += 4

        if self.state == new_state.state:
            return -1

        self.state = new_state.state
        return self.curr_score()

    def slide_right(self):
        self.reflect_horizontal()
        score = self.slide_left()
        self.reflect_horizontal()
        return score

    def slide_up(self):
        self.transpose()
        score = self.slide_left()
        self.transpose()
        return score

    def slide_down(self):
        self.transpose()
        score = self.slide_right()
        self.transpose()
        return score

    def reflect_horizontal(self):
        tmp = [self[r + i] for r in range(0, 16, 4) for i in reversed(range(4))]
        new_state = 0
        for pos in range(15):
            new_state += tmp[pos] << (pos + 4)
        self.state = new_state
        return

    def reflect_vertical(self):
        tmp = [self[c + i] for c in reversed(range(0, 16, 4)) for i in range(4)]
        new_state = 0
        for pos in range(15):
            new_state += tmp[pos] << (pos + 4)
        self.state = new_state
        return

    def transpose(self):
        tmp = [self[r + i] for i in range(4) for r in range(0, 16, 4)]
        new_state = 0
        for pos in range(15):
            new_state += tmp[pos] << (pos + 4)
        self.state = new_state
        return

    def rotate(self, rot=1):
        rot = ((rot % 4) + 4) % 4
        if rot == 1:
            self.rotate_right()
            return
        if rot == 2:
            self.reverse()
            return
        if rot == 3:
            self.rotate_left()
            return
        return

    def rotate_right(self):
        """ clockwise rotate the board """
        self.transpose()
        self.reflect_horizontal()
        return

    def rotate_left(self):
        """ counterclockwise rotate the board """
        self.transpose()
        self.reflect_vertical()
        return

    def reverse(self):
        self.reflect_horizontal()
        self.reflect_vertical()
        return

    def __str__(self):
        curr_state = '+' + '-' * 24 + '+\n'
        for row in [[self[r], self[r + 1], self[r + 2], self[r + 3]] for r in range(0, 16, 4)]:
            curr_state += ('|' + ''.join('{0:6d}'.format(t) for t in row) + '|\n')
        curr_state += '+' + '-' * 24 + '+'
        return curr_state


if __name__ == '__main__':
    print('2048 Demo: board.py\n')

    state = board()
    state[1] = 2
    state[2] = 2
    state[5] = 3
    state[15] = 1
    print(state[1])
    print(state)
