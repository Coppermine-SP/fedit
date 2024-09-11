/*
*   termui.h - fedit <2024-2 Advanced Data Structure>
*   Copyright (C) 2024 Coppermine-SP <창원대학교 컴퓨터공학과 20233063 손유찬>
*/
#include <stdbool.h>
#include "termui_types.h"

typedef struct{
    char* file_name;
    int total_lines;
    int cursor_line;
} status_t;

/// @brief TUI 환경을 초기화합니다.
void ui_init();

/// @brief 상태 바를 업데이트 합니다.
/// @param cursor_line 
/// @param total_lines 
/// @param file_name 
void ui_set_status(int cursor_line, int total_lines, char* const file_name);

/// @brief UI 메시지를 설정합니다.
/// @param msg 메시지 스트링
void ui_show_message(const char* msg);

/// @brief 기본 UI 메세지를 출력합니다.
void ui_show_default_message();

/// @brief MOTD를 표출 또는 삭제합니다.
/// @param state 표시(true), 삭제(false)
void ui_render_motd(bool state);

/// @brief stdin에서 키 입력을 받으면 콜백 함수를 실행합니다.
/// @param callback 콜백 함수를 지정합니다. 콜백 함수의 반환값이 false이면, 루프를 탈출합니다.
void ui_input_loop(bool (*callback)(enum key_type type, char c));

/// @brief 메시지 바에 프롬프트를 표시하고, 사용자의 입력을 스트링으로 받아 반환합니다.
/// @param msg 프롬프트 스트링
/// @param buf 대상 버퍼
bool ui_show_prompt(char* const msg, char* buf);

/// @brief 스크린 버퍼를 그립니다.
/// @param screen_buf 스크린 버퍼
/// @param len 버퍼 크기
void ui_render(const char* screen_buf, int len);

/// @brief 종료하기 위해 정리합니다.
void ui_dispose();

/// @brief 터미널 크기를 구합니다.
/// @param cols 터미널 열 변수 intptr (nullable)
/// @param rows 터미널 행 변수 intptr (nullable)
void ui_get_terminal_size(int* cols, int* rows);

/// @brief 편집 영역에서 커서를 이동합니다.
/// @param x X 좌표
/// @param y Y 좌표
void ui_cursor_move(unsigned x, unsigned y);

/// @brief 터미널에서 경고음을 재생합니다.
void ui_alert();
