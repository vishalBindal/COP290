#ifndef PLAGIARISM_CHECKER_SIMILARITY_H
#define PLAGIARISM_CHECKER_SIMILARITY_H

char **get_vocabulary(char ***all_words, int **word_count, int no_of_docs,
                      int *no_of_unique_words);

double **get_count_matrix(char ***all_words, int no_of_docs, int **word_count,
                          char **vocabulary, int no_of_unique_words);

void convert_to_tf_idf(double **matrix, int no_of_docs, int no_of_unique_words,
                       int **word_count);

double cosine(const double *a, const double *b, int n);

#endif //PLAGIARISM_CHECKER_SIMILARITY_H
