### macro definitions
PROJ = node
SVN_LOC = http://cs371d-pj1.googlecode.com/svn/trunk/

MAIN = main
EXT = cpp
APP = dcnode
INPUT = input
SVN_FILE = Subversion.log

### compiler definitions
CC = g++
CFLAGS = -o $(APP) -ansi -pedantic -Wall -pthread -O2 -ggdb
BOOST_ROOT = ~/boost/
LIB =

ifeq ($(shell hostname | cut -d. -f2-),cs.utexas.edu)
	BOOST_ROOT = /public/linux/include/boost-1_38/
	LIB = /public/linux/lib/libboost_system-gcc42-mt-1_38.a
else
	BOOST_ROOT = ~/boost/
	LIB = ~/lib/boost/lib/libboost_system-gcc43-mt.a
endif
CFLAGS += -I $(BOOST_ROOT)

### make options
main: $(MAIN).$(EXT)
	@clear
	### COMPILING
	$(CC) $(CFLAGS)	$(MAIN).$(EXT) $(LIB)
	@#valgrind $(APP) <$(INPUT) 

run: main
	@echo "\n"
	### EXECUTING
	./$(APP) 10000

debug: $(MAIN).$(EXT)
	@clear
	### COMPILING
	$(CC) $(CFLAGS)	-DDEBUG $(MAIN).$(EXT) $(LIB)

	@echo "\n"
	### EXECUTING
	@./$(APP) 10000 < $(INPUT)
	@#valgrind $(APP) <$(INPUT) >$(PROJ).out 2>&1
	@#less $(PROJ).out
	@#rm -f $(PROJ).out

client:
	$(CC) $(CFLAGS)	client.cpp -o client $(LIB)

server:
	$(CC) $(CFLAGS)	server.cpp -o server $(LIB)

network:
	@clear
	$(CC) $(CFLAGS)	client.cpp -o client $(LIB)
	$(CC) $(CFLAGS)	server.cpp -o server $(LIB)

gdb: $(MAIN).$(EXT)
	@clear
	$(CC) $(CFLAGS) -ggdb -DDEBUG $(MAIN).$(EXT)
	gdb $(APP)

clean:
	rm -f $(APP)
	rm -f client
	rm -f server
	rm -f *.out
	rm -f *.log

docs: Doxyfile
	doxygen Doxyfile

doxyfile:
	doxygen -g

log:
	svn log $(SVN_LOC) >$(SVN_FILE)
	head -n20 $(SVN_FILE)
