#ifndef ARBPRECISION_H
#define ARBPRECISION_H
#define max_limit 50000
typedef struct number {
    char n[max_limit]; //0-most significant
    int len; //length of string n
    int dec; //position of decimal from rhs
    char sign; //'+' or '-'
} bnum;
bnum unit();
bnum stringtobnum(char *input);
char *bnumtostring(bnum *bn);
int is_zero(bnum *bn);
bnum add(bnum *bn1, bnum *bn2);
bnum subtract(bnum *bn1, bnum *bn2);
bnum multiply(bnum *bn1, bnum *bn2);
bnum divide(bnum *bn1, bnum *bn2, int * invalid);
bnum absolutevalue(bnum *bn);
bnum power(bnum *a, bnum *b, int* invalid);
int compare(bnum *bn1, bnum *bn2);
bnum sqroot(bnum *bn1);
#endif