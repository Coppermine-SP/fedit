# fedit
<img src="https://img.shields.io/badge/c-064F8C?style=for-the-badge&logo=c&logoColor=white">

Lightweight, Open-Source Visual Text Editor Project.

<p align="center">
<img src="/img/sample_1.png" style="width:50%"><img src="/img/sample_2.png" style="width:50%">
</p>

fedit is a lightweight, C-based visual text editor that implements all core editing features. It is a cross-platform project designed to run on both POSIX-compliant systems and Windows.

**Compatibility checked on:**
|Compiler|Target|OS|Checked|
|-|-|-|:-:|
|GCC 11.4.0|aarch64-linux-gnu|Ubuntu 24.04.5 LTS|✅|
|Apple clang 15.0.0|arm64-apple-darwin23.5.0|macOS Sequoia 15.1|✅|
|Microsoft Visual C++ 1936|x86-64|Windows 11 Enterprise x64|✅|
|GCC 13.2.0|x86_64-w64-mingw32|Windows 11 Enterprise x64|✅|

### Table of Contents
- [Features](#features)
- [Requirements](#requirements)
- [Project Status Tracker](#project-status-tracker)
- [Technical Documentation](/doc/index.md)

## Features
### 구현된 기능
- 동적 화면 리사이징
- 텍스트 보기 및 편집
- 방향 키, Home, End, PgUp, PgDn 키를 통한 커서 이동
- 기존 파일 불러오기 및 저장
- 탐색

### 명령 및 조작
- 새 파일 (Ctrl + N)
- 파일 열기 (Ctrl + O)
- 저장 (Ctrl + S)
- 다른 이름으로 저장 (Ctrl + A)
- 탐색 (Ctrl + F)
- 나가기 (Ctrl + Q)
- 행 스크롤 (방향키 위 또는 아래, 마우스 휠)
- 열 스크롤 (방향키 왼쪽 또는 오른쪽, 마우스 왼쪽 키 + 마우스 휠)
- 페이지 업 다운 (PgUp, PgDn)
- 문단 처음 / 끝 (Home, End)
- - -

명령 인수에 파일 이름을 명시하여 해당 파일로 새 에디터 세션을 여십시오.
```
fedit /etc/hosts
```

## Requirements
- POSIX 호환 운영 체제 (Linux, macOS) 또는 Windows 10 1067 (Redstone 1) 이상의 운영 체제
- GCC 또는 LLVM/Clang, Microsoft Visual C++ 컴파일러

>[!WARNING]
> **Windows 환경에서 컴파일 할 경우에는 Windows SDK 설치가 필요합니다.**
>
> 이는 win32_native_termui.c가 Win32 API를 사용하기 때문입니다.

>[!WARNING]
> **MSVC에서 컴파일하는 경우, SDL Check를 비활성화하십시오.**
>
> 이에 대한 자세한 사항은 다음 Microsoft 문서를 참조하십시오:
> https://learn.microsoft.com/en-us/cpp/build/reference/sdl-enable-additional-security-checks?view=msvc-170


## Project Status Tracker
### 프로젝트
|페이즈|목표|상태|
|-|-|-|
|Beta 1|탐색을 제외한 모든 기능|✅|
|Beta 2|완전한 기능|✅|
|Release Candidate|최종 릴리즈 후보|✅|

### 모듈
|기능|모듈|상태|기능|모듈|상태|
|-|-|:-:|-|-|:-:|
|기본 터미널 환경 구축|native_termui|✅|새로운 파일 생성|textengine|✅|
|문자 키 입력|native_termui|✅|기존 파일 열기|textengine|✅|
|키 조합 및 특수 키 입력|native_termui|✅|텍스트 버퍼 보기|textengine|✅|
|상태 바|termui|✅|텍스트 버퍼 저장|textengine|✅|
|메세지 바|termui|✅|문자 삽입|textengine|✅|
|프롬프트|termui|✅|문자 삭제|textengine|✅|
|동적 화면 리사이징|termui|✅|갭 관리|textengine|✅|
|편집 영역 텍스트 랜더링|termui|✅|
  
### 프로그램
|기능|상태|
|-|:-:|
|컴파일 가능|✅|
|상태 바|✅|
|메시지 바|✅|
|이동|✅|
|텍스트 편집|✅|
|저장|✅|
|탐색|✅|
|나가기|✅|
