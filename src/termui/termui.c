/*
*   termui.c - fedit <2024-2 Advanced Data Structure>
*   Copyright (C) 2024 Coppermine-SP <창원대학교 컴퓨터공학과 20233063 손유찬>
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "termui.h"
#include "native_termui.h"

#define PROMPT_INPUT_BUFFER_SIZE 30

#define CURSOR_GOTO(row, col) printf("\033[%d;%dH", (row), (col))
#define CURSOR_HIDE printf("\x1b[?25l")
#define CURSOR_SHOW printf("\x1b[?25h")
#define CLEAR_LINE printf("\x1b[K")
#define COLOR_INVERT printf("\x1b[7m")
#define COLOR_NORMAL printf("\x1b[0m")

const char* default_message_string = "HELP: Ctrl-S = Save | Ctrl-Q = Quit | Ctrl-F = Find";
const char* default_file_name_string = "No Name";
const char* default_file_type_string = "no ft";
const char* title_string = "fedit -- Visual Text Editor";
const char* subtitle_string = "Copyright (C) 2024 Coppermine-SP";

terminal_size_t terminal_size;
status_t status;
bool motd_showing = false;

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
    char filetype_string[10];

    int left = sprintf(left_buf,"[%s] - %d lines", (status.file_name == NULL ? default_file_name_string : status.file_name), status.total_lines);
    int right = sprintf(right_buf,"%s | %d/%d",(status.file_name == NULL) ? default_file_type_string : filetype_string ,status.cursor_line, status.total_lines);

    printf("%s",left_buf);
    for(int i = 0; i < terminal_size.cols - (left + right); i++) printf(" ");
    printf("%s", right_buf);
    COLOR_NORMAL;
}

void ui_set_motd(bool state){
    int title_len = strlen(title_string);
    int subtitle_len = strlen(subtitle_string);
    CURSOR_HIDE;
    CURSOR_GOTO((terminal_size.rows/2)-3, (terminal_size.cols - title_len) / 2);

    if(state)printf("%s", title_string);
    else CLEAR_LINE;

    CURSOR_GOTO((terminal_size.rows/2)-2, (terminal_size.cols - subtitle_len) / 2);
    if(state)printf("%s", subtitle_string);
    else CLEAR_LINE;
    
    motd_showing = state;
    if(!state){
        CURSOR_GOTO(0, 0);
        CURSOR_SHOW;
    }
}

void ui_set_status(status_t x){
    status.cursor_line = x.cursor_line;
    status.file_name = x.file_name;
    status.total_lines = x.total_lines;

    ui_update_status();
}

void ui_input_loop(bool (*callback)(char c)){
    while(true){
        char c;
        get_raw_input(&c);

        if(!callback(c)) break;
    }
}

char prompt_input_buf[PROMPT_INPUT_BUFFER_SIZE];
int prompt_input_idx;
static bool prompt_input_event(char c){
    if(prompt_input_idx - 1 > PROMPT_INPUT_BUFFER_SIZE) return false;

    if(iscntrl(c)){
        return false;
    }
    else{
        prompt_input_buf[prompt_input_idx++] = c;
        printf("%c", c);
        return true;
    }
}

char* const ui_show_prompt(char* msg){
    CURSOR_GOTO(terminal_size.rows, 0);
    CLEAR_LINE;
    printf("%s : ", msg);

    //입력 버퍼 초기화
    prompt_input_idx = 0;
    memset(prompt_input_buf, 0, PROMPT_INPUT_BUFFER_SIZE);
    ui_input_loop(&prompt_input_event);
    prompt_input_buf[prompt_input_idx] = '/0';

    ui_set_message(default_message_string);
    return prompt_input_buf;
}

void ui_init(){
    setvbuf(stdout, NULL, _IONBF, 0);
    enable_raw_input();
    printf("\x1b[2J");

    terminal_size = get_terminal_size();
    ui_set_message(default_message_string);

    status_t x;
    x.cursor_line = 1;
    x.file_name = NULL;
    x.total_lines = 0;
    ui_set_status(x);
}
 