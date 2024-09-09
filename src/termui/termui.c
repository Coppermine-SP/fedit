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

// #region Macro constants
#define PROMPT_INPUT_BUFFER_SIZE 50
// #endregion

// #region Macro functions
#define CURSOR_GOTO(row, col) printf("\033[%d;%dH", (row), (col))
#define CURSOR_HIDE printf("\x1b[?25l")
#define CURSOR_SHOW printf("\x1b[?25h")
#define CLEAR_LINE printf("\x1b[K")
#define CLEAR_SCREEN printf("\x1b[2J")
#define CLEAR_UP printf("\x1b[1J")
#define COLOR_INVERT printf("\x1b[7m")
#define COLOR_NORMAL printf("\x1b[0m")
// #endregion

// #region String constants
static const char* default_message_string = "HELP: Ctrl-S = Save | Ctrl-Q = Quit | Ctrl-F = Find";
static const char* default_file_name_string = "No Name";
static const char* default_file_type_string = "no ft";
static const char* title_string = "fedit -- Visual Text Editor";
static const char* subtitle_string = "Copyright (C) 2024 Coppermine-SP";
// #endregion

// #region Global variables
static terminal_size_t terminal_size;
static status_t status;
static bool motd_showing = false;
// #endregion

void ui_alert(){
    printf("\a");
}

void ui_show_message(const char* msg){
    CURSOR_HIDE;
    CURSOR_GOTO(terminal_size.rows, 0);
    printf("%s", msg);
}

void ui_show_default_message(){
    ui_show_message(default_message_string);
}

static bool get_file_type(char* const src, char* buf, int buf_len){
    int src_len = strlen(src);
    for(int i = src_len - 1; i >= 0; i--){
        if(src[i] == '.'){
            int ext_len = src_len - i - 1;
            if (ext_len < buf_len) {
                strncpy(buf, &src[i + 1], ext_len);
                buf[ext_len] = '\0';
            } 
            else {
                strncpy(buf, &src[i + 1], buf_len - 1);
                buf[buf_len - 1] = '\0';
            }
            return true;
        }
    }
    return false;
}

static void render_status(){
    CURSOR_HIDE;
    COLOR_INVERT;
    CURSOR_GOTO(terminal_size.rows-1, 0);

    char left_buf[40];
    char right_buf[20];
    char filetype_string[15];
    bool has_filetype = false;

    if(status.file_name != NULL)
    has_filetype = get_file_type(status.file_name, filetype_string, 15);
    
    int left = sprintf(left_buf,"[%s] - %d lines", (status.file_name == NULL ? default_file_name_string : status.file_name), status.total_lines);
    int right = sprintf(right_buf,"%s | %d/%d",(has_filetype) ? filetype_string :default_file_type_string ,status.cursor_line, status.total_lines);

    printf("%s",left_buf);
    for(int i = 0; i < terminal_size.cols - (left + right); i++) printf(" ");
    printf("%s", right_buf);
    COLOR_NORMAL;
}

void ui_render_motd(bool state){
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

void ui_set_status(int cursor_line, int total_lines, char* const file_name){
    status.cursor_line = cursor_line;
    status.file_name = file_name;
    status.total_lines = total_lines;

    render_status();
}

void ui_input_loop(bool (*callback)(char c)){
    while(true){
        char c;
        get_raw_input(&c);
        if(!callback(c)) break;
    }
}

static char prompt_input_buf[PROMPT_INPUT_BUFFER_SIZE];
static int prompt_input_idx;
static bool prompt_input_event(char c){
    if(prompt_input_idx - 1 > PROMPT_INPUT_BUFFER_SIZE) return false;

    if(iscntrl(c)){
        if(c == ESC_KEY){
            prompt_input_buf[0] = '\0';
            return false;
        }
        else if(c == ENTER_KEY){
            prompt_input_buf[prompt_input_idx] = '\0';
            return false;
        }
    }
    else{
        prompt_input_buf[prompt_input_idx++] = c;
        printf("%c", c);
        return true;
    }
}

void ui_get_terminal_size(int* cols, int* rows){
    if(cols != NULL) *cols = terminal_size.cols;
    if(rows != NULL) *rows = terminal_size.rows;
}

char* const ui_show_prompt(char* msg){
    CURSOR_GOTO(terminal_size.rows, 0);
    CLEAR_LINE;
    printf("%s : ", msg);
    ui_alert();

    //입력 버퍼 초기화
    prompt_input_idx = 0;
    memset(prompt_input_buf, 0, PROMPT_INPUT_BUFFER_SIZE);
    ui_input_loop(&prompt_input_event);

    ui_show_message(default_message_string);
    if(prompt_input_buf[0] == '\0') return NULL;

    return prompt_input_buf;
}

void ui_render(const char* screen_buf, int len){
    CURSOR_HIDE;
    CURSOR_GOTO(terminal_size.rows-2, terminal_size.cols);
    CLEAR_UP;
    CURSOR_GOTO(0, 0);
    
    const char* cur = screen_buf;
    for(int i = 1; i < terminal_size.rows - 2; i++){
        CURSOR_GOTO(i, 0);

        if((cur - screen_buf) < len){
            for(int j = 0; j < terminal_size.cols - 2; j++){
                if((cur - screen_buf) >= len) break;

                if(*cur == '\r' || *cur == '\0'){
                    cur++;
                    continue;
                }
                else if(*cur == '\n'){
                    cur++;
                    break;
                }

                printf("%c", *(cur++));
            }
        }
        else{
            printf("~");
        }
    }
}

void ui_init(){
    setvbuf(stdout, NULL, _IONBF, 0);
    configure_term_env();
    CLEAR_SCREEN;

    terminal_size = get_terminal_size();
    ui_show_message(default_message_string);
}
 