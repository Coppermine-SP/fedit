/*
*   win32_native_termui.c - fedit <2024-2 Advanced Data Structure>
*   Copyright (C) 2024 Coppermine-SP <창원대학교 컴퓨터공학과 20233063 손유찬>
*/

#if defined _WIN32 || defined _WIN64
#include <windows.h>
#include <conio.h>
#include <stdio.h>
#include "native_termui.h"

#define ESC_KEY 27
#define ENTER_KEY 13
#define BACKSPACE_KEY 8
#define SPECIAL_KEY_BEGIN -32
#define UP_ARROW_KEY 72
#define DOWN_ARROW_KEY 80
#define LEFT_ARROW_KEY 75
#define RIGHT_ARROW_KEY 77
#define PGUP_KEY 73
#define PGDOWN_KEY 81
#define HOME_KEY 71
#define END_KEY 79

terminal_size_t nt_get_terminal_size(){
    CONSOLE_SCREEN_BUFFER_INFO info;
    terminal_size_t result;
  
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
    result.cols =  info.srWindow.Right - info.srWindow.Left + 1;
    result.rows = info.srWindow.Bottom - info.srWindow.Top + 1;
    return result;
}

void nt_configure_term_env(){
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dw_mode;

    GetConsoleMode(handle, &dw_mode);
    dw_mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(handle, dw_mode);

    /*
        There is no need to enable mouse tracking to intercept mouse wheel event in Win32.
        In Windows Terminal, mouse wheel inputs are the same as up and down arrow keys.

        About Windows Console Virtual Terminal Sequences:
        https://learn.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences
    */

    //1049h: Saving the cursor, switch to the Alternate Screen Buffer.
    //2 SP q: Steady block cursor shape.
    printf("\x1b[?1049h\x1b[2 q");
}

void nt_restore_term_env(){
    //1049l: Use Normal Screen Buffer and restore cursor.
    //0 SP q: Default cursor shape configured by the user.
    printf("\x1b[?1049l\x1b[0 q");
}

enum key_type nt_get_raw_input(char* out){
    char c;
    if(kbhit()){
        c = _getch();

        if(iscntrl(c)){
            if(c == ESC_KEY) return ESC;
            else if (c == ENTER_KEY) return ENTER;
            else if(c == BACKSPACE_KEY) return BACKSPACE;
            else{
                *out = c;
                return CONTROL_KEY;
            }
        }
        //I love Windows so much. Windows is the best operating system in the world.
        else if(c == SPECIAL_KEY_BEGIN){
            char buf;
            if(nt_get_raw_input(&buf) == TIMEOUT) return TIMEOUT;

            switch(buf){
                case UP_ARROW_KEY:
                    return UP_ARROW;
                case DOWN_ARROW_KEY:
                    return DOWN_ARROW;
                case LEFT_ARROW_KEY:
                    return LEFT_ARROW;
                case RIGHT_ARROW_KEY:
                    return RIGHT_ARROW;
                case PGUP_KEY:
                    return PGUP;
                case PGDOWN_KEY:
                    return PGDOWN;
                case HOME_KEY:
                    return HOME;
                case END_KEY:
                    return END;
                default:
                    return TIMEOUT;
            }

        }
        else{
            *out = c;
            return NORMAL_KEY;
        }

    }
    else{
        Sleep(1);
        return TIMEOUT;
    }
}
#endif