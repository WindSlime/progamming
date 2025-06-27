#define _CRT_SECURE_NO_WARNINGS // Visual Studio �����Ϸ� ��� ��Ȱ��ȭ
#include <stdio.h> // ǥ�� ����� �Լ�
#include <stdlib.h> // �Ϲ� ��ƿ��Ƽ �Լ� (system, rand, srand)
#include <time.h> // �ð� ���� �Լ� (time, difftime)
#include <string.h> // ���ڿ� ���� �Լ� (strlen, strcmp)

// �ü���� ��ũ�� ���� (Windows�� POSIX �ý��� ȣȯ��)
#ifdef _WIN32
#include <windows.h> // Windows API (Sleep)
#include <conio.h> // �ܼ� ����� (getch, kbhit)
#define CLEAR_SCREEN "cls" // ȭ�� ����� ��ɾ� (Windows)
#define SLEEP_MS(ms) Sleep(ms) // �и��� ���� ���
#define GET_KEY _getch // Ű �Է� �Լ� (Enter ���� ��� �Է�)
#define CHECK_KEY_HIT _kbhit // Ű �Է� ���� �Լ�
#else
#include <unistd.h> // POSIX �ý��� API (usleep)
#include <termios.h> // �͹̳� �Ӽ� ���� (non-blocking �Է�)
#include <sys/time.h> // �ð� ���� (struct timeval)

// POSIX �ý��ۿ� non-blocking �Է� �Լ�
void changemode(int dir) {
    static struct termios oldt, newt;
    if (dir == 1) { // raw ��� ����
        tcgetattr(STDIN_FILENO, &oldt); // ���� �͹̳� ���� ����
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO); // ĳ����� ���� ���� ��Ȱ��ȭ
        tcsetattr(STDIN_FILENO, TCSANOW, &newt); // �� ���� ����
    }
    else { // ���� ���� ����
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    }
}

int kbhit(void) {
    struct timeval tv;
    fd_set rdfs;
    tv.tv_sec = 0; tv.tv_usec = 0; // Ÿ�Ӿƿ� 0���� ���� (��� ��ȯ)
    FD_ZERO(&rdfs); // ���� ��ũ���� ���� �ʱ�ȭ
    FD_SET(STDIN_FILENO, &rdfs); // ǥ�� �Է�(Ű����)�� ���տ� �߰�
    select(STDIN_FILENO + 1, &rdfs, NULL, NULL, &tv); // �Է� ���
    return FD_ISSET(STDIN_FILENO, &rdfs); // �Է��� ������ 1 ��ȯ
}

#define CLEAR_SCREEN "clear" // ȭ�� ����� ��ɾ� (POSIX)
#define SLEEP_MS(ms) usleep(ms * 1000) // �и��ʸ� ����ũ���ʷ� ��ȯ�Ͽ� ���
#define GET_KEY getchar // Ű �Է� �Լ� (Enter �ʿ�, ������ kbhit�� �Բ� non-blockingó�� ����)
#define CHECK_KEY_HIT kbhit // Ű �Է� ���� �Լ�
#endif

// ANSI �̽������� �ڵ� (���� �� ��Ÿ��)
#define ANSI_COLOR_RED     "\x1b[31m" // ������ �ؽ�Ʈ
#define ANSI_COLOR_GREEN   "\x1b[32m" // �ʷϻ� �ؽ�Ʈ
#define ANSI_COLOR_YELLOW  "\x1b[33m" // ����� �ؽ�Ʈ
#define ANSI_COLOR_BLUE    "\x1b[34m" // �Ķ��� �ؽ�Ʈ
#define ANSI_COLOR_MAGENTA "\x1b[35m" // ����Ÿ�� �ؽ�Ʈ
#define ANSI_COLOR_CYAN    "\x1b[36m" // �þȻ� �ؽ�Ʈ
#define ANSI_COLOR_RESET   "\x1b[0m"  // ���� �ʱ�ȭ

// --- �Լ� ���� ---
void clearScreen();
void print_centered(const char* text, int offset, const char* color);
void draw_ui(int player_hp, int demon_hp, int focus, double remaining_time, int yokelin_active);
void generate_pattern(char* buffer, int length);
void print_pattern_with_highlight(const char* pattern, const char* player_input, int current_pos, int offset);

