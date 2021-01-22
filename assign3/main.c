#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define max_limit 50000

typedef struct number {
    char n[max_limit]; //0-most significant
    int len; //length of string n
    int dec; //position of decimal from rhs
    char sign; //'+' or '-'
} bnum;

// returns bnum of value 1
bnum unit() {
    bnum n1 = {.len=1, .dec=0, .sign='+'};
    n1.n[0] = '1';
    return n1;
}

//remove trailing and leading zeros
void stripnum(bnum *bn) {
    for (int i = bn->len - 1; i >= 0; i--) {
        if (bn->n[i] != '0' || bn->dec <= 0) break;
        bn->len--;
        bn->dec--;
    }
    int remove = 0;
    for (int i = 0; i < bn->len; i++) {
        if (bn->n[i] != '0') break;
        remove++;
        if (remove + bn->dec >= bn->len) {
            remove--;
            break;
        }
    }
    bn->len -= remove;
    for (int i = 0; i < bn->len; i++)
        bn->n[i] = bn->n[i + remove];
}

// convert string to bnum
bnum stringtobnum(char *input) {
    bnum bn = {.len = (int) strlen(input), .sign='+'};
    for (int i = 0; i < (int) strlen(input); i++)
        if (input[i] == '.') {
            bn.len--;
            bn.dec = (int) strlen(input) - i - 1;
            break;
        }
    if (input[0] == '+' || input[0] == '-') {
        bn.sign = input[0];
        bn.len--;
    }
    for (int i = 0, cur = 0; i < strlen(input); i++)
        if (input[i] != '+' && input[i] != '-' && input[i] != '.') {
            bn.n[cur] = input[i];
            cur++;
        }
    return bn;
}

// convert bnum to string
char *bnumtostring(bnum *bn) {
    int length = bn->len + 2;
    if (bn->sign != '-') length--;
    if (bn->dec == 0) length--;
    char *str = malloc(length + 1);
    int cur = 0;
    if (bn->sign == '-') {
        str[0] = '-';
        cur++;
    }
    for (int i = 0; i < bn->len - bn->dec; i++) {
        str[cur] = bn->n[i];
        cur++;
    }
    if (bn->dec != 0) {
        str[cur] = '.';
        cur++;
        for (int i = 0; i < bn->dec; i++) {
            str[cur] = bn->n[bn->len - bn->dec + i];
            cur++;
        }
    }
    return str;
}

// min of two integers
int min(int a, int b) {
    return a < b ? a : b;
}

//reverse a string of given length
void reverse(char *str, int len) {
    for (int i = 0; i < len / 2; i++) {
        char temp = str[i];
        str[i] = str[len - 1 - i];
        str[len - 1 - i] = temp;
    }

}

// equalise the decimal position of 2 bignums
void equalise(bnum *n1, bnum *n2) {
    if (n1->dec > n2->dec) {
        for (int i = 0; i < (n1->dec - n2->dec); i++)
            n2->n[n2->len + i] = '0';
        n2->len += (n1->dec - n2->dec);
        n2->dec = n1->dec;
    } else if (n2->dec > n1->dec) {
        for (int i = 0; i < (n2->dec - n1->dec); i++)
            n1->n[n1->len + i] = '0';
        n1->len += (n2->dec - n1->dec);
        n1->dec = n2->dec;
    }
}

// add 2 positive numbers
bnum add_aplusb(bnum *n1, bnum *n2) {
    bnum bn4 = {.len = n1->len, .sign = n1->sign, .dec = n1->dec};
    if (n1 == n2) {
        strcpy(bn4.n, n1->n);
        n1 = &bn4;
    }
    if (n1 == n2) {
        bnum n3 = *n1;
        n1 = &n3;
    }
    int carry = 0;
    bnum n3 = {.sign='+'};
    equalise(n1, n2);
    for (int i = 0; i < min(n1->len, n2->len); i++) {
        if (n1->n[n1->len - 1 - i] - '0' + n2->n[n2->len - 1 - i] - '0' + carry > 9) {
            n3.n[i] = n1->n[n1->len - 1 - i] + n2->n[n2->len - 1 - i] + carry - '0' - 10;
            carry = 1;
        } else {
            n3.n[i] = n1->n[n1->len - 1 - i] + n2->n[n2->len - 1 - i] + carry - '0';
            carry = 0;
        }
    }
    n3.len = n2->len;
    n3.dec = n2->dec;
    if (n1->len > n2->len) {
        n3.len = n1->len;
        for (int i = n2->len; i < n1->len; i++)
            if (n1->n[n1->len - 1 - i] + carry - '0' > 9) {
                n3.n[i] = n1->n[n1->len - 1 - i] + carry - 10;
                carry = 1;
            } else {
                n3.n[i] = n1->n[n1->len - 1 - i] + carry;
                carry = 0;
            }
    } else {
        for (int i = n1->len; i < n2->len; i++)
            if (n2->n[n2->len - 1 - i] + carry - '0' > 9) {
                n3.n[i] = n2->n[n2->len - 1 - i] + carry - 10;
                carry = 1;
            } else {
                n3.n[i] = n2->n[n2->len - 1 - i] + carry;
                carry = 0;
            }
    }
    if (carry == 1) {
        n3.len++;
        n3.n[n3.len - 1] = '1';
    }

    reverse(n3.n, n3.len);
    return n3;
}

