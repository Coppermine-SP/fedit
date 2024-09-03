/*
*   termui.c - fedit <2024-2 Advanced Data Structure>
*   Copyright (C) 2024 Coppermine-SP <창원대학교 컴퓨터공학과 20233063 손유찬>
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "termui.h"
#include "native_termui.h"

#define CURSOR_GOTO(row, col) printf("\033[%d;%dH", (row), (col))
#define CURSOR_HIDE printf("\x1b[?25l")
#define CURSOR_SHOW printf("\x1b[?25h")
#define COLOR_INVERT printf("\x1b[7m")
#define COLOR_NORMAL printf("\x1b[0m")

const char* default_message_string = "HELP: Ctrl-S = Save | Ctrl-Q = Quit | Ctrl-F = Find";
const char* default_file_name_string = "No Name";
const char* title_string = "fedit -- Visual Text Editor";
const char* subtitle_string = "Copyright (C) 2024 Coppermine-SP";
terminal_size_t terminal_size;
status_t status;

void ui_set_message(char* const msg){
    CURSOR_HIDE;
    CURSOR_GOTO(terminal_size.rows, 0);
    printf("%s", msg);
}

void ui_update_status(){
    CURSOR_HIDE;
    COLOR_INVERT;
    CURSOR_GOTO(terminal_size.rows-1, 0);

    char left_buf[40];
    char right_buf[20];
    int left = sprintf(left_buf,"[%s] - %d lines", (status.file_name == NULL ? default_file_name_string : status.file_name), status.total_lines);
    int right = sprintf(right_buf,"| %d/%d", status.cursor_line, status.total_lines);

    printf("%s",left_buf);
    for(int i = 0; i < terminal_size.cols - (left + right); i++) printf(" ");
    printf("%s", right_buf);
    COLOR_NORMAL;
}

void ui_show_motd(){
    int title_len = strlen(title_string);
    int subtitle_len = strlen(subtitle_string);
    CURSOR_GOTO(terminal_size.rows/2, (terminal_size.cols - title_len) / 2);
    printf("%s", title_string);

    CURSOR_GOTO((terminal_size.rows/2)+1, (terminal_size.cols - subtitle_len) / 2);
    printf("%s", subtitle_string);
}

void ui_set_status(status_t x){
    status.cursor_line = x.cursor_line;
    status.file_name = x.file_name;
    status.total_lines = x.total_lines;

    ui_update_status();
}

void ui_init(bool show_motd){
    setvbuf(stdout, NULL, _IONBF, 0);
    printf("\x1b[2J");

    terminal_size = get_terminal_size();
    ui_set_message(default_message_string);

    status_t x;
    x.cursor_line = 1;
    x.file_name = NULL;
    x.total_lines = 0;
    ui_set_status(x);
    ui_show_motd();
}
 