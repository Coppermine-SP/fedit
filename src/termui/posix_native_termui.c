/*
*   posix_native_termui.c - fedit <2024-2 Advanced Data Structure>
*   Copyright (C) 2024 Coppermine-SP <창원대학교 컴퓨터공학과 20233063 손유찬>
*/

#if defined __APPLE__ || defined __linux__ || defined __linux
#include "native_termui.h"
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>

terminal_size_t get_terminal_size(){
    struct winsize w;
    terminal_size_t result;

    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    result.cols = w.ws_col;
    result.rows = w.ws_row;
    return result;
}

void enable_raw_input(){
    struct termios attribute;
    tcgetattr(STDIN_FILENO, &attribute);
    attribute.c_iflag &= ~(IXON);
    attribute.c_oflag &= ~(OPOST);
    attribute.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &attribute);
}

void get_raw_input(char* out){ read(STDIN_FILENO, out, 1); }

#endif
