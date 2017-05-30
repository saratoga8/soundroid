#!/bin/bash

PORT=$1
if [ "$#" -ne 2 ]; then
    echo "ERROR: the port number and the invalid port number aren't given"
    exit
fi

DAEMON=vol_daemon
if [ `pgrep $DAEMON` ]; then
    echo "ERROR: stop the daemon before running the check!"
    exit
fi

if [[ `sockstat | grep $PORT` != "" ]]; then
    echo "ERROR: The port number $PORT is used"
    exit
fi

RESULTS_FILE=memcheck.res
echo "Test valid port $PORT ---------------------------" >> $RESULTS_FILE

echo "Run valgrind............."
valgrind -q --leak-check=full --log-file=$RESULTS_FILE ../test $PORT 1> test.out &
echo "Wait for port openning..."
while [[ `sockstat | grep $PORT` == "" ]] ; do sleep 1 ; done
echo "1st client connection...."
./client.sh "one" "two" "three" 1> client.out1
sleep 1
echo "2nd client connection...."
./client.sh "one" "two" "quit" 1> client.out2

sleep 3

TMP_FILE=tmp.res
INVALID_PORT=$2
echo "Test invalid port $INVALID_PORT -----------------" >> $RESULTS_FILE

echo "Check invalid port $INVALID_PORT"
valgrind -q --leak-check=full --log-file=$TMP_FILE ../test $INVALID_PORT test-port 1> test.out

cat $TMP_FILE >> $RESULTS_FILE
rm -f $TMP_FILE
