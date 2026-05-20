#include <windows.h>
#include "Piece.h"

Piece::Piece()
{
    x = W / 2 - 2;
    y = 1;

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            shape[i][j] = ' ';
        }
    }
}



void Piece::moveLeft()
{
    x--;
}

void Piece::moveRight()
{
    x++;
}

void Piece::moveDown()
{
    y++;
}

// Xoay khối gạch 90 độ và kiểm tra va chạm
void Piece::rotate(char board[H][W])
{
    char temp[4][4];

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            temp[j][3 - i] = shape[i][j];
        }
    }

    bool canRotate = true;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (temp[i][j] != ' ') {
                int tx = x + j;
                int ty = y + i;

                if (tx < 1 || tx >= W - 1 || ty >= H - 1 || ty < 1) {
                    canRotate = false; break;
                }
                if (board[ty][tx] != ' ') {
                    canRotate = false; break;
                }
            }
        }
        if (!canRotate) break;
    }

    if (canRotate) {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                shape[i][j] = temp[i][j];
            }
        }
    }
}

// Kiểm tra va chạm: xem khối gạch có di chuyển được tới khoảng dx, dy không
bool Piece::canMove(int dx, int dy, char board[H][W])
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (shape[i][j] != ' ')
            {
                int tx = x + j + dx;
                int ty = y + i + dy;

                if (tx < 1 || tx >= W - 1 || ty >= H - 1 || ty < 1)
                    return false;

                if (board[ty][tx] != ' ')
                    return false;
            }
        }
    }

    return true;
}

// Chốt vị trí khối gạch vào mảng bảng game khi chạm đáy
void Piece::block2Board(char board[H][W],int boardColor[H][W])
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (shape[i][j] != ' ' && y + i < H)
            {
                board[y + i][x + j] = shape[i][j];
                boardColor[y + i][x + j] = color;
            }
        }
    }
}

void Piece::boardDelBlock(char board[H][W], int boardColor[H][W]    )
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (shape[i][j] != ' ' && y + i >= 0 && y + i < H)
            {
                board[y + i][x + j] = ' ';
                boardColor[y + i][x + j] = 7;
            }
        }
    }
}

int Piece::getX()
{
    return x;
}

int Piece::getY()
{
    return y;
}

char Piece::getCell(int i, int j)
{
    return shape[i][j];
}

void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}