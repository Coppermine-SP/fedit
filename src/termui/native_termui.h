/*
    native_termui.h - fedit <2024-2 Advanced Data Structure>
    Copyright (C) 2024 Coppermine-SP <창원대학교 컴퓨터공학과 20233063 손유찬>
*/

#include <stdbool.h>
#include "termui_types.h"

#define INPUT_TIMEOUT 10

typedef struct {
    int cols;
    int rows;
} terminal_size_t;

terminal_size_t nt_get_terminal_size();
void nt_configure_term_env();
void nt_restore_term_env();
const char* const nt_get_platform_name();
enum key_type nt_get_raw_input(char* out);

