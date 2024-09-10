/*
*   posix_native_termui.c - fedit <2024-2 Advanced Data Structure>
*   Copyright (C) 2024 Coppermine-SP <창원대학교 컴퓨터공학과 20233063 손유찬>
*/

#if defined __APPLE__ || defined __linux__ || defined __linux
#include "native_termui.h"
#include "termui_types.h"
#include <sys/ioctl.h>
#include <sys/select.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#define HOME_KEY 72
#define END_KEY 70
#define PGUP_KEY 53
#define PGDOWN_KEY 54
#define PG_COMMON_END 126
#define UP_ARROW_KEY 65
#define DOWN_ARROW_KEY 66
#define LEFT_ARROW_KEY 68
#define RIGHT_ARROW_KEY 67

terminal_size_t nt_get_terminal_size(){
    struct winsize w;
    terminal_size_t result;

    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    result.cols = w.ws_col;
    result.rows = w.ws_row;
    return result;
}

void nt_configure_term_env(){
    struct termios attribute;
    tcgetattr(STDIN_FILENO, &attribute);
    attribute.c_iflag &= ~(ICRNL | IXON | BRKINT | INPCK);
    attribute.c_oflag &= ~(OPOST);
    attribute.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    attribute.c_cflag |= (CS8);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &attribute);

    //switch to the alternate screen buffer mode.
    printf("\033[?1049h\033[2J\033[H");
}

enum key_type nt_get_raw_input(char* out){ 
    struct timeval tv;
    fd_set fds;

    tv.tv_sec = 0;
    tv.tv_usec = 100000;

    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);

    int ret = select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);

    if(ret > 0){
        char c;
        read(STDIN_FILENO, &c, 1);

        if(iscntrl(c)){
            if(c == ESC_KEY){
                char buf[3];
                if(nt_get_raw_input(buf) == TIMEOUT) return ESC;
                if(buf[0] == '[' && nt_get_raw_input((buf + 1)) != TIMEOUT){
                    switch(buf[1]){
                        case UP_ARROW_KEY:
                            return UP_ARROW;
                        case DOWN_ARROW_KEY:
                            return DOWN_ARROW;
                        case LEFT_ARROW_KEY:
                            return LEFT_ARROW;
                        case RIGHT_ARROW_KEY:
                            return RIGHT_ARROW;
                        case HOME_KEY:
                            return HOME;
                        case END_KEY:
                            return END;
                        case PGUP_KEY:
                        case PGDOWN_KEY:
                            if(nt_get_raw_input((buf + 2)) != TIMEOUT && buf[2] == PG_COMMON_END){
                                if(buf[1] == PGUP_KEY) return PGUP;
                                else return PGDOWN;
                            }
                            return ESC;
                    }
                }

                else return ESC;
            }
            else{
                *out = c;
                return CONTROL_KEY;
            }
        }
        else{
            *out = c;
            return NORMAL_KEY;
        }
    }
    else{
        return TIMEOUT;
    }

}

#endif
