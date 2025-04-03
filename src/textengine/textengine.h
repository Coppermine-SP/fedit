/*
    textengine.h - fedit <2024-2 Advanced Data Structure>
    Copyright (C) 2024 Coppermine-SP <창원대학교 컴퓨터공학과 20233063 손유찬>
*/

#include <stdbool.h>

#define IRESULT_SUCEESS 0
#define IRESULT_NOENT_ERROR 1
#define IRESULT_ACCESS_ERROR 2
#define IRESULT_UNKNOWN_ERROR 3
typedef int IRESULT;

/// @brief TextEngine을 초기화합니다.
/// @param file_name (nullable) 파일 이름
/// @return 초기화 결과
IRESULT te_init(const char* file_name);

/// @brief 종료하기 위해 정리합니다.
void te_dispose();

/// @brief 커서를 닫습니다.
void te_close_cursor();

/// @brief 커서 위치를 지정합니다.
/// @param pos 이동 할 절대 위치
void te_set_cursor(int pos);

/// @brief 현재 커서 위치에 문자를 삽입합니다.
/// @param x 삽입 할 문자
void te_insert(char x);

/// @brief 현재 커서 위치에서 문자를 삭제합니다.
void te_delete();

/// @brief 현재 텍스트 버퍼를 파일에 저장힙니다.
/// @param file_name 저장 할 파일
/// @return 저장 성공 여부
bool te_buffer_save(char* const file_name);

/// @brief 텍스트 버퍼를 가져옵니다.
/// @param len 버퍼 길이를 저장 할 intptr
/// @return 텍스트 버퍼 charptr
const char* te_get_buffer(int* len);
