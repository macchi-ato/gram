#include <unistd.h>
#include <termios.h>

/* 
    *
    *   Turn off echoing to enable raw mode
    * 
*/
void enableRawMode() {
    struct termios raw;

    tcgetattr(STDIN_FILENO, &raw);

    raw.c_cflag &= ~(ECHO);

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main() {
    enableRawMode();

    char c;
    
    while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q');
    return 0;
}