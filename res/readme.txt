## 개요
fedit (viva)는 간단한 TUI 텍스트 편집기입니다.

-   영어 이외의 다국어 입출력은 지원하지 않습니다. (유니코드 미 지원)
    !(반드시 입력기가 US Keyboard로 설정되어 있는지 확인하십시오.)

-   불러 올 파일의 개행 문자는 반드시 LF 형식이여야 합니다.

-   Windows 10 1607 (Redstone 1) 이상, 또는 POSIX 호환 운영 체제(macOS, Linux)에서 실행 가능합니다.

-   GCC, Clang/LLVM 또는 Microsoft Visual C++ Compiler에서 컴파일 할 수 있습니다.
    !(Windows 환경에서 컴파일 할 경우에는 Windows SDK 설치가 필요합니다: https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/)
    !(MSVC를 통해 컴파일하는 경우에는 반드시 SDL Check를 비활성화하십시오: https://learn.microsoft.com/en-us/cpp/build/reference/sdl-enable-additional-security-checks?view=msvc-170)

-   macOS에서 행 이동 속도가 너무 느린 경우에는 키 반복 속도를 확인하십시오. (System Settings > Keyboard > Key repeat rate, Delay until repeat)

make 명령을 사용하여 프로그램을 컴파일하고, 아래 명령을 사용해 빈 파일 또는 기존 파일을 여십시오.

-   `./viva test.txt`   현재 디렉터리의 test.txt를 엽니다.
-   `./viva`            빈 파일을 엽니다.

## 기능

### 수정
-   문자 키와 백 스페이스 키를 사용하여 현재 커서 위치에서 문자를 삽입하거나 삭제하십시오.

### 이동
-   왼쪽 및 오른쪽 화살표 키를 통해 현재 행에서 좌우 열로 이동하십시오.

-   위쪽 및 아래쪽 화살표 키를 통해 현재 행에서 위 아래 행으로 이동하십시오.
    !(행 이동시 해당 행의 가장 첫 번째 열로 이동합니다.)

-   Home, End 키를 통해 현재 행에서 가장 첫번째 열 또는 마지막 열로 이동하십시오.

-   PgUp, PgDn 키를 통해 이전 페이지 또는 다음 페이지로 이동하십시오. 
    !(페이지 이동시 해당 페이지의 가장 첫 번째 행의 첫 번째 열로 이동합니다.)

### 저장
-   Ctrl-S 키 조합으로 현재 버퍼의 내용을 파일에 저장하십시오. 
    !(빈 파일로 시작 한 경우 파일 이름을 결정해야 합니다.)

-   Ctrl-N 키 조합으로 현재 버퍼의 내용을 새 파일에 저장하십시오. 
    !(현재 파일 이름이 변경됩니다.)

### 탐색
-   Ctrl-F 키 조합으로 탐색 모드를 시작하십시오.

-   찾을 패턴을 입력하십시오.
    !(패턴은 2자보다 길고 30자보다 짧아야 합니다.)

-   왼쪽 화살표 키로 이전 항목, 오른쪽 화살표 키로 다음 항목으로 이동하십시오.

-   ESC 키로 탐색을 중단하고 기존 위치로 이동하십시오.

-   Enter 키로 탐색을 중단하고 현재 항목의 위치로 이동하십시오.

### 종료
-   Ctrl-Q 키 조합으로 fedit에서 나가십시오.
    !(현재 변경 사항을 저장하지 않은 경우에는 Ctrl-Q 키 조합을 한번 더 눌러야 합니다.)
