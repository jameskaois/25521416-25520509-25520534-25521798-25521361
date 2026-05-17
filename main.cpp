#include <iostream>
#include <conio.h>
#include <windows.h>
#include <mmsystem.h>
#include <time.h>
#include <vector>
#include "Piece.h"

#pragma comment(lib, "winmm.lib")

using namespace std;

#define H 20
#define W 20

int gameSpeed = 400; 
char board[H][W] = {};

Piece* currentPiece = nullptr;


// ================== SOUND EFFECTS - PHẦN ĐƯỢC GIAO ==================
// Dùng âm thanh hệ thống của Windows để không cần thêm file .wav ngoài.
// Nếu nhóm có file âm thanh riêng, có thể đổi SND_ALIAS thành SND_FILENAME
// và thay tên dưới đây bằng đường dẫn file, ví dụ: TEXT("sounds\\rotate.wav").
void playRotateSound() {
    PlaySound(TEXT("SystemAsterisk"), NULL, SND_ALIAS | SND_ASYNC | SND_NODEFAULT);
}

void playLandSound() {
    PlaySound(TEXT("SystemHand"), NULL, SND_ALIAS | SND_ASYNC | SND_NODEFAULT);
}

void playClearLineSound() {
    PlaySound(TEXT("SystemExclamation"), NULL, SND_ALIAS | SND_ASYNC | SND_NODEFAULT);
}

void rotateCurrentPieceWithSound() {
    if (currentPiece == nullptr) return;

    char oldShape[4][4];
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            oldShape[i][j] = currentPiece->getCell(i, j);
        }
    }

    currentPiece->rotate(board);

    bool rotated = false;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (oldShape[i][j] != currentPiece->getCell(i, j)) {
                rotated = true;
                break;
            }
        }
        if (rotated) break;
    }

    if (rotated) playRotateSound();
}
// =====================================================================

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

// Logic mới cho tính năng ăn điểm
int removeLine() {
    vector<int> fullLines;
    
    // Bước 1: Tìm tất cả các hàng đã được lấp đầy
    for (int i = H - 2; i >= 1; i--) {
        bool full = true; 
        for (int j = 1; j < W - 1; j++) {
            if (board[i][j] == ' ') {
                full = false;
                break;
            }
        }
        if (full) {
            fullLines.push_back(i);
        }
    }

    // Nếu không có hàng nào đầy thì thoát luôn
    if (fullLines.empty()) return 0; 

    // Bước 2: Hiệu ứng nhấp nháy 3 lần cho các hàng ăn điểm
    for (int blink = 0; blink < 3; blink++) {
        // Tắt gạch (thành khoảng trắng)
        for (int i : fullLines) {
            for (int j = 1; j < W - 1; j++) board[i][j] = ' ';
        }
        draw();
        Sleep(10);

        // Bật gạch (dùng ký tự 'O' để hàm draw() in ra ██)
        for (int i : fullLines) {
            for (int j = 1; j < W - 1; j++) board[i][j] = 'O'; 
        }
        draw();
        Sleep(10);
    }

    // Bước 3: Dọn dẹp hàng và kéo phần gạch phía trên rớt xuống
    int writeRow = H - 2;
    for (int readRow = H - 2; readRow >= 1; readRow--) {
        bool isFull = false;
        for (int r : fullLines) {
            if (r == readRow) {
                isFull = true;
                break;
            }
        }

        // Nếu hàng đang đọc không phải hàng bị xóa, thì chép nó xuống dưới
        if (!isFull) {
            for (int j = 1; j < W - 1; j++) {
                board[writeRow][j] = board[readRow][j];
            }
            writeRow--;
        }
    }

    // Làm trống các hàng trên cùng còn dư lại
    while (writeRow >= 1) {
        for (int j = 1; j < W - 1; j++) {
            board[writeRow][j] = ' ';
        }
        writeRow--;
    }

    draw();
    return fullLines.size(); 
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
                if (c == 72) rotateCurrentPieceWithSound();                                     
                if (c == 80 && currentPiece->canMove(0, 1, board)) currentPiece->moveDown();  // Soft Drop
            } else {
                if ((c=='a' || c=='A') && currentPiece->canMove(-1, 0, board)) currentPiece->moveLeft();
                if ((c=='d' || c=='D') && currentPiece->canMove(1, 0, board)) currentPiece->moveRight();
                if (c=='w' || c=='W') rotateCurrentPieceWithSound();           
                
                // Soft Drop
                if ((c=='s' || c=='S' || c=='x' || c=='X') && currentPiece->canMove(0, 1, board)) currentPiece->moveDown();    
                
                // Hard Drop
                if (c == ' ') {
                    while (currentPiece->canMove(0, 1, board)) {
                        currentPiece->moveDown();
                    }
                    timer = gameSpeed; 
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
                playLandSound();
                currentPiece->block2Board(board);
                
                int clearedLines = removeLine(); 
                if (clearedLines > 0) {
                    playClearLineSound();
                    gameSpeed -= (clearedLines * 2); // Chỉnh sửa tốc độ tăng vừa phải
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