// --- ���� ���� ---
int main() {
    srand((unsigned int)time(NULL)); // ���� ������ �õ� �ʱ�ȭ

#ifndef _WIN32
    changemode(1); // POSIX �ý��ۿ��� �͹̳��� raw ���� ����
#endif

    // --- ���� �Ұ� �� ���丮 ��Ʈ�� ---
    clearScreen();
    print_centered("==========================================================", 0, ANSI_COLOR_YELLOW);
    print_centered("   3�� ���� �ְ� ������ �����߸� ����, ������ ���踦 ���ߴ� �̼��� ������?!", -2, ANSI_COLOR_YELLOW);
    print_centered("==========================================================", -4, ANSI_COLOR_YELLOW);
    print_centered("��õ �� ��, õ������ ������ߴ� ������ õ������ ������� ��ȭ�� ��ã������,", -7, ANSI_COLOR_RESET);
    print_centered("���� ������ �������� �ٽ� �����޽��ϴ�. ������ ��� �ӿ��� 'ȥ����'�� ����� �����", -8, ANSI_COLOR_RESET);
    print_centered("õ���� ������ ��Ȱ�� �����ÿ� Ȧ�� �����մϴ�.", -9, ANSI_COLOR_RESET);
    print_centered("���� �� 3�� �ȿ� õ���� �����߸��� ���ϸ� ������ ������ ��ҿ� ��� ���Դϴ�.", -10, ANSI_COLOR_RESET);
    print_centered("�η��� ���ſ� �̷��� �� ������ 180�ʿ� �����մϴ�.", -11, ANSI_COLOR_RESET);
    print_centered(" ", -12, ANSI_COLOR_RESET); // �� �� ����
    print_centered("õ���� ���� '����'�� ���� ��Ȯ�� Ű(A, S, W, D)�� �޾�ġ�ʽÿ�.", -14, ANSI_COLOR_CYAN);
    print_centered("�޺��� �׾� '���߷�'�� ������, �ʻ��(F) 'ȥ����'���� ġ��Ÿ�� ��������!", -15, ANSI_COLOR_CYAN);
    print_centered("����, Q �Ǵ� E�� ���� '�����˰�'�� �ߵ��Ͽ� ��� ���ݷ��� ��ȭ�� �� �ֽ��ϴ�.", -16, ANSI_COLOR_CYAN);
    print_centered("�ƹ� Ű�� ���� ����...", -19, ANSI_COLOR_GREEN);
    (void)GET_KEY(); // ����� �Է� ���

    // --- ���� ���� �ʱ�ȭ ---
    int player_hp = 100; // �÷��̾� ü��
    int demon_hp = 900; // õ�� ü��
    int focus = 0; // ���߷� (�ʻ�� ������)
    int combo = 0; // �޺� ī��Ʈ

    char pattern[10] = ""; // õ���� ���� ���� (��: "aswd")
    char player_input[10] = ""; // �÷��̾��� ���� �Է�
    int pattern_len = 3; // �ʱ� ���� ����
    int current_input_pos = 0; // ���� �Է��ؾ� �� ���� ��ġ

    int needs_new_pattern = 1; // �� ���� ���� �ʿ� ���� �÷���

    time_t start_time = time(NULL); // ���� ���� �ð�
    const int TIME_LIMIT = 180; // �ð� ���� (��)
    double elapsed_time = 0; // ��� �ð�

    // �����˰� ���� ����
    int yokelin_active = 0; // �����˰� Ȱ��ȭ ����
    time_t yokelin_duration_start_time = 0; // �����˰� ���� �ð�
    const int YOKELIN_DURATION = 5; // �����˰� ���� �ð� (��)
    const int YOKELIN_FOCUS_COST = 20; // �����˰� ���߷� �Ҹ�
    const double YOKELIN_DAMAGE_MULTIPLIER = 1.5; // �����˰� ���ݷ� ����

    // --- ���� ���� ���� ---
    while (player_hp > 0 && demon_hp > 0) {
        elapsed_time = difftime(time(NULL), start_time); // ��� �ð� ���
        if (elapsed_time >= TIME_LIMIT) break; // �ð� �ʰ� �� ���� ����

        // �����˰� ���� �ð� Ȯ�� �� ��Ȱ��ȭ
        if (yokelin_active && difftime(time(NULL), yokelin_duration_start_time) >= YOKELIN_DURATION) {
            yokelin_active = 0;
            print_centered("�����˰� ȿ���� �������ϴ�.", 10, ANSI_COLOR_BLUE);
            SLEEP_MS(800);
        }

        if (needs_new_pattern) {
            generate_pattern(pattern, pattern_len); // �� ���� ����
            current_input_pos = 0; // �Է� ��ġ �ʱ�ȭ
            player_input[0] = '\0'; // �÷��̾� �Է� ���� �ʱ�ȭ
            needs_new_pattern = 0; // �� ���� ���� �÷��� ��Ȱ��ȭ
        }

        clearScreen(); // ȭ�� �����
        draw_ui(player_hp, demon_hp, focus, TIME_LIMIT - elapsed_time, yokelin_active); // UI �׸���

        print_centered("õ���� ���� ����:", 2, ANSI_COLOR_RESET);
        print_pattern_with_highlight(pattern, player_input, current_input_pos, 3); // ���̶���Ʈ�� ���� ���
        print_centered(" ", 4, ANSI_COLOR_RESET); // ����
        print_centered("����� �Է�:", 5, ANSI_COLOR_RESET);
        print_centered(player_input, 6, ANSI_COLOR_RESET); // �÷��̾� �Է� ���

        // ���� �ð� ���
        if (TIME_LIMIT - elapsed_time <= 30 && TIME_LIMIT - elapsed_time > 0) {
            if ((int)(TIME_LIMIT - elapsed_time) % 5 == 0) { // 5�ʸ��� ���
                print_centered("!! �ð��� �� ���� �ʾҽ��ϴ� !!", 8, ANSI_COLOR_RED);
            }
        }

        // Ű �Է� ����
        if (CHECK_KEY_HIT()) {
            char key = GET_KEY(); // Ű �Է� �ޱ�

            // �ʻ�� 'ȥ����' ��� (F Ű)
            if ((key == 'f' || key == 'F') && focus >= 100) {
                int damage = 250;
                if (yokelin_active) { // �����˰� Ȱ��ȭ �� �߰� ����
                    damage = (int)(damage * YOKELIN_DAMAGE_MULTIPLIER);
                    print_centered("ȥ���� + �����˰�! �ش� ����!", 8, ANSI_COLOR_MAGENTA);
                }
                else {
                    print_centered("�ʻ��� �ϰ�! ȥ����!", 8, ANSI_COLOR_MAGENTA);
                }
                demon_hp -= damage;
                focus = 0; // ���߷� �Ҹ�
                combo = 0; // �޺� �ʱ�ȭ
                SLEEP_MS(1500); // ����ڿ��� �޽��� Ȯ���� �ð� �ֱ�
                needs_new_pattern = 1; // �� ���� �ʿ�
                continue; // ���� ���� ���� (���� �Է� ó�� �ǳʶٱ�)
            }

            // ��� '�����˰�' ��� (Q �Ǵ� E Ű)
            if ((key == 'q' || key == 'Q' || key == 'e' || key == 'E') && focus >= YOKELIN_FOCUS_COST && !yokelin_active) {
                focus -= YOKELIN_FOCUS_COST; // ���߷� �Ҹ�
                yokelin_active = 1; // �����˰� Ȱ��ȭ
                yokelin_duration_start_time = time(NULL); // ���� �ð� ���� ����
                print_centered("�����˰� �ߵ�! ���ݷ��� ����մϴ�!", 8, ANSI_COLOR_BLUE);
                SLEEP_MS(1200);
                // �����˰��� ���� �Է°� �����̹Ƿ� ���� ������ ����
                continue;
            }

            // �Ϲ� ���� �Է� ó��
            if (current_input_pos < pattern_len) {
                player_input[current_input_pos] = key; // �÷��̾� �Է� ����
                player_input[current_input_pos + 1] = '\0'; // �� ���� ���� �߰�
            }

            // �Է��� ���ϰ� ��ġ���� ���� ��
            if (key != pattern[current_input_pos]) {
                player_hp -= 20; // �÷��̾� ü�� ����
                focus = (focus > 10) ? focus - 10 : 0; // ���߷� ���� (�ּ� 0)
                combo = 0; // �޺� �ʱ�ȭS
                print_centered("������ ��Ʈ������! õ���� �ݰ�!", 8, ANSI_COLOR_RED);
                SLEEP_MS(1000); // ��� ����Ͽ� �޽��� Ȯ��
                needs_new_pattern = 1; // �� ���� �ʿ�
            }
            // �Է��� ���ϰ� ��ġ�� ��
            else {
                current_input_pos++; // ���� �Է� ��ġ�� �̵�

                // ������ ��� ���������� �Է����� ��
                if (current_input_pos == pattern_len) {
                    int damage = 20 + (combo * 2); // �⺻ ���ط� + �޺� ���ʽ�
                    if (yokelin_active) { // �����˰� Ȱ��ȭ �� ���ط� ����
                        damage = (int)(damage * YOKELIN_DAMAGE_MULTIPLIER);
                    }
                    demon_hp -= damage; // õ�� ü�� ����
                    focus += 15; // ���߷� ȹ�� (���� 15����. 20 -> 15�� �ٽ� ����)
                    if (focus > 100) focus = 100; // ���߷� �ִ�ġ ����
                    combo++; // �޺� ����

                    print_centered("������ �޾��ƴ�! �������� ����!", 8, ANSI_COLOR_GREEN);
                    SLEEP_MS(500); // ��� ���

                    // �޺��� ���� ���� ���� ���� (�ִ� 7�� ����)
                    if (combo > 0 && combo % 7 == 0 && pattern_len < 7) { // 7�޺����� ���� ����
                        pattern_len++;
                        print_centered("õ���� ������ ���� ���������ϴ�!", 9, ANSI_COLOR_YELLOW);
                        SLEEP_MS(800);
                    }
                    needs_new_pattern = 1; // �� ���� �ʿ�
                }
            }
        }

        SLEEP_MS(33); // ���� ���� (�� 30 FPS)
    }

    // --- ���� ��� ȭ�� ---
    clearScreen(); // ȭ�� �����
    draw_ui(player_hp, demon_hp, focus, 0, yokelin_active); // ���� UI �׸��� (���� �ð� 0)

    if (demon_hp <= 0) { // õ�� ���� ����
        print_centered("*****************************************************************", -2, ANSI_COLOR_YELLOW);
        print_centered("* õ�� ����! �η��� �����߽��ϴ�! ���� ��ȭ�� ã�ƿɴϴ�!     *", -3, ANSI_COLOR_GREEN);
        print_centered("*****************************************************************", -4, ANSI_COLOR_YELLOW);
        print_centered("����� �η��� ���ſ� �̷��� ��Ų ������ �����Դϴ�.", -6, ANSI_COLOR_RESET);
        print_centered("2�� 58��, ������ ���۵˴ϴ�...", -7, ANSI_COLOR_RESET);
    }
    else if (player_hp <= 0) { // �÷��̾� ü�� ����
        print_centered("*****************************************************************", -2, ANSI_COLOR_RED);
        print_centered("* ����� ���������ϴ�... ��� ���� ��ҿ� �����ϴ�...         *", -3, ANSI_COLOR_RED);
        print_centered("*****************************************************************", -4, ANSI_COLOR_RED);
        print_centered("�η��� ������ ����� �������, ������ õ���� �տ� �Ѿ���ϴ�.", -6, ANSI_COLOR_RESET);
    }
    else { // �ð� �ʰ�
        print_centered("*****************************************************************", -2, ANSI_COLOR_RED);
        print_centered("* �ð� �ʰ�... ������ ������ ��ҿ� �����ϴ�...           *", -3, ANSI_COLOR_RED);
        print_centered("*****************************************************************", -4, ANSI_COLOR_RED);
        print_centered("������ 180�ʴ� ��������, õ���� �׸��ڰ� ������ �ڵ������ϴ�.", -6, ANSI_COLOR_RESET);
    }
    print_centered("--- GAME OVER ---", -9, ANSI_COLOR_YELLOW);
    SLEEP_MS(5000); // ���� �޽��� Ȯ���� ���� 5�� ���

#ifndef _WIN32
    changemode(0); // POSIX �ý��ۿ��� �͹̳� ��� ������� ����
#endif
    return 0; // ���α׷� ����
}


