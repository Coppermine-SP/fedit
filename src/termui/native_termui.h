/*
*   native_termui.h - fedit <2024-2 Advanced Data Structure>
*   Copyright (C) 2024 Coppermine-SP <창원대학교 컴퓨터공학과 20233063 손유찬>
*/

typedef struct {
    int cols;
    int rows;
} terminal_size_t;

terminal_size_t get_terminal_size();
