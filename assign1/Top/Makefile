outFile:=exe/out
mainObj:=obj/main.o
sub1:=1
sub2:=2
sub3:=3
sub4:=4
functions:=$(sub1) $(sub2) $(sub3) $(sub4)
.PHONY: all $(functions)

all:$(outFile)

$(outFile): $(mainObj) $(functions) exe
	gcc $(wildcard obj/*.o) -o $(outFile) -I/usr/include/python3.6 -lpython3.6m -I/usr/include/x86_64-linux-gnu/ruby-2.5.0 -I/usr/include/ruby-2.5.0 -lruby-2.5

$(mainObj): main.c obj  
	gcc -c main.c -o $(mainObj)

$(functions):
	$(MAKE) --directory=$@ 

exe:
	mkdir exe

obj:
	mkdir obj

clean:
	if [ -d "./obj" ]; then find obj -name "*.o" -type f -delete ; fi
	if [ -d "./exe" ]; then find exe -name "out" -type f -delete ; fi

cleanAll:
	rm -rf exe
	rm -rf obj

fun1:
	$(MAKE) --directory=1

fun2:
	$(MAKE) --directory=2

fun3:
	$(MAKE) --directory=3

fun4:
	$(MAKE) --directory=4

run: $(outFile)
	clear
	$(outFile)