// --- ��ƿ��Ƽ �Լ� ���� ---

// ȭ���� ����� �Լ�
void clearScreen() {
    system(CLEAR_SCREEN);
}

// �ؽ�Ʈ�� �߾� �����Ͽ� ����ϴ� �Լ�
// offset: ����� �ؽ�Ʈ ���� �� �� �߰�, ������ �ؽ�Ʈ �Ʒ��� �� �� �߰�
// color: ANSI ���� �ڵ� ���ڿ� (��: ANSI_COLOR_RED)
void print_centered(const char* text, int offset, const char* color) {
    int len = strlen(text);
    // �͹̳� �ʺ� 80���� �����ϰ� �߾� ���� �е� ���
    int pad = (80 - len) / 2;
    if (pad < 0) pad = 0; // �ؽ�Ʈ�� 80�ں��� ��� �е� ����

    // offset�� ������ ��� (�ؽ�Ʈ �Ʒ��� �� �� �߰�)
    if (offset < 0) {
        for (int i = 0; i < abs(offset); i++) printf("\n");
    }

    // �е� �� ���� ����� �ؽ�Ʈ ���
    for (int i = 0; i < pad; i++) printf(" ");
    printf("%s%s%s", color, text, ANSI_COLOR_RESET); // ���� ���� �� �ʱ�ȭ

    // offset�� ����� ��� (�ؽ�Ʈ ���� �� �� �߰�) �Ǵ� �Ϲ����� ���
    if (offset >= 0) printf("\n");
}

