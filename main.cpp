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

enum GameMode {
    EASY_MODE = 1,
    NORMAL_MODE = 2,
    HARD_MODE = 3
};

GameMode currentMode = NORMAL_MODE;

const int EASY_GAME_SPEED = 1000;
const int NORMAL_GAME_SPEED = 400;
const int HARD_GAME_SPEED = 200;

const int EASY_MIN_SPEED = 120;
const int NORMAL_MIN_SPEED = 70;
const int HARD_MIN_SPEED = 50;

const int EASY_SPEED_STEP = 5;
const int NORMAL_SPEED_STEP = 3;
const int HARD_SPEED_STEP = 2;

const int HARD_GARBAGE_CHANCE = 25;
const int HARD_GARBAGE_HOLES = 2;

int gameSpeed = NORMAL_GAME_SPEED; 
char board[H][W] = {};

Piece* currentPiece = nullptr;

const DWORD SOUND_FLAGS = SND_ALIAS | SND_ASYNC | SND_NODEFAULT;

void playRotateSound() {
    PlaySound(TEXT("SystemAsterisk"), NULL, SOUND_FLAGS);
}

void playLandSound() {
    PlaySound(TEXT("SystemHand"), NULL, SOUND_FLAGS);
}

void playClearLineSound() {
    PlaySound(TEXT("SystemExclamation"), NULL, SOUND_FLAGS);
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

const char* getModeName() {
    switch (currentMode) {
        case EASY_MODE: return "Easy";
        case NORMAL_MODE: return "Normal";
        case HARD_MODE: return "Hard";
        default: return "Normal";
    }
}

void applyGameMode() {
    switch (currentMode) {
        case EASY_MODE:
            gameSpeed = EASY_GAME_SPEED;
            break;
        case NORMAL_MODE:
            gameSpeed = NORMAL_GAME_SPEED;
            break;
        case HARD_MODE:
            gameSpeed = HARD_GAME_SPEED;
            break;
        default:
            currentMode = NORMAL_MODE;
            gameSpeed = NORMAL_GAME_SPEED;
            break;
    }
}


int getMinimumSpeed() {
    switch (currentMode) {
        case EASY_MODE: return EASY_MIN_SPEED;
        case NORMAL_MODE: return NORMAL_MIN_SPEED;
        case HARD_MODE: return HARD_MIN_SPEED;
        default: return NORMAL_MIN_SPEED;
    }
}

int getSpeedStepPerClearedLine() {
    switch (currentMode) {
        case EASY_MODE: return EASY_SPEED_STEP;
        case NORMAL_MODE: return NORMAL_SPEED_STEP;
        case HARD_MODE: return HARD_SPEED_STEP;
        default: return NORMAL_SPEED_STEP;
    }
}

void updateGameSpeedAfterClear(int clearedLines) {
    if (clearedLines <= 0) return;

    gameSpeed -= clearedLines * getSpeedStepPerClearedLine();

    int minimumSpeed = getMinimumSpeed();
    if (gameSpeed < minimumSpeed) {
        gameSpeed = minimumSpeed;
    }
}

void drawGameInfo() {
    gotoxy(W * 2 + 5, 1);
    cout << "Mode : " << getModeName() << "        ";

    gotoxy(W * 2 + 5, 2);
    cout << "Speed: " << gameSpeed << " ms       ";

    gotoxy(W * 2 + 5, 4);
    cout << "Controls:";

    gotoxy(W * 2 + 5, 5);
    cout << "A/D or <-/->: Move";

    gotoxy(W * 2 + 5, 6);
    cout << "W or ^      : Rotate";

    gotoxy(W * 2 + 5, 7);
    cout << "S/X or v    : Soft Drop";

    gotoxy(W * 2 + 5, 8);
    cout << "Space       : Hard Drop";

    gotoxy(W * 2 + 5, 9);
    cout << "P: Pause | Q: Quit";

    gotoxy(W * 2 + 5, 11);
    if (currentMode == HARD_MODE) {
        cout << "Hard: " << HARD_GARBAGE_CHANCE << "% garbage/spawn";
    } else {
        cout << "                         ";
    }
}

void showModeMenu() {
    system("cls");
    cout << "==============================\n";
    cout << "          TETRIS GAME         \n";
    cout << "==============================\n";
    cout << "1. Easy Mode   - Toc do cham\n";
    cout << "2. Normal Mode - Toc do mac dinh\n";
    cout << "3. Hard Mode   - Toc do nhanh + co the xuat hien gach rac\n";
    cout << "==============================\n";
    cout << "Chon che do choi (1-3): ";

    while (true) {
        char choice = _getch();
        if (choice == '1') {
            currentMode = EASY_MODE;
            break;
        }
        if (choice == '2') {
            currentMode = NORMAL_MODE;
            break;
        }
        if (choice == '3') {
            currentMode = HARD_MODE;
            break;
        }
    }

    applyGameMode();
    system("cls");
}


void initBoard() {
    for (int i = 0 ; i < H ; i++)
        for (int j = 0 ; j < W ; j++)
            if (i == 0 || i == H-1 || j == 0 || j == W-1)
                board[i][j] = '#';
            else
                board[i][j] = ' ';
}

void addHardModeGarbageLine() {
    bool hole[W] = {};

    for (int count = 0; count < HARD_GARBAGE_HOLES; count++) {
        int randomHole;
        do {
            randomHole = 1 + rand() % (W - 2);
        } while (hole[randomHole]);

        hole[randomHole] = true;
    }

    // Đẩy toàn bộ gạch hiện có lên 1 hàng, chỉ dịch phần bên trong tường.
    for (int i = 1; i < H - 2; i++) {
        for (int j = 1; j < W - 1; j++) {
            board[i][j] = board[i + 1][j];
        }
    }

    // Tạo hàng gạch rác ở đáy, có lỗ để người chơi vẫn có thể xử lý.
    for (int j = 1; j < W - 1; j++) {
        board[H - 2][j] = hole[j] ? ' ' : 'G';
    }
}

void maybeAddHardModeGarbageLine() {
    if (currentMode != HARD_MODE) return;

    int randomPercent = rand() % 100;
    if (randomPercent < HARD_GARBAGE_CHANCE) {
        addHardModeGarbageLine();
    }
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
    drawGameInfo();
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

    // Phát nhạc ngắn ngay trong removeLine() khi xử lý ăn hàng thành công
    playClearLineSound();

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

void spawnNewBlock(bool allowGarbage = true) {
    if (allowGarbage) {
        maybeAddHardModeGarbageLine();
    }

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
    
    showModeMenu();
    initBoard();
    spawnNewBlock(false);
    
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
                updateGameSpeedAfterClear(clearedLines);
                
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