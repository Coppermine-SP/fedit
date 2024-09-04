/*
*   viva.c - fedit <2024-2 Advanced Data Structure>
*   Copyright (C) 2024 Coppermine-SP <창원대학교 컴퓨터공학과 20233063 손유찬>
*/

#include "textengine/textengine.h"
#include "termui/termui.h"
#include <stdio.h>

bool input_event(char c){
    if(c == CTRL_KEY('q'))return false;
    else if(c == CTRL_KEY('F')){
        ui_show_prompt("Find");
    }
    return true;
}

int main(int argc, char* argv[]){
    te_init(argc < 1 ? argv[1] : NULL);
    ui_init((argc == 1));

    //파일을 불러오지 않았을 경우, MOTD 출력
    if(argc == 1) ui_set_motd(true);

    ui_input_loop(&input_event);
}
