PLATFORM=UNIX
CAPENGINEPATH=/home/caleb/Projects/games/github/capengine
LOGLEVEL=Logger::CDEBUG
CC=g++
CPP=g++
INCLUDE=./src
INCLUDEDIRS=-I$(CAPENGINEPATH)
CFLAGS= -g -Wall -D$(PLATFORM) -DDEBUG -DLOGLEVEL=$(LOGLEVEL) -std=c++0x $(INCLUDEDIRS) `sdl-config --cflags`
LIBS=
#CFLAGS= -O3 -D$(PLATFORM) -DDEBUG

# (setq compile-command "(cd ~/Projects/games/topdown/src && make)")

#Application
topdown: topdown.o topdowngame.o
	$(CPP) $(CFLAGS) -o topdown topdown.o topdowngame.o libcapengine.so $(LIBS)

objects = topdown.o topdowngame.o

all: $(objects)

$(objects): %.o: %.cpp
	$(CC) -c $(CFLAGS) $< -o $@


.PHONEY: clean tests tags extract

clean:
	-rm -f *.o
	-rm -f *.so
	-rm topdown
	-rm test/*.o
	-rm test/test

extract:
	cp $(CAPENGINEPATH)/libcapengine.so ./

tests:
	cd test && $(MAKE)

tags:
	cd .. && ctags -e -R . && ctags -e -R /usr/include/SDL -a
