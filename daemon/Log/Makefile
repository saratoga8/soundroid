CC=gcc
CFLAGS=-Wall -c -O2

LIB=libLog.so
LIBS_DIR=../lib

vpath %.c tests
vpath %.h tests

MEMCHECK_FILE=memcheck.res
TEST=test
DATE=$(shell date '+%a %b %_d %H:%M')

mem_leak_chk: $(TEST) 
	valgrind -q --log-file=$(MEMCHECK_FILE) --leak-check=full ./$(TEST) $(DATE) > /dev/null

$(TEST):	$(TEST).o install
	$(CC) -L. -o $@ $< -lLog -lcunit
	./$(TEST) $(DATE)

$(LIB): Log.o
	$(CC) -shared -o $@ $<

$(TEST).o:	test.c Log.h $(LIB)
	$(CC) $(CFLAGS) $<

Log.o:	Log.c Log.h
	$(CC) $(CFLAGS) -fPIC $<

install:	$(LIB)
	cp $(LIB) $(LIBS_DIR)

clean:
	rm -f *~ *.o *.so log.txt $(MEMCHECK_FILE) $(TEST)

.PHONY:	clean install mem_leak_chk

