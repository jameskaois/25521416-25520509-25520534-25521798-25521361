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
    virtual ~Piece() {}

    virtual void initShape() = 0;

    void moveLeft();
    void moveRight();
    void moveDown();

    virtual void rotate(char board[H][W]);

    bool canMove(int dx, int dy, char board[H][W]);

    void block2Board(char board[H][W]);

    void boardDelBlock(char board[H][W]);

    int getX();
    int getY();

    char getCell(int i, int j);
    void getShape(char dest[4][4]) 
    {
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            dest[i][j] = shape[i][j];
    }
};

class PieceO : public Piece {
public:
    void initShape() override {
        shape[1][1] = 'O'; shape[1][2] = 'O';
        shape[2][1] = 'O'; shape[2][2] = 'O';
    }
    void rotate(char board[H][W]) override {
    }
};

class PieceI : public Piece {
public:
    void initShape() override {
        shape[1][0] = 'I'; shape[1][1] = 'I';
        shape[1][2] = 'I'; shape[1][3] = 'I';
    }
};

class PieceT : public Piece {
public:
    void initShape() override {
        shape[1][1] = 'T'; 
        shape[2][0] = 'T'; shape[2][1] = 'T'; shape[2][2] = 'T';
    }
};

class PieceL : public Piece {
public:
    void initShape() override {
        shape[1][2] = 'L'; 
        shape[2][0] = 'L'; shape[2][1] = 'L'; shape[2][2] = 'L';
    }
};

class PieceJ : public Piece {
public:
    void initShape() override {
        shape[1][0] = 'J'; 
        shape[2][0] = 'J'; shape[2][1] = 'J'; shape[2][2] = 'J';
    }
};

class PieceS : public Piece {
public:
    void initShape() override {
        shape[1][1] = 'S'; shape[1][2] = 'S';
        shape[2][0] = 'S'; shape[2][1] = 'S';
    }
};

class PieceZ : public Piece {
public:
    void initShape() override {
        shape[1][0] = 'Z'; shape[1][1] = 'Z';
        shape[2][1] = 'Z'; shape[2][2] = 'Z';
    }
};