#define _CRT_SECURE_NO_WARNINGS // Visual Studio 컴파일러 경고 비활성화
#include <stdio.h> // 표준 입출력 함수
#include <stdlib.h> // 일반 유틸리티 함수 (system, rand, srand)
#include <time.h> // 시간 관련 함수 (time, difftime)
#include <string.h> // 문자열 관련 함수 (strlen, strcmp)

// 운영체제별 매크로 정의 (Windows와 POSIX 시스템 호환성)
#ifdef _WIN32
#include <windows.h> // Windows API (Sleep)
#include <conio.h> // 콘솔 입출력 (getch, kbhit)
#define CLEAR_SCREEN "cls" // 화면 지우기 명령어 (Windows)
#define SLEEP_MS(ms) Sleep(ms) // 밀리초 단위 대기
#define GET_KEY _getch // 키 입력 함수 (Enter 없이 즉시 입력)
#define CHECK_KEY_HIT _kbhit // 키 입력 감지 함수
#else
#include <unistd.h> // POSIX 시스템 API (usleep)
#include <termios.h> // 터미널 속성 제어 (non-blocking 입력)
#include <sys/time.h> // 시간 관련 (struct timeval)

// POSIX 시스템용 non-blocking 입력 함수
void changemode(int dir) {
    static struct termios oldt, newt;
    if (dir == 1) { // raw 모드 설정
        tcgetattr(STDIN_FILENO, &oldt); // 현재 터미널 설정 저장
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO); // 캐노니컬 모드와 에코 비활성화
        tcsetattr(STDIN_FILENO, TCSANOW, &newt); // 새 설정 적용
    }
    else { // 원래 모드로 복원
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    }
}

int kbhit(void) {
    struct timeval tv;
    fd_set rdfs;
    tv.tv_sec = 0; tv.tv_usec = 0; // 타임아웃 0으로 설정 (즉시 반환)
    FD_ZERO(&rdfs); // 파일 디스크립터 집합 초기화
    FD_SET(STDIN_FILENO, &rdfs); // 표준 입력(키보드)을 집합에 추가
    select(STDIN_FILENO + 1, &rdfs, NULL, NULL, &tv); // 입력 대기
    return FD_ISSET(STDIN_FILENO, &rdfs); // 입력이 있으면 1 반환
}

#define CLEAR_SCREEN "clear" // 화면 지우기 명령어 (POSIX)
#define SLEEP_MS(ms) usleep(ms * 1000) // 밀리초를 마이크로초로 변환하여 대기
#define GET_KEY getchar // 키 입력 함수 (Enter 필요, 하지만 kbhit과 함께 non-blocking처럼 동작)
#define CHECK_KEY_HIT kbhit // 키 입력 감지 함수
#endif

// ANSI 이스케이프 코드 (색상 및 스타일)
#define ANSI_COLOR_RED     "\x1b[31m" // 빨간색 텍스트
#define ANSI_COLOR_GREEN   "\x1b[32m" // 초록색 텍스트
#define ANSI_COLOR_YELLOW  "\x1b[33m" // 노란색 텍스트
#define ANSI_COLOR_BLUE    "\x1b[34m" // 파란색 텍스트
#define ANSI_COLOR_MAGENTA "\x1b[35m" // 마젠타색 텍스트
#define ANSI_COLOR_CYAN    "\x1b[36m" // 시안색 텍스트
#define ANSI_COLOR_RESET   "\x1b[0m"  // 색상 초기화

// --- 함수 선언 ---
void clearScreen();
void print_centered(const char* text, int offset, const char* color);
void draw_ui(int player_hp, int demon_hp, int focus, double remaining_time, int yokelin_active);
void generate_pattern(char* buffer, int length);
void print_pattern_with_highlight(const char* pattern, const char* player_input, int current_pos, int offset);