// tens complement of bn ( with respect to another big number to find no of digits to be taken )
void tenscomp(bnum *bn, bnum *wrto) {

    for (int i = 0; i < bn->len; i++)
        bn->n[i] = '0' + '9' - bn->n[i];
    if (wrto->len > bn->len) {
        int newnines = wrto->len - bn->len;
        for (int i = bn->len - 1; i >= 0; i--) {
            bn->n[i + newnines] = bn->n[i];
        }
        for (int i = 0; i < newnines; i++)
            bn->n[i] = '9';
        bn->len += newnines;
    }
    bnum one = unit();
    one.dec = bn->dec;
    *bn = add_aplusb(bn, &one);
}

// check if bn is zero
int is_zero(bnum *bn) {
    stripnum(bn);
    return (bn->len == 1 && bn->n[0] == '0');
}

// subtract positive numbers a and b
bnum subtract_aminusb(bnum *bn1, bnum *bn2) {
    if (is_zero(bn2) == 1) return *bn1;
    bnum bn4 = {.len = bn1->len, .sign = bn1->sign, .dec = bn1->dec};
    if (bn1 == bn2) {
        strcpy(bn4.n, bn1->n);
        bn1 = &bn4;
    }
    equalise(bn1, bn2);
    tenscomp(bn2, bn1);
    bnum bn3 = add_aplusb(bn1, bn2);
    if (bn3.len > bn1->len && bn3.len > bn2->len) {
        //there was a carry
        bn3.len--;
        for (int i = 0; i < bn3.len; i++)
            bn3.n[i] = bn3.n[i + 1];
    } else {
        tenscomp(&bn3, &bn3);
        bn3.sign = '-';
    }
    return bn3;
}

// add 2 bignums bn1 and bn2 (all cases of sign)
bnum add(bnum *bn1, bnum *bn2) {
    bnum bn4 = {.len = bn1->len, .sign = bn1->sign, .dec = bn1->dec};
    if (bn1 == bn2) {
        strcpy(bn4.n, bn1->n);
        bn1 = &bn4;
    }
    if (bn1->sign == '+' && bn2->sign == '+') {
        bnum bn3 = add_aplusb(bn1, bn2);
        stripnum(&bn3);
        return bn3;
    } else if (bn1->sign == '+' && bn2->sign == '-') {
        bn2->sign = '+';
        bnum bn3 = subtract_aminusb(bn1, bn2);
        stripnum(&bn3);
        return bn3;
    } else if (bn1->sign == '-' && bn2->sign == '-') {
        bn1->sign = '+';
        bn2->sign = '+';
        bnum bn3 = add_aplusb(bn1, bn2);
        stripnum(&bn3);
        bn3.sign = '-';
        return bn3;
    } else {
        bn1->sign = '+';
        bnum bn3 = subtract_aminusb(bn2, bn1);
        stripnum(&bn3);
        return bn3;
    }
}

// subtract 2 bignums
bnum subtract(bnum *bn1, bnum *bn2) {
    bnum bn4 = {.len = bn1->len, .sign = bn1->sign, .dec = bn1->dec};
    if (bn1 == bn2) {
        strcpy(bn4.n, bn1->n);
        bn1 = &bn4;
    }
    if (bn2->sign == '+') bn2->sign = '-';
    else bn2->sign = '+';
    return add(bn1, bn2);
}

