obj := obj/main.o obj/handle_input.o obj/similarity.o 
exe := plagChecker	

all: $(exe)

$(exe): obj1 obj2 obj3
	gcc $(obj) -o $(exe) -lm

obj1: obj src/main.c
	gcc -c src/main.c -o obj/main.o -lm
obj2: obj src/handle_input.c
	gcc -c src/handle_input.c -o obj/handle_input.o -lm
obj3: obj src/similarity.c
	gcc -c src/similarity.c -o obj/similarity.o -lm

obj:
	mkdir obj

run:
	$(MAKE)
	./plagChecker corpus_files/catchmeifyoucan.txt corpus_files
	
clean:
	rm -rf obj
	rm -rf $(exe)




