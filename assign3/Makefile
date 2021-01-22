outObj:=mainfile

all:$(outObj)

$(outObj): main.c
	gcc main.c -o $(outObj)

clean:
	if [ -f "./mainfile" ]; then rm mainfile ; fi