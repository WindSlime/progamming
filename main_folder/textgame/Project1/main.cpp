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
}
void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}
void setup() {
    srand((unsigned int)time(NULL));

    system("title Text Based Game");
    system("mode con:cols=80 lines=25");
}
void title() {
    setColor(BLUE); gotoxy(10, 5); printf("==========================");
    setColor(WHITE);  gotoxy(10, 6); printf("     타이틀 화면 입니다.");
    setColor(WHITE);  gotoxy(10, 7); printf("아무키나 누르세요. 다음 ㄱㄱ");
    setColor(BLUE); gotoxy(10, 8); printf("==========================");
    _getch();
    system("cls");
}
void ingame() {
    int rx, ry;
    int rc;
    int key;

    setColor(D_BLUE); gotoxy(10, 5); printf("==========================");
    setColor(WHITE); gotoxy(10, 6); printf("     인게임 화면 입니다.");
    setColor(D_BLUE); gotoxy(10, 7); printf("==========================");

}