// multiply 2 bignums
bnum multiply(bnum *bn1, bnum *bn2) {
    bnum bn4 = {.len = bn1->len, .sign = bn1->sign, .dec = bn1->dec};
    if (bn1 == bn2) {
        strcpy(bn4.n, bn1->n);
        bn1 = &bn4;
    }
    stripnum(bn1);
    stripnum(bn2);
    bnum bn3 = {.len= bn1->len + bn2->len, .dec = bn1->dec + bn2->dec};
    bnum *p = &bn3;
    if (bn1->sign == bn2->sign)
        bn3.sign = '+';
    else bn3.sign = '-';

    for (int i = 0; i < bn1->len + bn2->len + 1; i++)
        bn3.n[i] = '0';
    for (int i = 0; i < bn2->len; i++) {
        // i = start index for incrementing bn3
        int cur = i;
        int digit = bn2->n[bn2->len - 1 - i] - '0';
        int carry = 0;
        for (int j = bn1->len - 1; j >= 0; j--) {
            int dig2 = bn1->n[j] - '0';
            int increment = (dig2 * digit + carry) % 10;
            carry = (dig2 * digit + carry) / 10;
            int last = bn3.n[cur] - '0';
            int tempcur = cur;
            while (last + increment > 9) {
                bn3.n[tempcur] += increment - 10;
                tempcur++;
                last = bn3.n[tempcur] - '0';
                increment = 1;
            }
            bn3.n[tempcur] += increment;
            cur++;
        }
        bn3.n[cur] += carry;
    }
    reverse(bn3.n, bn3.len);
    stripnum(&bn3);
    return bn3;
}

// return bignum of value 10^pow
bnum power10(int pow) {
    if (pow <= 0) return unit();
    bnum bn = {.len = pow + 1, .dec=0, .sign='+'};
    bn.n[0] = '1';
    for (int i = 0; i < pow; i++)
        bn.n[1 + i] = '0';
    return bn;
}

// ensure decimal length of bignum >= mindec
void incprecision(bnum *bn, int mindec) {
    if (bn->dec >= mindec) return;
    for (int i = 0; i < (mindec - bn->dec); i++)
        bn->n[bn->len + i] = '0';
    bn->len += (mindec - bn->dec);
    bn->dec = mindec;
}

// ensure length of bignum >= minlen
void ensurelength(bnum *bn, int minlen) {
    if (bn->len >= minlen) return;
    for (int i = bn->len; i < minlen; i++)
        bn->n[i] = '0';
    bn->dec += (minlen - bn->len);
    bn->len = minlen;
}

// find max factor to multiply divbn so that it is less than bn
int multiplyfactor2(bnum *bn, bnum *divbn) {
    bnum divbncopy = *divbn;
    bnum temp1, temp2;
    int mf = 0;
    while ((temp1 = subtract_aminusb(bn, &divbncopy)).sign == '+') {
        temp2 = temp1;
        *bn = temp2;
        mf++;
        divbncopy = *divbn;
    }
    return mf;
}

// In cases where len becomes shorter than dec, correct length by multiplying bnum(with no dec) by 10^(-dec)
void correctlength(bnum *bn) {
    bnum bn2 = {.len = bn->dec + 1, .dec = bn->dec, .sign = '+'};
    for (int i = 0; i < bn2.len - 1; i++)
        bn2.n[i] = '0';
    bn2.n[bn2.len - 1] = '1';

    bn->dec = 0;
    *bn = multiply(bn, &bn2);
}

// Divide bn1 by bn2. If bn2 == 0 set invalid to 1
bnum divide(bnum *bn1, bnum *bn2, int * invalid) {
    bnum bn4 = {.len = bn1->len, .sign = bn1->sign, .dec = bn1->dec};
    if (bn1 == bn2) {
        strcpy(bn4.n, bn1->n);
        bn1 = &bn4;
    }
    stripnum(bn1);
    stripnum(bn2);
    if (bn2->len == 1 && bn2->n[0] == '0') {
        *invalid = 1;
        return *bn2;
    }
    //removing decimal from denominator
    bnum temp = power10(bn2->dec);
    *bn1 = multiply(bn1, &temp);
    bn2->dec = 0;
    stripnum(bn2);
    //inc precision in numerator to ensure 20 decimal places in answer
    ensurelength(bn1, bn2->len);
    bnum bn3 = {.len=0, .dec = 0};
    if (bn1->sign == bn2->sign) bn3.sign = '+';
    else bn3.sign = '-';

    //starting portion of dividend
    int startpos = 0, endpos = bn2->len - 1;
    char divpart[endpos - startpos + 1];
    divpart[endpos - startpos + 1] = '\0';
    for (int i = 0; i < endpos - startpos + 1; i++)
        divpart[i] = bn1->n[startpos + i];
    bnum bn = stringtobnum(divpart);
    int final_dec = bn1->dec;
    for (int i = 0;; i++) {
        bn3.n[i] = '0' + multiplyfactor2(&bn, bn2);
        endpos++;
        bn.len++;
        bn3.len++;
        if (endpos >= bn1->len) {
            if (final_dec >= 20) break;
            final_dec++;
            bn.n[bn.len - 1] = '0';
        } else
            bn.n[bn.len - 1] = bn1->n[bn.len - 1];
    }
    bn3.dec = final_dec;
    correctlength(&bn3);
    stripnum(&bn3);
    int lag = bn.len - (int)strlen(bn.n);
    for(int i=0;i<lag;i++)
        bn.n[bn.len-1-i] = '0';
    if(!is_zero(&bn)) {
        incprecision(&bn3, 20);
    }
    return bn3;
}

