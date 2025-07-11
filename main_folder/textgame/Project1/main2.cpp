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
#define GET_TIME_MS GetTickCount64()
#else
#include <unistd.h>
#include <termios.h>
#include <sys/time.h>
#include <sys/select.h>
#define CLEAR_SCREEN "clear"
#define SLEEP_MS(ms) usleep(ms * 1000)
#define GET_KEY getchar
#define CHECK_KEY_HIT kbhit_posix
static long long GetTimeMs_posix() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (long long)tv.tv_sec * 1000 + (long long)tv.tv_usec / 1000;
}
#define GET_TIME_MS GetTimeMs_posix()
#endif

// 콘솔 색상 설정 매크로 (기존 코드 유지)
#ifdef _WIN32
void SetWinConsoleTextColor(WORD color) { SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color); }
void ResetWinConsoleTextColor() { SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); }
void SetCursorPosition(int x, int y) { COORD coord = { (short)x, (short)y }; SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord); }
#define SET_COLOR_RED SetWinConsoleTextColor(FOREGROUND_RED | FOREGROUND_INTENSITY)
#define SET_COLOR_GREEN SetWinConsoleTextColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY)
#define SET_COLOR_YELLOW SetWinConsoleTextColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY)
#define SET_COLOR_BLUE SetWinConsoleTextColor(FOREGROUND_BLUE | FOREGROUND_INTENSITY)
#define SET_COLOR_MAGENTA SetWinConsoleTextColor(FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY)
#define SET_COLOR_CYAN SetWinConsoleTextColor(FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY)
#define RESET_COLOR ResetWinConsoleTextColor()
#else
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
void SetCursorPosition(int x, int y) { printf("\033[%d;%dH", y + 1, x + 1); }
#define SET_COLOR_RED printf("\x1b[31m")
#define SET_COLOR_GREEN printf("\x1b[32m")
#define SET_COLOR_YELLOW printf("\x1b[33m")
#define SET_COLOR_BLUE printf("\x1b[34m")
#define SET_COLOR_MAGENTA printf("\x1b[35m")
#define SET_COLOR_CYAN printf("\x1b[36m")
#define RESET_COLOR printf("\x1b[0m")
#endif

// --- 게임 데이터 구조 및 변수 ---
#define NUM_STAGES 8
int playerHealth = 100;
int playerGold = 0;
int playerIsGuarding = 0; // 플레이어 가드 상태 변수

typedef enum { NORMAL, GUARD, RAMPAGE } MonsterPattern;

typedef struct {
    char name[20];
    int maxHealth;
    int currentHealth;
    int attack;
    MonsterPattern pattern;
    int guard_next_turn; // 가드 패턴 상태
} Monster;

// 8단계 몬스터 정보 배열
Monster monsters[NUM_STAGES] = {
    {"Slime", 50, 50, 10, NORMAL, 0},
    {"Goblin", 70, 70, 15, NORMAL, 0},
    {"Orc", 100, 100, 20, NORMAL, 0},
    {"Werewolf", 120, 120, 25, NORMAL, 0},
    {"Golem", 200, 200, 30, GUARD, 0},
    {"Dragon", 300, 300, 35, NORMAL, 0},
    {"Demon Lord", 400, 400, 40, NORMAL, 0},
    {"Final Boss", 500, 500, 50, RAMPAGE, 0}
};

// --- 함수 선언 ---
void ClearScreen();
void print_centered(const char* text, int y_offset);
void DisplayMainMenu();
void DisplayGameScreen(const Monster* currentMonster, int stage, const char* playerActionMessage);
void ClearAttackBarArea();
int PlayerAttack(Monster* targetMonster, int stage);
void PlayerTurn(Monster* currentMonster, int stage);
void MonsterTurn(Monster* currentMonster, int* playerHealth, int stage);
void StartCombat(int stage);

