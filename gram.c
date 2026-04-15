#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

/* 
    *
    *   Disable raw mode
    * 
*/
struct termios orig_termios;

void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

/* 
    *
    *   Turn off echoing to enable raw mode
    * 
*/
void enableRawMode() {
    // Save current terminal settings into orig_termios
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode);

    struct termios raw = orig_termios;

    // Turn off terminal flags

    // IXON software flow controls (ctrl-s and ctrl-q)
    // ICRNL carriage returns (ctrl-m and enter)
    raw.c_iflag &= ~(IXON | ICRNL);
    // Output processing
    raw.c_oflag &= ~(OPOST);
    // ECHO Input printing 
    // ICANON canonical mode
    // IEXTEN disables ctrl-v
    // ISIG ctrl signals (ctrl-c and ctrl-z)
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main() {
    enableRawMode();

    char c;
    while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q') {
        if (iscntrl(c)) {
            printf("%d\r\n", c);
        } else {
            printf("%d ('%c')\r\n", c, c);
        }
    }

    return 0;
}