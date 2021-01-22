#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "arbprecision.h"
#include <complex.h>
#include <time.h>

typedef struct {
    bnum real;
    bnum imag;
} comp;

struct stringcomp {
    char real[max_limit];
    char imag[max_limit];
};

struct stringcomp parsetotwo(char *str) {
    struct stringcomp sc;
    char *ptr = str;
    while (*ptr == '(' || *ptr == ' ') {
        ptr++;
    }
    int numptr = 0;
    for (numptr = 0; numptr < max_limit; numptr++) {
        sc.real[numptr] = *ptr;
        ptr++;
        if (*ptr == ',' || *ptr == ' ') break;
    }
    sc.real[numptr + 1] = '\0';
    while (*ptr == ' ' || *ptr == ',') {
        ptr++;
    }
    for (numptr = 0; numptr < max_limit; numptr++) {
        sc.imag[numptr] = *ptr;
        ptr++;
        if (*ptr == ')' || *ptr == ' ') break;
    }
    sc.imag[numptr + 1] = '\0';
    return sc;
}

comp stringtocomp(char *str, long double complex *dc) {
    struct stringcomp sc = parsetotwo(str);
    *dc = strtold(sc.real, NULL) + I * strtold(sc.imag, NULL);

    comp c = {.real = stringtobnum(sc.real), .imag = stringtobnum(sc.imag)};
    return c;
}

char *comptostring(comp *c) {
    int len = c->real.len + c->imag.len + 6;
    char *str = malloc(len);
    int ptr = 0;
    str[ptr++] = '(';
    char *real = bnumtostring(&(c->real));
    for (int i = 0; i < (int) strlen(real); i++)
        str[ptr++] = real[i];
    str[ptr++] = ',';
    char *complex1 = bnumtostring(&(c->imag));
    for (int i = 0; i < (int) strlen(complex1); i++)
        str[ptr++] = complex1[i];
    str[ptr++] = ')';
    str[ptr] = '\0';
    return str;
}

comp addc(comp *ca, comp *cb) {
    comp cc = {.real = add(&(ca->real), &(cb->real)), .imag = add(&(ca->imag), &(cb->imag))};
    return cc;
}

comp subc(comp *ca, comp *cb) {
    comp cc = {.real = subtract(&(ca->real), &(cb->real)),
            .imag = subtract(&(ca->imag), &(cb->imag))};
    return cc;
}

comp mulc(comp *ca, comp *cb) {
    comp cc;
    comp cac = *ca, cbc = *cb;
    bnum c1 = multiply(&(cac.real), &(cbc.imag));
    bnum c2 = multiply(&(cbc.real), &(cac.imag));
    cc.imag = add(&c1, &c2);
    c1 = multiply(&(ca->real), &(cb->real));
    c2 = multiply(&(ca->imag), &(cb->imag));
    cc.real = subtract(&c1, &c2);
    return cc;
}

bnum modsquare(comp *c) {
    bnum sq = multiply(&(c->real), &(c->real));
    bnum te = multiply(&(c->imag), &(c->imag));
    sq = add(&sq, &te);
    return sq;
}

bnum absc(comp *c) {
    bnum sq = modsquare(c);
    sq = sqroot(&sq);
    return sq;
}

comp divc(comp *ca, comp *cb, int *invalid) {
    comp cbc = *cb;
    bnum divsr = modsquare(&cbc);
    if (is_zero(&divsr)) {
        *invalid = 1;
        return *cb;
    }
    bnum divsrcp = divsr;
    if (cb->imag.sign == '+') cb->imag.sign = '-';
    else cb->imag.sign = '+';
    comp dv = mulc(ca, cb);
    int inv;
    dv.real = divide(&(dv.real), &divsr, &inv);
    dv.imag = divide(&(dv.imag), &divsrcp, &inv);
    return dv;
}

// Remove leading and trailing whitespaces, newline characters from input string
char *trim(char *str) {
    int i = 0;
    while (*str == ' ') {
        i++;
        str++;
    }
    char *end;
    end = str + strlen(str) - 1;
    while (*end == ' ' || *end == '\r' || *end == '\n') end--;
    end[1] = '\0';
    return str;
}

// parse input string and store in array, using ' ' as delimiter
int word_array(char *str, char *array[]) {
    int i = 0;
    char *word = strtok(str, " ");
    while (word != NULL && i < max_limit) {
        array[i] = word;
        word = strtok(NULL, " ");
        i++;
    }
    array[i] = NULL;
    return i;
}

