%{
#include <stdio.h>
#include <string.h>
int yylex(void);
void yyerror(char *s);
%}

%code requires{
    struct exp{
    char val[1000];
    int in;
};
}

%union {struct exp data;}
%start s
%token operator
%token operand
%type <data> operator operand s expr

%%
s:
    expr '\n' {printf("%s %d\n",$$.val, $$.in);}
    | s expr '\n' {printf("%s %d\n",$2.val, $2.in);}
    | error '\n'{yyerrok; yyclearin;}
    ;
expr:
    operand {strcpy($$.val, $1.val); $$.in = $1.in;}
    | expr expr operator {strcpy($$.val,$3.val); strcat($$.val, " "); strcat($$.val, $1.val);strcat($$.val, " "); strcat($$.val, $2.val);
                          $$.in = calculate($1.in, $2.in, $3.val);}
    ;

%%

void yyerror(char *s) {
 printf("invalid_input\n");
} 

int calculate(int a, int b, char* op)
{
    if(strcmp(op, "+")==0) return a + b;
    else if(strcmp(op, "-")==0) return a - b;
    else if(strcmp(op, "*")==0) return a * b;
    else if(strcmp(op, "/")==0) return a / b;
}

int main(int argc, char** argv) {
    if(argc==3)
    {
        freopen(argv[1], "r", stdin);
        freopen(argv[2], "w", stdout);
    }
    else{
        freopen("postfix.txt", "r", stdin);
        freopen("result.txt", "w", stdout);
    }
    yyparse();
    return 0;
}