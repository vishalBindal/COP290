outFile:=bin/qSim
obj:=obj/qSim.o
mainsrc:= src/main.c

all: $(outFile)

$(outFile): $(obj) bin output
	gcc $(obj) -o $(outFile) -lm

$(obj): $(mainsrc) obj  
	gcc -c $(mainsrc) -o $(obj) -lm

bin:
	mkdir bin

obj:
	mkdir obj

output:
	mkdir output

clean:
	rm -rf obj
	rm -rf bin
	rm -rf output


