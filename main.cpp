#include <iostream>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include "Piece.h"

using namespace std;

#define H 20
#define W 20

int gameSpeed = 200; 
char board[H][W] = {};

// Dùng một con trỏ đa hình duy nhất để quản lý khối gạch hiện tại
Piece* currentPiece = nullptr;

void gotoxy(int x, int y) {
    COORD c = {(SHORT)x, (SHORT)y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void initBoard() {
    for (int i = 0 ; i < H ; i++)
        for (int j = 0 ; j < W ; j++)
            if (i == 0 || i == H-1 || j == 0 || j == W-1)
                board[i][j] = '#';
            else
                board[i][j] = ' ';
}

void draw() {
    gotoxy(0,0);
    for (int i = 0; i < H; i++){
        for (int j = 0; j < W; j++){
            if (board[i][j] == '#') cout << "##";     
            else if (board[i][j] == ' ') cout << "  ";      
            else cout << "██";
        }
        cout << endl;
    }
}

int removeLine() {
    int linesCleared = 0; 
    for (int i = H - 2; i >= 1; i--) {
        bool full = true; 
        for (int j = 1; j < W - 1; j++) {
            if (board[i][j] == ' ') {
                full = false;
                break;
            }
        }
        if (full) {
            linesCleared++; 
            for (int k = i; k > 1; k--) {
                for (int j = 1; j < W - 1; j++) {
                    board[k][j] = board[k - 1][j];
                }
            }
            for (int j = 1; j < W - 1; j++) {
                board[1][j] = ' ';
            }
            i++;
            draw();
            Sleep(gameSpeed / 2); 
        }
    }
    return linesCleared; 
}

void spawnNewBlock() {
    // Xóa khối cũ khỏi bộ nhớ để tránh tràn RAM
    if (currentPiece != nullptr) {
        delete currentPiece;
    }
    
    int b = rand() % 7;
    switch(b) {
        case 0: currentPiece = new PieceI(); break;
        case 1: currentPiece = new PieceO(); break;
        case 2: currentPiece = new PieceT(); break;
        case 3: currentPiece = new PieceS(); break;
        case 4: currentPiece = new PieceZ(); break;
        case 5: currentPiece = new PieceJ(); break;
        case 6: currentPiece = new PieceL(); break;
    }
    
    // Nạp hình dáng tương ứng của khối con
    currentPiece->initShape();
}

int main()
{
    SetConsoleOutputCP(CP_UTF8);

    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.dwSize = 100; cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);

    srand(time(0));
    
    // Thứ tự đã được cập nhật
    initBoard();
    spawnNewBlock();
    
    system("cls");
    int timer = 0;
    
    while (1){
        // Xóa vết cũ bằng phương thức của class
        currentPiece->boardDelBlock(board);
        
        while (_kbhit()){
            char c = _getch();
            if (c=='a' && currentPiece->canMove(-1, 0, board)) currentPiece->moveLeft();
            if (c=='d' && currentPiece->canMove(1, 0, board)) currentPiece->moveRight();
            
            // Đã truyền biến board vào để kiểm tra va chạm
            if (c=='w') currentPiece->rotate(board);           
            
            if (c=='s' && currentPiece->canMove(0, 1, board)) currentPiece->moveDown();    
            if (c=='q') exit(0);
        }
    
        timer += 10;
        
        if (timer >= gameSpeed) {
            if (currentPiece->canMove(0, 1, board)) {
                currentPiece->moveDown();
            } else {
                currentPiece->block2Board(board);
                
                int clearedLines = removeLine(); 
                if (clearedLines > 0) {
                    gameSpeed -= (clearedLines * 15);
                    if (gameSpeed < 50) gameSpeed = 50; 
                }
                
                spawnNewBlock();

                if (!currentPiece->canMove(0, 0, board)) {
                    currentPiece->block2Board(board); 
                    draw();        
                    gotoxy(W * 2 + 5, H / 2);
                    cout << "GAME OVER!";
                    Sleep(2000);
                    break;
                }
            }
            timer = 0;
        }
        
        // Vẽ khối mới vào bảng
        currentPiece->block2Board(board);
        draw();

        Sleep(10); 
    }
    gotoxy(0, H + 2); 
    return 0;
}