// ���� UI�� �׸��� �Լ�
void draw_ui(int player_hp, int demon_hp, int focus, double remaining_time, int yokelin_active) {
    // �÷��̾� HP ��
    printf(" ��� HP    : " ANSI_COLOR_GREEN "[");
    for (int i = 0; i < 20; i++) {
        if (i < (player_hp / 5)) printf("#"); else printf("-");
    }
    printf("]" ANSI_COLOR_RESET " %d/100\n", player_hp);

    // õ�� HP ��
    printf(" õ�� HP    : " ANSI_COLOR_RED "[");
    for (int i = 0; i < 20; i++) {
        if (i < (demon_hp / 45)) printf("#"); else printf("-"); // 900 / 45 = 20
    }
    printf("]" ANSI_COLOR_RESET " %d/900\n", demon_hp);

    // ���߷� �� (�����˰� Ȱ��ȭ �� �Ķ������� ǥ��)
    printf(" ���߷�(F)%s : ", yokelin_active ? ANSI_COLOR_BLUE "(�����˰� ON)" ANSI_COLOR_RESET : "");
    printf(ANSI_COLOR_CYAN "[");
    for (int i = 0; i < 20; i++) {
        if (i < (focus / 5)) printf("+"); else printf(" ");
    }
    printf("]" ANSI_COLOR_RESET " %d/100\n", focus);

    printf("================================================================================\n");
    // ���� �ð� ǥ��
    if (remaining_time <= 30) {
        printf("���� �ð�: " ANSI_COLOR_RED "%.0f ��" ANSI_COLOR_RESET "\n", remaining_time);
    }
    else {
        printf("���� �ð�: %.0f ��\n", remaining_time);
    }
    printf("================================================================================\n");
}

