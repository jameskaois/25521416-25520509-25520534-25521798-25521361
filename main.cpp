#include <iostream>
#include <conio.h>
#include <windows.h>
#include <time.h>
using namespace std;

#define H 20
#define W 20

int gameSpeed = 200; 

char board[H][W] = {} ;

const char blocks[7][4][4] = {
        {{' ','I',' ',' '},
         {' ','I',' ',' '},
         {' ','I',' ',' '},
         {' ','I',' ',' '}},
        
        {{' ',' ',' ',' '},
         {' ','O','O',' '},
         {' ','O','O',' '},
         {' ',' ',' ',' '}},
        
        {{' ',' ',' ',' '},
         {'T','T','T',' '},
         {' ','T',' ',' '},
         {' ',' ',' ',' '}},
        
        {{' ',' ',' ',' '},
         {' ','S','S',' '},
         {'S','S',' ',' '},
         {' ',' ',' ',' '}},
         
        {{' ',' ',' ',' '},
         {'Z','Z',' ',' '},
         {' ','Z','Z',' '},
         {' ',' ',' ',' '}},
         
        {{' ','J',' ',' '},
         {' ','J',' ',' '},
         {'J','J',' ',' '},
         {' ',' ',' ',' '}},
         
        {{' ','L',' ',' '},
         {' ','L',' ',' '},
         {' ','L','L',' '},
         {' ',' ',' ',' '}}
};

char activeBlockShape[4][4]; 
int x = W / 2 - 2, y = 1, b = 1;

void gotoxy(int x, int y) {
    COORD c = {(SHORT)x, (SHORT)y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void boardDelBlock(){
    for (int i = 0 ; i < 4 ; i++)
        for (int j = 0 ; j < 4 ; j++)
            if (activeBlockShape[i][j] != ' ' && y+i < H)
                board[y+i][x+j] = ' ';
}

void block2Board(){
    for (int i = 0 ; i < 4 ; i++)
        for (int j = 0 ; j < 4 ; j++)
            if (activeBlockShape[i][j] != ' ' && y+i < H )
                board[y+i][x+j] = activeBlockShape[i][j];
}

void initBoard(){
    for (int i = 0 ; i < H ; i++)
        for (int j = 0 ; j < W ; j++)
            if (i == 0 || i == H-1 || j == 0 || j == W-1)
                board[i][j] = '#';
            else
                board[i][j] = ' ';
}

void draw(){
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

bool canMove(int dx, int dy){
    for (int i = 0 ; i < 4 ; i++)
        for (int j = 0 ; j < 4 ; j++)
            if (activeBlockShape[i][j] != ' '){
                int tx = x + j + dx;
                int ty = y + i + dy;
                if ( tx < 1 || tx >= W-1 || ty >= H-1 || ty < 1) return false;
                if ( board[ty][tx] != ' ') return false;
            }
    return true;
}

void rotateBlock() {
    char tempBlock[4][4];
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            tempBlock[j][3 - i] = activeBlockShape[i][j];
        }
    }
    bool canRotate = true;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (tempBlock[i][j] != ' ') {
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
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                activeBlockShape[i][j] = tempBlock[i][j];
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
    x = W / 2 - 2; 
    
    b = rand() % 7;
    
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
            activeBlockShape[i][j] = blocks[b][i][j];

    int offset = 0;
    for (int i = 0; i < 4; i++) {
        bool hasBlock = false;
        for (int j = 0; j < 4; j++) {
            if (activeBlockShape[i][j] != ' ') {
                hasBlock = true;
                break;
            }
        }
        if (hasBlock) {
            offset = i;
            break;
        }
    }
    y = 1 - offset;
}

int main()
{
    SetConsoleOutputCP(CP_UTF8);

    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.dwSize = 100; cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);

    srand(time(0));
    spawnNewBlock();

    system("cls");
    initBoard();
    
    int timer = 0;
    
    while (1){
        boardDelBlock();
        
        while (_kbhit()){
            char c = _getch();
            if (c=='a' && canMove(-1,0)) x--;
            if (c=='d' && canMove(1,0) ) x++;
            if (c=='w') rotateBlock();           
            if (c=='s' && canMove(0,1))  y++;    
            if (c=='q') exit(0);
        }
    
        timer += 10;
        
        if (timer >= gameSpeed) {
            if (canMove(0,1)) y++;
            else {
                block2Board();
                
                int clearedLines = removeLine(); 
                if (clearedLines > 0) {
                    gameSpeed -= (clearedLines * 15);
                    if (gameSpeed < 50) gameSpeed = 50; 
                }
                
                spawnNewBlock();

                if (!canMove(0, 0)) {
                    block2Board(); 
                    draw();        
                    gotoxy(W * 2 + 5, H / 2);
                    cout << "GAME OVER!";
                    Sleep(2000);
                    break;
                }
            }
            timer = 0;
        }
        block2Board();
        draw();

        Sleep(10); 
    }
    gotoxy(0, H + 2); 
    return 0;
}