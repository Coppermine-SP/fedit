/*
    viva.c - fedit <2024-2 Advanced Data Structure>
    Copyright (C) 2024 Coppermine-SP (copperminesp@cloudint.corp) - <창원대학교 컴퓨터공학과 20233063 손유찬>
 
    See this repository from GitHub: https://github.com/Coppermine-SP/fedit
 */

#define _CRT_SECURE_NO_WARNINGS

// #region Includes
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include "textengine/textengine.h"
#include "termui/termui.h"
#include "termui/termui_types.h"
// #endregion

// #region Macro constants
#define PATTERN_LEN_MIN 2
#define PATTERN_LEN_MAX 30
// #endregion

// #region Global variables
static char* file_name;
static bool is_saved = true;
static int total_lines = 0;

static int rel_pos = 0;
static int base_pos = 0;

static const char* buf;
static int buf_len;

static int cols;
static int rows;
// #endregion

// #region Function prototypes
bool quit_function();
bool find_function();
bool save_function(bool force_new_file);

void editor_resize_event();
bool editor_input_event(enum key_type type, char c);

void editor_cursor_move(enum key_type x);
void editor_draw(bool force);
void editor_insert(char x);
void editor_delete();
void editor_draw_cursor();
void editor_quit();
// #endregion

// #region Helper functions
static inline void update_terminal_size(){
    ui_get_terminal_size(&cols, &rows);
}

static inline int max_screen_pos(){
    return ((cols*(rows-2))-1);
}

static inline void buffer_update(){ 
    buf = te_get_buffer(&buf_len); 
}

static inline int power(int x, int y){
    int result = 1;
    for(int i = 0; i < y; i++) result *= x;
    return result;
}

int hash(const char* str, int len){
    int result = 0;
    for(int i = 0; i < len; i++) result += (int)str[i] * power(2, len - i-1);
    return result;
}

bool compare_string(const char* lhs, const char* rhs, int len){
    for(int i = 0; i < len; i++) if(lhs[i] != rhs[i]) return false;
    return true;
}

void concat_string(char* str, const char* x, int pos, int* len){
    int x_len = strlen(x);
    memmove(str + pos + x_len, str + pos, *len - pos);
    *len += x_len;
    
    for(int i = 0; i < x_len; i++) str[pos + i] = x[i];
}

int get_screen_pos(int base, int x){
    int screen_pos = 0;
    unsigned pad = cols;
    for(int i = 0; i < x; i++){
        char tmp = buf[base + i];
        if(tmp == LF){
            screen_pos += pad;
            pad = cols;
        }
        else{
            if(tmp == 0) continue;
            else if(tmp == TAB){
                int tab_size = TAB_SPACE - ((cols - pad) % TAB_SPACE);
                pad -= tab_size;
                screen_pos += tab_size;
                continue;
            }
            
            if(pad <= 1) pad = cols;
            else pad--;
            
            screen_pos++;
        }
    }

    return screen_pos;
}

void calc_total_lines(){
    int lines = 0;
    bool is_empty = true;
    for(int i = 0; i < buf_len; i++){
        if(buf[i] != 0){
            if(buf[i] == LF) lines++;
            is_empty = false;
        }
    }
 
    if(!is_empty)lines++;
    total_lines = lines;
}

int get_base_lines(int base){
    int lines = 1;
    for(int i = 0; i <= base; i++) if(buf[i] == LF) lines++;
    return lines;
}

int get_rel_lines(){
    int lines = 0;
    for(int i = 1; i + base_pos < base_pos + rel_pos; i++) if(buf[base_pos + i] == LF) lines++;
    return lines;
}

void adjust_basepos_down(int* base, int* rel){
    while(get_screen_pos(*base, *rel) > max_screen_pos()){
        int i;
        for(i = 0; i < cols; i++) 
            if(buf[(*base) + i] == LF){
                i++;
                break;
            }
        
        *base += i;
        *rel -= i;
    }
}

