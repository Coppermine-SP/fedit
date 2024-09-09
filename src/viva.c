/*
 *   viva.c - fedit <2024-2 Advanced Data Structure>
 *   Copyright (C) 2024 Coppermine-SP <창원대학교 컴퓨터공학과 20233063 손유찬>
 */

#include "textengine/textengine.h"
#include "termui/termui.h"
#include <stdio.h>

// #region Global variables
static char *file_name;
static bool is_saved = false;
// #endregion

// #region Function prototypes
bool quit_function();
bool find_function();
bool save_function();
// #endregion

bool input_event(char c)
{
    if (c == CTRL_KEY('q')) return quit_function();
    else if (c == CTRL_KEY('f')) return find_function();
    else if (c == CTRL_KEY('s')) return save_function();
    else
    {
        ui_render(te_get_screen_buffer(), 100);
    }
    return true;
}

int main(int argc, char *argv[])
{
    file_name = argc > 1 ? argv[1] : NULL;
    te_init(file_name);
    ui_init();

    // 파일을 불러오지 않았을 경우, MOTD 출력
    if (argc == 1)
        ui_render_motd(true);

    ui_set_status(0, 0, file_name);
    ui_input_loop(input_event);
}

// #region Quit function
static bool quit_override = false;
bool quit_function_input_event(char c)
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
    return false;
}
// #endregion