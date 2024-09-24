## II. termui, native_termui 모듈 설계

### A. 모듈 개요
termui모듈은 ANSI 이스케이프 시퀀스를 사용하여 TUI 환경을 구축하고, 사용자 키 입력을 핸들링한다. ANSI Escape Sequences(ANSI X3.64)는 비디오 터미널이나 터미널 에뮬레이터에서 커서의 위치, 색상, 폰트 스타일과 같은 것들을 제어하기 위한 표준이다.

POSIX 계열 운영체제의 터미널은 기본적으로 이를 지원하고, Windows는 Windows 10 1607 (Redstone 1) 버전부터 Windows Subsystem for Linux 지원을 위해 Windows Console Host는 Virtual Terminal Sequences 를 지원한다. 이 것보다 이전 버전의 Windows는 모두 지원 종료 되었기 때문에 호환을 고려하지 않는다.

이 모듈의 기능을 구현하기 위하여 필요한 모든 플랫폼-종속적인 함수는 다음과 같다:
-	`nt_get_terminal_size()`: 현재 터미널의 크기를 구한다.
  
-	`nt_configure_term_env()`: 터미널 환경을 구성한다.
    - POSIX에서는 termios를 사용하여 ECHO와 Canonical Input을 끈다.
    -	Win32에서는 Win32 API를 통해 Console Handle의 Virtual Terminal Processing을 킨다.
      
-	`nt_restore_term_env()`: 터미널 환경을 기존 상태로 복원한다.
  
-	`nt_get_raw_input()`: stdin 에서 RAW 입력을 받는다. (Non-blocking I/O)
위의 모든 함수는 native_termui 모듈에 포함된다. 앞으로 이 문서에서 다루는 모든 접두사 NT 함수는 Windows NTAPI 함수가 아닌 native_termui 모듈의 함수임을 유의하자.
		
`main()` 함수는 `ui_init()` 함수를 실행함으로써 termui 모듈을 초기화하며, 해당 함수의 파라미터로 콜백 함수를 전달하는데, 이 콜백 함수는 터미널 크기가 조정되었을 때 호출된다. 프로그램을 종료할 때 `ui_dispose()` 함수를 실행하여 터미널을 정리한다.

모듈의 기능을 구현하는데 있어 플랫폼 종속성을 최소화 하기 위해 ncurses와 같은 외부 라이브러리는 사용하지 않고 직접 구현된다. 키 입력과 터미널 UI와 관련된 모든 기능은 이 모듈에서 추상화되기 때문에, viva.c에서 텍스트 에디터 로직을 구현하기 위하여 콘솔에 직접 접근할 필요가 없다. 

#

### B. 사용자 인터페이스 구축

`ui_init()` 함수는 내부적으로 `nt_configure_term_env()` 함수를 실행하여 터미널 환경을 설정한다. `nt_configure_term_env()` 함수에서 가장 중요한 작업은 XTerm Control Sequences를 사용하여 터미널이 Alternate Screen Buffer를 사용하도록 설정하여, 기존 화면 출력에 영향을 주지 않고 터미널의 전체 화면을 사용할 수 있도록 하는 것이다. 

또한 POSIX에서는 Mouse tracking을 활성화 할 필요가 있는데, 이는 터미널에 휠, 클릭과 같은 마우스 입력을 수행하면 이를 터미널 프로그램에서 처리하지 않고 stdin 버퍼에 직접 전달되도록 하는 것이다. POSIX 터미널에서 Alternate Screen Buffer를 사용해도 마우스 휠을 사용하면 스크롤이 올라가는 문제를 막기 위해 프로그램이 마우스 이벤트를 가로채야 한다.

stdin에서 입력을 받는 `nt_get_raw_input()` 네이티브 함수의 POSIX 구현은 이러한 마우스 이벤트 값을 적절하게 필터링하고 사용자의 키 입력 값만을 받도록 해야 한다. 또한 프로그램이 종료될 때 자동으로 터미널 구성이 복원되지 않기 때문에, 터미널 구성을 다시 원 상태로 복원해야 한다. 

`ui_dispose()` 함수는 내부적으로 `nt_restore_term_env()` 네이티브 함수를 호출하여 터미널 구성을 복원한다. 또한 viva.c에서 SIGINT 시그널의 시그널 핸들러로 `ui_dispose()` 함수를 실행하여 터미널을 정리하고 안전하게 종료하도록 한다.

상태 바, 메시지 바, 편집 영역은 ANSI 이스케이프 시퀀스를 이용하여 직접 그린다.
상태 바는 터미널 가장 아래의 2번째 행으로 커서를 이동하고, 폰트의 색상을 반전색으로 지정한 후 터미널의 너비만큼 공백 문자를 포함하여 텍스트를 출력하는 방식으로 그린다. 이는 `ui_set_status()` 함수로 상태 정보 구조체를 전달하면, 이를 `draw_status()` 내부 함수로 화면에 그린다.

메시지 바는 터미널 가장 아래 행에 존재하며, `ui_show_message()` 함수를 통해 스트링을 전달하면, 메시지 바에 특정한 스트링을 표시할 수 있다. 편집 영역은 `ui_draw_text()` 함수에 인자로 스크린 버퍼 의 시작 주소와 길이를 전달하여 그린다. 빈 행에는 첫번째 열에 문자 ‘~’을 출력한다. 이 함수는 터미널 플래싱을 최소화하기 위해 버퍼를 두고 stdout에 텍스트를 한번에 쓴다.

편집 영역에서의 커서 이동은 `ui_cursor_move()` 함수에 파라미터로 좌표를 전달하여 수행한다. 이 모듈에서의 커서 이동은 단순히 UI상의 표시되는 커서의 이동을 의미한다. 좌표에 따른 상대 위치 관리와 같은 것은 이 모듈의 역할이 아니다.