// --- 메인 함수 ---
int main() {
    srand((unsigned int)time(NULL));
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#else
    changemode(1);
#endif

    int choice;
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
            for (int stage = 0; stage < NUM_STAGES; ++stage) {
                if (playerHealth <= 0) break; // 플레이어가 죽으면 루프 종료
                StartCombat(stage);
            }
            if (playerHealth <= 0) {
                ClearScreen();
                SET_COLOR_RED;
                print_centered("YOU DIED...", 10);
                RESET_COLOR;
                print_centered("Press any key to return to the main menu.", 12);
                GET_KEY();
            }
            else {
                ClearScreen();
                SET_COLOR_MAGENTA;
                print_centered("Congratulations! You defeated all the monsters!", 10);
                RESET_COLOR;
                print_centered("Press any key to return to the main menu.", 12);
                GET_KEY();
            }
            playerHealth = 100; // 게임 종료 후 플레이어 체력 초기화
            break;
        case 2:
            ClearScreen();
            print_centered("===========================", 5);
            print_centered("         GAME EXPLANATION\n", 6);
            print_centered("===========================", 7);
            print_centered("Fight monsters and win!\n", 9);
            print_centered("Press Spacebar at the right time to increase damage.\n", 10);
            print_centered("Press any key to return to the main menu...\n", 12);
            GET_KEY();
            break;
        case 3:
            ClearScreen();
            print_centered("Exiting game. Goodbye!\n", 10);
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

// --- 게임 로직 함수 ---
void StartCombat(int stage) {
    Monster* currentMonster = &monsters[stage];
    currentMonster->currentHealth = currentMonster->maxHealth; // 몬스터 체력 초기화

    // 4라운드(인덱스 3)와 7라운드(인덱스 6) 시작 시 체력 회복
    if (stage == 3 || stage == 6) {
        playerHealth = 100;
        DisplayGameScreen(currentMonster, stage + 1, "체력이 모두 회복되었습니다!");
        SLEEP_MS(2000);
    }

    DisplayGameScreen(currentMonster, stage + 1, "A new monster appeared!");
    SLEEP_MS(2000);

    while (currentMonster->currentHealth > 0 && playerHealth > 0) {
        // 플레이어 턴
        PlayerTurn(currentMonster, stage);

        // 몬스터 턴
        if (currentMonster->currentHealth > 0) {
            MonsterTurn(currentMonster, &playerHealth, stage);
        }
    }
}

void PlayerTurn(Monster* currentMonster, int stage) {
    int choice = 0;
    while (choice != 1 && choice != 2) {
        DisplayGameScreen(currentMonster, stage + 1, "What will you do?\n\n1. Attack\n2. Guard");
        SetCursorPosition(5, 18);
        printf("Select an action: ");
        if (!(scanf("%d", &choice))) {
            while (getchar() != '\n');
        }
    }

    if (choice == 1) {
        PlayerAttack(currentMonster, stage);
    }
    else if (choice == 2) {
        playerIsGuarding = 1; // 가드 상태 활성화
        DisplayGameScreen(currentMonster, stage + 1, "You prepared for the monster's attack!");
        SLEEP_MS(2000);
    }
}

int PlayerAttack(Monster* targetMonster, int stage) {
    int totalDamage = 0;
    int hitCount = 0;
    const int maxHits = 4;
    const int barLength = 30;
    const int barX = 25;
    const int barY = 24;
    const int barDurationMs = 600;

    struct HitZone { int startX_rel; int length; };
    HitZone goodZone = { barLength / 2 - 2, 4 };
    HitZone perfectZone = { barLength / 2 - 1, 2 };

    DisplayGameScreen(targetMonster, stage + 1, "Attack! Press Spacebar!"); // 화면 업데이트

    for (int i = 0; i < maxHits; ++i) {
        ClearAttackBarArea();

        SetCursorPosition(5, 15);
        printf("Attack %d/%d: Press Spacebar at the right time!         ", i + 1, maxHits);

        SetCursorPosition(barX + goodZone.startX_rel - 4, barY - 1); SET_COLOR_GREEN; printf("GOOD"); RESET_COLOR;
        SetCursorPosition(barX + perfectZone.startX_rel - 4, barY + 1); SET_COLOR_YELLOW; printf("PERFECT"); RESET_COLOR;

        SetCursorPosition(barX - 2, barY - 1); printf("╔"); for (int k = 0; k < barLength + 2; ++k) printf("═"); printf("╗");
        SetCursorPosition(barX - 2, barY);     printf("║"); for (int k = 0; k < barLength + 2; ++k) printf(" "); printf("║");
        SetCursorPosition(barX - 2, barY + 1); printf("╚"); for (int k = 0; k < barLength + 2; ++k) printf("═"); printf("╝");

        SetCursorPosition(barX, barY);
        for (int k = 0; k < barLength; ++k) {
            if (k >= perfectZone.startX_rel && k < (perfectZone.startX_rel + perfectZone.length)) { SET_COLOR_YELLOW; printf("="); }
            else if (k >= goodZone.startX_rel && k < (goodZone.startX_rel + goodZone.length)) { SET_COLOR_GREEN; printf("="); }
            else { printf("-"); }
            RESET_COLOR;
        }
        fflush(stdout);
        SLEEP_MS(300);

        long long startTime = GET_TIME_MS;
        int currentPosition = 0;
        int previousPosition = -1;
        int inputReceived = 0;

        while (!inputReceived) {
            long long currentTime = GET_TIME_MS;
            long long elapsedTime = currentTime - startTime;
            currentPosition = (int)(((double)elapsedTime / barDurationMs) * barLength) % barLength;

            if (currentPosition != previousPosition) {
                SetCursorPosition(barX + previousPosition, barY); printf(" ");
                SetCursorPosition(barX + currentPosition, barY); SET_COLOR_CYAN; printf("|"); RESET_COLOR;
                fflush(stdout);
                previousPosition = currentPosition;
            }

            if (CHECK_KEY_HIT()) {
                if (GET_KEY() == ' ') {
                    inputReceived = 1;
                    SetCursorPosition(barX, barY + 3);
                    int hitScore = 0;
                    if (currentPosition >= perfectZone.startX_rel && currentPosition < (perfectZone.startX_rel + perfectZone.length)) { hitScore = 2; }
                    else if (currentPosition >= goodZone.startX_rel && currentPosition < (goodZone.startX_rel + goodZone.length)) { hitScore = 1; }

                    if (hitScore == 2) {
                        hitCount++; totalDamage += targetMonster->attack * 2; SET_COLOR_YELLOW; printf("PERFECT!                         "); RESET_COLOR;
                    }
                    else if (hitScore == 1) {
                        hitCount++; totalDamage += targetMonster->attack; SET_COLOR_GREEN; printf("GOOD!                            "); RESET_COLOR;
                    }
                    else {
                        totalDamage += (targetMonster->attack / 4); SET_COLOR_RED; printf("MISSED!                          "); RESET_COLOR;
                    }
                    fflush(stdout);
                    SLEEP_MS(300);
                }
                else { GET_KEY(); }
            }
            SLEEP_MS(5);
        }
        SLEEP_MS(500);
    }

    // 몬스터 가드 상태일 경우 피해 감소
    if (targetMonster->guard_next_turn) {
        totalDamage /= 2;
        targetMonster->guard_next_turn = 0; // 가드 상태 해제
        SET_COLOR_YELLOW;
        SetCursorPosition(5, 17);
        printf("the blow was but half as dire!");
        RESET_COLOR;
    }

    targetMonster->currentHealth -= totalDamage;
    SetCursorPosition(5, 15);
    printf("You dealt a total of %d damage!                                       \n", totalDamage);
    SLEEP_MS(2500);

    return totalDamage;
}

void MonsterTurn(Monster* currentMonster, int* playerHealth, int stage) {
    int damage_to_player = currentMonster->attack;

    ClearAttackBarArea();
    DisplayGameScreen(currentMonster, stage + 1, "The monster is attacking...");
    SLEEP_MS(1500);

    // 플레이어 가드 상태일 경우 피해 감소
    if (playerIsGuarding) {
        damage_to_player *= 0.4; // 60% 감소 (40%만 받음)
        DisplayGameScreen(currentMonster, stage + 1, "Your guard hath lessened the blow!");
        SLEEP_MS(1500);
        playerIsGuarding = 0; // 가드 상태 해제
    }

    switch (currentMonster->pattern) {
    case NORMAL:
        *playerHealth -= damage_to_player;
        DisplayGameScreen(currentMonster, stage + 1, "The monster attacked you!");
        printf("You took %d damage.\n", damage_to_player);
        break;
    case GUARD:
        if (rand() % 2 == 0) { // 50% 확률로 가드
            currentMonster->guard_next_turn = 1;
            SET_COLOR_YELLOW;
            DisplayGameScreen(currentMonster, stage + 1, "The foul beast hath raised its guard -");
            RESET_COLOR;
            printf("It will take less damage next turn.\n");
            damage_to_player = 0;
        }
        else {
            *playerHealth -= damage_to_player;
            DisplayGameScreen(currentMonster, stage + 1, "The Golem attacked you!");
            printf("You took %d damage.\n", damage_to_player);
        }
        break;
    case RAMPAGE:
        // Final Boss의 공격 패턴을 확률적으로 변경
        int attack_roll = rand() % 100;
        if (attack_roll < 50) { // 50% 확률로 일반 공격
            *playerHealth -= damage_to_player;
            DisplayGameScreen(currentMonster, stage + 1, "The Final Boss unleashed a common strike!");
            printf("You took %d damage.\n", damage_to_player);
        }
        else if (attack_roll < 80) { // 30% 확률로 강력한 공격
            *playerHealth -= (int)(damage_to_player * 1.5);
            DisplayGameScreen(currentMonster, stage + 1, "The Final Boss unleashed a powerful blow!");
            printf("You took %d damage.\n", (int)(damage_to_player * 1.5));
        }
        else { // 20% 확률로 광폭 공격 (기존 패턴)
            DisplayGameScreen(currentMonster, stage + 1, "The Final Boss started a rampage!");
            for (int i = 0; i < 3; ++i) { // 3번 연타
                *playerHealth -= damage_to_player / 2;
                printf("You took %d damage! (%d/%d)\n", damage_to_player / 2, i + 1, 3);
                SLEEP_MS(500);
            }
        }
        break;
    }

    SLEEP_MS(1500);
}

// --- 보조 함수 ---
void ClearScreen() { system(CLEAR_SCREEN); }

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
    print_centered("         It's just a game          ", 6);
    print_centered("===========================", 7);
    print_centered("1. Start Game", 9);
    print_centered("2. Game Explanation", 10);
    print_centered("3. Exit Game", 11);
    print_centered("Select your choice: ", 13);
    SetCursorPosition(30 + strlen("Select your choice: "), 13);
}

