OBJS=MsgsQueue.o MsgsQueueClient.o MsgsQueueServer.o

LOG_LIB_SRC_DIR=../Log
LOG_LIB_SRC_FILES=Log.h

CC=gcc
CFLAGS=-Wall -c -O2

LIBS_DIR=../lib
LIBS=-lMsgsQueue -lLog -lpthread -lcunit

LIB=libMsgsQueue.a

vpath %.h . $(LOG_LIB_SRC_DIR)
vpath %.c . $(LOG_LIB_SRC_DIR)


MEMCHECK_FILE=memcheck.res
TEST=test
TEST_SND_MSGS=testSndMsgs

mem_leak_chk: $(TEST)
	valgrind -q --log-file=$(MEMCHECK_FILE) --leak-check=full ./$(TEST) > /dev/null

$(TEST):	install $(TEST).o
	$(CC) -L$(LIBS_DIR) -o $@ $(TEST).o $(LIBS)
	./$(TEST)

$(TEST).o: $(TEST).c $(LOG_LIB_SRC_FILES)
	$(CC) $(CFLAGS) -I$(LOG_LIB_SRC_DIR) $<

$(TEST_SND_MSGS):	install $(TEST_SND_MSGS).o
	$(CC) -L$(LIBS_DIR) -o $@ $(TEST_SND_MSGS).o $(LIBS)

$(TEST_SND_MSGS).o: $(TEST_SND_MSGS).c $(LOG_LIB_SRC_FILES)
	$(CC) $(CFLAGS) -I$(LOG_LIB_SRC_DIR) -I../headers/commands $<

$(LIB):	$(OBJS)
	ar -rcs $@ $^

MsgsQueue.o:	MsgsQueue.c MsgsQueue.h
	$(CC) $(CFLAGS) -static -I$(LOG_LIB_SRC_DIR) $<

MsgsQueueClient.o:	MsgsQueueClient.c MsgsQueueClient.h
	$(CC) $(CFLAGS) -static -I$(LOG_LIB_SRC_DIR) $<

MsgsQueueServer.o:	MsgsQueueServer.c MsgsQueueServer.h
	$(CC) $(CFLAGS) -static -I$(LOG_LIB_SRC_DIR) $<


install:	$(LIB)
	mv $(LIB) $(LIBS_DIR) 

clean: 
	rm -f *~ *.o *.a $(TEST) log.txt $(MEMCHECK_FILE)

.PHONY:	clean install mem_leak_chk

