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
    *   Enable raw mode
    * 
*/
void enableRawMode() {
    // Save current terminal settings into orig_termios
    tcgetattr(STDIN_FILENO, &orig_termios);
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


    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main() {
    enableRawMode();

    while (1) {
        char c = '\0';
        read(STDIN_FILENO, &c, 1);
        if (iscntrl(c)) {
            printf("%d\r\n", c);
        } else {
            printf("%d ('%c')\r\n", c, c);
        }

        if (c == 'q') break;
    }

    return 0;
}