// --- 메인 로직 ---
int main() {
    srand((unsigned int)time(NULL)); // 난수 생성기 시드 초기화

#ifndef _WIN32
    changemode(1); // POSIX 시스템에서 터미널을 raw 모드로 설정
#endif

    // --- 게임 소개 및 스토리 인트로 ---
    clearScreen();
    print_centered("==========================================================", 0, ANSI_COLOR_YELLOW);
    print_centered("   3분 만에 최강 마왕을 쓰러뜨린 내가, 전생에 세계를 구했던 이세계 용사라고요?!", -2, ANSI_COLOR_YELLOW);
    print_centered("==========================================================", -4, ANSI_COLOR_YELLOW);
    print_centered("수천 년 전, 천마에게 지배당했던 세상은 천공인의 희생으로 평화를 되찾았으나,", -7, ANSI_COLOR_RESET);
    print_centered("그의 봉인이 약해지며 다시 위협받습니다. 스승의 희생 속에서 '혼절검'을 계승한 당신은", -8, ANSI_COLOR_RESET);
    print_centered("천마가 완전히 부활한 혈월궁에 홀로 잠입합니다.", -9, ANSI_COLOR_RESET);
    print_centered("이제 단 3분 안에 천마를 쓰러뜨리지 못하면 세상은 영원히 어둠에 잠길 것입니다.", -10, ANSI_COLOR_RESET);
    print_centered("인류의 과거와 미래를 건 최후의 180초에 직면합니다.", -11, ANSI_COLOR_RESET);
    print_centered(" ", -12, ANSI_COLOR_RESET); // 한 줄 공백
    print_centered("천마의 공격 '궤적'을 보고 정확한 키(A, S, W, D)로 받아치십시오.", -14, ANSI_COLOR_CYAN);
    print_centered("콤보를 쌓아 '집중력'을 모으고, 필살기(F) '혼절검'으로 치명타를 날리세요!", -15, ANSI_COLOR_CYAN);
    print_centered("또한, Q 또는 E를 눌러 '역린검결'을 발동하여 잠시 공격력을 강화할 수 있습니다.", -16, ANSI_COLOR_CYAN);
    print_centered("아무 키나 눌러 시작...", -19, ANSI_COLOR_GREEN);
    (void)GET_KEY(); // 사용자 입력 대기

    // --- 게임 변수 초기화 ---
    int player_hp = 100; // 플레이어 체력
    int demon_hp = 900; // 천마 체력
    int focus = 0; // 집중력 (필살기 게이지)
    int combo = 0; // 콤보 카운트

    char pattern[10] = ""; // 천마의 공격 패턴 (예: "aswd")
    char player_input[10] = ""; // 플레이어의 현재 입력
    int pattern_len = 3; // 초기 패턴 길이
    int current_input_pos = 0; // 현재 입력해야 할 패턴 위치

    int needs_new_pattern = 1; // 새 패턴 생성 필요 여부 플래그

    time_t start_time = time(NULL); // 게임 시작 시간
    const int TIME_LIMIT = 180; // 시간 제한 (초)
    double elapsed_time = 0; // 경과 시간

    // 역린검결 관련 변수
    int yokelin_active = 0; // 역린검결 활성화 여부
    time_t yokelin_duration_start_time = 0; // 역린검결 시작 시간
    const int YOKELIN_DURATION = 5; // 역린검결 지속 시간 (초)
    const int YOKELIN_FOCUS_COST = 20; // 역린검결 집중력 소모량
    const double YOKELIN_DAMAGE_MULTIPLIER = 1.5; // 역린검결 공격력 배율

    // --- 메인 게임 루프 ---
    while (player_hp > 0 && demon_hp > 0) {
        elapsed_time = difftime(time(NULL), start_time); // 경과 시간 계산
        if (elapsed_time >= TIME_LIMIT) break; // 시간 초과 시 게임 종료

        // 역린검결 지속 시간 확인 및 비활성화
        if (yokelin_active && difftime(time(NULL), yokelin_duration_start_time) >= YOKELIN_DURATION) {
            yokelin_active = 0;
            print_centered("역린검결 효과가 끝났습니다.", 10, ANSI_COLOR_BLUE);
            SLEEP_MS(800);
        }

        if (needs_new_pattern) {
            generate_pattern(pattern, pattern_len); // 새 패턴 생성
            current_input_pos = 0; // 입력 위치 초기화
            player_input[0] = '\0'; // 플레이어 입력 버퍼 초기화
            needs_new_pattern = 0; // 새 패턴 생성 플래그 비활성화
        }

        clearScreen(); // 화면 지우기
        draw_ui(player_hp, demon_hp, focus, TIME_LIMIT - elapsed_time, yokelin_active); // UI 그리기

        print_centered("천마의 공격 궤적:", 2, ANSI_COLOR_RESET);
        print_pattern_with_highlight(pattern, player_input, current_input_pos, 3); // 하이라이트된 패턴 출력
        print_centered(" ", 4, ANSI_COLOR_RESET); // 공백
        print_centered("당신의 입력:", 5, ANSI_COLOR_RESET);
        print_centered(player_input, 6, ANSI_COLOR_RESET); // 플레이어 입력 출력

        // 남은 시간 경고
        if (TIME_LIMIT - elapsed_time <= 30 && TIME_LIMIT - elapsed_time > 0) {
            if ((int)(TIME_LIMIT - elapsed_time) % 5 == 0) { // 5초마다 경고
                print_centered("!! 시간이 얼마 남지 않았습니다 !!", 8, ANSI_COLOR_RED);
            }
        }

        // 키 입력 감지
        if (CHECK_KEY_HIT()) {
            char key = GET_KEY(); // 키 입력 받기

            // 필살기 '혼절검' 사용 (F 키)
            if ((key == 'f' || key == 'F') && focus >= 100) {
                int damage = 250;
                if (yokelin_active) { // 역린검결 활성화 시 추가 피해
                    damage = (int)(damage * YOKELIN_DAMAGE_MULTIPLIER);
                    print_centered("혼절검 + 역린검결! 극대 피해!", 8, ANSI_COLOR_MAGENTA);
                }
                else {
                    print_centered("필살의 일격! 혼절검!", 8, ANSI_COLOR_MAGENTA);
                }
                demon_hp -= damage;
                focus = 0; // 집중력 소모
                combo = 0; // 콤보 초기화
                SLEEP_MS(1500); // 사용자에게 메시지 확인할 시간 주기
                needs_new_pattern = 1; // 새 패턴 필요
                continue; // 다음 루프 시작 (현재 입력 처리 건너뛰기)
            }

            // 기술 '역린검결' 사용 (Q 또는 E 키)
            if ((key == 'q' || key == 'Q' || key == 'e' || key == 'E') && focus >= YOKELIN_FOCUS_COST && !yokelin_active) {
                focus -= YOKELIN_FOCUS_COST; // 집중력 소모
                yokelin_active = 1; // 역린검결 활성화
                yokelin_duration_start_time = time(NULL); // 지속 시간 측정 시작
                print_centered("역린검결 발동! 공격력이 상승합니다!", 8, ANSI_COLOR_BLUE);
                SLEEP_MS(1200);
                // 역린검결은 패턴 입력과 별개이므로 현재 패턴은 유지
                continue;
            }

            // 일반 패턴 입력 처리
            if (current_input_pos < pattern_len) {
                player_input[current_input_pos] = key; // 플레이어 입력 저장
                player_input[current_input_pos + 1] = '\0'; // 널 종료 문자 추가
            }

            // 입력이 패턴과 일치하지 않을 때
            if (key != pattern[current_input_pos]) {
                player_hp -= 20; // 플레이어 체력 감소
                focus = (focus > 10) ? focus - 10 : 0; // 집중력 감소 (최소 0)
                combo = 0; // 콤보 초기화S
                print_centered("궤적이 흐트러졌다! 천마의 반격!", 8, ANSI_COLOR_RED);
                SLEEP_MS(1000); // 잠시 대기하여 메시지 확인
                needs_new_pattern = 1; // 새 패턴 필요
            }
            // 입력이 패턴과 일치할 때
            else {
                current_input_pos++; // 다음 입력 위치로 이동

                // 패턴을 모두 성공적으로 입력했을 때
                if (current_input_pos == pattern_len) {
                    int damage = 20 + (combo * 2); // 기본 피해량 + 콤보 보너스
                    if (yokelin_active) { // 역린검결 활성화 시 피해량 증가
                        damage = (int)(damage * YOKELIN_DAMAGE_MULTIPLIER);
                    }
                    demon_hp -= damage; // 천마 체력 감소
                    focus += 15; // 집중력 획득 (원래 15였음. 20 -> 15로 다시 변경)
                    if (focus > 100) focus = 100; // 집중력 최대치 제한
                    combo++; // 콤보 증가

                    print_centered("공격을 받아쳤다! 정통으로 명중!", 8, ANSI_COLOR_GREEN);
                    SLEEP_MS(500); // 잠시 대기

                    // 콤보에 따라 패턴 길이 증가 (최대 7로 제한)
                    if (combo > 0 && combo % 7 == 0 && pattern_len < 7) { // 7콤보마다 길이 증가
                        pattern_len++;
                        print_centered("천마의 궤적이 더욱 복잡해집니다!", 9, ANSI_COLOR_YELLOW);
                        SLEEP_MS(800);
                    }
                    needs_new_pattern = 1; // 새 패턴 필요
                }
            }
        }

        SLEEP_MS(33); // 루프 지연 (약 30 FPS)
    }

    // --- 게임 결과 화면 ---
    clearScreen(); // 화면 지우기
    draw_ui(player_hp, demon_hp, focus, 0, yokelin_active); // 최종 UI 그리기 (남은 시간 0)

    if (demon_hp <= 0) { // 천마 격파 성공
        print_centered("*****************************************************************", -2, ANSI_COLOR_YELLOW);
        print_centered("* 천마 격파! 인류를 구원했습니다! 세상에 평화가 찾아옵니다!     *", -3, ANSI_COLOR_GREEN);
        print_centered("*****************************************************************", -4, ANSI_COLOR_YELLOW);
        print_centered("당신은 인류의 과거와 미래를 지킨 진정한 영웅입니다.", -6, ANSI_COLOR_RESET);
        print_centered("2분 58초, 전설은 시작됩니다...", -7, ANSI_COLOR_RESET);
    }
    else if (player_hp <= 0) { // 플레이어 체력 소진
        print_centered("*****************************************************************", -2, ANSI_COLOR_RED);
        print_centered("* 당신은 쓰러졌습니다... 모든 것이 어둠에 잠겼습니다...         *", -3, ANSI_COLOR_RED);
        print_centered("*****************************************************************", -4, ANSI_COLOR_RED);
        print_centered("인류의 마지막 희망은 사라졌고, 세상은 천마의 손에 넘어갔습니다.", -6, ANSI_COLOR_RESET);
    }
    else { // 시간 초과
        print_centered("*****************************************************************", -2, ANSI_COLOR_RED);
        print_centered("* 시간 초과... 세상은 영원히 어둠에 잠겼습니다...           *", -3, ANSI_COLOR_RED);
        print_centered("*****************************************************************", -4, ANSI_COLOR_RED);
        print_centered("최후의 180초는 지나갔고, 천마의 그림자가 세상을 뒤덮었습니다.", -6, ANSI_COLOR_RESET);
    }
    print_centered("--- GAME OVER ---", -9, ANSI_COLOR_YELLOW);
    SLEEP_MS(5000); // 최종 메시지 확인을 위해 5초 대기

#ifndef _WIN32
    changemode(0); // POSIX 시스템에서 터미널 모드 원래대로 복원
#endif
    return 0; // 프로그램 종료
}


