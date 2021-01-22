#include <stdio.h>
#include <stdlib.h>
#include "../headers/handle_input.h"
#include "../headers/similarity.h"

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Invalid no. of arguments!\n");
        return 1;
    }
    char ***all_words = calloc(32, sizeof(char **));
    int **word_count = calloc(32, sizeof(int *));
    char **doc_names = calloc(32, sizeof(char *));

    char *input_directory = argv[2];
    int no_of_docs = 0;

    if (!(take_input(input_directory, all_words, &no_of_docs, word_count, doc_names))) {
        fprintf(stderr, "Error while reading input docs\n");
        return 1;
    }


    int target_word_count=0;
    char **target_words;
    char *target_path = argv[1];
    if (!(target_words = getwords(target_path, &target_word_count))) {
        fprintf(stderr, "Error while reading target doc\n");
        return 1;
    }
    all_words[no_of_docs] = target_words;
    word_count[no_of_docs] = malloc(sizeof(int));
    *(word_count[no_of_docs]) = target_word_count;
    ++no_of_docs;

    int no_of_unique_words = 0;
    char **vocabulary;
    if (!(vocabulary = get_vocabulary(all_words, word_count, no_of_docs,
                                      &no_of_unique_words))) {
        fprintf(stderr, "Error getting vocabulary\n");
        return 1;
    }

    double** matrix;
    if(!(matrix = get_count_matrix(all_words, no_of_docs, word_count, vocabulary,
            no_of_unique_words)))
    {
        fprintf(stderr, "Error calculating count matrix\n");
        return 1;
    }

    convert_to_tf_idf(matrix, no_of_docs, no_of_unique_words, word_count);

    double* target_vector = matrix[no_of_docs-1];
    for(int i=0;i<no_of_docs-1;++i)
    {
        double* a = matrix[i];
        double cos_theta = cosine(a, target_vector, no_of_unique_words);
        printf("%s %.2f%%\n", doc_names[i], cos_theta*100);
    }

    return 0;
}