ALSA_INCLUDE_DIR=/usr/include/alsa

LOG_LIB_SRC_DIR=../Log
LOG_LIB_SRC_FILES=Log.h

LIB=libSound.a

LIBS_DIR=../lib
LIBS=-lSound -lasound -lLog -lcunit

CC=gcc
CFLAGS=-Wall -c -O2

SRC_FILES=SoundLib.c SoundLib.h

vpath %.h . $(LOG_LIB_SRC_DIR)
vpath %.c . $(LOG_LIB_SRC_DIR)

MEMCHECK_FILE=memcheck.res
TEST=test

mem_leak_chk: $(TEST)
	valgrind -q --log-file=$(MEMCHECK_FILE) --leak-check=full ./$(TEST) > /dev/null

$(TEST):	install $(TEST).o
	$(CC) -L$(LIBS_DIR) -o $@ $(TEST).o $(LIBS)
	./$(TEST)

$(LIB):	SoundLib.o
	ar -rcs $@ $<

$(TEST).o:	$(TEST).c $(SRC_FILES) $(LOG_LIB_SRC_FILES)
	$(CC) $(CFLAGS) -I$(LOG_LIB_SRC_DIR) $< 

SoundLib.o:	$(SRC_FILES) $(LOG_LIB_SRC_FILES)
	$(CC) $(CFLAGS) -static -I$(LOG_LIB_SRC_DIR) -I$(ALSA_INCLUDE_DIR) $<

install:	$(LIB)
	cp $(LIB) $(LIBS_DIR)

clean: 
	rm -f *~ *.o *.a $(TEST) log.txt $(MEMCHECK_FILE)

default: $(TEST)

.PHONY:	clean install mem_leak_chk
