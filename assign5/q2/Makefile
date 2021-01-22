all: conv

conv: lexfile yaccfile
	gcc -w lex.yy.c y.tab.c -o conversion

lexfile: yaccfile tokenize.l
	lex tokenize.l

yaccfile: parser.y
	yacc -d parser.y

run:
	$(MAKE)
	./conversion postfix.txt result.txt

clean:
	if [ -f "./conversion" ]; then rm conversion ; fi;
	if [ -f "lex.yy.c" ]; then rm lex.yy.c ; fi;
	if [ -f "y.tab.c" ]; then rm y.tab.c ; fi;
	if [ -f "y.tab.h" ]; then rm y.tab.h ; fi
