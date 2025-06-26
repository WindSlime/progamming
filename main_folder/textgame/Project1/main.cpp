#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include <stdlib.h>

enum {
    BLACK, //0
    D_BLUE,//1
    D_GREEN,//2
    D_SKYBLUE,//3
    D_RED,//4
    D_VIOLET,//
    D_YELLOW,
    GRAY,
    D_GRAY,
    BLUE,
    GREEN,
    SKYBLUE,
    RED,
    VIOLET,
    YELLOW,
    WHITE,//15
};

void gotoxy(int x, int y);
void setColor(int color);
void setup();
void title();
void ingame();

int main() {
    setup();
    title();
    ingame();


    return 0;
}

void gotoxy(int x, int y) {
    COORD pos = { x, y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);

    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD Cur;
    Cur.X = x;
    Cur.Y = y;
    SetConsoleCursorPosition(consoleHandle, Cur);
}
void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}
void setup() {
    srand((unsigned int)time(NULL));

    system("나중에");
    system("mode con:cols=250 lines=60");
}
void title() {
    setColor(RED); gotoxy(100, 20); printf("==========================");
    setColor(WHITE);  gotoxy(110, 21); printf("게임제목");
    setColor(WHITE);  gotoxy(111, 22); printf("부제목");
    setColor(RED); gotoxy(100, 23); printf("==========================");
    _getch();
    system("cls");
}


void ingame() {
    int rx, ry;
    int rc;
    int key;
    if (GetAsyncKeyState(VK_RETURN & 0x8000)) {
        printf("엔터키가 눌렸습니다\n");
    }

    setColor(D_BLUE); gotoxy(10, 5); printf("==============배경============");
    setColor(WHITE); gotoxy(10, 6); printf("배경");
    setColor(D_BLUE); gotoxy(10, 7); printf("==============배경============");

    //system("cls");
    setColor(WHITE); gotoxy(10, 6); printf("대사");
    Sleep(2000);
    //cls
    //다음대사

     while (1) {
        Sleep(500);
        rx = rand() % 300; //0~79
        ry = rand() % 100; //0~24
        rc = rand() % 15 + 1; //0~14 + 1 = 1~15

        setColor(rc); gotoxy(rx, ry); printf("Hello");

        if (_kbhit() == 1) {
            key = _getch();
            if (key == 'q' || key == 'Q') break;
        }
    } 
}