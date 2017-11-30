//
// Created by Edouard on 29/11/2017.
//

#ifndef GOMOKU_BRAIN_H
#define GOMOKU_BRAIN_H

#include "Board.h"

class Brain{
private:
    int _lastMoveX;
    int _lastMoveY;
public:
    Brain();
    ~Brain();
    int calculateTurn(int x, int y);
    int putFirstPiece();
    int putBoardPiece(int x, int y, Board::TypeStone type);
    int getLastMoveX() const;
    int getLastMoveY() const;
    int clear();
};

#endif //GOMOKU_BRAIN_H