/*
*   win32_native_termui.c - fedit <2024-2 Advanced Data Structure>
*   Copyright (C) 2024 Coppermine-SP <창원대학교 컴퓨터공학과 20233063 손유찬>
*/

#if defined _WIN32 || defined _WIN64
#include "native_termui.h"
#include <windows.h>

terminal_size_t get_terminal_size(){
    CONSOLE_SCREEN_BUFFER_INFO info;
    terminal_size_t result;
  
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
    result.cols =  info.srWindow.Right - info.srWindow.Left + 1;
    result.rows = info.srWindow.Bottom - info.srWindow.Top + 1;
    return result;
}

void enable_raw_input(){

}

void get_raw_input(char* out){
    
}
#endif