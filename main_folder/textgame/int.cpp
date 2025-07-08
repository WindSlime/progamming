#include <iostream>
#include <windows.h> // for Sleep and system("color"), SetConsoleTextAttribute, GetStdHandle
#include <conio.h>   // for _kbhit and _getch
#include <string>
#include <chrono>    // for high-resolution timer
#include <random>    // for random number generation
#include <limits>    // for numeric_limits (input buffer clear)

// 콘솔 텍스트 색상 설정 함수
void SetConsoleColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

// ====================================================================
// 게임 로직 함수들
// ====================================================================

// 게임 플레이 함수
void PlayGame(const std::string& gameTitle) {
    SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); // 기본 색상으로 복원
    system("cls"); // 화면 클리어

    std::cout << "========================================" << std::endl;
    SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN); // 밝은 노랑
    std::cout << "          " << gameTitle << " - 전투!        " << std::endl;
    SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); // 원래 색
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;

    std::cout << "스페이스바를 눌러 타이밍에 맞춰 공격하세요!" << std::endl;
    std::cout << "준비되면 아무 키나 누르세요..." << std::endl;
    _getch(); // 아무 키나 입력받기 전까지 대기

    int score = 0;
    int rounds = 5; // 총 라운드 수

    std::cout << "\n게임 시작!\n" << std::endl;
    Sleep(1000); // 1초 대기

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(1000, 2000); // 바 등장 간격 (1초 ~ 2초)

    for (int i = 0; i < rounds; ++i) {
        std::cout << "----------------------------------------" << std::endl;
        std::cout << "라운드 " << i + 1 << std::endl;

        Sleep(distrib(gen)); // 바가 등장하기 전 랜덤 대기 시간

        SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_BLUE); // 밝은 파랑
        std::cout << ">>>>> [     공격 준비!     ] <<<<<" << std::endl;
        SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); // 원래 색

        const int barLength = 40;
        const int targetPos = barLength / 2;
        const int successRange = 2;

        auto startTime = std::chrono::high_resolution_clock::now();
        bool hit = false;
        int hitPosition = -1;

        for (int j = 0; j <= barLength; ++j) {
            if (_kbhit()) {
                char key = _getch();
                if (key == ' ') {
                    hit = true;
                    hitPosition = j;
                    break;
                }
            }

            std::cout << "\r[";
            for (int k = 0; k < barLength; ++k) {
                if (k == j) {
                    SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_RED);
                    std::cout << "?";
                    SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                }
                else if (k >= targetPos - successRange && k <= targetPos + successRange) {
                    SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_GREEN);
                    std::cout << "=";
                    SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                }
                else {
                    std::cout << "-";
                }
            }
            std::cout << "]";
            Sleep(50);
        }

        std::cout << std::endl;

        if (hit) {
            int distance = abs(hitPosition - targetPos);
            if (distance <= successRange) {
                SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_GREEN);
                std::cout << "성공! 멋진 공격!" << std::endl;
                score += (successRange + 1 - distance) * 10;
            }
            else {
                SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN); // 밝은 노랑
                std::cout << "아쉽지만 공격 실패!" << std::endl;
            }
        }
        else {
            SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_RED);
            std::cout << "시간 초과! 공격 실패!" << std::endl;
        }
        SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

        std::cout << "현재 점수: " << score << std::endl;
        Sleep(1500);
    }

    std::cout << "\n========================================" << std::endl;
    SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
    std::cout << "          게임 종료!          " << std::endl;
    std::cout << "          최종 점수: " << score << std::endl;
    SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    std::cout << "========================================" << std::endl;

    std::cout << "메인 화면으로 돌아가려면 아무 키나 누르세요..." << std::endl;
    _getch();
}

// ====================================================================
// 메인 함수 (메인 메뉴 로직)
// ====================================================================

int main() {
    // 1. 콘솔 코드 페이지를 UTF-8로 변경 시도 (시스템 명령 사용)
    // 이 줄은 `SetConsoleOutputCP(CP_UTF8);`과 함께 사용될 때 효과적입니다.
    // "> nul"은 chcp 명령의 출력을 숨깁니다.
    system("chcp 65001 > nul");

    // 2. C++ 프로그램의 출력 코드 페이지를 UTF-8로 설정
    // 한글 출력을 위해 필요합니다.
    SetConsoleOutputCP(CP_UTF8);

    // 게임 제목 설정
    std::string gameTitle = "용사의 심장 박동";

    // 콘솔 기본 색상 설정 (배경색과 글자색)
    std::string backgroundColorCode = "0"; // 검정 배경
    std::string textColorCode = "F";     // 밝은 흰색 글자
    std::string colorCommand = "color " + backgroundColorCode + textColorCode;
    system(colorCommand.c_str());

    int choice = 0;

    while (true) {
        system("cls"); // 화면 클리어

        SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); // 기본 색상으로 복원
        std::cout << "========================================" << std::endl;
        SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN); // 밝은 노랑
        std::cout << "          " << gameTitle << "          " << std::endl;
        SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); // 원래 색
        std::cout << "========================================" << std::endl;
        std::cout << std::endl;

        std::cout << "1. 게임 시작" << std::endl;
        std::cout << "2. 게임 종료" << std::endl;
        std::cout << std::endl;
        std::cout << "선택: ";

        // 사용자 입력 받기
        std::cin >> choice;

        // 입력 버퍼 비우기 (잘못된 입력 처리)
        // 숫자가 아닌 다른 문자를 입력했을 때 무한 루프에 빠지는 것을 방지
        if (std::cin.fail()) {
            std::cin.clear(); // 오류 플래그 초기화
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // 입력 버퍼 비우기
            std::cout << "잘못된 입력입니다. 숫자를 입력해주세요." << std::endl;
            Sleep(1000);
            continue; // 루프의 처음으로 돌아감
        }

        switch (choice) {
        case 1:
            PlayGame(gameTitle); // 게임 플레이 함수 호출
            break;
        case 2:
            SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_