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
    
    def __init__(self, input_state=None):
        self.state = input_state[:] if input_state is not None else [0] * 16
        return
    
    def __getitem__(self, pos):
        return self.state[pos]
    
    def __setitem__(self, pos, tile):
        self.state[pos] = tile
        return
    
    def place(self, pos, tile):
        """
        place a tile (index value) to the specific position (1-d form index)
        return 0 if the action is valid, or -1 if not
        """
        if pos >= 16 or pos < 0:
            return -1
        if tile != 1 and tile != 2:
            return -1
        self.state[pos] = tile
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
    
    def slide_left(self):
        new_state, score = [], 0
        for i in range(0, 16, 4):
            row = self.state[i: i + 4]
            for offset in range(3):
                if (row[offset] + row[offset + 1]) % 3 == 0:
                    row[offset] = max(row[offset], row[offset + 1]) + 1
                    row[offset + 1] = 0
                    score += self.SCORES[row[offset]] - self.SCORES[row[offset] - 1]
            new_state += row

        if self.state == new_state:
            return -1

        self.state = new_state
        return score
    
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
        self.state = [self.state[r + i] for r in range(0, 16, 4) for i in reversed(range(4))]
        return
    
    def reflect_vertical(self):
        self.state = [self.state[c + i] for c in reversed(range(0, 16, 4)) for i in range(4)]
        return
    
    def transpose(self):
        self.state = [self.state[r + i] for i in range(4) for r in range(0, 16, 4)]
        return
    
    def rotate(self, rot = 1):
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
        state = '+' + '-' * 24 + '+\n'
        for row in [self.state[r:r + 4] for r in range(0, 16, 4)]:
            state += ('|' + ''.join('{0:6d}'.format((1 << t) & -2) for t in row) + '|\n')
        state += '+' + '-' * 24 + '+'
        return state
    
    
if __name__ == '__main__':
    print('2048 Demo: board.py\n')
    
    state = board()
    state[10] = 10
    print(state)
