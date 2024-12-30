#include "parameters.h"
#include "display.h"

void setup_term() {
    (void) initscr();      /* initialize the curses library */
    //(void) init_color();
    (void)start_color();
    init_pair(VALID, COLOR_GREEN, COLOR_BLACK);
    init_pair(ERROR, COLOR_RED, COLOR_BLACK);
    init_pair(CURRENT, COLOR_YELLOW, COLOR_BLUE);
    init_pair(AFTER, 11, COLOR_BLACK); // bright cyan
    keypad(stdscr, TRUE);  /* enable keyboard mapping */
    //raw();
    //(void) nonl();         /* tell curses not to do NL->CR/NL on output */
    (void) cbreak();       /* take input chars one at a time, no wait for \n */
    (void) noecho();       /* echo input - in color */
    intrflush(stdscr, FALSE);
    nodelay(stdscr, FALSE);
    timeout(0);
    curs_set(0);           /* disable the cursor (no blink) */
    clear();
}

void display_all(const char **text, const size_t textSize,
                 const size_t currLine, const size_t currPos, int mode) {
    clear();
    int row = 0;
    // lines before the current line
    attron(COLOR_PAIR(VALID));
    for (int i=(int)(currLine)-MARGIN; i<(int)currLine; i++) {
        if (i<0) {
            mvprintw(row, 0, " ");
        } else {
            mvprintw(row, 0, text[i]);
        }
        row++;
    }
    row++;
    // lines after the current line
    attron(COLOR_PAIR(AFTER));
    for (size_t i=currLine+1; i<currLine+MARGIN+1; i++) {
        if (i>=textSize) {
            mvprintw(row, 0, " ");
        } else {
            mvprintw(row, 0, text[i]);
        }
        row++;
    }
    display_line(text[currLine], textSize, currPos, mode);
    refresh();
}

void display_line(const char *line, const size_t textSize, const size_t currPos, int mode) {
    move(MARGIN, 0);
    attron(COLOR_PAIR(VALID));
    // bound for text mode and script mode
    int lowerP = mode==SCRIPT_MODE ? 0 : (int)currPos-PREVIOUS_CHAR_DISPLAYED;
    int upperP = currPos;
    for (int i=lowerP; i<upperP; i++) {
        if (i < 0 && mode==TEXT_MODE) { // never < 0 in script mode
            addch(' ');
        } else {
            addch(line[i]);
        }
    }
    attron(COLOR_PAIR(CURRENT));
    if (currPos == strlen(line)-1) { //in SCRIPT_MODE lines don't end with \n
        addch(' ');
    } else {
        addch(line[currPos]);
    }
    attron(COLOR_PAIR(AFTER));
    size_t lowerN = currPos+1;
    size_t upperN = mode==SCRIPT_MODE ? strlen(line): currPos+1+AFTER_CHAR_DISPLAYED;
    for (size_t i=lowerN; i<upperN; i++) {

        if (i>=textSize && mode==TEXT_MODE) {
            addch('e');
        } else {
            addch(line[i]);
        }
    }
    refresh();
}

void display_score(const unsigned int *score, const size_t size) {
    float avg = 0;
    for (size_t i=0; i<size; i++) {
        avg += score[i];
    }
    avg /= size; // average of the last SCORE_INTERVAL seconds
    avg *= 60; // average per minute
    attron(COLOR_PAIR(VALID));
    mvprintw(MARGIN*2+2, 0, "Words per minute: %3.0f", avg);
    refresh();
}
