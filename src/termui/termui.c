/*
    termui.c - fedit <2024-2 Advanced Data Structure>
    Copyright (C) 2024 Coppermine-SP <창원대학교 컴퓨터공학과 20233063 손유찬>
*/

//#define DEBUG_SHOW_EMPTY_SPACE

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "termui.h"
#include "native_termui.h"
#include "termui_types.h"

// #region Macro constants
#define PROMPT_INPUT_BUFFER_SIZE 100
#define MESSAGE_BUFFER_SIZE 150
#define SCREEN_BUFFER_SIZE 10000
#define TAB_SPACE 8

#define TITLE_STRING "Welcome to fedit (Visual Text Editor)."
#define SUBTITLE_STRING "See this repository on GitHub: Coppermine-SP/fedit"
#define COPYRIGHT_STRING "Copyright (C) 2024-2025 Coppermine-SP."
#define DEFAULT_MESSAGE_STRING "HELP: Ctrl-S = Save | Ctrl-Q = Quit | Ctrl-F = Find"
#define DEFAULT_FILE_NAME_STRING "No Name"
#define DEFAULT_FILE_TYPE_STRING "no ft"
#define EMPTY_ROW_STRING "\x1b[2m~\x1b[0m"
// #endregion

// #region Macro functions
#define CURSOR_GOTO(row, col) printf("\x1b[%d;%dH", (row), (col))
#define CURSOR_HIDE printf("\x1b[?25l")
#define CURSOR_SHOW printf("\x1b[?25h")
#define CURSOR_SAVE printf("\x1b[s")
#define CURSOR_RESTORE printf("\x1b[u")
#define CLEAR_LINE printf("\x1b[K")
#define CLEAR_SCREEN printf("\x1b[2J")
#define CLEAR_UP printf("\x1b[1J")
#define COLOR_INVERT printf("\x1b[7m")
#define COLOR_NORMAL printf("\x1b[0m")
// #endregion

// #region Global variables
static void (*resize_callback)();
static char* screen_buf = NULL;
static terminal_size_t terminal_size;
static status_t status;
static bool motd_showing = false;
// #endregion

void ui_alert(){
    printf("\a");
}

// Message must be buffered. because message bar needs to be redrawn during a terminal resize event.
static char message_buf[MESSAGE_BUFFER_SIZE];
void ui_show_message(const char* msg){
    CURSOR_HIDE;
    CURSOR_GOTO(terminal_size.rows, 0);
    CLEAR_LINE;
    if(msg != NULL)strcpy(message_buf, msg);
    printf("%s", message_buf);
}

