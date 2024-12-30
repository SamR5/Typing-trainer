#ifndef DISPLAY_H
#define DISPLAY_H

#include <ncurses.h>
#include <string.h>

void setup_term();
void display_all(const char **text, const size_t textSize,
                 const size_t currLine, const size_t currPos, int mode);
void display_line(const char *line, const size_t textSize, const size_t currPos, int mode);
void display_score(const unsigned int *score, const size_t size);


#endif
