%{
#include "y.tab.h"
#include <string.h>
#include <stdlib.h>
void yyerror(char *s25);
int distinct(char* str25);
%}

%%
[vishalVISHAL]{1,3} {yylval.data25.len25 = strlen(yytext); strcpy(yylval.data25.text25, yytext); if(distinct(yytext)) return WORDSP; return WORD;}
[a-zA-Z0-9]+   {yylval.data25.len25 = strlen(yytext); strcpy(yylval.data25.text25, yytext); return WORD;}
[,:;"']     {return PUNCT;}
[\.?!]+  {return EOS;}
[ \n\t]     ;
.   ;

%%

int yywrap(void) {
    return 1;
}

int distinct(char* str25)
{
    int len25 = strlen(str25);
    if(len25==0)
        return 0;
    if(len25==1)
        return 1;
    if(len25==2)
        return str25[0]!=str25[1];
    return str25[0]!=str25[1] && str25[1]!=str25[2] && str25[2]!=str25[0];
}