// absolute value of bn
bnum absolutevalue(bnum *bn) {
    bn->sign = '+';
    stripnum(bn);
    return *bn;
}

// fast power of a^b = (a^2)^(b/2) if b is even, else a*(a^(b-1)) if b is odd.
// uni is a bignum of value 1
bnum recpow(bnum a, bnum b, bnum *uni) {
    bnum un = *uni;
    if (is_zero(&b)) {
        return *uni;
    }
    if ((b.n[b.len - 1] - '0') % 2 == 0) {
        a = multiply(&a, &a);
        bnum two = add_aplusb(&un, &un);
        int *temp;
        b = divide(&b, &two, temp);
        return recpow(a, b, uni);
    }
    bnum c = recpow(a, subtract_aminusb(&b, &un), uni);
    return multiply(&a, &c);
}

// a^b. Set invalid to 1 if either b is float, or a=0 and b is -ve
bnum power(bnum *a, bnum *b, int* invalid) {
    if (b->dec > 0)
    {
        *invalid = 1;
        return *b;
    }
    bnum uni = unit();
    if (b->sign == '-') {
        b->sign = '+';
        bnum uni1 = unit();
        *a = divide(&uni1, a, invalid);
        if(*invalid == 1)
            return *b;
    }
    return recpow(*a, *b, &uni);
}

// return 1 if bn1>bn2, -1 if bn2>bn1, 0 if equal
int compare(bnum *bn1, bnum *bn2) {
    stripnum(bn1);
    stripnum(bn2);
    equalise(bn1, bn2);
    if (bn1->len != bn2->len) {
        if (bn1->len > bn2->len) return 1;
        else return -1;
    }
    for (int i = 0; i < bn1->len; i++)
        if (bn1->n[i] != bn2->n[i]) {
            if (bn1->n[i] > bn2->n[i]) return 1;
            else return -1;
        }
    return 0;
}

//find max factor such that, when appended to divbn and multiplied by the factor, is <= bn
int multiplyfactorroot(bnum *bn, bnum *divbn) {
    bnum divpre = *divbn;
    bnum quocur = {.len=1, .dec=0, .sign='+'};
    bnum prodcur;
    int comp;
    int mf = 0;
    for (int i = 0; i <= 9; i++) {
        quocur.n[0] = (char) ('0' + i);
        divpre.len++;
        divpre.n[divpre.len - 1] = (char) ('0' + i);
        prodcur = multiply(&divpre, &quocur);
        comp = compare(&prodcur, bn);
        if (comp == 0) mf = i;
        if (comp >= 0) break;
        mf = i;
        divpre = *divbn;
        quocur.len = 1;
        quocur.dec = 0;
        quocur.sign = '+';
    }
    quocur.n[0] = (char) ('0' + mf);
    divpre = *divbn;
    divpre.len++;
    divpre.n[divpre.len - 1] = (char) ('0' + mf);
    prodcur = multiply(&divpre, &quocur);
    *bn = subtract_aminusb(bn, &prodcur);
    return mf;
}

