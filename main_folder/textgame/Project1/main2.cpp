#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include <stdlib.h>

// �ܼ� ���� ����
enum {
    BLACK, D_BLUE, D_GREEN, D_SKYBLUE, D_RED, D_VIOLET, D_YELLOW,
    GRAY, D_GRAY, BLUE, GREEN, SKYBLUE, RED, VIOLET, YELLOW, WHITE
};

// �Լ� ����
void gotoxy(int x, int y);
void setColor(int color);
void setup();
void title();
void intro();
void stage1();
void printDialogue(const char* lines[], int lineCount, int startX, int startY);
void waitForEnter();

// ���� �Լ�
int main() {
    setup();
    title();
    intro();
    stage1();
    return 0;
}

// Ŀ�� ��ġ �̵�
void gotoxy(int x, int y) {
    COORD pos = { x, y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

// �ܼ� �ؽ�Ʈ ���� ����
void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

// �ܼ� ȯ�� ����
void setup() {
    srand((unsigned int)time(NULL));

    system("title Main");
    system("mode con: cols=120 lines=35");
    system("cls");
}

// Ÿ��Ʋ ȭ�� ���
void title() {
    setColor(RED); gotoxy(40, 10); printf("==============================================");
    setColor(WHITE); gotoxy(50, 12); printf("���� ����");
    setColor(D_YELLOW); gotoxy(50, 13); printf("����");
    setColor(RED); gotoxy(40, 15); printf("==============================================");
    gotoxy(50, 17); setColor(WHITE); printf("�ƹ� Ű�� ���� ����");
    _getch();
    system("cls");
}

// ��� �迭 ��� �Լ� (���1, ���2 ����)
void printDialogue(const char* lines[], int lineCount, int startX, int startY) {
    for (int i = 0; i < lineCount; i++) {
        system("cls");
        gotoxy(startX, startY);
        setColor(WHITE);
        printf("���%d: %s", i + 1, lines[i]);

        gotoxy(startX, startY + 2);
        setColor(D_GRAY);
        printf("���� ��縦 ������ [Enter] Ű�� ��������");
        while (1) {
            if (_kbhit()) {
                char key = _getch();
                if (key == 13) break; // Enter Ű
            }
        }
    }
}

// ��Ʈ�� ȭ��
void intro() {
    const char* introLines[] = {
        "���",
        "���",
        "���",
        "���"
    };
    int numLines = sizeof(introLines) / sizeof(introLines[0]);
    printDialogue(introLines, numLines, 5, 5);
}

// �������� 1 : �� �Ա�
void stage1() {
    system("cls");
    const char* stageLines[] = {
        "���",
        "���",
        "���"
    };
    int lineCount = sizeof(stageLines) / sizeof(stageLines[0]);
    printDialogue(stageLines, lineCount, 5, 5);

    gotoxy(5, 10); setColor(WHITE); printf("1. ������ 2");
    gotoxy(5, 11); printf("2. ������ 1");
    gotoxy(5, 13); printf("������ ���� ");

    char choice;
    while (1) {
        choice = _getch();
        if (choice == '1') {
            system("cls");
            gotoxy(5, 5); printf("������ 1 ���");
            break;
        }
        else if (choice == '2') {
            system("cls");
            gotoxy(5, 5); printf("������ 2 ���");
            break;
        }
    }

    waitForEnter();
    system("cls");
    gotoxy(5, 5); printf("�������� 2");
}

// Enter Ű ��� �Լ�
void waitForEnter() {
    gotoxy(5, 7); setColor(D_GRAY);
    printf("����Ϸ��� [Enter] Ű�� ��������");
    while (1) {
        if (_kbhit()) {
            char key = _getch();
            if (key == 13) break;
        }
    }
}
