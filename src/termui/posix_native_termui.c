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

#define CSI 91
#define MOUSE_POS 'M'
#define ESC_KEY 27
#define ENTER_KEY 13
#define BACKSPACE_KEY 127
#define HOME_KEY 72
#define END_KEY 70
#define PGUP_KEY 53
#define PGDOWN_KEY 54
#define PG_COMMON_END 126
#define UP_ARROW_KEY 65
#define DOWN_ARROW_KEY 66
#define LEFT_ARROW_KEY 68
#define RIGHT_ARROW_KEY 67

static struct termios default_attribute;

terminal_size_t nt_get_terminal_size(){
    struct winsize w;
    terminal_size_t result;

    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    result.cols = w.ws_col;
    result.rows = w.ws_row;
    return result;
}

void nt_configure_term_env(){
    tcgetattr(STDIN_FILENO, &default_attribute);
    struct termios attribute = default_attribute;

    attribute.c_iflag &= ~(ICRNL | IXON | BRKINT | INPCK);
    attribute.c_oflag &= ~(OPOST);
    attribute.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    attribute.c_cflag |= (CS8);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &attribute);

    /*
        Enable mouse tracking for prevent scroiling in the console.

        About XTerm Control Sequences:
        https://invisible-island.net/xterm/ctlseqs/ctlseqs.html
    */

    //1049h: Saving the cursor, switch to the Alternate Screen Buffer.
    //1000h: Enable Mouse Tracking.
    //7l: Disable Auto-Wrap Mode.
    printf("\x1b[?1049h\x1b[?1000h\x1b[?7l");
}

void nt_restore_term_env(){
    //1049l: Use Normal Screen Buffer and restore cursor.
    //1000l: Disable Mouse Tracking.
    //7h: Enable Auto-Wrap Mode.
    printf("\x1b[?1049l\x1b[?1000l\x1b[?7h");
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &default_attribute);

    puts("");
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
                if(buf[0] == CSI && nt_get_raw_input((buf + 1)) != TIMEOUT){
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
                        case MOUSE_POS:
                            for(int i = 0; i < 6; i++){
                                if(nt_get_raw_input(NULL) == TIMEOUT) break;
                            }
                            return TIMEOUT;
                        default:
                            return TIMEOUT;
                    }
                }
                else return ESC;
            }
            else if(c == ENTER_KEY) return ENTER;
            else if(c == BACKSPACE_KEY) return BACKSPACE;
            else{
                if(out != NULL) *out = c;
                return CONTROL_KEY;
            }
        }
        else{
            if(out != NULL) *out = c;
            return NORMAL_KEY;
        }
    }
    else{
        return TIMEOUT;
    }
}

#endif