// --- 유틸리티 함수 구현 ---

// 화면을 지우는 함수
void clearScreen() {
    system(CLEAR_SCREEN);
}

// 텍스트를 중앙 정렬하여 출력하는 함수
// offset: 양수면 텍스트 위로 빈 줄 추가, 음수면 텍스트 아래로 빈 줄 추가
// color: ANSI 색상 코드 문자열 (예: ANSI_COLOR_RED)
void print_centered(const char* text, int offset, const char* color) {
    int len = strlen(text);
    // 터미널 너비를 80으로 가정하고 중앙 정렬 패딩 계산
    int pad = (80 - len) / 2;
    if (pad < 0) pad = 0; // 텍스트가 80자보다 길면 패딩 없음

    // offset이 음수인 경우 (텍스트 아래에 빈 줄 추가)
    if (offset < 0) {
        for (int i = 0; i < abs(offset); i++) printf("\n");
    }

    // 패딩 및 색상 적용된 텍스트 출력
    for (int i = 0; i < pad; i++) printf(" ");
    printf("%s%s%s", color, text, ANSI_COLOR_RESET); // 색상 적용 및 초기화

    // offset이 양수인 경우 (텍스트 위에 빈 줄 추가) 또는 일반적인 경우
    if (offset >= 0) printf("\n");
}

