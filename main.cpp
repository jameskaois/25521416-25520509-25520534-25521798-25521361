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
    
    currentPiece->initShape();
}

int main()
{
    SetConsoleOutputCP(CP_UTF8);

    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.dwSize = 100; cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);

    srand(time(0));
    
    initBoard();
    spawnNewBlock();
    
    system("cls");
    int timer = 0;
    
    while (1){
        currentPiece->boardDelBlock(board);
        
        while (_kbhit()){
            char c = _getch();
            
            // Xử lý phím mũi tên
            if (c == -32 || c == 224) {
                c = _getch();
                if (c == 75 && currentPiece->canMove(-1, 0, board)) currentPiece->moveLeft();  
                if (c == 77 && currentPiece->canMove(1, 0, board)) currentPiece->moveRight(); 
                if (c == 72) currentPiece->rotate(board);                                     
                if (c == 80 && currentPiece->canMove(0, 1, board)) currentPiece->moveDown();  // Soft Drop
            } else {
                if ((c=='a' || c=='A') && currentPiece->canMove(-1, 0, board)) currentPiece->moveLeft();
                if ((c=='d' || c=='D') && currentPiece->canMove(1, 0, board)) currentPiece->moveRight();
                if (c=='w' || c=='W') currentPiece->rotate(board);           
                
                // Soft Drop
                if ((c=='s' || c=='S' || c=='x' || c=='X') && currentPiece->canMove(0, 1, board)) currentPiece->moveDown();    
                
                // Hard Drop
                if (c == ' ') {
                    while (currentPiece->canMove(0, 1, board)) {
                        currentPiece->moveDown();
                    }
                    timer = gameSpeed; // Ép block khóa ngay lập tức
                }

                // Pause Game
                if (c == 'p' || c == 'P') {
                    gotoxy(W * 2 + 5, H / 2);
                    cout << "TT: Tạm dừng   "; 
                    while (true) {
                        if (_kbhit()) {
                            char resumeKey = _getch();
                            if (resumeKey == -32 || resumeKey == 224) _getch(); 
                            break;
                        }
                        Sleep(100); 
                    }
                    gotoxy(W * 2 + 5, H / 2);
                    cout << "               "; 
                }

                if (c=='q' || c=='Q') exit(0);
            }
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
        
        currentPiece->block2Board(board);
        draw();
        Sleep(10); 
    }
    gotoxy(0, H + 2); 
    return 0;
}