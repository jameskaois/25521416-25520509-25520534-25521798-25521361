#include <iostream>
#include <vector>
#include <ctime>
#include <conio.h>
#include <windows.h>
#include "Piece.h"
#include <mmsystem.h>
#include <string>
#include <fstream> 

using namespace std;

#pragma comment(lib, "winmm.lib")

char board[H][W];
Piece* currentPiece;
Piece* nextPiece = nullptr;
Piece* holdPiece = nullptr;
bool canHold = true;
clock_t lastDropTime;
int gameSpeed = 500; 

int score = 0;
int bestScore = 0;
int lines = 0;
int level = 1;

enum GameMode { EASY_MODE, NORMAL_MODE, HARD_MODE };
GameMode currentMode = NORMAL_MODE; 
const int HARD_GARBAGE_CHANCE = 20;

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

void gotoxy(int x, int y)
{
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void hideCursor()
{
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &info);
}
void initBoard()
{
    score = 0;
    lines = 0;
    level = 1;
    loadBestScore();

    for (int i = 0; i < H; i++)
    {
        for (int j = 0; j < W; j++)
        {
            if (i == 0 || i == H - 1 || j == 0 || j == W - 1)
            {
                board[i][j] = '#';
            }
            else
            {
                board[i][j] = ' ';
            }
        }
    }
}

void drawBoard()
{
    for (int i = 0; i < H; i++)
    {
        for (int j = 0; j < W; j++)
        {
            gotoxy(j * 2, i);
            if (board[i][j] == '#') {
                if (i == 0 && j == 0) cout << "╭─";
                else if (i == 0 && j == W - 1) cout << "╮ ";
                else if (i == H - 1 && j == 0) cout << "╰─";
                else if (i == H - 1 && j == W - 1) cout << "╯ ";
                else if (i == 0 || i == H - 1) cout << "──";
                else cout << "│ ";
            } 
            else if (board[i][j] != ' ') {
                cout << "██";
            }
            else {
                cout << "  ";
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

    drawFrame(infoX, infoY + 16, 32, 9, "CONTROLS");
    gotoxy(infoX + 2, infoY + 18); cout << "A/D: Move";
    gotoxy(infoX + 2, infoY + 19); cout << "W : Rotate";
    gotoxy(infoX + 2, infoY + 20); cout << "S : Soft Drop";
    gotoxy(infoX + 2, infoY + 21); cout << "C : Hold Block";
}

//
void applyGameMode() {
    switch (currentMode) {
        case EASY_MODE:
            gameSpeed = 800; 
            break;
        case NORMAL_MODE:
            gameSpeed = 500; 
            break;
        case HARD_MODE:
            gameSpeed = 300; 
            break;
    }
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
    while (true) {
        vector<string> mainOpts = {"Choi ngay", "Chon do kho", "Thoat"};
        int choice = drawMenu(mainOpts, "TETRIS GAME");
        
        if (choice == 0) {
            break;
        } else if (choice == 1) {
            vector<string> modeOpts = {"Easy (Cham)", "Normal (Mac dinh)", "Hard (Nhanh + Co rac)", "Quay lai"};
            int modeChoice = drawMenu(modeOpts, "CHON DO KHO");
            
            if (modeChoice == 0) currentMode = EASY_MODE;
            else if (modeChoice == 1) currentMode = NORMAL_MODE;
            else if (modeChoice == 2) currentMode = HARD_MODE;
            
            applyGameMode();
        } else if (choice == 2) {
            exit(0);
        }
    }
    system("cls");
}

Piece* getRandomPiece()
{
    int type = rand() % 7;
    switch (type)
    {
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

void checkLine()
{
    int linesClearedThisTurn = 0; 
    for (int i = H - 2; i > 0; i--)
    {
        bool full = true;
        for (int j = 1; j < W - 1; j++)
        {
            if (board[i][j] == ' ')
            {
                full = false;
                break;
            }
        }

        if (full)
        {
            linesClearedThisTurn++;
            PlaySound(TEXT("sounds/clear_line.wav"), NULL, SND_FILENAME | SND_ASYNC);
            for (int k = i; k > 1; k--)
            {
                for (int j = 1; j < W - 1; j++)
                {
                    board[k][j] = board[k - 1][j];
                }
            }
            i++;
        }
    }
    
    if (linesClearedThisTurn > 0) {
        calculateScore(linesClearedThisTurn);
    }
}

int main()
{
    SetConsoleOutputCP(CP_UTF8);
    hideCursor();
    srand(time(0));
    
    showMainMenu(); 

    initBoard();
    currentPiece = getRandomPiece();
    currentPiece->initShape();
    nextPiece = getRandomPiece();
    nextPiece->initShape();
    holdPiece = nullptr;
    canHold = true;
    lastDropTime = clock();

    while (true)
    {
        drawBoard();
        drawGameInfo();
        currentPiece->block2Board(board);

        if (_kbhit())
        {
            char c = _getch();
            currentPiece->boardDelBlock(board);

            if (c == 'a')
            {
                if (currentPiece->canMove(-1, 0, board))
                {
                    currentPiece->moveLeft();
                }
            }
            else if (c == 'd')
            {
                if (currentPiece->canMove(1, 0, board))
                {
                    currentPiece->moveRight();
                }
            }
            else if (c == 's')
            {
                if (currentPiece->canMove(0, 1, board))
                {
                    currentPiece->moveDown();
                }
            }
            else if (c == 'w')
            {
                currentPiece->rotate(board); 
                PlaySound(TEXT("sounds/rotate.wav"), NULL, SND_FILENAME | SND_ASYNC);
            }
            else if (c == 'c' || c == 'C')
            {
                if (canHold)
                {
                    if (holdPiece == nullptr) {
                        holdPiece = currentPiece;
                        currentPiece = nextPiece;
                        nextPiece = getRandomPiece();
                        nextPiece->initShape();
                    } else {
                        Piece* temp = currentPiece;
                        currentPiece = holdPiece;
                        holdPiece = temp;
                    }
                    holdPiece->resetPos();
                    canHold = false;
                }
            }
            
            currentPiece->block2Board(board);
        }

        if (clock() - lastDropTime > gameSpeed)
        {
            currentPiece->boardDelBlock(board);
            if (currentPiece->canMove(0, 1, board))
            {
                currentPiece->moveDown();
            }
            else
            {
                currentPiece->block2Board(board);
                checkLine();
                PlaySound(TEXT("sounds/land.wav"), NULL, SND_FILENAME | SND_ASYNC);
                delete currentPiece;
                currentPiece = nextPiece;
                nextPiece = getRandomPiece();
                nextPiece->initShape();
                canHold = true;
                
                if (!currentPiece->canMove(0, 0, board))
                {
                    gotoxy(0, H + 2);
                    cout << "Game Over!\n";
                    break;
                }
            }
            currentPiece->block2Board(board);
            lastDropTime = clock();
        }

        Sleep(50);
    }

    return 0;
}