#ifndef PARAMETERS_H
#define PARAMETERS_H


// FOR SCRIPT MODE
#define MARGIN 3

// FOR TEXT MODE
#define MAX_LINE_SIZE 256
#define PREVIOUS_CHAR_DISPLAYED 10
#define AFTER_CHAR_DISPLAYED 30

// FOR FILE MANAGEMENT
#define SAVE_NAME_MAX_SIZE 32
#define SCRIPT_SAVE_EXTENSION ".svs"
#define TEXT_SAVE_EXTENSION ".svt"
#define SAVE_EXTENSION_SIZE strlen(TEXT_SAVE_EXTENSION)+1
#define SAVE_BYTES sizeof(SaveData) // current line + current position + score


#define SCORE_INTERVAL 10 //last ten seconds

extern enum char_state {VALID=1, ERROR, CURRENT, AFTER} yy;
extern enum display_mode {SCRIPT_MODE, TEXT_MODE} xx;

#endif
