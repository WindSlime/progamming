#include <iostream>     // std::cin
#include <string>       // std::string
#include <windows.h>    // SetConsoleCursorPosition, GetStdHandle, COORD, GetAsyncKeyState, SetConsoleTextAttribute, SetConsoleOutputCP 등
#include <chrono>       // 시간 측정 (std::chrono)
#include <thread>       // std::this_thread::sleep_for
#include <limits>       // std::numeric_limits
#include <conio.h>      // _getch()

// ====================================================================
// 유틸리티 함수들 (콘솔 화면 제어)
// ====================================================================

// 커서 위치 설정 함수
void SetCursorPosition(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// 화면 지우기 함수
void ClearScreen() {
    system("cls"); // Windows
}

// 텍스트 색상 설정 함수 (Windows 전용)
void SetTextColor(WORD color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

// 텍스트 색상 초기화 함수 (Windows 전용)
// 이 함수는 호출될 때마다 콘솔의 기본 흰색으로 설정합니다.
void ResetTextColor() {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

// ====================================================================
// 게임 엔티티 (몬스터)
// ====================================================================

struct Monster {
    std::string name;
    int health;
    int attack;
};

// ====================================================================
// 화면 출력 함수들
// ====================================================================

void DisplayMainMenu() {
    ClearScreen();
    SetCursorPosition(30, 5);   printf("===========================\n");
    SetCursorPosition(30, 6);   printf("       언더테일 스타일 게임\n");
    SetCursorPosition(30, 7);   printf("===========================\n");
    SetCursorPosition(30, 9);   printf("1. 게임 시작\n");
    SetCursorPosition(30, 10);  printf("2. 게임 설명\n");
    SetCursorPosition(30, 11);  printf("3. 게임 종료\n");
    SetCursorPosition(30, 13);  printf("메뉴를 선택하세요: ");
}

void DisplayGameScreen(const Monster& currentMonster, const char* playerActionMessage = "") {
    ClearScreen();

    SetCursorPosition(40 - (int)(currentMonster.name.length() * 0.7), 5);
    printf("---------------\n");
    SetCursorPosition(40 - (int)(currentMonster.name.length() * 0.7), 6);
    printf("%s\n", currentMonster.name.c_str());
    SetCursorPosition(40 - (int)(currentMonster.name.length() * 0.7), 7);
    printf("HP: %d\n", currentMonster.health);
    SetCursorPosition(40 - (int)(currentMonster.name.length() * 0.7), 8);
    printf("---------------\n");

    SetCursorPosition(5, 20); printf("플레이어 HP: 100\n");
    SetCursorPosition(5, 21); printf("골드: 0\n");

    SetCursorPosition(5, 15); printf("%s\n", playerActionMessage);

    SetCursorPosition(20, 23); printf("----------------------------------------\n");
    SetCursorPosition(20, 24); printf("        [공격 타이밍 대기중...]         \n");
    SetCursorPosition(20, 25); printf("----------------------------------------\n");
}

// ====================================================================
// 전투 시스템 함수 - 언더테일 '튼튼한 장갑' 스타일
// ====================================================================

int PerformAttack(Monster& targetMonster) {
    int totalDamage = 0;
    int hitCount = 0;
    const int maxHits = 4;

    int barY = 24;
    int barStartX = 25;
    int barLength = 30;

    struct HitZone {
        int startX;
        int length;
    };
    HitZone currentHitZone = { barStartX + 14, 2 };

    ClearScreen();
    DisplayGameScreen(targetMonster, "공격! Spacebar를 누르세요!");

    SetCursorPosition(barStartX - 5, barY - 1);
    printf("GOOD");
    SetCursorPosition(barStartX - 5, barY + 1);
    printf("PERFECT");

    // 게이지 바 전체 테두리 그리기
    SetCursorPosition(barStartX - 2, barY - 1); printf("╔");
    for (int i = 0; i < barLength + 2; ++i) printf("═");
    printf("╗\n");
    SetCursorPosition(barStartX - 2, barY); printf("║");
    for (int i = 0; i < barLength + 2; ++i) printf(" ");
    printf("║\n");
    SetCursorPosition(barStartX - 2, barY + 1); printf("╚");
    for (int i = 0; i < barLength + 2; ++i) printf("═");
    printf("╝\n");

    SetCursorPosition(barStartX - 2, barY + 4);
    printf("남은 공격 횟수: %d / %d", maxHits - hitCount, maxHits);

    for (int i = 0; i < maxHits; ++i) {
        auto startTime = std::chrono::high_resolution_clock::now();
        int currentPosition = 0;

        SetCursorPosition(5, 15);
        printf("공격 %d/%d: 타이밍에 맞춰 Spacebar를 누르세요!\n", i + 1, maxHits);

        bool inputReceived = false;
        while (!inputReceived) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            long long elapsedMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count();

            currentPosition = (elapsedMilliseconds / (600 / barLength)) % barLength;

            SetCursorPosition(barStartX, barY);

            for (int k = 0; k < barLength; ++k) {
                bool inHitZone = (k >= (currentHitZone.startX - barStartX) && k < (currentHitZone.startX - barStartX + currentHitZone.length));

                if (k == currentPosition) {
                    SetTextColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY); // 밝은 노란색 커서
                    printf("▶");
                }
                else if (inHitZone) {
                    SetTextColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY); // 밝은 초록색 성공 구간
                    printf("■");
                }
                else {
                    SetTextColor(FOREGROUND_INTENSITY); // 어두운 회색
                    printf("─");
                }
                ResetTextColor(); // 색상 초기화
            }

            if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
                inputReceived = true;
                SetCursorPosition(barStartX, barY + 2);

                if (currentPosition >= (currentHitZone.startX - barStartX) && currentPosition < (currentHitZone.startX - barStartX + currentHitZone.length)) {
                    hitCount++;
                    SetTextColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
                    printf("타이밍 성공!                 ");
                    ResetTextColor();
                }
                else {
                    SetTextColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
                    printf("타이밍 실패!                 ");
                    ResetTextColor();
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(300));
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        SetCursorPosition(barStartX - 2, barY + 4);
        printf("남은 공격 횟수: %d / %d", maxHits - (i + 1), maxHits);
    }

    SetCursorPosition(5, 15);
    printf("                                                     ");

    SetCursorPosition(barStartX, barY + 3);
    printf("                                                                  ");

    if (hitCount == maxHits) {
        totalDamage = targetMonster.attack * 3;
        SetTextColor(FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
        printf("완벽한 공격! 모든 타이밍에 성공하여 엄청난 데미지를 입혔습니다!\n");
    }
    else if (hitCount > 0) {
        totalDamage = targetMonster.attack + (hitCount * 10);
        SetTextColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        printf("타격 성공! 총 %d번 명중하여 데미지를 입혔습니다.\n", hitCount);
    }
    else {
        totalDamage = targetMonster.attack / 4;
        SetTextColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
        printf("공격이 모두 빗나갔습니다. 아주 낮은 데미지!\n");
    }
    ResetTextColor();

    targetMonster.health -= totalDamage;
    std::this_thread::sleep_for(std::chrono::milliseconds(2500));

    return totalDamage;
}

// ====================================================================
// 메인 게임 루프
// ====================================================================

int main() {
    // 콘솔에서 한글을 올바르게 출력하기 위한 설정 (Windows 전용)
    SetConsoleOutputCP(CP_UTF8);

    int choice;
    Monster slime = { "슬라임", 50, 10 };

    do {
        DisplayMainMenu();
        std::cin >> choice;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
            SetCursorPosition(30, 14);
            printf("잘못된 입력입니다. 다시 선택해주세요.\n");
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            continue;
        }

        switch (choice) {
        case 1:
            if (slime.health <= 0) {
                ClearScreen();
                SetCursorPosition(30, 10);
                printf("이미 슬라임을 물리쳤습니다! 다음 스테이지를 준비하세요.\n");
                std::this_thread::sleep_for(std::chrono::milliseconds(2000));
                continue;
            }

            DisplayGameScreen(slime, "슬라임이 나타났다!");
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            PerformAttack(slime);

            if (slime.health <= 0) {
                ClearScreen();
                SetCursorPosition(30, 10);
                printf("슬라임을 물리쳤습니다!\n");
                std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            }
            else {
                ClearScreen();
                DisplayGameScreen(slime, "슬라임이 당신을 공격합니다!");
                std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            }
            break;
        case 2:
            ClearScreen();
            SetCursorPosition(20, 5);   printf("===========================\n");
            SetCursorPosition(20, 6);   printf("        게임 설명\n");
            SetCursorPosition(20, 7);   printf("===========================\n");
            SetCursorPosition(20, 9);   printf("몬스터와 싸워 승리하세요!\n");
            SetCursorPosition(20, 10);  printf("공격 시 타이밍에 맞춰 Spacebar를 누르면 데미지가 증가합니다.\n");
            SetCursorPosition(20, 12);  printf("아무 키나 눌러 메인 메뉴로 돌아갑니다...\n");
            _getch();
            break;
        case 3:
            ClearScreen();
            SetCursorPosition(30, 10);
            printf("게임을 종료합니다. 안녕히 계세요!\n");
            std::this_thread::sleep_for(std::chrono::milliseconds(1500));
            break;
        default:
            SetCursorPosition(30, 14);
            printf("잘못된 선택입니다. 다시 선택해주세요.\n");
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            break;
        }
    } while (choice != 3);

    return 0;
}