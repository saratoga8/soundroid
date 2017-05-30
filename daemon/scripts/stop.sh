#!/bin/sh
FILE=pid.txt
PID=0
SIGNAL=15    # SIGTERM
PROG_NAME=vol_daemon

THIS_PROG_NAME=`echo $0 | sed -s 's/\.sh//' | sed -s 's/\.\///'`

SUCCESS=0
FAILED=-1
RESULT=$SUCCESS

PID=`pgrep $PROG_NAME | awk '{print $1}'`
if [ -z "$PID" ]; then
    echo "The distance sound control daemon isn't running"
    [ -e $FILE ] && rm -f $FILE
    exit $SUCCESS;
fi 

if [ -e $FILE ]; then 
    PID=`cat $FILE`
    echo "Killing the daemon by sending signal..."
    kill -s $SIGNAL $PID
    sleep 1
fi

for i in `seq 1 60`; do
    PID=`pgrep $PROG_NAME`;
    [ -z "$PID" ] && break || sleep 1; 
done

PID=`pgrep $PROG_NAME | awk '{print $1}'`
if [ -n "$PID" ]; then
    echo "WARNING: Killing the daemon by command 'kill -9'"
    kill -9 $PID
fi

PID=`pgrep $PROG_NAME | awk '{print $1}'`
if [ -n "$PID" ]; then
    echo "ERROR: Can't close the daemon $PROG_NAME!"
    RESULT=$FAILED
fi

[ -e $FILE ] && rm -f $FILE
exit $RESULT
