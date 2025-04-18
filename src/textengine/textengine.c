/*
    textengine.c - fedit <2024-2 Advanced Data Structure>
    Copyright (C) 2024 Coppermine-SP <창원대학교 컴퓨터공학과 20233063 손유찬>
*/

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include "textengine.h"

// #region Macro constants
#define GAP_SIZE 30
#define MEMORY_EXPANSION_FACTOR 5
#define BUF_INIT_SIZE 1024
// #endregion

// #region Global variables
static char* buf;
static int buf_len;
static int buf_high_watermark;

static int cursor_pos;
static bool gap_opened;
static int gap_begin;
// #endregion

// #region Internal functions
void gap_open(){
    if(buf_len + GAP_SIZE >= buf_high_watermark){
        buf = realloc(buf, buf_len + (GAP_SIZE * MEMORY_EXPANSION_FACTOR));
        buf_high_watermark = buf_len + (GAP_SIZE * MEMORY_EXPANSION_FACTOR);
    }
    
    //갭 크기만큼 현재 커서에서 배열 시프트
    memmove((buf + cursor_pos + GAP_SIZE), (buf + cursor_pos), (buf_len - cursor_pos));

    //갭 공간 메모리 초기화
    memset(buf + cursor_pos, 0, GAP_SIZE);
    gap_begin = cursor_pos;
    buf_len += GAP_SIZE;
    gap_opened = true;
}

void gap_close(){
    int delta = cursor_pos - gap_begin;
    int new_len = buf_len - GAP_SIZE + delta;
    memmove((buf + gap_begin + delta), (buf + gap_begin + GAP_SIZE), (new_len - cursor_pos));

    buf_len = new_len;
    gap_opened = false;
}
// #endregion

void te_set_cursor(int pos){
    if(gap_opened && (pos < gap_begin || pos > (gap_begin + GAP_SIZE))) gap_close();
    cursor_pos = pos;
}

void te_insert(char x){
    if(!gap_opened) gap_open();

    if(cursor_pos == (gap_begin + GAP_SIZE)) gap_open();
    buf[cursor_pos] = x;
}

void te_delete(){
    if(gap_opened && cursor_pos <= (gap_begin + GAP_SIZE)) buf[cursor_pos] = 0;
    else{
        memmove((buf + cursor_pos), (buf + cursor_pos + 1), (buf_len - cursor_pos-1));
        buf_len--;
    }
}

void te_close_cursor(){
    if(gap_opened) gap_close();
}

const char* te_get_buffer(int* len){
    *len = buf_len;
    return buf;
}

bool te_buffer_save(char* const file_name){
    if(file_name == NULL) return false;
    FILE* file = fopen(file_name, "wb");

    if(file == NULL) return false;
    if(gap_opened) gap_close();

    int written = fwrite(buf, sizeof(char), buf_len, file);
    fclose(file);

    return buf_len == written;
}

IRESULT te_init(const char* file_name){
    int err = 0;
    if(file_name != NULL){
        /*
            Only binary mode has well-defined behavior in standard C.
            In text mode, using ftell() to determine length of LF text in Windows will munging of newlines takes that messes up offsets.
        */
        FILE* file = fopen(file_name, "rb");

        if(file == NULL){
            err = errno;
            goto empty_file;
        }
        fseek(file, 0, SEEK_END);
        buf_len = ftell(file);
        fseek(file, 0, SEEK_SET);

        buf = (char*)calloc(buf_len, sizeof(char));
        fread(buf, sizeof(char), buf_len, file);
        fclose(file);
    }
    else{
        empty_file:
        buf_len = 0;
        buf = (char*)calloc(BUF_INIT_SIZE, sizeof(char));
    }

    buf_high_watermark = BUF_INIT_SIZE;
    gap_opened = false;
    gap_begin = 0;
    cursor_pos = 0;

    if(err == 0) return IRESULT_SUCEESS;
    else if(err == 2) return IRESULT_NOENT_ERROR;
    else if(err == 13) return IRESULT_ACCESS_ERROR;
    else return IRESULT_UNKNOWN_ERROR;
}

void te_dispose(){
    free(buf);
}