IP_OPS_SRC_FILES=IpOps.c IpOps.h
SOCKETS_LIB_SRC_FILES=SocketsLib.c SocketsLib.h 
CLOSING_CLIENT_SRC_FILES=ClosingClient.c ClosingClient.h

OBJS=SocketsLib.o ClosingClient.o IpOps.o synchronise.o

LOG_LIB_SRC_DIR=../../Log
LOG_LIB_SRC_FILES=Log.h

CC=gcc
CFLAGS=-Wall -c -O2

LIBS_DIR=../../lib
LIBS=-lpthread -lSockets -lLog

#LIB=libSockets.a
LIB=libSockets.so

TESTS_DIR=tests

TEST=test

PORT=5000
FORBIDDEN_PORT=80

vpath %.h . $(LOG_LIB_SRC_DIR) ..
vpath %.c . $(LOG_LIB_SRC_DIR) $(TESTS_DIR) ..

mem_leak_chk: $(TEST)
	cd $(TESTS_DIR) && ./test_mem_leak.sh $(PORT) $(FORBIDDEN_PORT) && cd ..

$(TEST):	$(TEST).o install
	$(CC) -L$(LIBS_DIR) -o $@ $< $(LIBS)
	cd $(TESTS_DIR) && bash ./chkConnections.sh 5000 && bash ./chkInvalidPort.sh 80

$(TEST).o: $(TEST).c $(LOG_LIB_SRC_FILES)
	$(CC) $(CFLAGS) -I$(LOG_LIB_SRC_DIR) -I.. $<

$(LIB):	$(OBJS)
#	ar -rvs $@ $^
	$(CC) -shared -o $@ $^

IpOps.o:	$(IP_OPS_SRC_FILES) $(LOG_LIB_SRC_DIR)
	$(CC) $(CFLAGS) -I$(LOG_LIB_SRC_DIR) -fPIC $<

SocketsLib.o:	$(SOCKETS_LIB_SRC_FILES) $(LOG_LIB_SRC_FILES) addr.h
	$(CC) $(CFLAGS) -I.. -I$(LOG_LIB_SRC_DIR) -fPIC $<

ClosingClient.o:	$(CLOSING_CLIENT_SRC_FILES) $(LOG_LIB_SRC_FILES)
	$(CC) $(CFLAGS) -I$(LOG_LIB_SRC_DIR) -fPIC $<

synchronise.o:	synchronise.c synchronise.h
	$(CC) $(CFLAGS) -fPIC $<

install:	$(LIB)
	mkdir -p $(LIBS_DIR)
	cp $(LIB) $(LIBS_DIR) 

clean: 
	rm -rf *~ *.o *.a *.so ./$(TEST) log.txt *.log *.out $(TESTS_DIR)/*.out* $(TESTS_DIR)/*.res

.PHONY:	clean install test
