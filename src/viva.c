/*
 *   viva.c - fedit <2024-2 Advanced Data Structure>
 *   Copyright (C) 2024 Coppermine-SP (copperminesp@cloudint.corp) - <창원대학교 컴퓨터공학과 20233063 손유찬>
 *   See this repository from GitHub: https://github.com/Coppermine-SP/fedit
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include "textengine/textengine.h"
#include "termui/termui.h"
#include "termui/termui_types.h"

// #region Macro constants
#define MAX_FILE_NAME_SIZE 128
// #endregion

// #region Global variables
static char* file_name;
static bool is_saved = false;

static int rel_pos = 0;
static int base_pos = 0;
static const char* screen_buf;
static int buf_len;

static int cols;
static int rows;
// #endregion

// #region Function prototypes
bool quit_function();
bool find_function();
bool save_function();

void editor_cursor_move(enum key_type x);
void editor_resize_event();
void editor_draw();
void editor_update();
void editor_draw_cursor();
void editor_quit();

void signal_handler(int sig);
// #endregion

// #region Helper functions
void update_terminal_size(){
    ui_get_terminal_size(&cols, &rows);
}

// #endregion

bool input_event(enum key_type type, char c)
{
    if(type != NORMAL_KEY){
        if(type == CONTROL_KEY){
            if (c == CTRL_KEY('q')) return quit_function();
            else if (c == CTRL_KEY('f')) return find_function();
            else if (c == CTRL_KEY('s')) return save_function();
            else{
                ui_alert();
                ui_show_default_message();
            }
        }
        else if(type >= UP_ARROW) editor_cursor_move(type);
        else if(type == ENTER) printf("ENTER ");
        else if(type == BACKSPACE) printf("BACKSPACE ");
        else if(type == PGUP) printf("PGUP ");
        else if(type == PGDOWN) printf("PGDOWN ");
        else if(type == HOME) printf("HOME ");
        else if(type == END) printf("END ");
    }
    else{
        printf("%c", c);
    }
    return true;
}

int main(int argc, char *argv[])
{
    file_name  = argc > 1 ? argv[1] : NULL;
    te_init(file_name);
    ui_init(editor_resize_event);
    signal(SIGINT, signal_handler);

    editor_update();
    editor_draw();
    if (argc == 1) ui_set_motd(true);

    ui_input_loop(input_event);
    editor_quit();
}

void signal_handler(int sig){
    if(sig == SIGINT){
        editor_quit();
        exit(0);
    }
}

// #region Editor functions
void editor_cursor_move(enum key_type x){
    int cols, rows;
    ui_get_terminal_size(&cols, &rows);

    if(x == UP_ARROW){
    }
    else if(x == DOWN_ARROW){
    }
    else if(x == LEFT_ARROW){
        if(rel_pos == 0) ui_alert();
        else rel_pos--;
    }
    else{
        if(false) ui_alert();
        else rel_pos++;
    }

    editor_draw_cursor();
}

void editor_draw(){
    int len;
    char* buf = te_get_buffer(&len);

    ui_set_status(1, 0, file_name);
    ui_draw_text(buf, len);
    editor_draw_cursor();
}

void editor_draw_cursor(){
    int screen_pos = 0;
    unsigned pad = cols;
    for(int i = 0; i < rel_pos; i++){
        char tmp = screen_buf[base_pos + i];
        if(tmp == '\n'){
            screen_pos += pad;
            pad = cols;
        }
        else{
            if(tmp == '\0') continue;
            if(pad == 1) pad = cols;
            else pad--;
            
            screen_pos++;
        }
    }

    int x_pos = (screen_pos % cols);
    int y_pos = (screen_pos / cols);

    //화면 리사이징시 스크린 좌표가 영역을 벗어나지 않도록 방지
    if(x_pos > cols || y_pos > rows){
        rel_pos = 0;
        x_pos = 0;
        y_pos = 0;
    }

    ui_cursor_move(x_pos, y_pos);
}

void editor_update(){
    screen_buf = te_get_buffer(&buf_len);
    update_terminal_size();
}

void editor_resize_event(){
    update_terminal_size();
    editor_draw();
}

void editor_quit(){
    te_dispose();
    ui_dispose();
}
// #endregion

// #region Quit
static bool quit_override = false;
bool quit_function_input_event(enum key_type type, char c)
{
    quit_override = (c == CTRL_KEY('q'));
    return false;
}

bool quit_function()
{
    if (!is_saved)
    {
        ui_show_message("No write since last change (Press Ctrl-Q again to override)");
        ui_alert();
        ui_input_loop(quit_function_input_event);

        if (!quit_override){
            ui_show_default_message();
            editor_draw_cursor();
            return true;
        }
    }
    return false;
}

// #endregion

// #region Find
bool find_function(){
    return false;
}
// #endregion

// #region Save
bool save_function(){
    if(is_saved){
        ui_alert();
        ui_show_message("There is no local changes to save.");
    }
    else{
        bool is_file_new = false;
        if(file_name == NULL){
            char buf[50];
            ui_show_prompt("File Name: ", buf);
            is_file_new = true;
        }

        char buf[100];
        sprintf(buf, "\"%s\"%s %dL, %dB written.", file_name, (is_file_new ? "[New]" : ""), 7, 1280);
        ui_show_message(buf);
        
    }

    editor_draw_cursor();
    return true;
}
// #endregion