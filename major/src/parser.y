%{
#include <stdio.h>
#include <string.h>
int yylex(void);
void yyerror(char *s);
int i25=0;
int single_letter25[2] = {0,0}, three_letter25[2] = {0,0}, special_words25[2] = {0,0};
int cur_line_words25 = 0, cur_line_punct25 = 0;
int total_punct25[2] = {0,0};
%}

%code requires{
    struct word25{
    char text25[100];
    int len25;
};
}

%union {struct word25 data25;}
%start s
%token EOS
%token SPACE
%token WORDSP
%token WORD
%token PUNCT
%type <data25> WORDSP WORD

%%
s: 
    sentence EOS    {if(cur_line_words25>10) total_punct25[i25] += (1+cur_line_punct25); cur_line_words25=0; cur_line_punct25=0;}
    | s sentence EOS    {if(cur_line_words25>10) total_punct25[i25] += (1+cur_line_punct25); cur_line_words25=0; cur_line_punct25=0;}
    ;

sentence:
    WORD  {if($1.len25==1) ++single_letter25[i25]; if($1.len25==3) ++three_letter25[i25]; ++cur_line_words25;}
    | WORDSP    {if($1.len25==1) ++single_letter25[i25]; if($1.len25==3) ++three_letter25[i25]; ++special_words25[i25]; ++cur_line_words25;}
    | PUNCT {++cur_line_punct25;}
    | sentence WORD    {if($2.len25==1) ++single_letter25[i25]; if($2.len25==3) ++three_letter25[i25]; ++cur_line_words25;}
    | sentence WORDSP  {if($2.len25==1) ++single_letter25[i25]; if($2.len25==3) ++three_letter25[i25]; ++special_words25[i25]; ++cur_line_words25;}
    | sentence PUNCT    {++cur_line_punct25;}
    ;

%%

void yyerror(char *s) {
 printf("\n");
} 

int main(int argc25, char** argv25) {
    char doc1_25[100], doc2_25[100];
    if(argc25==3)
    {
        strcpy(doc1_25, argv25[1]);
        strcpy(doc2_25, argv25[2]);
    }
    else return 1;

    i25 = 0;
    freopen(doc1_25, "r", stdin);
    yyparse();

    i25 = 1; cur_line_words25 = 0; cur_line_punct25 = 0;
    freopen(doc2_25, "r", stdin);
    yyparse();

    for(int i=0;i<2;++i)
    {
        printf("#a%d = %d\n", i+1, single_letter25[i]);
        printf("#b%d = %d\n", i+1, three_letter25[i]);
        printf("#c%d = %d\n", i+1, special_words25[i]);
        printf("#d%d = %d\n", i+1, total_punct25[i]);
    }

    FILE* plotp25 = fopen("output/plotdata", "w");
    fprintf(plotp25, "Parameter \"Doc1\" \"Doc2\"\n");
    fprintf(plotp25, "a %d %d\n", single_letter25[0], single_letter25[1]);
    fprintf(plotp25, "b %d %d\n", three_letter25[0], three_letter25[1]);
    fprintf(plotp25, "c %d %d\n", special_words25[0], special_words25[1]);
    fprintf(plotp25, "d %d %d\n", total_punct25[0], total_punct25[1]);
    fclose(plotp25);

    FILE* gnup25;
    gnup25 = popen("gnuplot -persistent", "w");
    fprintf(gnup25, "set terminal png\n"
                  "set output \"output/plot.png\"\n"
                  "set grid\n"
                  "set title \"Comparison of documents\"\n"
                  "set ylabel \"Count\"\n"
                  "set xlabel \"Parameter\"\n"
                  "set style data histogram\n"
                  "set style fill solid\n"
                  "set style histogram clustered\n"
                  "plot for [COL=2:3] 'output/plotdata' using COL:xticlabels(1) title columnheader \n");
    fclose(gnup25);

    return 0;
}