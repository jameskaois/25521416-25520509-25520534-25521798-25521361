#include <iostream>
#include <vector>
#include <ctime>
#include <conio.h>
#include <windows.h>
#include <mmsystem.h>
#include <string>
#include <fstream> 
#include "Piece.h"

using namespace std;

#pragma comment(lib, "winmm.lib")

#define H 20
#define W 20

enum GameMode {
    EASY_MODE = 1,
    NORMAL_MODE = 2,
    HARD_MODE = 3
};

GameMode currentMode = NORMAL_MODE;

const int EASY_GAME_SPEED = 1000;
const int NORMAL_GAME_SPEED = 500;
const int HARD_GAME_SPEED = 300;

const int EASY_MIN_SPEED = 120;
const int NORMAL_MIN_SPEED = 70;
const int HARD_MIN_SPEED = 50;

const int HARD_GARBAGE_CHANCE = 25;
const int HARD_GARBAGE_HOLES = 2;

int gameSpeed = NORMAL_GAME_SPEED; 
char board[H][W] = {};
int boardColor[H][W] = {};
void setColor(int color);
int score = 0;
int bestScore = 0;
int lines = 0;
int level = 1;

Piece* currentPiece = nullptr;
Piece* nextPiece = nullptr;
Piece* holdPiece = nullptr;
bool canHold = true;
clock_t lastDropTime;

const DWORD SOUND_FLAGS = SND_FILENAME | SND_ASYNC | SND_NODEFAULT;

void playRotateSound() {
    PlaySound(TEXT("sounds\\rotate.wav"), NULL, SOUND_FLAGS);
}

void playLandSound() {
    PlaySound(TEXT("sounds\\land.wav"), NULL, SOUND_FLAGS);
}

void playClearLineSound() {
    PlaySound(TEXT("sounds\\clear_line.wav"), NULL, SOUND_FLAGS);
}

void rotateCurrentPieceWithSound() {
    if (currentPiece == nullptr) return;
    currentPiece->rotate(board);
    playRotateSound();
}

void loadBestScore() {
    std::ifstream file("bestscore.txt");
    if (file.is_open()) {
        file >> bestScore;
        file.close();
    }
}

void saveBestScore() {
    std::ofstream file("bestscore.txt");
    if (file.is_open()) {
        file << bestScore;
        file.close();
    }
}

void calculateScore(int linesCleared) {
    if (linesCleared == 0) return;
    if (linesCleared == 1) score += 100 * level;
    else if (linesCleared == 2) score += 300 * level;
    else if (linesCleared == 3) score += 500 * level;
    else if (linesCleared >= 4) score += 800 * level;

    lines += linesCleared;
    level = (lines / 10) + 1;

    if (score > bestScore) {
        bestScore = score;
        saveBestScore();
    }
}

void gotoxy(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void hideCursor() {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &info);
}

int getMinimumSpeed() {
    switch (currentMode) {
        case EASY_MODE: return EASY_MIN_SPEED;
        case NORMAL_MODE: return NORMAL_MIN_SPEED;
        case HARD_MODE: return HARD_MIN_SPEED;
        default: return NORMAL_MIN_SPEED;
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
    }
}

void initBoard() {
    score = 0;
    lines = 0;
    level = 1;
    loadBestScore();

    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            if (i == 0 || i == H - 1 || j == 0 || j == W - 1) {
                board[i][j] = '#';
            } else {
                board[i][j] = ' ';
            }
        }
    }
}

void drawFrame(int x, int y, int width, int height, string title) {
    gotoxy(x, y); cout << "╭";
    for (int i = 0; i < width - 2; i++) cout << "─";
    cout << "╮";
    
    if (!title.empty()) {
        gotoxy(x + 2, y); cout << " " << title << " ";
    }

    for (int i = 1; i < height - 1; i++) {
        gotoxy(x, y + i); cout << "│";
        gotoxy(x + width - 1, y + i); cout << "│";
    }

    gotoxy(x, y + height - 1); cout << "╰";
    for (int i = 0; i < width - 2; i++) cout << "─";
    cout << "╯";
}

string getModeName() {
    if (currentMode == EASY_MODE) return "Easy";
    if (currentMode == NORMAL_MODE) return "Normal";
    if (currentMode == HARD_MODE) return "Hard";
    return "Unknown";
}

