# fedit
<img src="https://img.shields.io/badge/c-064F8C?style=for-the-badge&logo=c&logoColor=white">

**2024년 창원대학교 컴퓨터공학과 고급자료구조 과제**

A simple TUI-based text editor.

**Compatibility checked on:**
|Compiler|Target|OS|Checked|
|-|-|-|:-:|
|GCC 11.4.0|aarch64-linux-gnu|Ubuntu Server 24.04.1|🚧|
|Apple clang 15.0.0|arm64-apple-darwin23.5.0|macOS Sequoia 15.1|🚧|
|Microsoft Visual C++ 1936|arm64|Windows 11 Enterprise A64|🚧|
|GCC 13.2.0|x86_64-w64-mingw32|Windows 11 Enterprise x64|🚧|

### Features



## Project Status Tracker

### 모듈
|기능|모듈|상태|기능|모듈|상태|
|-|-|:-:|-|-|:-:|
|기본 터미널 환경 구축|native_termui|✅|새로운 파일 생성|textengine|✅|
|문자 키 입력|native_termui|✅|기존 파일 열기|textengine|✅|
|키 조합 및 특수 키 입력|native_termui|✅|텍스트 버퍼 보기|textengine|✅|
|상태 바|termui|✅|텍스트 버퍼 저장|textengine|🚧|
|메세지 바|termui|✅|문자 삽입|textengine|🚧|
|프롬프트|termui|✅|문자 삭제|textengine|🚧|
|동적 화면 리사이징|termui|✅|갭 관리|textengine|🚧|
|편집 영역 텍스트 랜더링|termui|✅|
  
### 프로그램
|기능|상태|
|-|:-:|
|컴파일 가능|✅|
|상태 바|✅|
|메시지 바|✅|
|이동|✅|
|텍스트 편집|🚧|
|저장|🚧|
|탐색|🚧|
|나가기|✅|
