#pragma once
#include <iostream>
using namespace std;

#define H 20
#define W 20

class Piece
{
protected:
    int x, y;
    char shape[4][4];

public:
    Piece();

    virtual void initShape() = 0;

    void moveLeft();
    void moveRight();
    void moveDown();

    void rotate();

    bool canMove(int dx, int dy, char board[H][W]);

    void block2Board(char board[H][W]);

    void boardDelBlock(char board[H][W]);

    int getX();
    int getY();

    char getCell(int i, int j);
};