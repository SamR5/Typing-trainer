#ifndef SAVE_H
#define SAVE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "parameters.h"


typedef struct {
    size_t currLine;
    size_t currPos;
    int score[SCORE_INTERVAL];
} SaveData;

size_t file_lines(const char *filename);
size_t file_size(const char *filename);
void get_save_name(const char *filename, char *svName, int mode);
void load_file_script(const char *filename, char **text);
void load_file_text(const char *filename, char *text);
bool check_save_integrity(const char *filename, int mode);
void load_save(const char *filename, SaveData *save, int mode);
void save(const char *filename, const SaveData *save, int mode);
void delete_save(const char *filename, int mode);

#endif
