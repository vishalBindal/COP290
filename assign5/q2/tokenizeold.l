%{
#include "y.tab.h"
#include <string.h>
#include <stdlib.h>
void yyerror(char *s);
%}

%%
-?[0-9]+    {strcpy(yylval.data.val, yytext); yylval.data.in = atoi(yytext); return operand;}
[-+/*]  {strcpy(yylval.data.val, yytext); return operator;}
[ \t]   ;
\n  return '\n';
.   ;
%%

int yywrap(void) {
    return 1;
}

