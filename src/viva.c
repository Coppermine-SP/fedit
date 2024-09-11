/*
 *   viva.c - fedit <2024-2 Advanced Data Structure>
 *   Copyright (C) 2024 Coppermine-SP (copperminesp@cloudint.corp) - <창원대학교 컴퓨터공학과 20233063 손유찬>
 *   See this repository from GitHub: https://github.com/Coppermine-SP/fedit
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "textengine/textengine.h"
#include "termui/termui.h"
#include "termui/termui_types.h"

// #region Macro constants
#define MAX_FILE_NAME_SIZE 128
// #endregion

// #region Global variables
static char* file_name;
static bool is_saved = false;

static unsigned x_pos = 0;
static unsigned y_pos = 0;
// #endregion

// #region Function prototypes
bool quit_function();
bool find_function();
bool save_function();
// #endregion

bool input_event(enum key_type type, char c)
{
    if(type != NORMAL_KEY){
        if(type == CONTROL_KEY){
            if (c == CTRL_KEY('q')) return quit_function();
            else if (c == CTRL_KEY('f')) return find_function();
            else if (c == CTRL_KEY('s')) return save_function();
        }
        else if(type == UP_ARROW) printf("UP ");
        else if(type == DOWN_ARROW) printf("DOWN ");
        else if(type == RIGHT_ARROW) printf("RIGHT ");
        else if(type == LEFT_ARROW) printf("LEFT ");
        else if(type == ENTER) printf("ENTER ");
        else if(type == BACKSPACE) printf("BACKSPACE ");
        else if(type == PGUP) printf("PGUP ");
        else if(type == PGDOWN) printf("PGDOWN ");
        else if(type == HOME) printf("HOME ");
        else if(type == END) printf("END ");
    }
    else{
        //ui_render(te_get_screen_buffer(), 100);
        ui_cursor_move(1, 1);
    }
    return true;
}

int main(int argc, char *argv[])
{
    file_name  = argc > 1 ? argv[1] : NULL;
    te_init(file_name);
    ui_init();

    // 파일을 불러오지 않았을 경우, MOTD 출력
    if (argc == 1)
        ui_render_motd(true);

    ui_set_status(0, 0, file_name);

    ui_cursor_move(0, 0);
    ui_input_loop(input_event);
}

// #region Quit function
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
            return true;
        }
    }

    te_dispose();
    ui_dispose();
    return false;
}
// #endregion

// #region Find function
bool find_function(){
    return false;
}
// #endregion

// #region Save function
bool save_function(){
    if(is_saved){
        ui_alert();
        ui_show_message("There is no local changes to save.");
    }
    else{

    }

    return true;
}
// #endregion