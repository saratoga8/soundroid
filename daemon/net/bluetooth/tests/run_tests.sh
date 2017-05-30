#!/bin/sh

USER="saratoga"
HOST="192.168.0.139"
DIR="/home/saratoga/progs"
FILES="client.c Makefile"
TESTS_DIR="$DIR/bluetooth_test"
COMPILE_COMMANDS="cd $TESTS_DIR && ls"
CLIENT_LOG=client.log
CLIENT_BT_MAC="68:5D:43:E9:DB:CA"

echo "Checking Blue tooth connection to client..................................."
CLIENT_BT_UP=`hcitool scan | grep $CLIENT_BT_MAC`
[ -z "$CLIENT_BT_UP" ] && echo "ERROR: Can't connect to the client by bluetooth" && exit

CLIENT_HOST_UP=`ping -c 2 $HOST | grep "2 received"`
[ -z "$CLIENT_HOST_UP" ] && echo "ERROR: Client host is unreachable!" && exit;
SSH_CLIENT_RUN=`nmap $HOST -PN -p ssh | grep open`
[ -z "$SSH_CLIENT_RUN" ] && echo "ERROR: SSH on the client host is DOWN!" && exit;

echo "Checking Blue Tooth connection from client................................."
LOCAL_BT_MAC=`hcitool dev | grep hci0 | awk '{print $2}'`
LOCAL_HOST_BT_UP=`ssh $USER@$HOST hcitool scan | grep "$LOCAL_BT_MAC"`
[ -z "$LOCAL_HOST_BT_UP" ] && echo "ERROR: Can't connect from client by bluetooth" && exit;

echo "Build test server.........................................................."
make -s test

echo "Create dir $TESTS_DIR at $USER@$HOST..."
ssh -q $USER@$HOST mkdir -p $TESTS_DIR

echo "Copy files $FILES to the $TESTS_DIR...."
scp -q $FILES $USER@$HOST:$TESTS_DIR
sleep 2
echo "Build client..............................................................."
ssh -q $USER@$HOST -t "cd $TESTS_DIR && gcc -o client client.c -lbluetooth; exit; bash"
sleep 2
ssh -q $USER@$HOST -t "[ -e $TESTS_DIR/client ] && echo 'client has built'; exit; bash"

echo "Run server................................................................."
SERVER_LOG=test.log
pkill -x test
rm -f log.txt
if [ $# -gt 0 ]; then
    valgrind -q --log-file=memcheck.res --leak-check=full ./test > $SERVER_LOG &
else
    ./test > $SERVER_LOG &
fi

sleep 5

SLEEP=5
if [ $# -gt 0 ]; then
    SLEEP=10
fi

echo "Run client................................................................."
ssh -q $USER@$HOST -t "cd $TESTS_DIR && ./client $LOCAL_BT_MAC $SLEEP > $CLIENT_LOG; exit; bash"
scp -q $USER@$HOST:$TESTS_DIR/$CLIENT_LOG .
ssh -q $USER@$HOST rm -rf $TESTS_DIR

echo -n "Compare client results.................................................."
CLIENT_VALID_LOG=client.valid
DIFF_RES=`diff $CLIENT_LOG $CLIENT_VALID_LOG`
[ -n "$DIFF_RES" ] && echo "FAILED" || echo "OK"

echo -n "Compare server results.................................................."
SERVER_VALID_LOG=test.valid
DIFF_RES=`diff $SERVER_VALID_LOG $SERVER_LOG`
[ -n "$DIFF_RES" ] && echo "FAILED" || echo "OK"
