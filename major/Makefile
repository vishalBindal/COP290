2018CS50425: all

all: main

main: lexfile yaccfile bin 
	gcc -w lex.yy.c y.tab.c -o bin/main

lexfile: yaccfile src/tokenize.l
	lex src/tokenize.l

yaccfile: src/parser.y
	yacc -d src/parser.y

bin:
	mkdir bin

run: output
	$(MAKE)
	bin/main doc1.txt doc2.txt

output:
	mkdir output

clean:
	if [ -f "bin/main" ]; then rm bin/main ; fi;
	if [ -f "lex.yy.c" ]; then rm lex.yy.c ; fi;
	if [ -f "y.tab.c" ]; then rm y.tab.c ; fi;
	if [ -f "y.tab.h" ]; then rm y.tab.h ; fi
	if [ -f "output/plot.png" ]; then rm output/plot.png ; fi
	if [ -f "output/plotdata" ]; then rm output/plotdata ; fi
