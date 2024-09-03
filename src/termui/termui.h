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

void ui_init(bool show_motd);
void ui_set_status();
void ui_set_message(char* msg);