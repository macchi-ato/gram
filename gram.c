#include <unistd.h>
#include <termios.h>
#include <stdlib.h>

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

    // Turns off input echoing
    // Bitwise: ~ECHO flips bits, &= clears just the ECHO flag
    raw.c_lflag &= ~(ECHO | ICANON); //  ICANON turns off canonical mode

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main() {
    enableRawMode();

    char c;
    while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q');
    return 0;
}