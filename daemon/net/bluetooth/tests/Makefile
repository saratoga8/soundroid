CC=gcc
CFLAGS=-Wall -c

LIBS_DIR=../../../lib
LIB=libBlueTooth.a
LIBS=-lpthread -lBlueTooth -lbluetooth -lLog

TESTS_DIR=tests
TEST=test

LOG_LIB_SRC_DIR=../../../Log
LOG_LIB_SRC_FILES=Log.h Log.c

NET_DIR=../..

CLIENT=client

vpath %.h . $(LOG_LIB_SRC_DIR) $(NET_DIR) ..
vpath %.c . $(LOG_LIB_SRC_DIR) $(NET_DIR)

tests:	$(TEST) $(CLIENT)


$(TEST):	$(TEST).o 
	$(CC) -L$(LIBS_DIR) -o $@ $< $(LIBS)

$(TEST).o: $(TEST).c $(LOG_LIB_SRC_FILES) BlueToothLib.h synchronise.h addr.h
	$(CC) $(CFLAGS) -I$(LOG_LIB_SRC_DIR) -I$(NET_DIR) -I.. $<

$(CLIENT): $(CLIENT).o
	$(CC) -o $@ $< -lbluetooth

$(CLIENT).o: $(CLIENT).c
	$(CC) $(CFLAGS) $<

clean:
	rm -f *.o *~ $(TEST) $(CLIENT) *.log *.res

.PHONY:	clean
