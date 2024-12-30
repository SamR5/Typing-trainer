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
#include "display.h"
#include "parameters.h"

#define KEY_ESCAPE 27 // quit with "Esc" (or "Echap") key
//for text mode
//#define PREVIOUS_CHAR_DISPLAYED 10
//#define AFTER_CHAR_DISPLAYED 30

// colors
//enum {VALID=1, ERROR, CURRENT, AFTER, BLACK};
//enum {SCRIPT_MODE, TEXT_MODE};

//void setup_term();
bool is_valid(const char c);
bool is_match(const char c, const char *line, const size_t position);
void update_score(unsigned int *score, unsigned int newScore);
bool is_end(size_t currLine, size_t currPos, size_t textSize, int mode);
bool train_global(const char *filename, SaveData *sv, int mode);

bool is_valid(const char c) {
    // printable characters except \t\n\x0b\x0c\r
    return 32 <= c || c <= 126 || c == 10;
}

bool is_match(const char c, const char *line, const size_t position) {
    return line[position] == c;
}

void update_score(unsigned int *score, unsigned int newScore) {
    for (int i=0; i<SCORE_INTERVAL-1; i++) {
        score[i] = score[i+1];
    }
    score[SCORE_INTERVAL-1] = newScore;
}

bool is_end(const size_t currLine, const size_t currPos,
            const size_t textSize, int mode) {
    if (mode == SCRIPT_MODE) {
        return currLine >= textSize;
    }
    return currPos >= textSize;
}

bool train_global(const char *filename, SaveData *sv, int mode) {
    size_t textSize, currLine, currPos;
    unsigned int score[SCORE_INTERVAL], keystrokes;
    char **script, *text; // one will be useless
    if (mode == SCRIPT_MODE) {
        textSize = file_lines(filename); 
        script = (char**)calloc(textSize, sizeof(char*));
        for (size_t i=0; i<textSize; i++) {
            script[i] = (char*)calloc(MAX_LINE_SIZE, sizeof(char));
        }
        load_file_script(filename, script);
        text = *script;
    } else {
        textSize = file_size(filename);
        text = (char*)calloc(textSize, sizeof(char));
        /*textSize -=*/
        load_file_text(filename, text);
    }
    memset(score, 0, sizeof(unsigned int)*SCORE_INTERVAL);
    load_save(filename, sv, mode);
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
        display_all(script, textSize, currLine, currPos, mode);
    } else {
        display_line(text, textSize, currPos, mode);
    }
    display_score(score, SCORE_INTERVAL);
    char c;
    while (!is_end(currLine, currPos, textSize, mode)) {
        c = wgetch(stdscr);
        if (c == KEY_ESCAPE)
            break;
        if (!is_valid(c)) {
            continue;
        }
        if (is_match(c, text, currPos)) {
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
                if (currLine >= textSize) {
                    break;
                }
                text = script[currLine]; // text++ doesn't work
                currPos = 0;
            }
            if (currPos == 0 && mode == SCRIPT_MODE) {
                display_all(script, textSize, currLine, currPos, mode);
            } else {
                display_line(text, textSize, currPos, mode);
            }
            display_score(score, SCORE_INTERVAL);
        }
        usleep(60*1000);
    }
    endwin(); // go back to classic terminal
    if (mode == SCRIPT_MODE) {
        for (size_t i=0; i<textSize; i++) {
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
    if (argc <= 2) {
        printf("Not enough arguments...\n"
               "Use '-s' for script mode or '-t' for text mode "
               "followed by the path to a file\n");
    }
    int mode = strncmp(argv[1], "-s", 2)==0 ? SCRIPT_MODE : TEXT_MODE;
    const char *filename = argv[2];
    SaveData sv;
    bool toSave;
    toSave = train_global(filename, &sv, mode);
    if (toSave) {
        save(filename, &sv, mode);
    } else {
        delete_save(filename, mode);
    }
    return 0;
}