void DisplayGameScreen(const Monster* currentMonster, int stage, const char* playerActionMessage) {
    ClearScreen();

    print_centered("---------------", 5);
    SetCursorPosition(30, 6); // 몬스터 이름 위치로 이동

    // 7단계(데몬 로드)일 경우 색상 변경
    if (stage == 7) {
        SET_COLOR_MAGENTA;
        printf("STAGE %d: %s", stage, currentMonster->name);
        RESET_COLOR;
    }
    else {
        printf("STAGE %d: %s", stage, currentMonster->name);
    }

    char hp_str[50];
    sprintf(hp_str, "HP: %d / %d", currentMonster->currentHealth, currentMonster->maxHealth);
    print_centered(hp_str, 7);
    print_centered("---------------", 8);

    SetCursorPosition(5, 20); printf("Player HP: %d\n", playerHealth);
    SetCursorPosition(5, 21); printf("Gold: %d\n", playerGold);

    SetCursorPosition(5, 15); printf("%s\n", playerActionMessage);

    SetCursorPosition(20, 23); printf("----------------------------------------\n");
    SetCursorPosition(20, 24); printf("         [Awaiting Attack Timing...]      \n");
    SetCursorPosition(20, 25); printf("----------------------------------------\n");
}

void ClearAttackBarArea() {
    for (int y = 23; y <= 28; ++y) {
        SetCursorPosition(0, y);
        for (int x = 0; x < 80; ++x) {
            printf(" ");
        }
    }
}