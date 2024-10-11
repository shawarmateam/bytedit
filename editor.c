#include <curses.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

struct abuff {
    char* filename;
    unsigned char *b;
    int len;
};

struct curpos {
    int x,y;
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

void init_byd() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    raw();
}

void printBytes(struct abuff ab) {
    for (size_t i = 0; i < ab.len; i++) {
        printw("%02X ", ab.b[i]);
    }

    printw("++");
}

void changeByte(struct curpos cp, struct abuff ab, unsigned char c) {
    int byte_i = cp.x;
    if ((byte_i-1) % 3 == 0) byte_i-1;
    byte_i /= 3;

    ab.b[byte_i] = c;

    clear();
    printBytes(ab);
}

unsigned char getByte(struct curpos cp, struct abuff ab) {
    int byte_i = cp.x;
    if ((byte_i-1) % 3 == 0) byte_i-1;
    byte_i /= 3;
    
    return ab.b[byte_i];
}

bool whatHalfOfb(struct curpos cp, struct abuff ab) {
    int byte_i = cp.x;
    if ((byte_i-1) % 3 == 0) return false;
    return true;
}

int boot_byd(int argc, char** argv) {
    init_byd();

    //int x = 0, y = 0;
    struct curpos cp;
    struct abuff ab;
    cp.x=0;cp.y=0;

    //apByte(&ab, 0x01);
    //apByte(&ab, 0x12);
    //apByte(&ab, 0xAA);
    size_t len;
    ab.filename = "test.bin";
    unsigned char* b = readFileToHexArray(ab.filename, &len);

    for (int i=0; i<len; i++) {
        apByte(&ab, b[i]);
    }

    refresh();

    int ch;
    printBytes(ab);
    
    move(cp.y, cp.x);

    while ((ch = getch()) != 'q') {
        clear();
        printBytes(ab);

        switch (ch) {
            case KEY_UP:
                if (cp.y > 0) cp.y--;
                break;
            case KEY_DOWN:
                cp.y++;
                break;
            case KEY_LEFT:
                if (cp.x <= 0) break;

                if ((cp.x-3) % 3 == 0) cp.x--;
                cp.x--;
                break;
            case KEY_RIGHT:
                if ((cp.x+2) % 3 == 0) cp.x++;
                cp.x++;
                break;
            case 'z':
                changeByte(cp, ab, 0x00);
                break;
            //case '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
            //     'a', 'b', 'c', 'd', 'e', 'f':
            case '0':case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':case 'a':case 'b':case 'c':case 'd':case 'e':case 'f':
                char bStr[2];
                sprintf(bStr, "%02X", getByte(cp, ab));

                if (whatHalfOfb(cp, ab)) {
                    bStr[0] = toupper(ch);

                    unsigned char c = (unsigned char) strtol(bStr, NULL, 16);
                    changeByte(cp, ab, c);

                    cp.x++;
                } else {
                    bStr[1] = toupper(ch);

                    unsigned char c = (unsigned char) strtol(bStr, NULL, 16);
                    changeByte(cp, ab, c);

                    cp.x+=2;
                }
                break;
            case '+':
                apByte(&ab, 0x00);

                clear();
                printBytes(ab);
                break;
            case 's':
                writeBytesToFile(ab.b, ab.len, ab.filename);
                break;
        }

        move(cp.y, cp.x);
        refresh();
    }

    endwin();
    return 0;
}

