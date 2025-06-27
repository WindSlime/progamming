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
    void stage2();
    void waitForEnter();
    void printDialogueWithNPC(const char* speaker[], const char* lines[], int lineCount, int startX, int startY);

    // ���� �Լ�
    int main() {
        setup();
        title();
        intro();
        stage1();
        stage2();
        return 0;
    }

    // Ŀ�� �̵�
    void gotoxy(int x, int y) {
        COORD pos = { x, y };
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
    }

    // ���� ����
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

    // Ÿ��Ʋ ���
    void title() {
        setColor(RED); gotoxy(40, 10); printf("==============================================");
        setColor(WHITE); gotoxy(50, 12); printf("���� ����");
        setColor(D_YELLOW); gotoxy(50, 13); printf("����");
        setColor(RED); gotoxy(40, 15); printf("==============================================");
        gotoxy(50, 17); setColor(WHITE); printf("�ƹ� Ű�� ���� ����");
        _getch();
        system("cls");
    }

// ��� ��� �Լ� - NPC ����
void printDialogueWithNPC(const char* speaker[], const char* lines[], int lineCount, int startX, int startY) {
    for (int i = 0; i < lineCount; i++) {
        system("cls");
        gotoxy(startX, startY);
        setColor(WHITE);
        printf("[%s] : %s", speaker[i], lines[i]);

        gotoxy(startX, startY + 2);
        setColor(D_GRAY);
        printf("���� ��縦 ������ [Enter] Ű�� ��������");

        while (1) {
            if (_kbhit()) {
                char key = _getch();
                if (key == 13) break;
            }
        }
    }
}

// ��Ʈ�� ȭ��
void intro() {
    const char* npcNames[] = {
        "�����̼�", "�����̼�", "�����̼�", "�����̼�"
    };
    const char* introLines[] = {
        "���1",
        "���2",
        "���3",
        "���4"
    };
    int numLines = sizeof(introLines) / sizeof(introLines[0]);
    printDialogueWithNPC(npcNames, introLines, numLines, 5, 5);
}

// ���� Ű ���
void waitForEnter() {
    gotoxy(5, 25); setColor(D_GRAY);
    printf("����Ϸ��� [Enter] Ű�� ��������");
    while (1) {
        if (_kbhit()) {
            char key = _getch();
            if (key == 13) break;
        }
    }
}

// �������� 1 ����
void stage1() {
    const char* npcNames[] = {
        "�ǽ� ���� �ֹ�", "�÷��̾�", "�ǽ� ���� �ֹ�"
    };
    const char* stageLines[] = {
        "���1(�ֹ�)",
        "���2(�÷��̾�)",
        "���3(�ֹ�)"
    };
    int numLines = sizeof(stageLines) / sizeof(stageLines[0]);
    printDialogueWithNPC(npcNames, stageLines, numLines, 5, 5);

    gotoxy(5, 10); setColor(WHITE); printf("1. ������1");
    gotoxy(5, 11); printf("2. ������2");
    gotoxy(5, 13); printf("���� : ");

    char choice;
    while (1) {
        choice = _getch();
        if (choice == '1') {
            system("cls");
            gotoxy(5, 5); printf("������1 ���");
            break;
        }
        else if (choice == '2') {
            system("cls");
            gotoxy(5, 5); printf("������ 2 ���");
            break;
        }
    }
}

    // �������� 2 �� ����
    void stage2() {
        const char* npcNames[] = {
            "NPC", "NPC", "NPC"
        };
        const char* stageLines[] = {
            "���1(NPC)",
            "���2(NPC)",
            "���3(NPC)"
        };
        int numLines = sizeof(stageLines) / sizeof(stageLines[0]);
        printDialogueWithNPC(npcNames, stageLines, numLines, 5, 5);

        gotoxy(5, 10); setColor(WHITE); printf("1. ������1");
        gotoxy(5, 11); printf("2. ������2");
        gotoxy(5, 13); printf("���� : ");
    };

    /*waitForEnter();
    system("cls");
    gotoxy(5, 5); printf("To be continued... (Stage 2 �غ� ��)");*/

