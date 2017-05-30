#!/bin/bash

# Checking connection to the running test daemon by sending commands
# There are 2 times of connections

if [ $# -ne 1 ]; then
    echo "Should be: $0 port_num"
    exit
fi

echo "Check client connections---------------------------"

RED='\e[0;31m'
GREEN='\e[0;32m'
NC='\e[0m'

STR_WITH_IP=`ifconfig | grep 'inet addr' | grep -v 127.0.0.1 | head -n 1`
IP=`echo $STR_WITH_IP | sed 's/inet addr:'// | awk '{print $1}'`

if [ -e test.correct ]; then
    `sed -i "s/Local IP:.*/Local IP: $IP/" test.correct`
fi

VALID_PORT=$1

if [[ `sockstat | grep $VALID_PORT` ]]; then
    echo -e "\tThe port $VALID_PORT is being used!"
    exit 1
fi

echo "Running programs..."
../test $VALID_PORT 1> test.out &
./client.sh "one" "two" "three" 1> client.out
sleep 1
./client.sh "one" "two" "quit" 1> client.out2

diff_res=`diff client.out client.correct`
if [ -n "$diff_res" ]; then
    echo -e "\tAnalysing 1st client connection result...${RED}FAILED${NC}"
else
    echo -e "\tAnalysing 1st client connection result...${GREEN}PASSED${NC}"
fi

diff_res=`diff client.out2 client.correct2`
if [ -n "$diff_res" ]; then
    echo -e "\tAnalysing 2nd client connection result...${RED}FAILED${NC}"
else
    echo -e "\tAnalysing 2nd client connection result...${GREEN}PASSED${NC}"
fi

sleep 1

diff_res=`diff test.out test.correct`
if [ -n "$diff_res" ]; then
    echo -e "\tAnalysing daemon's' result...............${RED}FAILED${NC}"
else
    echo -e "\tAnalysing daemon's result................${GEEN}PASSED${NC}"
fi


