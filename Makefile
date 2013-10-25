PROG=lcdsim
SOURCES=lcdsim.c handlers.c menudefs.c fdirection.c geofence.c
TESTS=geofence fdirection
CFLAGS=-Wall -std=c11 -Iinclude
LDFLAGS=-lncurses


all: $(PROG) $(TESTS)

clean:
	$(RM) $(PROG) $(TESTS) *.o
	# for OSX
	$(RM) -r $(PROG).dSYM

$(PROG): $(SOURCES)

geofence: geofence.c
	$(CC) $(CFLAGS) -DBUILDTEST -o geofence geofence.c

fdirection: fdirection.c
	$(CC) $(CFLAGS) -DBUILDTEST -o fdirection fdirection.c