// 게임 UI를 그리는 함수
void draw_ui(int player_hp, int demon_hp, int focus, double remaining_time, int yokelin_active) {
    // 플레이어 HP 바
    printf(" 용사 HP    : " ANSI_COLOR_GREEN "[");
    for (int i = 0; i < 20; i++) {
        if (i < (player_hp / 5)) printf("#"); else printf("-");
    }
    printf("]" ANSI_COLOR_RESET " %d/100\n", player_hp);

    // 천마 HP 바
    printf(" 천마 HP    : " ANSI_COLOR_RED "[");
    for (int i = 0; i < 20; i++) {
        if (i < (demon_hp / 45)) printf("#"); else printf("-"); // 900 / 45 = 20
    }
    printf("]" ANSI_COLOR_RESET " %d/900\n", demon_hp);

    // 집중력 바 (역린검결 활성화 시 파란색으로 표시)
    printf(" 집중력(F)%s : ", yokelin_active ? ANSI_COLOR_BLUE "(역린검결 ON)" ANSI_COLOR_RESET : "");
    printf(ANSI_COLOR_CYAN "[");
    for (int i = 0; i < 20; i++) {
        if (i < (focus / 5)) printf("+"); else printf(" ");
    }
    printf("]" ANSI_COLOR_RESET " %d/100\n", focus);

    printf("================================================================================\n");
    // 남은 시간 표시
    if (remaining_time <= 30) {
        printf("남은 시간: " ANSI_COLOR_RED "%.0f 초" ANSI_COLOR_RESET "\n", remaining_time);
    }
    else {
        printf("남은 시간: %.0f 초\n", remaining_time);
    }
    printf("================================================================================\n");
}