// ������ ���� ������ �����ϴ� �Լ�
void generate_pattern(char* buffer, int length) {
    const char* keys = "aswd"; // ��� ������ Ű ���
    for (int i = 0; i < length; i++) {
        buffer[i] = keys[rand() % 4]; // �������� Ű ����
    }
    buffer[length] = '\0'; // ���ڿ� ���� �� ���� ���� �߰�
}

// ������ ����ϵ�, �÷��̾��� �Է¿� ���� ���̶���Ʈ�� �����ϴ� �Լ�
void print_pattern_with_highlight(const char* pattern, const char* player_input, int current_pos, int offset) {
    int len = strlen(pattern);
    int pad = (80 - len) / 2;
    if (pad < 0) pad = 0;

    for (int i = 0; i < abs(offset); i++) printf("\n");

    for (int i = 0; i < pad; i++) printf(" ");

    // ������ �� ���ھ� ����ϸ鼭 ���̶���Ʈ ����
    for (int i = 0; i < len; i++) {
        if (i < current_pos) {
            // �̹� �ùٸ��� �Էµ� �κ� (���)
            printf(ANSI_COLOR_GREEN "%c" ANSI_COLOR_RESET, pattern[i]);
        }
        else if (i == current_pos) {
            // ���� �Է��ؾ� �� �κ� (�����)
            printf(ANSI_COLOR_YELLOW "%c" ANSI_COLOR_RESET, pattern[i]);
        }
        else {
            // ���� �Է����� ���� �κ� (�⺻ ����)
            printf("%c", pattern[i]);
        }
    }
    printf("\n");
}