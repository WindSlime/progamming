#include <iostream>
#include <windows.h> // for Sleep and system("color"), SetConsoleTextAttribute, GetStdHandle
#include <conio.h>   // for _kbhit and _getch
#include <string>
#include <chrono>    // for high-resolution timer
#include <random>    // for random number generation
#include <limits>    // for numeric_limits (input buffer clear)

// �ܼ� �ؽ�Ʈ ���� ���� �Լ�
void SetConsoleColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

// ====================================================================
// ���� ���� �Լ���
// ====================================================================

// ���� �÷��� �Լ�
void PlayGame(const std::string& gameTitle) {
    SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); // �⺻ �������� ����
    system("cls"); // ȭ�� Ŭ����

    std::cout << "========================================" << std::endl;
    SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN); // ���� ���
    std::cout << "          " << gameTitle << " - ����!        " << std::endl;
    SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); // ���� ��
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;

    std::cout << "�����̽��ٸ� ���� Ÿ�ֿ̹� ���� �����ϼ���!" << std::endl;
    std::cout << "�غ�Ǹ� �ƹ� Ű�� ��������..." << std::endl;
    _getch(); // �ƹ� Ű�� �Է¹ޱ� ������ ���

    int score = 0;
    int rounds = 5; // �� ���� ��

    std::cout << "\n���� ����!\n" << std::endl;
    Sleep(1000); // 1�� ���

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(1000, 2000); // �� ���� ���� (1�� ~ 2��)

    for (int i = 0; i < rounds; ++i) {
        std::cout << "----------------------------------------" << std::endl;
        std::cout << "���� " << i + 1 << std::endl;

        Sleep(distrib(gen)); // �ٰ� �����ϱ� �� ���� ��� �ð�

        SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_BLUE); // ���� �Ķ�
        std::cout << ">>>>> [     ���� �غ�!     ] <<<<<" << std::endl;
        SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); // ���� ��

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
                std::cout << "����! ���� ����!" << std::endl;
                score += (successRange + 1 - distance) * 10;
            }
            else {
                SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN); // ���� ���
                std::cout << "�ƽ����� ���� ����!" << std::endl;
            }
        }
        else {
            SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_RED);
            std::cout << "�ð� �ʰ�! ���� ����!" << std::endl;
        }
        SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

        std::cout << "���� ����: " << score << std::endl;
        Sleep(1500);
    }

    std::cout << "\n========================================" << std::endl;
    SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
    std::cout << "          ���� ����!          " << std::endl;
    std::cout << "          ���� ����: " << score << std::endl;
    SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    std::cout << "========================================" << std::endl;

    std::cout << "���� ȭ������ ���ư����� �ƹ� Ű�� ��������..." << std::endl;
    _getch();
}

// ====================================================================
// ���� �Լ� (���� �޴� ����)
// ====================================================================

int main() {
    // 1. �ܼ� �ڵ� �������� UTF-8�� ���� �õ� (�ý��� ��� ���)
    // �� ���� `SetConsoleOutputCP(CP_UTF8);`�� �Բ� ���� �� ȿ�����Դϴ�.
    // "> nul"�� chcp ����� ����� ����ϴ�.
    system("chcp 65001 > nul");

    // 2. C++ ���α׷��� ��� �ڵ� �������� UTF-8�� ����
    // �ѱ� ����� ���� �ʿ��մϴ�.
    SetConsoleOutputCP(CP_UTF8);

    // ���� ���� ����
    std::string gameTitle = "����� ���� �ڵ�";

    // �ܼ� �⺻ ���� ���� (������ ���ڻ�)
    std::string backgroundColorCode = "0"; // ���� ���
    std::string textColorCode = "F";     // ���� ��� ����
    std::string colorCommand = "color " + backgroundColorCode + textColorCode;
    system(colorCommand.c_str());

    int choice = 0;

    while (true) {
        system("cls"); // ȭ�� Ŭ����

        SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); // �⺻ �������� ����
        std::cout << "========================================" << std::endl;
        SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN); // ���� ���
        std::cout << "          " << gameTitle << "          " << std::endl;
        SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); // ���� ��
        std::cout << "========================================" << std::endl;
        std::cout << std::endl;

        std::cout << "1. ���� ����" << std::endl;
        std::cout << "2. ���� ����" << std::endl;
        std::cout << std::endl;
        std::cout << "����: ";

        // ����� �Է� �ޱ�
        std::cin >> choice;

        // �Է� ���� ���� (�߸��� �Է� ó��)
        // ���ڰ� �ƴ� �ٸ� ���ڸ� �Է����� �� ���� ������ ������ ���� ����
        if (std::cin.fail()) {
            std::cin.clear(); // ���� �÷��� �ʱ�ȭ
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // �Է� ���� ����
            std::cout << "�߸��� �Է��Դϴ�. ���ڸ� �Է����ּ���." << std::endl;
            Sleep(1000);
            continue; // ������ ó������ ���ư�
        }

        switch (choice) {
        case 1:
            PlayGame(gameTitle); // ���� �÷��� �Լ� ȣ��
            break;
        case 2:
            SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_