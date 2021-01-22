%{
    // lastcom = last comment open, linecount = current line no. , success = whether no errors
    int lastcom=1,linecount=1,success=1; 

/*
wofs : Without forward slash (/) or newline (\n) 
wostar : Without * or \n 
wocomclose : Not containing comment close or newline  
wocomopen : Not containing comment open or newline 
wofsq : Without / or newline or quotes 
wostarq : Without * or newline or quotes 
wocomopenandq : Without comment open and quotes 
*/
%}

wofs [^/\n]      
wostar [^*\n]  
wocomclose ({wostar}*(\*{wofs})?)*(\*)?   
wocomopen ({wofs}*(\/{wostar})?)*(\/)?   
wofsq [^/\n"']   
wostarq [^*\n"']  
wocomopenandq ({wofsq}*(\/{wostarq})?)*(\/)?    
%s copen      
%s cmulti     
%s dqopen     
%s sqopen     

%%
\n  {linecount++;ECHO;}    
<INITIAL>{wocomopenandq}\"  {ECHO;BEGIN dqopen;}   //  to sqopen state on detecting singlequote, for handling doublequotes
<INITIAL>{wocomopenandq}\'  {ECHO;BEGIN sqopen;}     // to sqopen state on detecting singlequote, for handling singlequotes
<INITIAL>"/*"   {lastcom = linecount;BEGIN copen; }    // to copen state on detecting open comment, update lastcom to preserve lineno in case error occurs
<copen>{wocomclose}\n {printf("\n"); BEGIN cmulti;} //to cmulti state, for handling multiline comments, print newline to preserve line no
<cmulti>{wocomclose}\n    printf("\n");     // print newline to preserve line no
<cmulti>{wocomclose}"*/"  {BEGIN INITIAL;}   // go back to initial state when multiline comment ends 
<copen>{wocomclose}"*/"   {printf(" ");BEGIN INITIAL;}      // print space (to ensure comment acts as delimiter) before going back to initial state when singleline comment ends 
<dqopen>\\\"  ECHO;     // for handling escape doublequotes inside string 
<sqopen>\\\'    ECHO;   // for handling escape singlequotes inside character literal
<dqopen>\"  {ECHO;BEGIN INITIAL;}   // back to initial state when string ends
<sqopen>\' {ECHO; BEGIN INITIAL;}   // back to initial state when character literal ends 
<copen>({wocomclose}\n*)*"<<EOF>>"  {success=0;fprintf(stderr, "Error: line %d: unterminated comment\n", lastcom);}     //if comment does not close till EOF, then print on stderr
%%

int main()
{
    yylex();
    if(success==0) 
        return EXIT_FAILURE; 
    return 0;
}