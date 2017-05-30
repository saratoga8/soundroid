CC=gcc
CFLAGS=-Wall -c -O2

LIBS_DIR=../lib

LIB=libUtils.a
OBJS=file_utils.o str_utils.o proc_utils.o
LIBS=-lUtils -lcunit -lLog

TESTS_DIR=tests

LOG_LIB_SRC_DIR=../Log

vpath %.h . $(LOG_LIB_SRC_DIR)
vpath %.c . $(TESTS_DIR)

install:	$(LIB)
	mv $(LIB) $(LIBS_DIR) 

MEMCHECK_FILE=memcheck.res
TEST=test param1 param2

mem_leak_chk: $(TEST)
	valgrind -q --log-file=$(MEMCHECK_FILE) --leak-check=full ./$(TEST) > /dev/null

$(TEST):	test_utils.o runable install
	$(CC) -L$(LIBS_DIR) -o $@ $< $(LIBS)
	./$(TEST)

runable:	runable.o
	$(CC) -o $@ $<

$(LIB):	$(OBJS)
	ar -rcs $@ $^

test_utils.o:	test_utils.c file_utils.h str_utils.h
	$(CC) $(CFLAGS) $<

file_utils.o:	file_utils.c file_utils.h
	$(CC) $(CFLAGS) $<

str_utils.o:	str_utils.c str_utils.h
	$(CC) $(CFLAGS) $<

proc_utils.o: proc_utils.c proc_utils.h Log.h
	$(CC) $(CFLAGS)  -I$(LOG_LIB_SRC_DIR) $<

runable.o:	runable.c
	$(CC) $(CFLAGS) $<

clean:
	rm -rf *.o *~ $(MEMCHECK_FILE)
	find . -type f -perm +111 | xargs rm -f 

.PHONY:	clean install $(TEST) mem_leak_chk

