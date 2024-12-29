#ifndef SAVE_H
#define SAVE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>


#define MAX_LINE_SIZE 256
#define MARGIN 3
#define SCORE_INTERVAL 10 //last ten seconds
#define SAVE_NAME_MAX_SIZE 32
#define SAVE_EXTENSION ".save"
#define SAVE_EXTENSION_SIZE strlen(SAVE_EXTENSION)+1
#define SAVE_BYTES sizeof(SaveData) // current line + current position + score

typedef struct {
    int currLine;
    int currPos;
    int score[SCORE_INTERVAL];
} SaveData;


int file_lines(char *filename);
int file_size(char *filename);
void get_save_name(char *filename, char *svName);
void load_file_script(char *filename, char **text);
void load_file_text(char *filename, char *text);
bool check_save_integrity(char *filename);
void load_save(char* filename, SaveData *save);
void save(char *filename, SaveData *save);
void delete_save(char *filename);

#endif
