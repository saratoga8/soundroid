OBJS=BlueToothLib.o service.o synchronise.o

CC=gcc
CFLAGS=-Wall -c

LIBS_DIR=../../lib
#LIB=libBlueTooth.a
LIB=libBlueTooth.so

LOG_LIB_SRC_DIR=../../Log

LIB_NAME=BlueToothLib
TESTS_DIR=tests

vpath %.h . $(LOG_LIB_SRC_DIR) ..
vpath %.c . $(LOG_LIB_SRC_DIR) ..

$(LIB_NAME).o:	$(LIB_NAME).c $(LIB_NAME).h
	$(CC) $(CFLAGS) -I$(LOG_LIB_SRC_DIR) -I.. -fPIC $<

service.o:	service.c service.h
	$(CC) $(CFLAGS) -fPIC $<

synchronise.o:	synchronise.c synchronise.h
	$(CC) $(CFLAGS) -fPIC $<

$(LIB):	$(OBJS)
#	ar -rvs $@ $^
	$(CC) -shared -o $@ $^

install:	$(LIB)
	mkdir -p $(LIBS_DIR)
	cp $(LIB) $(LIBS_DIR) 

test:
	cd $(TESTS_DIR) && ./run_tests.sh && cd ..

mem_leak_chk:
	cd $(TESTS_DIR) && ./run_tests.sh mem && cd ..

clean:
	$(MAKE) --directory=$(TESTS_DIR) clean
	rm -f *.o *~ $(LIB)

.PHONY:	clean
