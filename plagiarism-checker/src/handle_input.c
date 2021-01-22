#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include <dirent.h>

char **getwords(char *file_path, int *no_of_words) {
    FILE *fp = fopen(file_path, "r");
    if (!fp) {
        fprintf(stderr, "Error in opening file %s\n", file_path);
        return NULL;
    }

    char **words = NULL;
    char word[46] = "";

    int array_size = 512;
    // initial size = 512, since each doc is expected to have around or more than 500 words
    if (!(words = calloc(array_size, sizeof *words))) {
        fprintf(stderr, "Memory exhausted\n");
        return NULL;
    }
    char ch;
    int i = 0;
    while ((ch = (char) fgetc(fp)) != EOF) {
        // consider only alphabets and numbers
        if ((ch - 'a' >= 0 && ch - 'a' < 26) || (ch - '0' >= 0 && ch - '0' <= 9))
            word[i++] = ch;
        // make all words lowercase
        else if (ch - 'A' >= 0 && ch - 'A' < 26)
            word[i++] = (char) ('a' + (ch - 'A'));
        // consider words only of length >= 2
        else if (i > 1) {
            word[i] = 0;
            words[*no_of_words] = strdup(word);
            ++(*no_of_words);

            // resize words array if needed
            if (*no_of_words == array_size) {
                words = realloc(words, array_size * 2 * sizeof *words);
                if (!words) {
                    fprintf(stderr, "Memory exhausted\n");
                    return NULL;
                }
                memset(words + array_size, 0, array_size * sizeof *words);
                array_size *= 2;
            }
            i = 0;
        }
        else
        {
            // reject other characters and 1-letter/number words
            i = 0;
        }
    }
    // last word
    if (i > 1) {
        word[i] = 0;
        words[*no_of_words] = strdup(word);
        ++(*no_of_words);
    }

    fclose(fp);
    return words;
}

int take_input(char *input_directory, char ***all_words, int *no_of_docs,
               int **word_count, char **doc_names) {
    DIR *directory;
    struct dirent *in_file;

    /* Scanning the in directory */
    if (!(directory = opendir(input_directory))) {
        fprintf(stderr, "Error in opening input directory\n");
        return 0;
    }
    while ((in_file = readdir(directory))) {
        // ignore current and parent directories
        if (!strcmp(in_file->d_name, "."))
            continue;
        if (!strcmp(in_file->d_name, ".."))
            continue;

        char file_path[256] = "";
        strcpy(file_path, input_directory);
        strcat(file_path, "/");
        strcat(file_path, in_file->d_name);

        int *no_of_words = malloc(sizeof(int));
        *no_of_words = 0;
        char **words = NULL;

        if (!(words = getwords(file_path, no_of_words))) {
            fprintf(stderr, "Error in reading words from doc file");
            return 0;
        }

        all_words[*no_of_docs] = words;
        word_count[*no_of_docs] = no_of_words;
        doc_names[*no_of_docs] = strdup(in_file->d_name);
        ++(*no_of_docs);
    }
    return 1;
}

