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
    void stage2();
    void waitForEnter();
    void printDialogueWithNPC(const char* speaker[], const char* lines[], int lineCount, int startX, int startY);

    // 메인 함수
    int main() {
        setup();
        title();
        intro();
        stage1();
        stage2();
        return 0;
    }

    // 커서 이동
    void gotoxy(int x, int y) {
        COORD pos = { x, y };
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
    }

    // 색상 설정
    void setColor(int color) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
    }

    // 콘솔 환경 세팅
    void setup() {
        srand((unsigned int)time(NULL));

        system("title Main");
        system("mode con: cols=120 lines=35");
        system("cls");
    }

    // 타이틀 출력
    void title() {
        setColor(RED); gotoxy(40, 10); printf("==============================================");
        setColor(WHITE); gotoxy(50, 12); printf("게임 제목");
        setColor(D_YELLOW); gotoxy(50, 13); printf("부제");
        setColor(RED); gotoxy(40, 15); printf("==============================================");
        gotoxy(50, 17); setColor(WHITE); printf("아무 키나 눌러 시작");
        _getch();
        system("cls");
    }

// 대사 출력 함수 - NPC 포함
void printDialogueWithNPC(const char* speaker[], const char* lines[], int lineCount, int startX, int startY) {
    for (int i = 0; i < lineCount; i++) {
        system("cls");
        gotoxy(startX, startY);
        setColor(WHITE);
        printf("[%s] : %s", speaker[i], lines[i]);

        gotoxy(startX, startY + 2);
        setColor(D_GRAY);
        printf("다음 대사를 보려면 [Enter] 키를 누르세요");

        while (1) {
            if (_kbhit()) {
                char key = _getch();
                if (key == 13) break;
            }
        }
    }
}

// 인트로 화면
void intro() {
    const char* npcNames[] = {
        "나레이션", "나레이션", "나레이션", "나레이션"
    };
    const char* introLines[] = {
        "대사1",
        "대사2",
        "대사3",
        "대사4"
    };
    int numLines = sizeof(introLines) / sizeof(introLines[0]);
    printDialogueWithNPC(npcNames, introLines, numLines, 5, 5);
}

// 엔터 키 대기
void waitForEnter() {
    gotoxy(5, 25); setColor(D_GRAY);
    printf("계속하려면 [Enter] 키를 누르세요");
    while (1) {
        if (_kbhit()) {
            char key = _getch();
            if (key == 13) break;
        }
    }
}

// 스테이지 1 마을
void stage1() {
    const char* npcNames[] = {
        "의심 많은 주민", "플레이어", "의심 많은 주민"
    };
    const char* stageLines[] = {
        "대사1(주민)",
        "대사2(플레이어)",
        "대사3(주민)"
    };
    int numLines = sizeof(stageLines) / sizeof(stageLines[0]);
    printDialogueWithNPC(npcNames, stageLines, numLines, 5, 5);

    gotoxy(5, 10); setColor(WHITE); printf("1. 선택지1");
    gotoxy(5, 11); printf("2. 선택지2");
    gotoxy(5, 13); printf("선택 : ");

    char choice;
    while (1) {
        choice = _getch();
        if (choice == '1') {
            system("cls");
            gotoxy(5, 5); printf("선택지1 결과");
            break;
        }
        else if (choice == '2') {
            system("cls");
            gotoxy(5, 5); printf("선택지 2 결과");
            break;
        }
    }
}

    // 스테이지 2 숲 초입
    void stage2() {
        const char* npcNames[] = {
            "NPC", "NPC", "NPC"
        };
        const char* stageLines[] = {
            "대사1(NPC)",
            "대사2(NPC)",
            "대사3(NPC)"
        };
        int numLines = sizeof(stageLines) / sizeof(stageLines[0]);
        printDialogueWithNPC(npcNames, stageLines, numLines, 5, 5);

        gotoxy(5, 10); setColor(WHITE); printf("1. 선택지1");
        gotoxy(5, 11); printf("2. 선택지2");
        gotoxy(5, 13); printf("선택 : ");
    };

    /*waitForEnter();
    system("cls");
    gotoxy(5, 5); printf("To be continued... (Stage 2 준비 중)");*/

