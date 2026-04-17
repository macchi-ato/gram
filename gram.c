#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <errno.h>

/* 
    *
    *   Defines
    * 
*/

#define CTRL_KEY(k) ((k) & 0x1f)

/* 
    *
    *   Data
    * 
*/

struct termios orig_termios;    // Terminal attributes global variable

/* 
    *
    *   Terminal
    * 
*/

void die(const char *s) {
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
    perror(s);
    exit(1);
}

void disableRawMode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1) {
        die("tcsetattr");
    }
}

void enableRawMode() {
    // Save current terminal settings into orig_termios
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) {
        die("tcgetattr");
    }
    atexit(disableRawMode);

    struct termios raw = orig_termios;

    // Turn off terminal flags

    // BRKINT break condition
    // ICRNL carriage returns (ctrl-m and enter)
    // INPCK parity checking
    // ISTRIP byte stripping 
    // IXON software flow controls (ctrl-s and ctrl-q)
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    // Output processing
    raw.c_oflag &= ~(OPOST);
    // ECHO Input printing 
    // ICANON canonical mode
    // IEXTEN disables ctrl-v
    // ISIG ctrl signals (ctrl-c and ctrl-z)
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

    // Bit mask
    // Sets character size to 8 bits per byte 
    raw.c_cflag |= (CS8);

    // Control characters
    // Set timeout while read doesn't recieve input
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
        die("tcsetattr");
    }
}

char editorReadKey() {
    int nread;
    char c;

    while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
        if (nread == -1 && errno != EAGAIN) die("read");
    }

    return c;
}

/* 
    *
    *   Output
    * 
*/

void editorDrawRows() {
    int y;

    for (y = 0; y < 24; y++) {
        write(STDOUT_FILENO, "~\r\n", 3);
    }
}

void editorRefreshScreen() {
    // ANSI escape sequence
    // 2 clears entire screen
    // 1 start to cursor
    // 0 cursor to end
    write(STDOUT_FILENO, "\x1b[2J", 4);
    // Cursor position
    write(STDOUT_FILENO, "\x1b[H", 3);

    editorDrawRows();
    
    write(STDOUT_FILENO, "\x1b[H", 3);
}

/* 
    *
    *   Input
    * 
*/

void editorProcessKeypress() {
    char c = editorReadKey();

    switch (c) {
        case CTRL_KEY('q'):
            write(STDOUT_FILENO, "\x1b[2J", 4);
            write(STDOUT_FILENO, "\x1b[H", 3);
            exit(0);
            break;
    }
}

/* 
    *
    *   Init
    * 
*/

int main() {
    enableRawMode();

    while (1) {
        editorRefreshScreen();
        editorProcessKeypress();
    }

    return 0;
}