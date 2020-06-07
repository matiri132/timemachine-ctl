LIB_LOCATION=/usr/local/lib
ULFIUS_INCLUDE=/usr/local/include

CC=gcc
CFLAGS+=-c -I$(ULFIUS_INCLUDE) -D_REENTRANT -DDEBUG -g -O0
LIBS=-lc -lulfius -lorcania -lyder -L$(LIB_LOCATION) 

all: timemachineapi

clean:
	rm -f *.o timemachineapi

../../src/libulfius.so:
	cd $(LIB_LOCATION) && $(MAKE) debug CURLFLAG=1 GNUTLSFLAG=1

timemachineapi.o: 
	$(CC) $(CFLAGS) timemachineapi.c
timemachineapi: $(LIB_LOCATION)/libulfius.so.2.6.7 timemachineapi.o timerstructs.o
	$(CC) -Wall -g -c timerstructs.c -o timerstructs.o
	$(CC) -o timemachineapi timemachineapi.o timerstructs.o $(LIBS)



