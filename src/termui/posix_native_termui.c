/*
*   posix_native_termui.c - fedit <2024-2 Advanced Data Structure>
*   Copyright (C) 2024 Coppermine-SP <창원대학교 컴퓨터공학과 20233063 손유찬>
*/

#ifdef __APPLE__ || __linux__ || __linux
#include "native_termui.h"
#include <sys/ioctl.h>
#include <unistd.h>

terminal_size_t get_terminal_size(){
    struct winsize w;
    terminal_size_t result;

    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    result.cols = w.ws_col;
    result.rows = w.ws_row;
    return result;
}

#endif
