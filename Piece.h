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
    int color;
public:
    Piece();
    virtual ~Piece() {}

    virtual void initShape() = 0;

    void moveLeft();
    void moveRight();
    void moveDown();

    virtual void rotate(char board[H][W]);

    bool canMove(int dx, int dy, char board[H][W]);

    void block2Board(char board[H][W], int boardColor[H][W]);
    void boardDelBlock(char board[H][W], int boardColor[H][W]);

    int getX();
    int getY();

    char getCell(int i, int j);

    int getColor() { return color; }
};

class PieceO : public Piece {
public:
    void initShape() override {
        shape[1][1] = 'O'; shape[1][2] = 'O';
        shape[2][1] = 'O'; shape[2][2] = 'O';
    }
    void rotate(char board[H][W]) override {
    }

    PieceO() {
        color = 14; // vàng 
    }

};

class PieceI : public Piece {
public:
    void initShape() override {
        shape[1][0] = 'I'; shape[1][1] = 'I';
        shape[1][2] = 'I'; shape[1][3] = 'I';
    }

    PieceI() {
        color = 9; // xanh dương
    }
};

class PieceT : public Piece {
public:
    void initShape() override {
        shape[1][1] = 'T'; 
        shape[2][0] = 'T'; shape[2][1] = 'T'; shape[2][2] = 'T';
    }
    PieceT() { color = 13; } // tím
};

class PieceL : public Piece {
public:
    void initShape() override {
        shape[1][2] = 'L'; 
        shape[2][0] = 'L'; shape[2][1] = 'L'; shape[2][2] = 'L';
    }
    PieceL() { color = 6; } // cam

};

class PieceJ : public Piece {
public:
    void initShape() override {
        shape[1][0] = 'J'; 
        shape[2][0] = 'J'; shape[2][1] = 'J'; shape[2][2] = 'J';
    }
    PieceJ() { color = 11; } // xanh nhạt

};

class PieceS : public Piece {
public:
    void initShape() override {
        shape[1][1] = 'S'; shape[1][2] = 'S';
        shape[2][0] = 'S'; shape[2][1] = 'S';
    }
    PieceS() { color = 10; } // xanh lá
};

class PieceZ : public Piece {
public:
    void initShape() override {
        shape[1][0] = 'Z'; shape[1][1] = 'Z';
        shape[2][1] = 'Z'; shape[2][2] = 'Z';
    }
    PieceZ() { color = 12; } // đỏ
};