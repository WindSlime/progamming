#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#define CLEAR_SCREEN "cls"
#define SLEEP_MS(ms) Sleep(ms)
#define GET_KEY _getch
#define CHECK_KEY_HIT _kbhit

void SetWinConsoleTextColor(WORD color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void ResetWinConsoleTextColor() {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

void SetCursorPosition(int x, int y) {
    COORD coord = { (short)x, (short)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

#define SET_COLOR_RED      SetWinConsoleTextColor(FOREGROUND_RED | FOREGROUND_INTENSITY)
#define SET_COLOR_GREEN    SetWinConsoleTextColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY)
#define SET_COLOR_YELLOW   SetWinConsoleTextColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY)
#define SET_COLOR_BLUE     SetWinConsoleTextColor(FOREGROUND_BLUE | FOREGROUND_INTENSITY)
#define SET_COLOR_MAGENTA  SetWinConsoleTextColor(FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY)
#define SET_COLOR_CYAN     SetWinConsoleTextColor(FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY)
#define RESET_COLOR        ResetWinConsoleTextColor()

#else
#include <unistd.h>
#include <termios.h>
#include <sys/time.h>
#include <sys/select.h>

static struct termios oldt_posix, newt_posix;
void changemode(int dir) {
    if (dir == 1) {
        tcgetattr(STDIN_FILENO, &oldt_posix);
        newt_posix = oldt_posix;
        newt_posix.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt_posix);
    }
    else {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt_posix);
    }
}

int kbhit_posix(void) {
    struct timeval tv;
    fd_set rdfs;
    tv.tv_sec = 0; tv.tv_usec = 0;
    FD_ZERO(&rdfs);
    FD_SET(STDIN_FILENO, &rdfs);
    select(STDIN_FILENO + 1, &rdfs, NULL, NULL, &tv);
    return FD_ISSET(STDIN_FILENO, &rdfs);
}

#define CLEAR_SCREEN "clear"
#define SLEEP_MS(ms) usleep(ms * 1000)
#define GET_KEY getchar
#define CHECK_KEY_HIT kbhit_posix

void SetCursorPosition(int x, int y) {
    printf("\033[%d;%dH", y + 1, x + 1);
}

#define SET_COLOR_RED      printf("\x1b[31m")
#define SET_COLOR_GREEN    printf("\x1b[32m")
#define SET_COLOR_YELLOW   printf("\x1b[33m")
#define SET_COLOR_BLUE     printf("\x1b[34m")
#define SET_COLOR_MAGENTA  printf("\x1b[35m")
#define SET_COLOR_CYAN     printf("\x1b[36m")
#define RESET_COLOR        printf("\x1b[0m")
#endif

typedef struct {
    char name[20];
    int health;
    int attack;
} Monster;

void ClearScreen();
void DisplayMainMenu();
void DisplayGameScreen(const Monster* currentMonster, const char* playerActionMessage);
int PerformAttack(Monster* targetMonster);

void print_centered(const char* text, int y_offset);

int main() {
    srand((unsigned int)time(NULL));

#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#else
    changemode(1);
#endif

    int choice;
    Monster slime = { "Slime", 50, 10 };

    do {
        DisplayMainMenu();

        if (!(scanf("%d", &choice))) {
            while (getchar() != '\n');
            SetCursorPosition(30, 14);
            printf("Invalid input. Please choose again.\n");
            SLEEP_MS(1000);
            continue;
        }

        switch (choice) {
        case 1:
            if (slime.health <= 0) {
                ClearScreen();
                SetCursorPosition(30, 10);
                printf("You already defeated the Slime! Prepare for the next stage.\n");
                SLEEP_MS(2000);
                continue;
            }

            DisplayGameScreen(&slime, "A Slime appeared!");
            SLEEP_MS(2000);
            PerformAttack(&slime);

            if (slime.health <= 0) {
                ClearScreen();
                SetCursorPosition(30, 10);
                printf("You defeated the Slime!\n");
                SLEEP_MS(2000);
            }
            else {
                ClearScreen();
                DisplayGameScreen(&slime, "The Slime attacks you!");
                SLEEP_MS(2000);
            }
            break;
        case 2:
            ClearScreen();
            SetCursorPosition(20, 5);   printf("===========================\n");
            SetCursorPosition(20, 6);   printf("        GAME EXPLANATION\n");
            SetCursorPosition(20, 7);   printf("===========================\n");
            SetCursorPosition(20, 9);   printf("Fight monsters and win!\n");
            SetCursorPosition(20, 10);  printf("Press Spacebar at the right time during attack to increase damage.\n");
            SetCursorPosition(20, 12);  printf("Press any key to return to the main menu...\n");
            GET_KEY();
            break;
        case 3:
            ClearScreen();
            SetCursorPosition(30, 10);
            printf("Exiting game. Goodbye!\n");
            SLEEP_MS(1500);
            break;
        default:
            SetCursorPosition(30, 14);
            printf("Invalid choice. Please choose again.\n");
            SLEEP_MS(1000);
            break;
        }
    } while (choice != 3);

#ifndef _WIN32
    changemode(0);
#endif
    return 0;
}

void ClearScreen() {
    system(CLEAR_SCREEN);
}

void print_centered(const char* text, int y_offset) {
    int len = strlen(text);
    int console_width = 80;
    int pad = (console_width - len) / 2;
    if (pad < 0) pad = 0;

    SetCursorPosition(pad, y_offset);
    printf("%s", text);
}

void DisplayMainMenu() {
    ClearScreen();
    print_centered("===========================", 5);
    print_centered("       It's just a game       ", 6);
    print_centered("===========================", 7);
    print_centered("1. Start Game", 9);
    print_centered("2. Game Explanation", 10);
    print_centered("3. Exit Game", 11);
    print_centered("Select your choice: ", 13);
    SetCursorPosition(30 + strlen("Select your choice: "), 13);
}

void DisplayGameScreen(const Monster* currentMonster, const char* playerActionMessage) {
    ClearScreen();

    print_centered("---------------", 5);
    print_centered(currentMonster->name, 6);
    char hp_str[50];
    sprintf(hp_str, "HP: %d", currentMonster->health);
    print_centered(hp_str, 7);
    print_centered("---------------", 8);

    SetCursorPosition(5, 20); printf("Player HP: 100\n");
    SetCursorPosition(5, 21); printf("Gold: 0\n");

    SetCursorPosition(5, 15); printf("%s\n", playerActionMessage);

    SetCursorPosition(20, 23); printf("----------------------------------------\n");
    SetCursorPosition(20, 24); printf("        [Awaiting Attack Timing...]     \n");
    SetCursorPosition(20, 25); printf("----------------------------------------\n");
}

int PerformAttack(Monster* targetMonster) {
    int totalDamage = 0;
    int hitCount = 0;
    const int maxHits = 4; // 총 4번의 공격 기회

    int barY = 24;          // 공격 게이지 바가 표시될 Y 좌표
    int barStartX = 25;     // 바 시작 X 좌표
    int barLength = 30;     // 바 길이

    // 히트 존 설정
    // 튼튼한 장갑처럼 "딱 딱 딱" 맞추는 느낌을 위해 3개의 히트 존을 정의
    struct HitZone {
        int startX_rel; // 바 시작점에서 상대적인 시작 위치
        int length;     // 존의 길이
    };
    HitZone goodZone = { barLength / 2 - 2, 4 }; // 중앙의 GOOD 존
    HitZone perfectZone = { barLength / 2 - 1, 2 }; // GOOD 존 안의 PERFECT 존 (GOOD보다 작게)

    // UI 기본 틀 그리기
    ClearScreen();
    DisplayGameScreen(targetMonster, "Attack! Press Spacebar!");

    SetCursorPosition(barStartX + goodZone.startX_rel - 4, barY - 1); SET_COLOR_GREEN; printf("GOOD"); RESET_COLOR;
    SetCursorPosition(barStartX + perfectZone.startX_rel - 4, barY + 1); SET_COLOR_YELLOW; printf("PERFECT"); RESET_COLOR;

    SetCursorPosition(barStartX - 2, barY - 1); printf("╔"); for (int i = 0; i < barLength + 2; ++i) printf("═"); printf("╗");
    SetCursorPosition(barStartX - 2, barY);     printf("║"); for (int i = 0; i < barLength + 2; ++i) printf(" "); printf("║");
    SetCursorPosition(barStartX - 2, barY + 1); printf("╚"); for (int i = 0; i < barLength + 2; ++i) printf("═"); printf("╝");

    SetCursorPosition(barStartX - 2, barY + 4);
    printf("Attacks Left: %d / %d", maxHits - hitCount, maxHits);
    fflush(stdout); // 버퍼 비우기

    for (int i = 0; i < maxHits; ++i) {
        time_t startTime = time(NULL);
        int currentPosition = 0; // 바 내에서 커서의 0부터 barLength-1까지의 상대 위치
        int previousPosition = -1; // 이전 프레임의 커서 위치

        SetCursorPosition(5, 15);
        printf("Attack %d/%d: Press Spacebar at the right time!           ", i + 1, maxHits);
        fflush(stdout);

        int inputReceived = 0;
        while (!inputReceived) {
            time_t currentTime = time(NULL);
            double elapsedSeconds = difftime(currentTime, startTime);
            // 600ms 동안 barLength 만큼 움직이도록 속도 조절
            currentPosition = (int)((elapsedSeconds * 1000) / (600.0 / barLength)) % barLength;

            if (currentPosition != previousPosition) { // 커서 위치가 변경되었을 때만 그리기
                SetCursorPosition(barStartX, barY); // 바 내부 시작 위치로 이동

                // 바의 모든 칸을 다시 그림 (전체 바를 다시 그려 깜빡임 방지)
                for (int k = 0; k < barLength; ++k) {
                    // Good Zone과 Perfect Zone 표시
                    if (k >= goodZone.startX_rel && k < (goodZone.startX_rel + goodZone.length)) {
                        if (k >= perfectZone.startX_rel && k < (perfectZone.startX_rel + perfectZone.length)) {
                            SET_COLOR_YELLOW;
                            printf("|"); // PERFECT 존
                        }
                        else {
                            SET_COLOR_GREEN;
                            printf("||"); // GOOD 존 (PERFECT 제외)
                        }
                    }
                    else {
                        printf("-"); // 일반 바
                    }
                    RESET_COLOR; // 색상 리셋은 각 문자 뒤에 바로 적용
                }

                SetCursorPosition(barStartX + currentPosition, barY); // 커서 위치로 이동
                SET_COLOR_CYAN; // 커서 색상
                printf("I");
                RESET_COLOR;
                fflush(stdout); // 즉시 화면에 반영

                previousPosition = currentPosition;
            }

            if (CHECK_KEY_HIT()) {
                if (GET_KEY() == ' ') { // 스페이스바 감지
                    inputReceived = 1;
                    SetCursorPosition(barStartX, barY + 2); // 메시지 출력 위치 조정

                    int hitScore = 0; // 0: 실패, 1: GOOD, 2: PERFECT
                    if (currentPosition >= perfectZone.startX_rel && currentPosition < (perfectZone.startX_rel + perfectZone.length)) {
                        hitScore = 2; // PERFECT
                    }
                    else if (currentPosition >= goodZone.startX_rel && currentPosition < (goodZone.startX_rel + goodZone.length)) {
                        hitScore = 1; // GOOD
                    }

                    if (hitScore == 2) {
                        hitCount++;
                        SET_COLOR_YELLOW;
                        printf("PERFECT!                 "); // 공백으로 이전 메시지 지움
                        RESET_COLOR;
                    }
                    else if (hitScore == 1) {
                        hitCount++;
                        SET_COLOR_GREEN;
                        printf("GOOD!                 ");
                        RESET_COLOR;
                    }
                    else {
                        SET_COLOR_RED;
                        printf("MISSED!                 ");
                        RESET_COLOR;
                    }
                    fflush(stdout);
                    SLEEP_MS(300); // 사용자에게 메시지 확인할 시간 주기
                }
                else {
                    // 다른 키 입력 무시 (버퍼에서 제거)
                    GET_KEY();
                }
            }

            SLEEP_MS(10); // 루프 지연 (프레임 속도 조절)
        }
        SLEEP_MS(500); // 다음 공격 시도까지 잠시 대기
        SetCursorPosition(barStartX - 2, barY + 4);
        printf("Attacks Left: %d / %d", maxHits - (i + 1), maxHits);
        fflush(stdout);
    }

    SetCursorPosition(5, 15);
    printf("                                                     ");
    fflush(stdout);

    SetCursorPosition(barStartX, barY + 3);
    printf("                                                                  ");
    fflush(stdout);

    if (hitCount == maxHits) {
        totalDamage = targetMonster->attack * 3;
        SET_COLOR_MAGENTA;
        printf("Perfect Attack! You hit all timings for massive damage!\n");
        RESET_COLOR;
    }
    else if (hitCount > 0) {
        totalDamage = targetMonster->attack + (hitCount * 10);
        SET_COLOR_GREEN;
        printf("Hit! You landed %d hits for damage.\n", hitCount);
        RESET_COLOR;
    }
    else {
        totalDamage = targetMonster->attack / 4;
        SET_COLOR_RED;
        printf("All attacks missed! Very low damage!\n");
        RESET_COLOR;
    }

    targetMonster->health -= totalDamage;
    SLEEP_MS(2500);

    return totalDamage;
}