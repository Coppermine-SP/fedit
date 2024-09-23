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
static const char* buf;
static int buf_len;

static int cols;
static int rows;
// #endregion

// #region Macro functions
#define MAX_SCRREN_POS ((cols*(rows-2))-1)
// #endregion

// #region Function prototypes
bool quit_function();
bool find_function();
bool save_function();

void editor_cursor_move(enum key_type x);
void editor_resize_event();
void editor_draw(bool force);
void editor_draw_cursor();
void editor_quit();

void signal_handler(int sig);
// #endregion

// #region Helper functions
void update_terminal_size(){
    ui_get_terminal_size(&cols, &rows);
}

int get_screen_pos(int base, int x){
    int screen_pos = 0;
    unsigned pad = cols;
    for(int i = 0; i < x; i++){
        char tmp = buf[base + i];
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

    return screen_pos;
}

// #endregion

// #region Cursor functions
void cursor_move_up(){
    if(rel_pos == 0 && base_pos == 0){
        ui_alert();
        return;
    }

    bool nextline = false;
    int i = rel_pos-1;
    //상대 주소가 음수라는 것은, 기준 주소가 올라가야 한다는 것을 의미합니다.
    while(base_pos + i > 0){
        if(buf[base_pos + i--] == '\n'){
            if(nextline) break;
            else nextline = true;
        }
    }

    if(i <= 0){
        if((base_pos + i == 0)){
            rel_pos = 0;
            base_pos = 0;
        }
        else{
            rel_pos = 0;
            base_pos += i+2;
        }
    }
    else rel_pos = i+2;
}

void cursor_move_down(){
    int next = rel_pos;
    for(int i = next; i < (buf_len - base_pos); i++){
        if(buf[base_pos + i] == '\n'){
            next = i+1;
            break;
        }
    }
    
    if(next == rel_pos){
        ui_alert();
        return;
    }

    int base = base_pos;
    int rel = next;
    while(get_screen_pos(base, rel) > MAX_SCRREN_POS) {
        for(int i = base; i < buf_len; i++){
            if(buf[i] == '\n'){
                base = i+1;
                rel = next - (base - base_pos);
                break;
            }
        }
    }

    base_pos = base;
    rel_pos = rel;
}

void cursor_move_left(){
    if(rel_pos == 0){
        if(base_pos == 0) ui_alert();
        else{
            int i;
            for(i = 0; i < cols; i++)
                if(buf[base_pos-i] == '\n') break;

            base_pos -= i;
            rel_pos -= i;
        }
    }
    else{
        if(buf[base_pos + rel_pos - 1] == '\n') ui_alert();
        else rel_pos--;
    }
}

void cursor_move_right(){
    if((base_pos + rel_pos) >= buf_len || buf[base_pos + rel_pos+1] == '\n'){
        ui_alert();
        return;
    }

    int next = rel_pos+1;
    if(get_screen_pos(base_pos, next) > MAX_SCRREN_POS){
        int i;
        for(i = 0; i < cols; i++) if(buf[base_pos + i] == '\n') break;
        base_pos += i+1;
        next -= i;
    }
    
    rel_pos = next;
}

void cursor_home(){
    if(buf[base_pos + rel_pos] == '\n') return;

    int i = rel_pos;
    while(base_pos + i > 0){
        if(buf[base_pos + i--] == '\n') break;
    }

    if(i <= 0){
        if((base_pos + i == 0)){
            rel_pos = 0;
            base_pos = 0;
        }
        else{
            rel_pos = 0;
            base_pos += i+2;
        }
    }
    else rel_pos = i+2;

    editor_draw(false);
}

void cursor_end(){
    if(buf[base_pos + rel_pos] == '\n') return;
    
    int next = rel_pos;
    for(int i = next; i < (buf_len - base_pos); i++){
        if(buf[base_pos + i] == '\n'){
            next = i-1;
            break;
        }
    }
    
    if(next == rel_pos){
        ui_alert();
        return;
    }

    int base = base_pos;
    int rel = next;
    while(get_screen_pos(base, rel) > MAX_SCRREN_POS) {
        for(int i = base; i < buf_len; i++){
            if(buf[i] == '\n'){
                base = i+1;
                rel = next - (base - base_pos);
                break;
            }
        }
    }

    base_pos = base;
    rel_pos = rel;
    editor_draw(false);
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
        else if(type == HOME) cursor_home();
        else if(type == END) cursor_end();
        else if(type == ENTER) printf("ENTER ");
        else if(type == BACKSPACE) printf("BACKSPACE ");
        else if(type == PGUP) printf("PGUP ");
        else if(type == PGDOWN) printf("PGDOWN ");

    }
    else{
        printf("%c", c);
    }
    return true;
}

void signal_handler(int sig){
    if(sig == SIGINT){
        editor_quit();
        exit(0);
    }
}

// #region Entrypoint
int main(int argc, char *argv[])
{
    file_name  = argc > 1 ? argv[1] : NULL;
    te_init(file_name);
    ui_init(editor_resize_event);
    signal(SIGINT, signal_handler);

    update_terminal_size();
    editor_draw(true);
    if (argc == 1) ui_set_motd(true);

    ui_input_loop(input_event);
    editor_quit();
}
// #endregion

// #region Editor functions
void editor_cursor_move(enum key_type x){
    if(x == UP_ARROW) cursor_move_up();
    else if(x == DOWN_ARROW) cursor_move_down();
    else if(x == LEFT_ARROW) cursor_move_left();
    else cursor_move_right();

    editor_draw(false);
}

void editor_draw(bool force){
    static int last_base_pos;
    static int last_len;

    buf = te_get_buffer(&buf_len);
    ui_set_status(1, 0, file_name);

    if((last_base_pos != base_pos) || (last_len != buf_len) || force)
        ui_draw_text((char*)(buf + base_pos), (buf_len - base_pos));
    last_base_pos = base_pos;
    last_len = buf_len;

    editor_draw_cursor();
}

void editor_draw_cursor(){
    /*
        매우 청명한 하늘입니다.
        2024.09.13, KE643 (ICN => SIN)에서.
    */
    int screen_pos = get_screen_pos(base_pos, rel_pos);

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

void editor_resize_event(){
    update_terminal_size();
    editor_draw(true);
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

        char str_buf[100];
        sprintf(str_buf, "\"%s\"%s %dL, %dB written.", file_name, (is_file_new ? "[New]" : ""), 7, 1280);
        ui_show_message(str_buf);
        
    }

    editor_draw_cursor();
    return true;
}
// #endregion