int main(int argc, char *argv[]) {
    FILE *infp, *outfp, *plotp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    infp = fopen(argv[1], "r");
    outfp = fopen(argv[2], "w");
    if (infp == NULL || outfp == NULL)
        perror("Error in opening/closing files");
    char *arr[3];
    long double complex dc1, dc2;
    double totaltime1[5] = {0, 0, 0, 0, 0};
    double totaltime2[5] = {0, 0, 0, 0, 0};
    double countofop[5] = {0, 0, 0, 0, 0};
    char *op[5] = {"ADD", "SUB", "PROD", "QUOT", "ABS"};
    clock_t begin, end;
    while ((read = getline(&line, &len, infp)) != -1) {
        trim(line);
        int noofconst = word_array(line, arr);
        if (strcmp(arr[0], op[0]) == 0) {
            comp c1 = stringtocomp(arr[1], &dc1);
            comp c2 = stringtocomp(arr[2], &dc2);
            begin = clock();
            comp c3 = addc(&c1, &c2);
            end = clock();
            totaltime1[0] += ((double) (end - begin)) * 1000000 / CLOCKS_PER_SEC;
            begin = clock();
            dc1 += dc2;
            end = clock();
            totaltime2[0] += ((double) (end - begin)) * 1000000 / CLOCKS_PER_SEC;
            countofop[0]++;
            char *strans = comptostring(&c3);
            fprintf(outfp, "%s\n", strans);
        } else if (strcmp(arr[0], op[1]) == 0) {
            comp c1 = stringtocomp(arr[1], &dc1);
            comp c2 = stringtocomp(arr[2], &dc2);
            begin = clock();
            comp c3 = subc(&c1, &c2);
            end = clock();
            totaltime1[1] += ((double) (end - begin)) * 1000000 / CLOCKS_PER_SEC;
            begin = clock();
            dc1 -= dc2;
            end = clock();
            totaltime2[1] += ((double) (end - begin)) * 1000000 / CLOCKS_PER_SEC;
            countofop[1]++;
            char *strans = comptostring(&c3);
            fprintf(outfp, "%s\n", strans);
        } else if (strcmp(arr[0], op[2]) == 0) {
            comp c1 = stringtocomp(arr[1], &dc1);
            comp c2 = stringtocomp(arr[2], &dc2);
            begin = clock();
            comp c3 = mulc(&c1, &c2);
            end = clock();
            totaltime1[2] += ((double) (end - begin)) * 1000000 / CLOCKS_PER_SEC;
            begin = clock();
            dc1 *= dc2;
            end = clock();
            totaltime2[2] += ((double) (end - begin)) * 1000000 / CLOCKS_PER_SEC;
            countofop[2]++;
            char *strans = comptostring(&c3);
            fprintf(outfp, "%s\n", strans);
        } else if (strcmp(arr[0], op[3]) == 0) {
            int invalid = 0;
            comp c1 = stringtocomp(arr[1], &dc1);
            comp c2 = stringtocomp(arr[2], &dc2);
            begin = clock();
            comp c3 = divc(&c1, &c2, &invalid);
            end = clock();
            if (invalid == 1) fprintf(outfp, "Division by zero\n");
            else {
                totaltime1[3] += ((double) (end - begin)) * 1000000 / CLOCKS_PER_SEC;
                begin = clock();
                dc1 /= dc2;
                end = clock();
                totaltime2[3] += ((double) (end - begin)) * 1000000 / CLOCKS_PER_SEC;
                countofop[3]++;
                char *strans = comptostring(&c3);
                fprintf(outfp, "%s\n", strans);
            }
        } else if (strcmp(arr[0], op[4]) == 0) {
            comp c1 = stringtocomp(arr[1], &dc1);
            begin = clock();
            bnum c3 = absc(&c1);
            end = clock();
            totaltime1[4] += ((double) (end - begin)) * 1000000 / CLOCKS_PER_SEC;
            begin = clock();
            dc1 = cabsl(dc1);
            end = clock();
            totaltime2[4] += ((double) (end - begin)) * 1000000 / CLOCKS_PER_SEC;
            countofop[4]++;
            bnum zero = {.len=1, .sign='+', .dec=0, .n="0"};
            comp ca = {.real = c3, .imag = zero};
            char *strans = comptostring(&ca);
            fprintf(outfp, "%s\n", strans);
        }
    }
    fclose(infp);
    fclose(outfp);
    plotp = fopen("plotdata", "w");
    fprintf(plotp, "operation \"arbprecision\" \"complex.h\"\n");
    for (int i = 0; i < 5; i++)
        if (countofop[i] != 0) {
            totaltime1[i] /= countofop[i];
            totaltime2[i] /= countofop[i];
            fprintf(plotp, "%s %f %f\n", op[i], totaltime1[i], totaltime2[i]);
        }
    fclose(plotp);
    FILE* gnup;
    gnup = popen("gnuplot -persistent", "w");
    fprintf(gnup, "set terminal png\n"
                  "set output \"%s\"\n"
                  "set grid\n"
                  "set title \"Comparison of times\"\n"
                  "set ylabel \"Time (in us)\"\n"
                  "set xlabel \"Operation\"\n"
                  "set style data histogram\n"
                  "set style fill solid\n"
                  "set style histogram clustered\n"
                  "set key at graph 1.05, 1.14\n"
                  "fn(v) = sprintf(\"%%.1f\", v)\n"
                  "plot \\\n"
                  "    for [COL=2:3] 'plotdata' using COL:xticlabels(1) title columnheader fs pattern 2, \\\n"
                  "    'plotdata' u ($0-1-1./6):2:(fn($2)) w labels  offset char 1,0.5 t '' , \\\n"
                  "    'plotdata' u ($0-1+1./6):3:(fn($3)) w labels  offset char 1,0.5 t ''", argv[3]);
    fclose(gnup);
    return 0;
}