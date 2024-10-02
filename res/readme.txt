[README]

## 개요
fedit (viva)는 TUI 기반 텍스트 편집기입니다.

-   영어 이외의 다국어 입출력은 지원하지 않습니다. (유니코드 미지원)
    반드시 입력기가 US English로 설정되어 있는지 확인하십시오.

-   불러 올 파일의 개행 문자는 반드시 LF 형식이여야 합니다.

-   Windows 10 1607 이상, 또는 POSIX 호환 운영 체제(macOS, Linux)에서 실행 가능합니다.

-   GCC, LLVM/Clang 또는 Microsoft Visual C++ Compiler에서 컴파일 할 수 있습니다.

make 명령을 사용하여 프로그램을 컴파일하고, 아래 명령을 사용해 빈 파일 또는 기존 파일을 여십시오.

-   ./viva test.txt     - 현재 디렉터리의 test.txt를 엽니다.

-   ./viva              - 빈 파일을 엽니다.

## 기능 설명