만약 프로그램에 명령 인수를 지정하지 않아 빈 파일로 시작했을 경우에는 `main()`에서 `ui_set_motd(true)` 함수를 통해 화면에 MOTD를 그린다. MOTD가 그려졌을 경우에는 `ui_input_loop()`가 키 입력을 받으면 이를 `ui_set_motd(false)` 함수를 호출하여 지운다.

#

### C. 사용자 입력 핸들링
키 입력은 termui 모듈에서 처리하며, `ui_input_loop()` 함수는 사용자가 키를 입력하면 해당 이벤트를 파리미터로 전달된 콜백 함수로 전달하는 것을 반복하는 함수이다. 파라미터의 콜백 함수가 false를 반환하면 루프가 종료되며 반환된다. 이 함수는 `nt_get_raw_input()` 네이티브 함수를 호출하여 stdin 버퍼에서 문자를 가져온다.

이는 `main()`에서 호출되어 모든 사용자 입력을 받아 명령키 처리(CTRL-F, CTRL-Q 등) 및 문자 입력, 커서 이동을 수행한다. 또한 이는 하단 메시지 바에서 프롬프트를 구현할 때도 사용한다.

`nt_get_raw_input()` 네이티브 함수는 Non-blocking I/O 방식으로 stdin 버퍼에서 문자를 가져오는데, 해당 함수의 반환 타입은 `termui_types.h`에 정의된 열거형 key_types 이다. 

**enum key_types는 다음과 같이 정의된다:**
-	`NORMAL_KEY`: 영문자나 숫자와 같은 일반 키
-	`CONTROL_KEY`: CTRL + C와 같은 컨트롤 키 조합
-	`ESC`, `END`, `HOME`, `PGUP`, `ENTER` 등: ASCII로 표현 불가능한 특수 키
-	`TIMEOUT`: 지정된 시간안에 입력을 받지 못했거나, 올바르지 않은 값 조합

HOME, END, PGUP과 같은 특수 키 입력 때문에 Non-blocking I/O 방식으로 `nt_get_raw_input()` 함수를 구현해야 한다. 해당 특수 키들은 하나의 ASCII 값으로 표현할 수 없기 때문에 플랫폼에 따라 세부적인 값은 다르지만, 다수의 char 값 조합으로 이루어진다. 또한 Enter, Backspace 키도 플랫폼에 따라 표현되는 값이 다르기 때문에 이들 키의 구분도 이 함수에서 추상화 할 필요가 있다.

```
POSIX에서 상단 화살표 키는 다음 값의 조합으로 입력된다:  27 + ‘[‘ + ‘A’
Windows에서 상단 화살표 키는 다음 값의 조합으로 입력된다: 244 + 72
```

만약 이 함수를 Blocking I/O 방식으로 구현한다고 가정해보자. Windows 환경에서는 문제가 없을 수도 있다. 하지만 macOS 환경에서는 27은 ESC 키의 입력 값과 같기 때문에 문제가 발생한다. 연속적으로 값 조합 3개가 입력되었을 때 상단 화살표 키를 입력한 것으로 처리한다고 가정하면, 사용자가 ESC 키를 입력한 것인지 특수 키를 입력한 것인지 구분 할 수 없기 때문에 사용자가 ESC키를 누르면 다음 키가 입력될 때 까지 무한정 스레드가 블록된다.

#

### D. 프롬프트
<p align="center">
<img src="https://github.com/user-attachments/assets/3f96cad5-ebb5-4761-8269-94dc461bdabe" style="width:60%">
</p>
<p align="center">Figure II: 사용자에게 입력을 받는 프롬프트</p>
</br>

프롬프트는 하단 메시지 바에 표시되며, 사용자로부터 스트링을 입력 받을 때 `ui_show_prompt()` 함수를 통해 호출한다. 이는 사용자가 입력한 스트링을 반환한다. 
즉 텍스트 검색 기능과 같은 것은 내부적으로 `ui_show_prompt()` 함수를 통해 사용자로부터 스트링 입력을 받아 이를 처리하게 되는 것이다.

# 

### E. 동적 화면 리사이징
`SIGWINCH` 시그널을 시그널 핸들러로 받아 이때 화면을 리사이징 하는 것을 고려하였지만, 시그널 핸들러는 메인 스레드에서 소프트웨어 인터럽트로 동작한다. 즉, Asynchronous 하다는 것이다. 이 때문에 시그널 핸들러에서는 async-signal safe한 함수만 실행해야 하는데, UI를 그리기 위한 대부분의 함수는 그렇지 않다. (대표적으로 `printf()`는 async-signal safe하지 않다.) 

이러한 이유로 인해 async-signal safe한 이벤트 큐를 만들고, 시그널 핸들러에서 큐에 이벤트를 넣으면 인터럽트가 끝난 다음에 `ui_input_loop()`에서 처리하는 마치 Windows 커널 모드 드라이버 아키텍쳐의 DPC (Deferred Procedure Call)와 유사한 구조가 필요하다. 이는 구현이 과도하게 복잡하다. 또한 Win32에서는 `SIGWINCH` 시그널을 지원하지 않기 때문에 이 방법은 사용할 수 없다.

그래서 `ui_input_loop()`에서 I/O 요청을 보낸 다음 터미널 크기를 확인하여 현재 터미널 사이즈 구조체와 크기를 비교하고, 만약 터미널 사이즈가 다르다면 콜백 함수를 호출하여 이를 통지한다. 이 방법을 이용하면 아주 단순한 구현으로 터미널 크기 변화를 동기적으로 처리 할 수 있다. 

