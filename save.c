#include "save.h"

size_t file_lines(const char *filename) {
    FILE *input = fopen(filename, "r");
    if (!input)
        exit(EXIT_FAILURE);
    char *line = NULL;
    size_t len = 0;
    size_t counter = 0;
    while (getline(&line, &len, input) != EOF) {
        counter++;
    }
    fclose(input);
    if (input != NULL) {
        printf("File not properly closed...\n");
    }
    return counter;
}

size_t file_size(const char *filename) {
    FILE *input = fopen(filename, "r");
    if (!input)
        exit(EXIT_FAILURE);
    size_t size = 0;
    char previous, current;
    previous = fgetc(input);
    while ((current = fgetc(input)) != EOF) {
        if (current=='\n')
            current = ' ';
        if (previous==' ' && current==' ') {
            continue;
        } else {
            size++;
            previous = current;
        }
    }
    fclose(input);
    if (input != NULL) {
        printf("File not properly closed...\n");
    }
    return size;
}

void get_save_name(const char *filename, char *svName, int mode) {
    strncpy(svName, filename, SAVE_NAME_MAX_SIZE-SAVE_EXTENSION_SIZE);
    if (mode == SCRIPT_MODE) {
        strncat(svName, SCRIPT_SAVE_EXTENSION, SAVE_EXTENSION_SIZE);
    } else if (mode == TEXT_MODE) {
        strncat(svName, TEXT_SAVE_EXTENSION, SAVE_EXTENSION_SIZE);
    }
}

void load_file_script(const char *filename, char **text) {
    FILE *input = fopen(filename, "r");
    if (!input)
        exit(EXIT_FAILURE);
    char *line = NULL;
    size_t len = 0;
    // text will be modified and the pointer will remains at the start
    char **textLine = text;
    while (getline(&line, &len, input) != EOF) {
        // -1 to guarentee the line ends with '\0'
        strncpy(*textLine, line, MAX_LINE_SIZE-1);
        textLine++;
    }
    fclose(input);
}

void load_file_text(const char *filename, char *text) {
    FILE *input = fopen(filename, "r");
    if (!input)
        exit(EXIT_FAILURE);
    // we don't want to change the text pointer
    char *buffer = text;                                                           
    // add a char first to avoid getting outside the text in the while loop
    char current = fgetc(input);
    *buffer = current;
    buffer++;
    while ((current = fgetc(input)) != EOF) {
        if (current == '\n') {
            current = ' ';
        } // skip when several spaces appear
        if (*(buffer-1) == ' ' && current == ' ') {
            continue;
        }
        *buffer = current;
        buffer++;
    }
    fclose(input);
}

bool check_save_integrity(const char *filename, int mode) {
    char svName[SAVE_NAME_MAX_SIZE] = {0};
    get_save_name(filename, svName, mode);
    FILE *sv = fopen(svName, "rb");
    if (!sv) {
        return false;
    }
    fseek(sv, 0, SEEK_END);
    size_t svSize = ftell(sv);
    fclose(sv);
    return svSize == SAVE_BYTES;
}

void load_save(const char *filename, SaveData *save, int mode) {
    if (!check_save_integrity(filename, mode)) {
        delete_save(filename, mode); // in case of corrupted file
        save->currLine = 0;
        save->currPos = 0;
        memset(save->score, 0, sizeof(int)*SCORE_INTERVAL);
    } else {
        char svName[SAVE_NAME_MAX_SIZE] = {0};
        get_save_name(filename, svName, mode);
        FILE *svFile = fopen(svName, "rb");
        if (!svFile) {
            perror("An error occured when loading the save file");
            exit(EXIT_FAILURE);
        }
        fread(save, sizeof(SaveData), 1, svFile);
        fclose(svFile);
    }
}

void save(const char *filename, const SaveData *save, int mode) {
    char svName[SAVE_NAME_MAX_SIZE] = {0};
    get_save_name(filename, svName, mode);
    FILE *svFile = fopen(svName, "wb");
    fwrite(save, sizeof(SaveData), 1, svFile);
    fclose(svFile);
}

void delete_save(const char *filename, int mode) {
    char svName[SAVE_NAME_MAX_SIZE] = {0};
    get_save_name(filename, svName, mode);
    remove(svName);
}
