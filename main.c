/*
 *   Train your typing skills.
 *
 *   Usage: user:~$ TypingTrainer text.txt
 *
 *   The timer starts when you start typing and stops automatically
 *   when you stop.
 *
 *   Press the <Escape> key to stop, the current training session will be saved
 *   Each text file generates its own save
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ncurses.h>
#include "save.h"

#define KEY_ESCAPE 27
#define PREVIOUS_CHAR_DISPLAYED 10
#define AFTER_CHAR_DISPLAYED 30

// colors
enum {VALID=1, ERROR, CURRENT, AFTER, BLACK};
enum {SCRIPT_MODE, TEXT_MODE};

void setup_term();
bool is_valid(char c);
bool is_match(char c, char *line, int position);
void display_all(char **text, int currLine, int currPos, int textSize);
void display_line(char *text, int currPos, int textSize);
void display_score(int *score);
void update_score(int *score, int newScore);

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

bool is_valid(char c) {
    // printable characters except \t\n\x0b\x0c\r
    return 32 <= (int)c || (int)c <= 126 || (int)c == 10;
}

bool is_match(char c, char *line, int position) {
    return line[position] == c;
}

void display_all(char **text, int currLine, int currPos, int textSize) {
    clear();
    int row = 0;
    // lines before the current line
    attron(COLOR_PAIR(VALID));
    for (int i=currLine-MARGIN; i<currLine; i++) {
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
    for (int i=currLine+1; i<currLine+MARGIN+1; i++) {
        if (i>=textSize) {
            mvprintw(row, 0, " ");
        } else {
            mvprintw(row, 0, text[i]);
        }
        row++;
    }
    display_line(text[currLine], currPos, -1);
    refresh();
}

void display_line(char *line, int currPos, int textSize) {
    move(MARGIN, 0);
    attron(COLOR_PAIR(VALID));
    // bound for text mode and script mode
    int lower = textSize==-1 ? 0 : currPos-PREVIOUS_CHAR_DISPLAYED;
    int upper = currPos;
    for (int i=lower; i<upper; i++) {
        if (i < 0) { // never < 0 in script mode
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
    int endLine = strlen(line);
    lower = currPos+1;
    upper = textSize==-1 ? endLine : currPos+1+AFTER_CHAR_DISPLAYED;
    for (int i=lower; i<upper; i++) {
        if (i>textSize && textSize != -1) {
            addch(' ');
        } else {
            addch(line[i]);
        }
    }
    refresh();
}

void display_score(int *score) {
    float avg = 0;
    for (int i=0; i<SCORE_INTERVAL; i++) {
        avg += score[i];
    }
    avg /= SCORE_INTERVAL; // average of the last SCORE_INTERVAL seconds
    avg *= 60; // average per minute
    attron(COLOR_PAIR(VALID));
    mvprintw(MARGIN*2+2, 0, "Words per minute: %3.0f", avg);
    refresh();
}

void update_score(int *score, int newScore) {
    for (int i=0; i<SCORE_INTERVAL-1; i++) {
        score[i] = score[i+1];
    }
    score[SCORE_INTERVAL-1] = newScore;
}

bool is_end(int currLine, int currPos, int textSize, int mode) {
    if (mode == SCRIPT_MODE) {
        return currLine >= textSize;
    } else {
        return currPos >= textSize;
    }
}

bool train_global(char *filename, SaveData *sv, int mode) {
    int textSize, currLine, currPos, score[SCORE_INTERVAL], keystrokes;
    char **script, *text; // one will be useless
    if (mode == SCRIPT_MODE) {
        textSize = file_lines(filename); 
        script = (char**)calloc(textSize, sizeof(char*));
        for (int i=0; i<textSize; i++) {
            script[i] = (char*)calloc(MAX_LINE_SIZE, sizeof(char));
        }
        load_file_script(filename, script);
        text = *script;
    } else {
        textSize = file_size(filename);
        text = (char*)calloc(textSize, sizeof(char));
        load_file_text(filename, text);
    }
    memset(score, 0, SCORE_INTERVAL);
    load_save(filename, sv);
    currLine = sv->currLine;
    currPos = sv->currPos;
    memcpy(score, sv->score, SCORE_INTERVAL);
    if (mode == SCRIPT_MODE) {
        text = script[currLine]; // text++ doesn't work
    }
    setup_term();
    attron(COLOR_PAIR(CURRENT));
    clock_t one = clock();
    clock_t two = clock();
    if (mode == SCRIPT_MODE) {
        display_all(script, currLine, currPos, textSize);
    } else {
        display_line(text, currPos, textSize);
    }
    display_score(score);
    int c;
    while (!is_end(currLine, currPos, textSize, mode)) {
        c = wgetch(stdscr);
        if (c == KEY_ESCAPE)
            break;
        if (!is_valid((char)c)) {
            continue;
        }
        if (is_match((char)c, text, currPos)) {
            two = clock();
            keystrokes++;
            float interval = 1000*(float)(two-one)/CLOCKS_PER_SEC;
            if (interval > 2) {
                one = clock();
                keystrokes = 0;
            } else if (interval > 1) {
                one = clock();
                update_score(score, keystrokes);
                keystrokes = 0;
            }
                currPos++;
            if (c == '\n' && mode == SCRIPT_MODE) {
                currLine++;
                text = script[currLine]; // text++ doesn't work
                currPos = 0;
            }
            if (currPos == 0 && mode == SCRIPT_MODE) {
                display_all(script, currLine, currPos, textSize);
            } else if (mode == SCRIPT_MODE) {
                display_line(text, currPos, -1);
            } else {
                display_line(text, currPos, textSize);
            }
            display_score(score);
        }
        usleep(60*1000);
    }
    if (mode == SCRIPT_MODE) {
        for (int i=0; i<textSize; i++) {
            free(script[i]);
        }
        free(script);
    } else {
        free(text);
    }
    if (is_end(currLine, currPos, textSize, mode)) {
        return false;
    } else {
        sv->currLine = currLine;
        sv->currPos = currPos;
        memcpy(sv->score, score, SCORE_INTERVAL);
        return true;
    }
}

int main(int argc, char* argv[]) {
    char *mode = argv[1];
    char *filename = argv[2];
    SaveData sv;
    bool toSave;
    if (strncmp(argv[1], "-s", 2) == 0) {
        toSave = train_global(filename, &sv, SCRIPT_MODE);
    } else {
        toSave = train_global(filename, &sv, TEXT_MODE);
    }
    if (toSave) {
        save(filename, &sv);
    } else {
        delete_save(filename);
    }
    endwin(); // go back to classic terminal
    return 0;
}
