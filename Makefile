PROG=lcdsim
SOURCES=lcdsim.c handlers.c menudefs.c
CFLAGS=-Wall -std=c99 -g 
LDFLAGS=-lncurses

$(PROG): $(SOURCES)

clean:
	$(RM) $(PROG) *.o
	# for OSX
	$(RM) -r $(PROG).dSYM
