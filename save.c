#include "save.h"

int file_lines(char *filename) {
    FILE *input = fopen(filename, "r");
    if (!input)
        exit(EXIT_FAILURE);
    char *line = NULL;
    size_t len = 0;
    int counter = 0;
    while (getline(&line, &len, input) != EOF) {
        counter++;
    }
    fclose(input);
    if (input != NULL) {
        printf("File not properly closed...\n");
    }
    return counter;
}

int file_size(char *filename) {
    FILE *input = fopen(filename, "r");
    if (!input)
        exit(EXIT_FAILURE);
    fseek(input, 0, SEEK_END); 
    int size = ftell(input);
    fclose(input);
    if (input != NULL) {
        printf("File not properly closed...\n");
    }
    return size;
}

void get_save_name(char *filename, char *svName) {
    size_t fnLen = strlen(filename);
    strncpy(svName, filename, SAVE_NAME_MAX_SIZE-SAVE_EXTENSION_SIZE);
    strncat(svName, SAVE_EXTENSION, SAVE_EXTENSION_SIZE);
}

void load_file_script(char *filename, char **text) {
    FILE *input = fopen(filename, "r");
    if (!input)
        exit(EXIT_FAILURE);
    char *line = NULL;
    size_t len = 0;
    char **textLine = text;
    while (getline(&line, &len, input) != EOF) {
        // -1 to guarentee the line ends with '\0'
        strncpy(*textLine, line, MAX_LINE_SIZE-1);
        textLine++;
    }
    fclose(input);
}

void load_file_text(char *filename, char *text) {
    FILE *input = fopen(filename, "r");
    if (!input)
        exit(EXIT_FAILURE);
    char *buffer = text;                                                           
    // add a char first to avoid getting outside the text in the while loop
    int current = fgetc(input);
    *buffer = current;
    while ((current = fgetc(input)) != EOF) {
        if (current == '\n') {
            current = ' ';
        }
        if (*(buffer-1) == ' ' && current == ' ') {
            continue;
        }
        *buffer = current;
        buffer++;
    }
    fclose(input);
}

bool check_save_integrity(char *filename) {
    char svName[SAVE_NAME_MAX_SIZE] = {0};
    get_save_name(filename, svName);
    FILE *sv = fopen(svName, "rb");
    if (!sv) {
        return false;
    }
    fseek(sv, 0, SEEK_END);
    int svSize = ftell(sv);
    fclose(sv);
    return svSize == SAVE_BYTES;
}

void load_save(char* filename, SaveData *save) {
    if (!check_save_integrity(filename)) {
        delete_save(filename); // in case of corrupted file
        save->currLine = 0;
        save->currPos = 0;
        memset(save->score, 0, SCORE_INTERVAL);
    } else {
        char svName[SAVE_NAME_MAX_SIZE] = {0};
        get_save_name(filename, svName);
        FILE *svFile = fopen(svName, "rb");
        if (!svFile) {
            perror("An error occured when loading the save file");
            exit(EXIT_FAILURE);
        }
        fread(save, sizeof(SaveData), 1, svFile);
        fclose(svFile);
    }
}

void save(char *filename, SaveData *save) {
    char svName[SAVE_NAME_MAX_SIZE] = {0};
    get_save_name(filename, svName);
    FILE *svFile = fopen(svName, "wb");
    fwrite(save, sizeof(SaveData), 1, svFile);
    fclose(svFile);
}

void delete_save(char *filename) {
    char svName[SAVE_NAME_MAX_SIZE] = {0};
    get_save_name(filename, svName);
    remove(svName);
}
