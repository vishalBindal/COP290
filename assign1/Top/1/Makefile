OBJDIR = ../obj
SOURCEFILE = factorial.c
OBJPATH = $(OBJDIR)/factorial.o

all: $(OBJPATH) $(OBJDIR)

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJPATH): $(SOURCEFILE) $(OBJDIR)
	gcc -c $(SOURCEFILE) -o $(OBJPATH)

clean: 
	find $(OBJDIR) -name "factorial.o" -type f -delete