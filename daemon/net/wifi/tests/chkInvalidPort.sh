#!/bin/bash

# Checking connection to an invalid port of the running test daemon

if [ $# -ne 1 ]; then
    echo "Should be: $0 invalid_port_num"
    exit
fi


RED='\e[0;31m'
GREEN='\e[0;32m'
NC='\e[0m'

rm -f log.txt *.out


INVALID_PORT=$1
echo "Check invalid port $INVALID_PORT -----------------------------"
../test $INVALID_PORT test-port
RES=`cat log.txt | grep "ERROR: bindSocket(): Bad value for"`
if [ -n "$RES" ]; then
    echo -e "\tCheck invalid port $INVALID_PORT....................${GREEN}PASSED${NC}"
else
    echo -e "\tCheck invalid port $INVALID_PORT....................${RED}FAILED${NC}"
fi