void drawPieceUI(Piece* p, int startX, int startY) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            gotoxy(startX + j * 2, startY + i);
            if (p != nullptr && p->getCell(i, j) != ' ') {
                setColor(boardColor[i][j]);
                cout << "██"; 
            } else {
                cout << "  "; 
            }
        }
    }
}

void drawGameInfo() {
    int infoX = W * 2 + 4; 
    int infoY = 0;
    
    drawFrame(infoX, infoY, 15, 6, "HOLD (C)");
    drawFrame(infoX + 17, infoY, 15, 6, "NEXT");

    drawPieceUI(holdPiece, infoX + 4, infoY + 1);
    drawPieceUI(nextPiece, infoX + 21, infoY + 1);

    drawFrame(infoX, infoY + 7, 32, 8, "STATS");
    gotoxy(infoX + 2, infoY + 9);  cout << "Score : " << score << "        ";
    gotoxy(infoX + 2, infoY + 10); cout << "Best  : " << bestScore << "        ";
    gotoxy(infoX + 2, infoY + 11); cout << "Lines : " << lines << "        ";
    gotoxy(infoX + 2, infoY + 12); cout << "Level : " << level << "        ";
    gotoxy(infoX + 2, infoY + 13); cout << "Mode  : " << getModeName() << "        "; 

    drawFrame(infoX, infoY + 16, 36, 10, "CONTROLS");
    gotoxy(infoX + 2, infoY + 18); cout << "A/D | <-/-> : Move";
    gotoxy(infoX + 2, infoY + 19); cout << "W   | ^     : Rotate";
    gotoxy(infoX + 2, infoY + 20); cout << "S   | v     : Soft Drop";
    gotoxy(infoX + 2, infoY + 21); cout << "Space       : Hard Drop";
    gotoxy(infoX + 2, infoY + 22); cout << "C           : Hold Block";
    gotoxy(infoX + 2, infoY + 23); cout << "P: Pause | Q: Quit";
}

int drawMenu(const vector<string>& options, const string& title) {
    int selected = 0;
    while (true) {
        system("cls");
        cout << "\n=========================================\n";
        cout << "          " << title << "\n";
        cout << "=========================================\n\n";
        
        for (int i = 0; i < options.size(); i++) {
            if (i == selected) {
                cout << "      >> " << options[i] << " <<\n"; 
            } else {
                cout << "         " << options[i] << "\n";
            }
        }
        
        cout << "\n=========================================\n";
        cout << " Dung mui ten ^/v hoac W/S de di chuyen.\n Enter de chon.";
        
        char c = _getch();
        if (c == -32 || c == 224) {
            c = _getch();
            if (c == 72) selected = (selected - 1 + options.size()) % options.size(); 
            if (c == 80) selected = (selected + 1) % options.size(); 
        } else if (c == 'w' || c == 'W') {
            selected = (selected - 1 + options.size()) % options.size();
        } else if (c == 's' || c == 'S') {
            selected = (selected + 1) % options.size();
        } else if (c == 13) {
            return selected;
        }
    }
}

