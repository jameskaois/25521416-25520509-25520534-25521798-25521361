#include <iostream>
#include <fstream>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include <vector>
#include "Piece.h"

using namespace std;

#define H 20
#define W 20

int gameSpeed = 400; 
char board[H][W] = {};

// Biến điểm.
int score = 0;
int highScore = 0;

Piece* currentPiece = nullptr;

Piece* nextPiece = nullptr;

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

Piece* createRandomPiece() 
{
    int b = rand() % 7;

    switch(b) 
    {
        case 0: return new PieceI();
        case 1: return new PieceO();
        case 2: return new PieceT();
        case 3: return new PieceS();
        case 4: return new PieceZ();
        case 5: return new PieceJ();
        case 6: return new PieceL();
    }
    return nullptr; 
}

// Spawn ra block mới và hiển thị block tiếp theo.
void spawnNewBlock() 
{
    Piece* oldPiece = currentPiece;

    currentPiece = nextPiece;
    nextPiece = createRandomPiece();

    currentPiece->initShape();
    nextPiece->initShape();

    if (oldPiece != nullptr)
        delete oldPiece;
}

// Hiển thị block tiếp theo ở góc phải.
void drawNextBlock() 
{
    gotoxy(W * 2 + 5, 2);
    cout << "Khối tiếp theo:";

    char preview[4][4];
    nextPiece->getShape(preview);

    for (int i = 0; i < 4; i++) 
    {
        gotoxy(W * 2 + 5, 4 + i);
        for (int j = 0; j < 4; j++) 
        {
            if (preview[i][j] != ' ')
                cout << "██";
            else
                cout << "  ";
        }
    }
}

// Hàm load điểm cao nhất từ file txt.
void loadHighScore() 
{
    ifstream file("highscore.txt");
    if (file.is_open()) 
    {
        file >> highScore;
        file.close();
    }
}

// Lưu điểm cao nhất vào file.
void saveHighScore() 
{
    ofstream file("highscore.txt");
    if (file.is_open()) 
    {
        file << highScore;
        file.close();
    }
}

// Vẽ khung điểm.
void drawInfoBox() 
{
    int startX = W * 2 + 3;
    int startY = 9;
    int boxWidth = 18;
    int boxHeight = 8;

    for (int i = 0; i < boxHeight; i++) 
    {
        gotoxy(startX, startY + i);
        for (int j = 0; j < boxWidth; j++) 
        {
            if (i == 0 || i == boxHeight - 1 || j == 0 || j == boxWidth - 1)
                cout << "#";
            else
                cout << " ";
        }
    }
}

int main()
{
    SetConsoleOutputCP(CP_UTF8);

    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.dwSize = 100; cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);

    srand(time(0));

    loadHighScore();

    initBoard();

    currentPiece = createRandomPiece();
    currentPiece->initShape();

    nextPiece = createRandomPiece();
    nextPiece->initShape();
    
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
                    timer = gameSpeed; 
                }

                // Pause Game
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
                    gotoxy(W * 2 + 6, 15);
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
                // Cộng điểm sau khi xóa line.
                score += clearedLines * 100;

                if (clearedLines > 0) {
                    gameSpeed -= (clearedLines * 2); // Chỉnh sửa tốc độ tăng vừa phải
                    if (gameSpeed < 50) gameSpeed = 50; 
                }
                
                spawnNewBlock();

                if (!currentPiece->canMove(0, 0, board)) {
                    currentPiece->block2Board(board); 
                    draw();        
                    gotoxy(W * 2 + 6, 16);
                    cout << "GAME OVER!";
                    if (score > highScore) 
                    {
                        highScore = score;
                        saveHighScore();
                    }
                    Sleep(2000);
                    break;
                }
            }
            timer = 0;
        }
        
        currentPiece->block2Board(board);
        draw();
        drawNextBlock();
        drawInfoBox();

        gotoxy(W * 2 + 6, 11);
        cout << "Điểm: " << score << "   ";

        gotoxy(W * 2 + 6, 13);
        cout << "Kỷ lục: " << highScore << "   ";

        Sleep(10); 
    }
    gotoxy(0, H + 2);

    delete currentPiece;
    delete nextPiece;

    return 0;
}