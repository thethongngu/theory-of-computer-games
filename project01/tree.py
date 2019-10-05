#!/usr/bin/env python3


class node:

    def __init__(self, left_node, right_node, data):
        self.left_node = left_node
        self.right_node = right_node
        self.data = data

    def get_score(self):
        return self.data

    def get_max(self):
        if self.left_node is None and self.right_node is None:
            return self.get_score()
        return max(self.left_node.get_max(), self.right_node.get_max())

    def get_min(self):
        if self.left_node is None and self.right_node is None:
            return self.get_score()
        return min(self.left_node.get_min(), self.right_node.get_min())
