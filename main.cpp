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

#define H 27
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
                boardColor[i][j] = 7;
            } else {
                board[i][j] = ' ';
                boardColor[i][j] = 7;
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
                setColor(p->getColor());
                cout << "██"; 
            } else {
                setColor(7);
                cout << "  "; 
            }
        }
    }
    setColor(7);
}

void drawGameInfo() {
    int infoX = W * 2 + 4; 
    int infoY = 0;
    
    setColor(7);
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
        boardColor[H - 2][j] = hole[j] ? 7 : 8; // Gạch rác màu xám đen
    }
}

void maybeAddHardModeGarbageLine() {
    if (currentMode != HARD_MODE) return;

    int randomPercent = rand() % 100;
    if (randomPercent < HARD_GARBAGE_CHANCE) {
        addHardModeGarbageLine();
    }
}

Piece* createGhost(Piece* current)
{
    if (!current) return nullptr;
    Piece* ghost = nullptr;

    if (dynamic_cast<PieceO*>(current)) ghost = new PieceO();
    else if (dynamic_cast<PieceI*>(current)) ghost = new PieceI();
    else if (dynamic_cast<PieceT*>(current)) ghost = new PieceT();
    else if (dynamic_cast<PieceL*>(current)) ghost = new PieceL();
    else if (dynamic_cast<PieceJ*>(current)) ghost = new PieceJ();
    else if (dynamic_cast<PieceS*>(current)) ghost = new PieceS();
    else if (dynamic_cast<PieceZ*>(current)) ghost = new PieceZ();

    if (!ghost) return nullptr;

    ghost->initShape();
    
    char s[4][4];
    current->getShape(s);
    ghost->setShape(s);
    ghost->setX(current->getX());
    ghost->setY(current->getY());

    return ghost;
}

void draw() {
    gotoxy(0,0);
    string buffer = "";
    int lastColor = -1;

    char rChar[H][W];
    int rColor[H][W];

    // Chép Board gốc vào bộ đệm tạm thời
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            rChar[i][j] = board[i][j];
            rColor[i][j] = boardColor[i][j];
        }
    }

    // Phủ Ghost lên bộ đệm
    Piece* ghost = nullptr;
    if (currentMode != HARD_MODE && currentPiece != nullptr) {
        ghost = createGhost(currentPiece);
        if (ghost) {
            while (ghost->canMove(0, 1, board)) ghost->moveDown();
            for(int i=0; i<4; i++){
                for(int j=0; j<4; j++){
                    if(ghost->getCell(i,j) != ' ') {
                        int gy = ghost->getY() + i;
                        int gx = ghost->getX() + j;
                        // Chỉ vẽ bóng nếu chỗ đó chưa có gạch
                        if(gy>=0 && gy<H && gx>=0 && gx<W && rChar[gy][gx] == ' ') {
                            rChar[gy][gx] = 'g'; // Ký tự g dùng cho ghost
                            rColor[gy][gx] = 8;  // Màu xám
                        }
                    }
                }
            }
            delete ghost;
        }
    }

    // Phủ khối gạch đang rơi lên bộ đệm
    if (currentPiece != nullptr) {
        for(int i=0; i<4; i++){
            for(int j=0; j<4; j++){
                if(currentPiece->getCell(i,j) != ' ') {
                    int cy = currentPiece->getY() + i;
                    int cx = currentPiece->getX() + j;
                    if(cy>=0 && cy<H && cx>=0 && cx<W) {
                        rChar[cy][cx] = 'C';
                        rColor[cy][cx] = currentPiece->getColor();
                    }
                }
            }
        }
    }

    // In 1 lần ra màn hình với thuật toán gom nhóm màu để tăng FPS
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            int cColor = rColor[i][j];
            char cChar = rChar[i][j];
            string s = "";

            if (cChar == '#') {
                cColor = 7; 
                if (i == 0 && j == 0) s = "╭─";
                else if (i == 0 && j == W - 1) s = "╮ ";
                else if (i == H - 1 && j == 0) s = "╰─";
                else if (i == H - 1 && j == W - 1) s = "╯ ";
                else if (i == 0 || i == H - 1) s = "──";
                else s = "│ ";
            } else if (cChar == ' ') {
                cColor = 7;
                s = "  ";
            } else if (cChar == 'g') {
                s = "[]"; // Ghost
            } else if (cChar == 'O') {
                cColor = 12; // Chớp đỏ khi ăn điểm
                s = "██";
            } else {
                s = "██"; // Block bình thường hoặc Block rác (G)
            }

            if (cColor != lastColor) {
                if (!buffer.empty()) {
                    setColor(lastColor);
                    cout << buffer;
                    buffer = "";
                }
                lastColor = cColor;
            }
            buffer += s;
        }
        buffer += "\n";
    }
    
    if (!buffer.empty()) {
        setColor(lastColor);
        cout << buffer;
    }

    setColor(7);
    drawGameInfo();
}

