/*
*   termui_types.h - fedit <2024-2 Advanced Data Structure>
*   Copyright (C) 2024 Coppermine-SP <창원대학교 컴퓨터공학과 20233063 손유찬>
*/

#ifndef _TERMUI_TYPES_H_
#define _TERMUI_TYPES_H_

#define CTRL_KEY(x) ((x)&0x1f)
#define ESC_KEY 27
#define ENTER_KEY 13
#define BACKSPACE_KEY 127

enum key_type{
    NORMAL_KEY,
    CONTROL_KEY,
    ESC,
    HOME,
    END,
    PGUP,
    PGDOWN,
    UP_ARROW,
    DOWN_ARROW,
    LEFT_ARROW,
    RIGHT_ARROW,
    TIMEOUT
};

#endif