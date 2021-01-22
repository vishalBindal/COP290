#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int b_search(int l, int r, char *el, char **array) {
    if(l>r)
        return -1;
    if (l == r)
        return l;
    int mid = (l + r) / 2;
    int c = strcmp(array[mid], el);
    if (c > 0)
        return b_search(l, mid - 1, el, array);
    else if (c < 0)
        return b_search(mid + 1, r, el, array);
    else return mid;
}

int string_cmp(const void *a, const void *b) {
    const char **ia = (const char **) a;
    const char **ib = (const char **) b;
    return strcmp(*ia, *ib);
}

char **get_vocabulary(char ***all_words, int **word_count, int no_of_docs,
                      int *no_of_unique_words) {
    int total_word_count = 0;
    for (int i = 0; i < no_of_docs; ++i)
        total_word_count += *(word_count[i]);
    char **words_inc_dup; // all words including duplicates
    if (!(words_inc_dup = calloc(total_word_count, sizeof(char *)))) {
        fprintf(stderr, "Out of memory for storing all words\n");
        return NULL;
    }
    int k = 0;
    for (int i = 0; i < no_of_docs; ++i)
        for (int j = 0; j < *(word_count[i]); ++j)
            words_inc_dup[k++] = strdup(all_words[i][j]);

    // sorting array of all words (including duplicates)
    qsort(words_inc_dup, total_word_count, sizeof(char *), string_cmp);

    char **vocabulary;
    if (!(vocabulary = calloc(total_word_count, sizeof(char *)))) {
        fprintf(stderr, "Out of memory for storing vocabulary\n");
        return NULL;
    }

    vocabulary[0] = strdup(words_inc_dup[0]);
    k = 1;
    for (int i = 1; i < total_word_count; ++i)
        if (strcmp(words_inc_dup[i], words_inc_dup[i - 1]) != 0)
            vocabulary[k++] = strdup(words_inc_dup[i]);

    *(no_of_unique_words) = k;
    return vocabulary;
}

double **get_count_matrix(char ***all_words, int no_of_docs, int **word_count,
                          char **vocabulary, int no_of_unique_words) {
    double **matrix;
    if (!(matrix = calloc(no_of_docs, sizeof(double *)))) {
        fprintf(stderr, "Memory exhausted for allocating TF-IDF model matrix");
        return NULL;
    }
    for (int i = 0; i < no_of_docs; ++i)
        if (!(matrix[i] = calloc(no_of_unique_words, sizeof(double)))) {
            fprintf(stderr, "Memory exhausted for allocating TF-IDF model matrix");
            return NULL;
        }

    for (int i = 0; i < no_of_docs; ++i) {
        for (int j = 0; j < *(word_count[i]); ++j) {
            int pos = b_search(0, no_of_unique_words - 1, all_words[i][j], vocabulary);
            if (pos < 0 || pos >= no_of_unique_words) {
                fprintf(stderr, "Word not found in vocabulary\n");
                return NULL;
            } else
                ++matrix[i][pos];
        }
    }
    return matrix;
}

void convert_to_tf_idf(double **matrix, int no_of_docs, int no_of_unique_words,
                       int **word_count) {
    for (int j = 0; j < no_of_unique_words; ++j) {
        int docs_having_word = 0;
        for (int i = 0; i < no_of_docs; ++i)
            if (matrix[i][j] > 0)
                ++docs_having_word;
        for (int i = 0; i < no_of_docs; ++i) {
            // term frequency
            matrix[i][j] /= *(word_count[i]);
            // inverse document frequency
            matrix[i][j] *= log(((double) (1 + no_of_docs)) / (1 + docs_having_word));
        }
    }
}

double cosine(const double *a, const double *b, int n) {
    double a_norm = 0, b_norm = 0, dot_product = 0;
    for (int i = 0; i < n; ++i) {
        a_norm += a[i] * a[i];
        b_norm += b[i] * b[i];
        dot_product += a[i] * b[i];
    }
    a_norm = sqrt(a_norm);
    b_norm = sqrt(b_norm);
    double cos_theta = dot_product / (a_norm * b_norm);
    return cos_theta;
}