//Square root of bn1
bnum sqroot(bnum *bn1) {
    stripnum(bn1);
    //inc precision in numerator to ensure 20 decimal places in answer
    incprecision(bn1, 40);
    if (bn1->dec % 2 == 1) incprecision(bn1, bn1->dec + 1);

    bnum bn3 = {.len=bn1->len / 2, .dec = bn1->dec / 2, .sign='+'};
    if (bn1->len % 2 == 1) bn3.len++;

    //starting portion of dividend
    int startpos = 0, endpos = 0;
    if ((bn1->len - bn1->dec) % 2 == 0) endpos++;
    char divpart[endpos - startpos + 1];
    divpart[endpos - startpos + 1] = '\0';
    for (int i = 0; i < endpos - startpos + 1; i++)
        divpart[i] = bn1->n[startpos + i];
    bnum bn = stringtobnum(divpart);

    bnum lhsnum = {.n[0]='0', .len=1, .dec=0, .sign='+'};
    char lastfact;
    bnum lastfactnum = unit();
    for (int i = 0; endpos < bn1->len; i++) {
        lastfact = (char) ('0' + multiplyfactorroot(&bn, &lhsnum));
        bn3.n[i] = lastfact;
        lastfactnum.n[0] = lastfact;
        lhsnum.len++;
        lhsnum.n[lhsnum.len - 1] = lastfact;
        lhsnum = add_aplusb(&lhsnum, &lastfactnum);
        bn.len += 2;
        endpos += 2;
        bn.n[bn.len - 1] = bn1->n[endpos];
        bn.n[bn.len - 2] = bn1->n[endpos - 1];
    }
    correctlength(&bn3);
    stripnum(&bn3);
    int lag = bn.len - (int)strlen(bn.n);
    for(int i=0;i<lag;i++)
        bn.n[bn.len-1-i] = '0';
    if(!is_zero(&bn)) {
        incprecision(&bn3, 20);
    }
    return bn3;
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
    FILE *infp, *outfp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    infp = fopen(argv[1], "r");
    outfp = fopen(argv[2], "w");
    if (infp == NULL || outfp == NULL)
        perror("Error in opening/closing files");
    char *arr[3];
    while ((read = getline(&line, &len, infp)) != -1) {
        trim(line);
        int noofconst = word_array(line, arr);
        if (strcmp(arr[0], "ADD") == 0) {
            bnum b1 = stringtobnum(arr[1]);
            bnum b2 = stringtobnum(arr[2]);
            bnum b3 = add(&b1, &b2);
            char *strans = bnumtostring(&b3);
            fprintf(outfp, "%s\n", strans);
        } else if (strcmp(arr[0], "SUB") == 0) {
            bnum b1 = stringtobnum(arr[1]);
            bnum b2 = stringtobnum(arr[2]);
            bnum b3 = subtract(&b1, &b2);
            char *strans = bnumtostring(&b3);
            fprintf(outfp, "%s\n", strans);
        } else if (strcmp(arr[0], "MUL") == 0) {
            bnum b1 = stringtobnum(arr[1]);
            bnum b2 = stringtobnum(arr[2]);
            bnum b3 = multiply(&b1, &b2);
            char *strans = bnumtostring(&b3);
            fprintf(outfp, "%s\n", strans);
        } else if (strcmp(arr[0], "DIV") == 0) {
            int invalid=0;
            bnum b1 = stringtobnum(arr[1]);
            bnum b2 = stringtobnum(arr[2]);
            bnum b3 = divide(&b1, &b2, &invalid);
            if(invalid == 1) fprintf(outfp, "Division by zero\n");
            else {
                char *strans = bnumtostring(&b3);
                fprintf(outfp, "%s\n", strans);
            }
        } else if (strcmp(arr[0], "ABS") == 0) {
            bnum b1 = stringtobnum(arr[1]);
            bnum b3 = absolutevalue(&b1);
            char *strans = bnumtostring(&b3);
            fprintf(outfp, "%s\n", strans);
        } else if (strcmp(arr[0], "POW") == 0) {
            int invalid = 0;
            bnum b1 = stringtobnum(arr[1]);
            bnum b2 = stringtobnum(arr[2]);
            bnum b3 = power(&b1, &b2, &invalid);
            if(invalid == 1) fprintf(outfp, "Error: float exponent OR negative power of zero\n");
            else {
                char *strans = bnumtostring(&b3);
                fprintf(outfp, "%s\n", strans);
            }
        } else if (strcmp(arr[0], "SQRT") == 0) {
            bnum b1 = stringtobnum(arr[1]);
            if(b1.sign == '-') fprintf(outfp, "Square root of negative number\n");
            else {
                bnum b3 = sqroot(&b1);
                char *strans = bnumtostring(&b3);
                fprintf(outfp, "%s\n", strans);
            }
        }
    }
    fclose(infp);
    fclose(outfp);
    return 0;
}