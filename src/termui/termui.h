/*
*   termui.h - fedit <2024-2 Advanced Data Structure>
*   Copyright (C) 2024 Coppermine-SP <창원대학교 컴퓨터공학과 20233063 손유찬>
*/
#include <stdbool.h>

typedef struct{
    char* file_name;
    int total_lines;
    int cursor_line;
} status_t;

#define CTRL_KEY(x) ((x)&0x1f)

/// @brief TUI 환경을 초기화합니다.
void ui_init();
void ui_set_status();

/// @brief UI 메시지를 설정합니다.
/// @param msg 메시지 스트링
void ui_set_message(char* const msg);

/// @brief MOTD를 표출 또는 삭제합니다.
/// @param state 표시(true), 삭제(false)
void ui_set_motd(bool state);

/// @brief stdin에서 키 입력을 받으면 콜백 함수를 실행합니다.
/// @param callback 콜백 함수를 지정합니다. 콜백 함수의 반환값이 false이면, 루프를 탈출합니다.
void ui_input_loop(bool (*callback)(char c));

/// @brief 메시지 바에 프롬프트를 표시하고, 사용자의 입력을 스트링으로 받아 반환합니다.
/// @param msg 프롬프트 스트링
char* const ui_show_prompt(char* const msg);