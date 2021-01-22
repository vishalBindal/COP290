all: runlex

run:
	$(MAKE)
	./decomment <somefile.c >somefileWithoutComments.c 2> errorsAndWarnings

runlex: main.l
	lex main.l
	cc -o decomment lex.yy.c -ll

clean:
	if [ -f "./decomment" ]; then rm decomment; fi;
	if [ -f "lex.yy.c" ]; then rm lex.yy.c; fi

