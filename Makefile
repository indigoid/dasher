PROG=lcdsim
SOURCES=lcdsim.c handlers.c menudefs.c
TESTS=geofence
CFLAGS=-Wall -std=c99
LDFLAGS=-lncurses

$(PROG): $(SOURCES)

all: $(PROG) $(TESTS)

clean:
	$(RM) $(PROG) $(TESTS) *.o
	# for OSX
	$(RM) -r $(PROG).dSYM

geofence: geofence.c
	$(CC) $(CFLAGS) -DBUILDTEST -o geofence geofence.c
