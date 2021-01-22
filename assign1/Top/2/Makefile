OBJDIR = ../obj
SOURCEFILE = randomPython.c
OBJPATH = $(OBJDIR)/randomPython.o

all: $(OBJPATH) $(OBJDIR)

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJPATH): $(SOURCEFILE) $(OBJDIR)
	gcc -c $(SOURCEFILE) -o $(OBJPATH) -I/usr/include/python3.6 -lpython3.6m

clean: 
	find $(OBJDIR) -name "randomPython.o" -type f -delete