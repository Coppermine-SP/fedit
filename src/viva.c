/*
*   viva.c - fedit <2024-2 Advanced Data Structure>
*   Copyright (C) 2024 Coppermine-SP <창원대학교 컴퓨터공학과 20233063 손유찬>
*/

#include "textengine/textengine.h"
#include "termui/termui.h"
#include <stdio.h>

const char* file_name;

bool input_event(char c){
    if(c == CTRL_KEY('q'))return false;
    else if(c == CTRL_KEY('F')){
        char* str = ui_show_prompt("Find");
    }
    else{
        ui_render(te_get_screen_buffer(), 100);
    }
    return true;
}

int main(int argc, char* argv[]){
    file_name = argc > 1 ? argv[1] : NULL;
    te_init(file_name);
    ui_init();

    //파일을 불러오지 않았을 경우, MOTD 출력
    if(argc == 1) ui_render_motd(true);

    ui_set_status(0, 0, file_name);


    
    ui_input_loop(&input_event);
}
