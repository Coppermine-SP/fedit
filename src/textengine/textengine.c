/*
*   textengine.c - fedit <2024-2 Advanced Data Structure>
*   Copyright (C) 2024 Coppermine-SP <창원대학교 컴퓨터공학과 20233063 손유찬>
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define GAP_SIZE 15

static char* buf;
static long buf_len;

static bool is_gap_open = false;
static char* gap_begin;

void te_init(char* const file_name){
    if(file_name != NULL){
        FILE* file = fopen(file_name, "r");

        if(file == NULL) goto empty_file;
        fseek(file, 0, SEEK_END);
        buf_len = ftell(file);
        fseek(file, 0, SEEK_SET);

        buf = (char*)malloc(buf_len * sizeof(char));
        fread(buf, sizeof(char), buf_len, file);
    }
    else{
        empty_file:

        buf_len = 1;
        buf = (char*)calloc(1, sizeof(char));
    }
}

void te_dispose(){
    
}

const char* te_get_screen_buffer(){
    return buf;
}

void gap_open(){
    if(!is_gap_open)
    {
        int cursor = 2;
        buf = realloc(buf, buf_len + GAP_SIZE);

        //갭 크기만큼 현재 커서에서 배열 시프트
        memmove((buf + cursor + GAP_SIZE), (buf + cursor) , (buf_len - cursor));

        //갭 공간 메모리 초기화
        memset(buf + cursor, 0, GAP_SIZE);
        is_gap_open = true;
    }
}