int removeLine() {
    vector<int> fullLines;
    
    for (int i = H - 2; i >= 1; i--) {
        bool full = true; 
        for (int j = 1; j < W - 1; j++) {
            // Block rác 'G' giờ được tính như một block bình thường
            if (board[i][j] == ' ') {
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
            for (int j = 1; j < W - 1; j++) {
                board[i][j] = ' '; 
            }
        }
        draw();
        Sleep(30);

        for (int i : fullLines) {
            for (int j = 1; j < W - 1; j++) {
                board[i][j] = 'O'; 
            }
        }
        draw();
        Sleep(30);
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
    system("mode con cols=125 lines=40"); 
    hideCursor();
    srand(time(0));
    
    bool backToMenu = true;
    
    while (true) { 
        if (backToMenu) {
            showMainMenu();
            backToMenu = false;
        }
        
        initBoard();

        if (currentPiece) { delete currentPiece; currentPiece = nullptr; }
        if (nextPiece) { delete nextPiece; nextPiece = nullptr; }
        if (holdPiece) { delete holdPiece; holdPiece = nullptr; }

        currentPiece = createRandomPiece();
        currentPiece->initShape();
        nextPiece = createRandomPiece();
        nextPiece->initShape();
        holdPiece = nullptr;
        canHold = true;
        
        spawnNewBlock(false); 
        
        system("cls");
        lastDropTime = clock();
        
        while (true) { 
            
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
                        setColor(7);
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
                        gotoxy(W * 2 + 6, 15);
                        cout << "             "; 
                    }

                    if (c == 'q' || c == 'Q') {
                        exit(0);
                    }
                }
            }

            if (clock() - lastDropTime > gameSpeed) {
                if (currentPiece->canMove(0, 1, board)) {
                    currentPiece->moveDown();
                } else {
                    currentPiece->block2Board(board, boardColor);
                    playLandSound();
                    
                    Piece* temp = currentPiece;
                    currentPiece = nullptr; 
                    
                    int clearedLines = removeLine();
                    
                    currentPiece = temp; 
                    
                    calculateScore(clearedLines);
                    spawnNewBlock();

                    // ======= GAME OVER =======
                    if (!currentPiece->canMove(0, 0, board)) {
                        currentPiece->block2Board(board, boardColor); 
                        draw();        
                        
                        // Đã tăng width của bảng lên 40 và dịch sang phải một chút
                        int boxX = W * 2 + 40; 
                        int boxY = 7;          
                        
                        setColor(7); 
                        drawFrame(boxX, boxY, 40, 11, "");
                        gotoxy(boxX + 15, boxY + 1); cout << "GAME OVER!";
                        gotoxy(boxX + 4, boxY + 3);  cout << "Ky luc      : " << bestScore << "      ";
                        gotoxy(boxX + 4, boxY + 4);  cout << "Diem cua ban: " << score << "      ";
                        gotoxy(boxX + 4, boxY + 5);  cout << "So hang xoa : " << lines << "      ";
                        gotoxy(boxX, boxY + 7);      cout << "├──────────────────────────────────────┤";
                        gotoxy(boxX + 3, boxY + 8);  cout << "[R] Choi lai | [M] Menu | [Q] Thoat";
                        
                        while (true) {
                            char ch = _getch();
                            if (ch == 'r' || ch == 'R') {
                                break; 
                            } else if (ch == 'm' || ch == 'M') {
                                backToMenu = true;
                                break; 
                            } else if (ch == 'q' || ch == 'Q') {
                                exit(0); 
                            }
                        }
                        break; 
                    }
                }
                lastDropTime = clock();
            }

            draw();
            Sleep(10); 
        }
    }

    return 0;
}