OBJDIR = ../obj
SOURCEFILE = sum.c
OBJPATH = $(OBJDIR)/sum.o

all: $(OBJPATH) $(OBJDIR)

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJPATH): $(SOURCEFILE) $(OBJDIR)
	gcc -c $(SOURCEFILE) -o $(OBJPATH)

clean: 
	find $(OBJDIR) -name "sum.o" -type f -delete