void signal_handler(int sig){
    if(sig == SIGINT){
        editor_quit();
        exit(0);
    }
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
    //상대 주소가 음수이면, 기준 주소를 상대 주소만큼 빼고, 상대 주소를 0으로 합니다.
    while(base_pos + i > 0){
        if(buf[base_pos + i--] == LF){
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
        if(buf[base_pos + i] == LF){
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
    while(get_screen_pos(base, rel) > max_screen_pos()) {
        for(int i = base; i < buf_len; i++){
            if(buf[i] == LF){
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
                if(buf[base_pos-i] == LF) {
                    i--;
                    break;
                }

            if(i == 0) ui_alert();
            else{
                base_pos -= i;
                rel_pos += i;
            }
        }
    }
    else{
        if(buf[base_pos + rel_pos - 1] == LF) ui_alert();
        else rel_pos--;
    }
}

void cursor_move_right(){
    if((base_pos + rel_pos) >= buf_len || buf[base_pos + rel_pos] == LF){
        ui_alert();
        return;
    }

    int next = rel_pos+1;
    int base = base_pos;
    adjust_basepos_down(&base, &next);
    
    base_pos = base;
    rel_pos = next;
}

void cursor_home(){
    te_close_cursor();
    buffer_update();
    if(buf[base_pos + rel_pos] == LF){
        if(rel_pos > 0 && buf[base_pos + rel_pos - 1] != LF) rel_pos--;
        else return;
    }

    int i = rel_pos;
    while(base_pos + i > 0){
        if(buf[base_pos + i--] == LF) break;
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
    te_close_cursor();
    buffer_update();
    if(buf[base_pos + rel_pos] == LF) return;

    int next = rel_pos;
    int base = base_pos;
    for(int i = next; i <= (buf_len - base_pos); i++){
        if(buf[base_pos + i] == LF || (base_pos + i) == buf_len){
            next = i;
            break;
        }
    }
    
    if(next == rel_pos){
        ui_alert();
        return;
    }

    adjust_basepos_down(&base, &next);

    base_pos = base;
    rel_pos = next;
    editor_draw(false);
}

void cursor_pgup(){
    te_close_cursor();
    buffer_update();
    if(base_pos == 0){
        ui_alert();
        return;
    }

    //기준 위치를 텍스트 기준 행으로 설정
    int next = base_pos-1;
    if(buf[next] != LF){
        int i = 0;
        while(next+i > 0){
            if(buf[next + i--] == LF) break;
        }
        next += i;
    }

    bool line = false;
    while(get_screen_pos(next, base_pos-next) < max_screen_pos()){
        for(int i = 1; next-i >=-1; i++){
            if(next-i < 0){
                next = -1;
                goto out;
            }

            if(buf[next - i] == LF){
                next -= i;
                break;
            }

        }
    }

    //There is no better way than using a goto statement to exit a nested loops.
    out:
    if(next == base_pos){
        ui_alert();
        return;
    }

    base_pos = next+1;
    rel_pos = 0;
    editor_draw(false);
}

void cursor_pgdown(){
    te_close_cursor();
    buffer_update();
    int next = base_pos;
    int tmp;
    while(get_screen_pos(base_pos, next - base_pos) < max_screen_pos()){
        tmp = next;
        for(int i = 0; next+i <= buf_len; i++){
            if(buf[next + i] == LF){
                next += i+1;
                break;
            }
            else if(next+i == buf_len) goto out;     
        }
    }

    out:
    next = tmp;
    if(next == base_pos){
        ui_alert();
        return;
    }

    base_pos = next;
    rel_pos = 0;
    editor_draw(false);
}
// #endregion

// #region Entrypoint
int main(int argc, char *argv[]){
    file_name  = argc > 1 ? argv[1] : NULL;

    te_init(file_name);
    buffer_update();
    calc_total_lines();

    ui_init();
    signal(SIGINT, signal_handler);

    update_terminal_size();
    editor_draw(true);
    if (argc == 1) ui_set_motd(true);

    ui_input_loop(editor_input_event, editor_resize_event);
    editor_quit();
}
// #endregion

// #region Editor event callbacks
bool editor_input_event(enum key_type type, char c){
    if(type != NORMAL_KEY){
        if(type == CONTROL_KEY){
            if (c == CTRL_KEY('q')) return quit_function();
            else if (c == CTRL_KEY('f')) return find_function();
            else if (c == CTRL_KEY('s')) return save_function(false);
            else if(c == CTRL_KEY('n')) return save_function(true);
            else if (c == TAB) editor_insert(TAB);
            else{
                ui_alert();
                ui_show_default_message();
                return true;
            }
        }
        else if(type >= UP_ARROW) editor_cursor_move(type);
        else if(type == HOME) cursor_home();
        else if(type == END) cursor_end();
        else if(type == PGUP) cursor_pgup();
        else if(type == PGDOWN) cursor_pgdown();
        else if(type == ENTER) editor_insert(LF);
        else if(type == BACKSPACE) editor_delete();
    }
    else editor_insert(c);

    te_set_cursor(base_pos + rel_pos);
    return true;
}

void editor_resize_event(){
    update_terminal_size();
    editor_draw(true);
}
// #endregion

// #region Editor functions
void editor_insert(char x){
    te_insert(x);

    is_saved = false;
    rel_pos++;
    adjust_basepos_down(&base_pos, &rel_pos);

    te_set_cursor(base_pos + rel_pos);

    if(x == LF || (total_lines == 0 && base_pos + rel_pos == 1)) total_lines++;
    editor_draw(true);
}

void editor_delete(){
    if(base_pos + rel_pos == 0){
        ui_alert();
        return;
    }

    int abs_pos = base_pos + --rel_pos;
    if(buf[abs_pos] == LF) total_lines--;
    is_saved = false;
    te_set_cursor(abs_pos);
    te_delete();

    if(rel_pos < 0){
        int i;
        for(i = 0; i < cols; i++){
            if(base_pos-i == 0) break;
            else if(buf[base_pos-i] == LF){
                i--;
                break;
            }
        }

        base_pos -= i;
        rel_pos = i-1;
    }
    editor_draw(true);

    if(abs_pos == 0){
        calc_total_lines();
        editor_draw(false);
    }
}

void editor_cursor_move(enum key_type x){
    te_close_cursor();
    buffer_update();
    if(x == UP_ARROW) cursor_move_up();
    else if(x == DOWN_ARROW) cursor_move_down();
    else if(x == LEFT_ARROW) cursor_move_left();
    else cursor_move_right();

    editor_draw(false);
}

void editor_draw(bool force){
    static int last_basepos = -1;
    static int last_relpos;
    static int last_len;

    static int base_lines;
    static int rel_lines;

    /*
        If the base position has not changed, there is no need to redraw entire editor area.
        parameter force determines whether to redraw entire editor area even the base position has not changed. 
        (e.g., text insertion, deletion and terminal resize event.)
    */

    buffer_update();
    if(last_basepos != base_pos) base_lines = get_base_lines(base_pos);
    if(last_relpos != rel_pos) rel_lines = get_rel_lines();

    ui_set_status((base_lines + rel_lines), total_lines, file_name);

    if((last_basepos != base_pos) || (last_len != buf_len) || force)
        ui_draw_text((char*)(buf + base_pos), (buf_len - base_pos));
    last_basepos = base_pos;
    last_relpos = rel_pos;
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
    if(x_pos >= cols || y_pos >= rows-2){
        rel_pos = 0;
        x_pos = 0;
        y_pos = 0;
    }

    ui_cursor_move(x_pos, y_pos);
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
    if (!is_saved){
        ui_show_message("No write since last change (Press Ctrl-Q again to override)");
        ui_alert();
        ui_input_loop(quit_function_input_event, editor_resize_event);

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
typedef struct node{
    struct node* prev;
    struct node* next;

    int idx;
    int base_pos;
    int rel_pos;
} node_t;

static node_t* find_current_node;
static int find_node_count;
static char* find_pattern_str;
static int find_pattern_len;

void find_draw(){
    ui_set_status(get_base_lines(find_current_node->base_pos), total_lines, file_name);

    char message_buf[100];
    sprintf(message_buf, "Pattern \"%s\": %d / %d", find_pattern_str, find_current_node->idx+1, find_node_count);
    ui_show_message(message_buf);

    int base = find_current_node->base_pos;
    int rel = find_current_node->rel_pos;
    int len = buf_len-base > max_screen_pos() ? max_screen_pos() : buf_len-base;
    
    adjust_basepos_down(&base, &rel);
    char* screen_buf = (char*)malloc((max_screen_pos()*sizeof(char))+8);

    memcpy(screen_buf, buf + base, len);
    concat_string(screen_buf, "\x1b[7m", find_current_node->rel_pos, &len);
    concat_string(screen_buf, "\x1b[0m", find_current_node->rel_pos+find_pattern_len+4, &len);
    ui_draw_text(screen_buf, len);

    /*
        Windows CRT에서 free() 호출 시 ntdll!RtlRegisterSecureMemoryCacheCallback() assert가 발생하는 경우에는,
        Memory overflow가 발생하였는지 확인하십시오.
    */
    free(screen_buf);
}

bool find_input_event(enum key_type type, char c){
    if(type == LEFT_ARROW){
        if(find_current_node->prev == NULL) ui_alert();
        else find_current_node = find_current_node->prev;
    }
    else if(type == RIGHT_ARROW){
        if(find_current_node->next == NULL) ui_alert();
        else find_current_node = find_current_node->next;
    }
    else if(type == ESC) return false;
    else if(type == ENTER){
        base_pos = find_current_node->base_pos;
        rel_pos = find_current_node->rel_pos;
        adjust_basepos_down(&base_pos, &rel_pos);

        te_set_cursor(base_pos + rel_pos);
        return false;
    }
    else ui_alert();

    find_draw();
    return true;
}

void find_resize_event(){
    update_terminal_size();
    find_draw();
}

bool find_function(){
    find_pattern_str = calloc(PATTERN_LEN_MAX, sizeof(char));
    find_pattern_len = ui_show_prompt("Pattern: ", find_pattern_str, editor_resize_event);

    if(find_pattern_len == PROMPT_CANCELLED) return true;
    else if(find_pattern_len < PATTERN_LEN_MIN){
        ui_alert();
        ui_show_message("\x1b[1;31mFind: Pattern is too short.\x1b[0m");
        return true;
    }
    else if(find_pattern_len > PATTERN_LEN_MAX){
        ui_alert();
        ui_show_message("\x1b[1;31mFind: Pattern is too long.\x1b[0m");
        return true;
    }

    te_close_cursor();
    buffer_update();
    if(find_pattern_len > buf_len) goto find_exit;

    node_t* head = NULL;
    node_t* cur = NULL;
    int pattern_hash = hash(find_pattern_str, find_pattern_len);
    int window_hash = hash(buf, find_pattern_len);
    int found = 0;
    int current_line_base_pos = 0;

    //Rabin-Karp algorithm
    for(int i = 0; i < buf_len; i++){
        if(buf[i] == LF) current_line_base_pos = i+1;
        if(window_hash == pattern_hash && compare_string(buf + i, find_pattern_str, find_pattern_len)){
            node_t* node = (node_t*)malloc(sizeof(node_t));
            if(cur != NULL) cur->next = node;

            node->base_pos = current_line_base_pos;
            node->rel_pos = i - current_line_base_pos;
            node->next = NULL;
            node->prev = cur;
            node->idx = found++;

            cur = node;
            if(head == NULL)head = node;
        }

        int end = i + find_pattern_len;
        if(end > buf_len) break;

        //Sliding window
        window_hash = 2 * (window_hash - (buf[i] * power(2, find_pattern_len-1))) + buf[end];
    }

    find_current_node = head;
    find_node_count = found;
    
    if(head == NULL){
        find_exit:
        ui_alert();
        ui_show_message("\x1b[1;31mFind: No results.\x1b[0m");
        editor_draw_cursor();
        return true;
    }
    else{
        find_draw();
        ui_input_loop(find_input_event, find_resize_event);
        
        //Dispose all nodes
        do{
            node_t* tmp = head->next;
            free(head);
            head = tmp;
        }
        while(head != NULL);
    }

    free(find_pattern_str);
    ui_show_default_message();
    editor_draw(true);
    return true;
}
// #endregion

// #region Save
bool save_function(bool force_new_file){
    static bool is_string_malloc = false;
    if(is_saved){
        ui_alert();
        ui_show_message("There is no local changes to save.");
    }
    else{
        bool is_new_file = false;
        if(file_name == NULL || force_new_file){
            char buf[PROMPT_INPUT_LEN_MAX];
            if(ui_show_prompt("New file name: ", buf, editor_resize_event) == PROMPT_CANCELLED){
                editor_draw_cursor();
                return true;
            }
            is_new_file= true;

            if(is_string_malloc) free(file_name);
            file_name = (char*)malloc(PROMPT_INPUT_LEN_MAX * sizeof(char));
            is_string_malloc = true;
            strcpy(file_name, buf);
            editor_draw(false);
        }

        char str_buf[100];
        if(te_buffer_save(file_name)){
            sprintf(str_buf, "\"%s\"%s %dL, %dB written.", file_name, (is_new_file ? " [New]" : ""), total_lines, buf_len);
            ui_show_message(str_buf);
            is_saved = true;
        }
        else{
            ui_alert();
            ui_show_message("\x1b[1;31mERR_FILE_SAVE: Unable to write buffer to file.\x1b[0m");
            is_saved = false;
        }
    }

    editor_draw_cursor();
    return true;
}
// #endregion