// 무작위 공격 패턴을 생성하는 함수
void generate_pattern(char* buffer, int length) {
    const char* keys = "aswd"; // 사용 가능한 키 목록
    for (int i = 0; i < length; i++) {
        buffer[i] = keys[rand() % 4]; // 무작위로 키 선택
    }
    buffer[length] = '\0'; // 문자열 끝에 널 종료 문자 추가
}

// 패턴을 출력하되, 플레이어의 입력에 따라 하이라이트를 적용하는 함수
void print_pattern_with_highlight(const char* pattern, const char* player_input, int current_pos, int offset) {
    int len = strlen(pattern);
    int pad = (80 - len) / 2;
    if (pad < 0) pad = 0;

    for (int i = 0; i < abs(offset); i++) printf("\n");

    for (int i = 0; i < pad; i++) printf(" ");

    // 패턴을 한 글자씩 출력하면서 하이라이트 적용
    for (int i = 0; i < len; i++) {
        if (i < current_pos) {
            // 이미 올바르게 입력된 부분 (녹색)
            printf(ANSI_COLOR_GREEN "%c" ANSI_COLOR_RESET, pattern[i]);
        }
        else if (i == current_pos) {
            // 현재 입력해야 할 부분 (노란색)
            printf(ANSI_COLOR_YELLOW "%c" ANSI_COLOR_RESET, pattern[i]);
        }
        else {
            // 아직 입력하지 않은 부분 (기본 색상)
            printf("%c", pattern[i]);
        }
    }
    printf("\n");
}