void showMainMenu() {
    int selected = 0;
    vector<string> options = {"1. Bat dau choi", "2. Cai dat che do", "3. Xem Ky Luc", "4. Thoat"};
    
    while (true) {
        system("cls");
        cout << "\n";
        cout << "   _______ ______ _______ _____  _____  _____  \n";
        cout << "  |__   __|  ____|__   __|  __ \\|_   _|/ ____| \n";
        cout << "     | |  | |__     | |  | |__) | | | | (___   \n";
        cout << "     | |  |  __|    | |  |  _  /  | |  \\___ \\  \n";
        cout << "     | |  | |____   | |  | | \\ \\ _| |_ ____) | \n";
        cout << "     |_|  |______|  |_|  |_|  \\_\\_____|_____/  \n";
        cout << "\n===============================================\n\n";
        
        for (int i = 0; i < options.size(); i++) {
            if (i == selected) {
                cout << "             >> " << options[i] << " <<\n"; 
            } else {
                cout << "                " << options[i] << "\n";
            }
        }
        
        cout << "\n===============================================\n";
        cout << "   Dung mui ten ^/v hoac W/S de chon. \n   Enter de xac nhan.\n";
        
        char c = _getch();
        if (c == -32 || c == 224) {
            c = _getch();
            if (c == 72) selected = (selected - 1 + options.size()) % options.size(); 
            if (c == 80) selected = (selected + 1) % options.size(); 
        } else if (c == 'w' || c == 'W') {
            selected = (selected - 1 + options.size()) % options.size();
        } else if (c == 's' || c == 'S') {
            selected = (selected + 1) % options.size();
        } else if (c == 13) { // Phím Enter
            if (selected == 0) {
                break;
            } else if (selected == 1) {
                vector<string> modeOpts = {"Easy (Cham)", "Normal (Mac dinh)", "Hard (Nhanh + Co rac)", "Quay lai"};
                int modeChoice = drawMenu(modeOpts, "CHON DO KHO");
                
                if (modeChoice == 0) currentMode = EASY_MODE;
                else if (modeChoice == 1) currentMode = NORMAL_MODE;
                else if (modeChoice == 2) currentMode = HARD_MODE;
                
                applyGameMode();
            } else if (selected == 2) {
                system("cls");
                loadBestScore();
                cout << "\n=========================================\n";
                cout << "         BANG XEP HANG (KY LUC)          \n";
                cout << "=========================================\n\n";
                cout << "      Diem cao nhat hien tai: " << bestScore << "\n\n";
                cout << "=========================================\n";
                cout << "       Nhan phim bat ky de quay lai...   \n";
                _getch();
            } else if (selected == 3) {
                exit(0);
            }
        }
    }
    system("cls");
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

    for (int i = 1; i < H - 2; i++) {
        for (int j = 1; j < W - 1; j++) {
            board[i][j] = board[i + 1][j];
            boardColor[i][j] = boardColor[i + 1][j];
        }
    }

    for (int j = 1; j < W - 1; j++) {
        board[H - 2][j] = hole[j] ? ' ' : 'G';
        boardColor[H - 2][j] = hole[j] ? 7 : 8;
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
    string frame = "";
    for (int i = 0; i < H; i++){
        for (int j = 0; j < W; j++){
            if (board[i][j] == '#') {
                if (i == 0 && j == 0) frame += "╭─";
                else if (i == 0 && j == W - 1) frame += "╮ ";
                else if (i == H - 1 && j == 0) frame += "╰─";
                else if (i == H - 1 && j == W - 1) frame += "╯ ";
                else if (i == 0 || i == H - 1) frame += "──";
                else frame += "│ ";
            }     
            else if (board[i][j] == ' ') frame += "  ";      
            else frame += "██";
        }
        frame += "\n";
    }
    cout << frame;
    drawGameInfo();
}

int removeLine() {
    vector<int> fullLines;
    
    for (int i = H - 2; i >= 1; i--) {
        bool full = true; 
        for (int j = 1; j < W - 1; j++) {
            if (board[i][j] == ' ' || board[i][j] == 'G') {
                full = false;
                break;
            }
        }
        if (full) {
            fullLines.push_back(i);
        }
    }

    if (fullLines.empty()) return 0; 

    playClearLineSound();

    for (int blink = 0; blink < 3; blink++) {
        for (int i : fullLines) {
            for (int j = 1; j < W - 1; j++) board[i][j] = ' ';
        }
        draw();
        Sleep(10);

        for (int i : fullLines) {
            for (int j = 1; j < W - 1; j++) board[i][j] = 'O'; 
        }
        draw();
        Sleep(10);
    }

    int writeRow = H - 2;
    for (int readRow = H - 2; readRow >= 1; readRow--) {
        bool isFull = false;
        for (int r : fullLines) {
            if (r == readRow) {
                isFull = true;
                break;
            }
        }

        if (!isFull) {
            for (int j = 1; j < W - 1; j++) {
                board[writeRow][j] = board[readRow][j];
                boardColor[writeRow][j] = boardColor[readRow][j];
            }
            writeRow--;
        }
    }

    while (writeRow >= 1) {
        for (int j = 1; j < W - 1; j++) {
            board[writeRow][j] = ' ';
            boardColor[writeRow][j] = 7;
        }
        writeRow--;
    }

    draw();
    return fullLines.size(); 
}

Piece* createRandomPiece() {
    int b = rand() % 7;
    switch(b) {
        case 0: return new PieceO();
        case 1: return new PieceI();
        case 2: return new PieceT();
        case 3: return new PieceL();
        case 4: return new PieceJ();
        case 5: return new PieceS();
        case 6: return new PieceZ();
    }
    return nullptr;
}

void spawnNewBlock(bool allowGarbage = true) {
    if (allowGarbage) {
        maybeAddHardModeGarbageLine();
    }

    Piece* oldPiece = currentPiece;
    currentPiece = nextPiece;
    nextPiece = createRandomPiece();

    if (currentPiece != nullptr) currentPiece->initShape();
    if (nextPiece != nullptr) nextPiece->initShape();
    canHold = true;

    if (oldPiece != nullptr) delete oldPiece;
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    hideCursor();
    srand(time(0));
    
    showMainMenu(); 
    initBoard();

    currentPiece = createRandomPiece();
    currentPiece->initShape();
    nextPiece = createRandomPiece();
    nextPiece->initShape();
    holdPiece = nullptr;
    
    spawnNewBlock(false); 
    
    system("cls");
    lastDropTime = clock();
    
    while (1){
        currentPiece->boardDelBlock(board,boardColor);
        
        while (_kbhit()){
            char c = _getch();
            
            if (c == -32 || c == 224) {
                c = _getch();
                if (c == 75 && currentPiece->canMove(-1, 0, board)) currentPiece->moveLeft();  
                if (c == 77 && currentPiece->canMove(1, 0, board)) currentPiece->moveRight(); 
                if (c == 72) rotateCurrentPieceWithSound();                                   
                if (c == 80 && currentPiece->canMove(0, 1, board)) currentPiece->moveDown();  
            } else {
                if ((c=='a' || c=='A') && currentPiece->canMove(-1, 0, board)) currentPiece->moveLeft();
                if ((c=='d' || c=='D') && currentPiece->canMove(1, 0, board)) currentPiece->moveRight();
                if (c=='w' || c=='W') rotateCurrentPieceWithSound();            
                
                if ((c=='s' || c=='S' || c=='x' || c=='X') && currentPiece->canMove(0, 1, board)) currentPiece->moveDown();    
                
                if (c == 'c' || c == 'C') {
                    if (canHold) {
                        if (holdPiece == nullptr) {
                            holdPiece = currentPiece;
                            holdPiece->resetPos();  
                            
                            currentPiece = nullptr; 
                            
                            spawnNewBlock(false);
                        } else {
                            Piece* temp = currentPiece;
                            currentPiece = holdPiece;
                            holdPiece = temp;
                            currentPiece->resetPos(); 
                        }
                        canHold = false;
                    }
                }
                

                if (c == ' ') {
                    while (currentPiece->canMove(0, 1, board)) {
                        currentPiece->moveDown();
                    }
                    lastDropTime = clock() - gameSpeed; 
                }

                if (c == 'p' || c == 'P') {
                    gotoxy(W * 2 + 6, 15);
                    cout << "Tạm dừng     "; 
                    while (true) {
                        if (_kbhit()) {
                            char resumeKey = _getch();
                            if (resumeKey == -32 || resumeKey == 224) _getch(); 
                            break;
                        }
                        Sleep(100); 
                    }
                }

                if (c == 'q' || c == 'Q') {
                    exit(0);
                }
            }
        }
        currentPiece->block2Board(board,boardColor);
        draw();

        if (clock() - lastDropTime > gameSpeed) {
            currentPiece->boardDelBlock(board,boardColor);
            
            if (currentPiece->canMove(0, 1, board)) {
                currentPiece->moveDown();
            } else {
                currentPiece->block2Board(board,boardColor);
                playLandSound();
                
                int clearedLines = removeLine();
                calculateScore(clearedLines);
                setColor(currentPiece->getColor());
                spawnNewBlock();

                if (!currentPiece->canMove(0, 0, board)) {
                    currentPiece->block2Board(board,boardColor); 
                    draw();        
                    gotoxy(W * 2 + 6, 16);
                    cout << "GAME OVER!";
                    Sleep(2000);
                    break;
                }
            }
            currentPiece->block2Board(board,boardColor);
            lastDropTime = clock();
        }

        Sleep(30); 
    }

    return 0;
}