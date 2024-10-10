#include <curses.h>
#include <string.h>
#include <stdlib.h>

struct abuff {
    unsigned char *b;
    int len;
};


void apByte(struct abuff *ab, unsigned char c) {
    size_t len = sizeof(ab->b+1);

    if (ab->b == NULL || ab->len == 0) {
        ab->b = malloc(1);

        if (ab->b == NULL) {
            perror("Failed to allocate memory");
            return;
        }

        ab->b[0] = c;
        ab->len = 1;
        return;
    } else {
        unsigned char *temp = realloc(ab->b, ab->len + 1);

        temp[len-1] = c;

        ab->b = temp;
        ab->b[ab->len] = c;
        ab->len++;
    }
}

int boot_byd(int argc, char** argv) {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    raw();

    int x = 0, y = 0;
    struct abuff ab;

    apByte(&ab, 0x01);
    apByte(&ab, 0x12);
    apByte(&ab, 0xAA);
    
    refresh();

    int ch;
    for (size_t i = 0; i < ab.len; i++) {
        printw("%02X ", ab.b[i]);
    }
    move(y, x);

    while ((ch = getch()) != 'q') {
        clear();

        for (size_t i = 0; i < ab.len; i++) {
            printw("%02X ", ab.b[i]);
        }

        switch (ch) {
            case KEY_UP:
                if (y > 0) y--;
                break;
            case KEY_DOWN:
                y++;
                break;
            case KEY_LEFT:
                if (x > 0) x--;
                break;
            case KEY_RIGHT:
                x++;
                break;
        }

        move(y, x);
        refresh();
    }

    endwin();
    return 0;
}

