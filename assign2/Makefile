outFile:=exe/out
runFile:=exe/helperfile
mainObj:=obj/main.o
runObj:=obj/helper.o

all:$(outFile)

$(outFile): $(mainObj) $(runObj) exe
	gcc $(mainObj) -o $(outFile)
	gcc $(runObj) -o $(runFile)

$(mainObj): 2018CS50425_sh.c obj  
	gcc -c 2018CS50425_sh.c -o $(mainObj)

$(runObj): helper.c obj
	gcc -c helper.c -o $(runObj)

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

run: $(outFile) $(runFile)
	clear
	$(runfile) exe/out
