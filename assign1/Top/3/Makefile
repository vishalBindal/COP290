OBJDIR = ../obj
SOURCEFILE = rubyInC.c
OBJPATH = $(OBJDIR)/rubyInC.o

all: $(OBJPATH) $(OBJDIR)

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJPATH): $(SOURCEFILE) $(OBJDIR)
	gcc -c $(SOURCEFILE) -o $(OBJPATH) -I/usr/include/x86_64-linux-gnu/ruby-2.5.0 -I/usr/include/ruby-2.5.0 -lruby-2.5

clean: 
	find $(OBJDIR) -name "rubyInC.o" -type f -delete