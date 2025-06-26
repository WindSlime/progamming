#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include <stdlib.h>

// 콘솔 색상 정의
enum {
    BLACK, D_BLUE, D_GREEN, D_SKYBLUE, D_RED, D_VIOLET, D_YELLOW,
    GRAY, D_GRAY, BLUE, GREEN, SKYBLUE, RED, VIOLET, YELLOW, WHITE
};

// 함수 선언
void gotoxy(int x, int y);
void setColor(int color);
void setup();
void title();
void intro();
void stage1();
void printDialogue(const char* lines[], int lineCount, int startX, int startY);
void waitForEnter();

// 메인 함수
int main() {
    setup();
    title();
    intro();
    stage1();
    return 0;
}

// 커서 위치 이동
void gotoxy(int x, int y) {
    COORD pos = { x, y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

// 콘솔 텍스트 색상 설정
void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

// 콘솔 환경 설정
void setup() {
    srand((unsigned int)time(NULL));
    system("mode con: cols=120 lines=35");
    system("cls");
}

// 타이틀 화면 출력
void title() {
    setColor(RED); gotoxy(40, 10); printf("==============================================");
    setColor(WHITE); gotoxy(50, 12); printf("게임 제목");
    setColor(D_YELLOW); gotoxy(50, 13); printf("부제");
    setColor(RED); gotoxy(40, 15); printf("==============================================");
    gotoxy(50, 17); setColor(WHITE); printf("아무 키나 눌러 시작");
    _getch();
    system("cls");
}

// 대사 배열 출력 함수 (대사1, 대사2 형식)
void printDialogue(const char* lines[], int lineCount, int startX, int startY) {
    for (int i = 0; i < lineCount; i++) {
        system("cls");
        gotoxy(startX, startY);
        setColor(WHITE);
        printf("대사%d: %s", i + 1, lines[i]);

        gotoxy(startX, startY + 2);
        setColor(D_GRAY);
        printf("다음 대사를 보려면 [Enter] 키를 누르세요");
        while (1) {
            if (_kbhit()) {
                char key = _getch();
                if (key == 13) break; // Enter 키
            }
        }
    }
}

// 인트로 화면
void intro() {
    const char* introLines[] = {
        "대사",
        "대사",
        "대사",
        "대사"
    };
    int numLines = sizeof(introLines) / sizeof(introLines[0]);
    printDialogue(introLines, numLines, 5, 5);
}

// 스테이지 1 : 숲 입구
void stage1() {
    system("cls");
    const char* stageLines[] = {
        "대사",
        "대사",
        "대사"
    };
    int lineCount = sizeof(stageLines) / sizeof(stageLines[0]);
    printDialogue(stageLines, lineCount, 5, 5);

    gotoxy(5, 10); setColor(WHITE); printf("1. 선택지 2");
    gotoxy(5, 11); printf("2. 선택지 1");
    gotoxy(5, 13); printf("선택지 고른거 ");

    char choice;
    while (1) {
        choice = _getch();
        if (choice == '1') {
            system("cls");
            gotoxy(5, 5); printf("선택지 1 결과");
            break;
        }
        else if (choice == '2') {
            system("cls");
            gotoxy(5, 5); printf("선택지 2 결과");
            break;
        }
    }

    waitForEnter();
    system("cls");
    gotoxy(5, 5); printf("스테이지 2");
}

// Enter 키 대기 함수
void waitForEnter() {
    gotoxy(5, 7); setColor(D_GRAY);
    printf("계속하려면 [Enter] 키를 누르세요");
    while (1) {
        if (_kbhit()) {
            char key = _getch();
            if (key == 13) break;
        }
    }
}
