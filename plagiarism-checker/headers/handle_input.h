#ifndef PLAGIARISM_CHECKER_HANDLE_INPUT_H
#define PLAGIARISM_CHECKER_HANDLE_INPUT_H

int take_input(char *input_directory, char ***all_words, int *no_of_docs, int **word_count, char **doc_names);
char **getwords(char *file_path, int *no_of_words);

#endif //PLAGIARISM_CHECKER_HANDLE_INPUT_H