void ui_show_default_message(){
    ui_show_message(DEFAULT_MESSAGE_STRING);
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

static void draw_status(){
    //Prevent screen flashing via buffered stdout
    setvbuf(stdout, screen_buf, _IOFBF, terminal_size.rows * terminal_size.cols);
    CURSOR_HIDE;
    COLOR_INVERT;
    CURSOR_GOTO(terminal_size.rows-1, 0);
    CLEAR_LINE;

    char left_buf[40];
    char right_buf[20];
    char filetype_string[15];
    bool has_filetype = false;

    if(status.file_name != NULL)
    has_filetype = get_file_type(status.file_name, filetype_string, 15);
    
    int left = sprintf(left_buf,"[%s] - %d lines", (status.file_name == NULL ? DEFAULT_FILE_NAME_STRING : status.file_name), status.total_lines);
    int right = sprintf(right_buf,"%s | %d/%d ",(has_filetype) ? filetype_string : DEFAULT_FILE_TYPE_STRING ,status.cursor_line, status.total_lines);

    printf("%s",left_buf);
    for(int i = 0; i < terminal_size.cols - (left + right); i++) printf(" ");
    printf("%s ", right_buf);
    COLOR_NORMAL;

    fflush(stdout);
    setvbuf(stdout, NULL, _IONBF, 0);
}

void ui_set_motd(bool state){
    CURSOR_HIDE;
    CURSOR_GOTO((terminal_size.rows/2)-3, (int)(terminal_size.cols - strlen(TITLE_STRING)) / 2);

    if(state)printf("%s", TITLE_STRING);
    else CLEAR_LINE;

    CURSOR_GOTO((terminal_size.rows/2)-1, (int)(terminal_size.cols - strlen(SUBTITLE_STRING)) / 2);
    if(state)printf("%s", SUBTITLE_STRING);
    else CLEAR_LINE;

    CURSOR_GOTO((terminal_size.rows/2), (int)(terminal_size.cols - strlen(COPYRIGHT_STRING)) / 2);
    if(state)printf("%s", COPYRIGHT_STRING);
    else CLEAR_LINE;
    
    motd_showing = state;
    ui_cursor_move(0,0);
    
}

void ui_set_status(int cursor_line, int total_lines, char* const file_name){
    status.cursor_line = cursor_line;
    status.file_name = file_name;
    status.total_lines = total_lines;

    draw_status();
}

void check_terminal_size(){
    terminal_size_t size = nt_get_terminal_size();
    if(size.cols != terminal_size.cols || size.rows != terminal_size.rows){
        terminal_size = size;
        if(motd_showing) ui_set_motd(false);
        ui_show_message(NULL);
        resize_callback();
    }
}

void ui_input_loop(bool (*callback)(enum key_type type, char c)){
    while(true){
        char c;
        enum key_type type;

        type = nt_get_raw_input(&c);
        check_terminal_size();
        if(type == TIMEOUT) continue;

        if(motd_showing) ui_set_motd(false);
        if(!callback(type, c)) break;
    }
}

static char* prompt_msg;
static char prompt_input_buf[PROMPT_INPUT_BUFFER_SIZE];
static char prompt_message_buf[MESSAGE_BUFFER_SIZE];
static int prompt_input_idx;
static bool prompt_input_event(enum key_type type, char c){
    /*
        I considered making this function to nested functions in ui_show_prompt()
        to remove global variables for this prompt behavior and limit the visiblity of this function.

        However, nested functions are not part of the C standard; it is an extension in GNU C.
        https://gcc.gnu.org/onlinedocs/gcc/Nested-Functions.html
    */
    if(prompt_input_idx - 1 > PROMPT_INPUT_BUFFER_SIZE) return false;

    if(type == ESC){
        prompt_input_buf[0] = '\0';
        return false;
    }
    else if(type == ENTER){
        prompt_input_buf[prompt_input_idx] = '\0';
        return false;
    }
    else if(type == BACKSPACE){
        if(prompt_input_idx == 0) return false;
        prompt_input_buf[--prompt_input_idx] = '\0';
    }
    else if(type == NORMAL_KEY){
        prompt_input_buf[prompt_input_idx++] = c;
    }
    else return true;
    
    strcpy(prompt_message_buf, prompt_msg);
    strcat(prompt_message_buf, prompt_input_buf);
    ui_show_message(prompt_message_buf);
    return true;
}

bool ui_show_prompt(char* const msg, char* buf){
    ui_alert();
    prompt_input_idx = 0;
    memset(prompt_input_buf, 0, PROMPT_INPUT_BUFFER_SIZE);
    prompt_msg = msg;
    ui_show_message(msg);
    ui_input_loop(&prompt_input_event);

    ui_show_message(DEFAULT_MESSAGE_STRING);
    if(prompt_input_buf[0] == '\0') return false;

    strcpy(buf, prompt_input_buf);
    return true;
}

void ui_draw_text(const char* begin, int len){
    //Prevent screen flashing via buffered stdout
    setvbuf(stdout, screen_buf, _IOFBF, terminal_size.rows * terminal_size.cols);
    CURSOR_HIDE;
    CURSOR_GOTO(terminal_size.rows-2, terminal_size.cols);
    CLEAR_UP;
    CURSOR_GOTO(0, 0);
    
    const char* cur = begin;
    int line_pos = 0;
    int line_left = 0;
    for(int i = 1; i < terminal_size.rows - 1; i++){
        CURSOR_GOTO(i, 0);

        if((cur - begin) < len){
            for(int j = 0; j < terminal_size.cols; j++){
                if((cur - begin) >= len) break;

                if(line_left > 0){
                    /*
                       I don't like this tab spcaing behavior.
                       There must be a more concise implementation, but i can't spend any more time on this.
                    */
                    for(int k = 0; k < line_left; k++) printf(" ");
                    line_left = 0;
                }

                if(*cur == '\r' || *cur == 0){
                    if(*cur == 0){
                        if(len == 1) printf(EMPTY_ROW_STRING);
                        #ifdef DEBUG_SHOW_EMPTY_SPACE
                        else{
                            printf("_");
                            j++;
                        }
                        #endif   
                    }
                    cur++;
                    j--;
                    continue;
                }
                else if(*cur == '\n'){
                    line_pos = 0;
                    cur++;
                    break;
                }
                else if(*cur == '\t'){
                    int space = TAB_SPACE - (line_pos % TAB_SPACE);
                    line_pos += space;
                    for(int k = 0; k < space; k++, j++){
                        if(j >= terminal_size.cols){
                            line_left = space - k;
                            break;
                        }
                        else printf(" ");
                        
                    }

                    j--;
                    cur++;
                }
                else {
                    printf("%c", *(cur++));
                    line_pos++;
                }

            }
        }
        else printf(EMPTY_ROW_STRING);  
    }

    fflush(stdout);
    setvbuf(stdout, NULL, _IONBF, 0);
}

void ui_get_terminal_size(int* cols, int* rows){
    if(cols != NULL) *cols = terminal_size.cols;
    if(rows != NULL) *rows = terminal_size.rows;
}

void ui_cursor_move(unsigned x, unsigned y){
    if(x > terminal_size.cols || y > (terminal_size.rows -2)) return;

    CURSOR_GOTO(y+1, x+1);
    CURSOR_SHOW;
}

void ui_init(void (*callback)(int cols, int rows)){
    resize_callback = callback;
    setvbuf(stdout, NULL, _IONBF, 0);
    nt_configure_term_env();
    CLEAR_SCREEN;

    terminal_size = nt_get_terminal_size();
    screen_buf = calloc(SCREEN_BUFFER_SIZE, sizeof(char));
    ui_show_message(DEFAULT_MESSAGE_STRING);
}

void ui_dispose(){
    CLEAR_SCREEN;
    free(screen_buf);
    nt_restore_term_env();
}
 