outObj:=mainfile

all:$(outObj)

$(outObj): main.c
	gcc main.c -o $(outObj) -L. -larbprecision -lm

clean:
	if [ -f "./mainfile" ]; then rm mainfile ; fi

cleanAll:
	if [ -f "./mainfile" ]; then rm mainfile ; fi;
	if [ -f "./codegnu" ]; then rm codegnu ; fi;
	if [ -f "./plotdata" ]; then rm plotdata ; fi
