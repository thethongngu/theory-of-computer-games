//
// Created by thethongngu on 12/15/19.
//

#ifndef PROJECT04_MCTS_H
#define PROJECT04_MCTS_H

#endif //PROJECT04_MCTS_H

#include <iostream>
#include <cmath>
#include <cstring>

#include "global.h"
#include "MCTS.h"

using namespace std;

double MCTS::getscore(Node *nodeptr, int child) {
    Node *tmp = (nodeptr->child_ptr) + child;
    auto &p = tmp->last_pos;
    auto &c = tmp->last_color;
    double &N = tmp->count;
    double &NR = tmp->rave_count;
    double ret = tmp->rave_mean * NR + tmp->mean * N + sqrt(nodeptr->log_count * N) * C_BIAS;

    return ret / (N + NR);
}

Node *MCTS::getbestchild(Node *nodeptr) {
    if (nodeptr->num_child == 0)return nullptr;
    int i, ret = 0;
    double ans, tmp = getscore(nodeptr, 0), tma;//tmp minus anwser
    ans = tmp;
    selectlist[0] = 0;
    slsize = 1;
    for (i = 1; i < (nodeptr->num_child); i++) {
        tmp = getscore(nodeptr, i);
        tma = tmp - ans;
        if (tma > -0.0001)//tmp >= ans
        {
            if (tma > 0.0001) // tmp > ans
            {
                selectlist[0] = i;
                slsize = 1;
                ans = tmp;
            } else  //tmp == ans
            {
                selectlist[slsize] = i;
                slsize++;
            }
        }
    }
    //for(i=0;i<slsize;i++)
    //{
    //	cout<<selectlist[i]<<' ';
    //}
    //cout<<endl;
    ret = selectlist[rand() % slsize];
    return (nodeptr->child_ptr + ret);
}

void MCTS::select(Board &b) {

    bool j = !b.just_play_color();//next to play
    Node *nodeptr = root;
    b.bpsize = 0;
    b.wpsize = 0;
    path.clear();
    path.push_back(nodeptr);
    while (nodeptr->child_ptr != nullptr && nodeptr->num_child != 0) {
        nodeptr = getbestchild(nodeptr);
        path.push_back(nodeptr);
        //	cout<<inttostring(nodeptr->last_pos)<<' ';
        if (nodeptr->last_color == BLACK) {
            b.addbp(nodeptr->last_pos);
            sbnum++;
        } else {
            b.addwp(nodeptr->last_pos);
            swnum++;
        }
        b.add_piece(nodeptr->last_pos, nodeptr->last_color);
    }
//	b.showBoard();
    //system("pause");
}

void MCTS::update(double result, Board &b) {
    for (int i = 0; i < path.size(); i++) {
        path[i]->add_normal_result(result);
        if (path[i]->last_color == 0) {
            for (int j = 0; j < b.wpsize; j++) {
                int k = (path[i]->child_pos[b.wpath[j]]);
                if (k != -1)
                    ((path[i]->child_ptr) + k)->add_rave_result(result);
            }
        } else {
            for (int j = 0; j < b.bpsize; j++) {
                int k = (path[i]->child_pos[b.bpath[j]]);
                if (k != -1)
                    ((path[i]->child_ptr) + k)->add_rave_result(result);
            }
        }
    }
}

void MCTS::run_a_cycle() {
    Board board;
    double result;
    board = rBoard;
    sbnum = swnum = 0;
    select(board);
    Node &last = (*(path.back()));
    Node *nodeptr;
    if (last.num_child == 0 && last.count > PARENT_SIMS) {
        last.expansion(board);
        if (last.num_child != 0) {
            totalnode += last.num_child;
            nodeptr = getbestchild(&last);
            path.push_back(nodeptr);
            if (nodeptr->last_color == 0) {
                board.addbp(nodeptr->last_pos);
                sbnum++;
            } else {
                board.addwp(nodeptr->last_pos);
                swnum++;
            }
            board.add_piece(nodeptr->last_pos, nodeptr->last_color);

        }
    }
    total += sbnum;
    total += swnum;
    board.getv(bone, wone, two, bsize, wsize, tsize);

    if ((board.just_play_color() == BLACK) && (wsize + tsize) == 0) {
        result = 1;
    } else if (board.just_play_color() == WHITE && (bsize + tsize) == 0) {
        result = -1;
    } else {
        result = board.simulate(!board.just_play_color(), bone, wone, two, bsize, wsize, tsize);
    }
    update(result, board);
}

void MCTS::reset(Board &b) {
    rBoard = b;
    root = new Node;
    root->last_color = rBoard.just_play_color();
    root->last_pos = 81;
    root->count = 0;
    root->log_count = 1;
    memset(root->child_pos, -1, sizeof(root->child_pos));
    root->expansion(b);
    total = 0;
    totalnode = 0;
}

void MCTS::show_path() {
    Node *nodeptr = root;
    int i, k;
    i = 0;
    while (nodeptr->child_ptr != nullptr && i < 10) {
        k = nodeptr->get_max_move();
        nodeptr = nodeptr->child_ptr + k;
        i++;
        if (nodeptr != nullptr) {
            if (nodeptr->last_color == BLACK)
                cerr << "B(";
            else
                cerr << "W(";
            cerr << inttostring(nodeptr->last_pos) << ") ";
        }
    }
    cerr << endl;
}

void MCTS::clear() {
    delete root;
}

string MCTS::inttoGTPstring(int i) {
    string s = "A1";
    s[0] += i / 9;
    if (s[0] >= 'I')s[0]++;
    s[1] += i % 9;
    return s;
}

string MCTS::inttostring(int i) {
    return inttoGTPstring(i);
}

MCTS::MCTS() {

}

MCTS::~MCTS() {}

