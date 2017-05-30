#!/bin/sh

cd ..
make -s clean
sudo make -s uninstall
make -s -j8 all
make -s -j8 gui
sudo make -s install

RESULTS_FILE=results.txt

echo "Run tests of release..................."
make tests > $RESULTS_FILE
RESULT=`grep -i "\.\.\.fail" $RESULTS_FILE`
if [ -n "$RESULT" ]
then
    echo "There are FAILS in the tests of release. See the $RESULTS_FILE"
    cd -
    exit;
fi

rm -rf $RESULTS_FILE

echo "Run check of distribution.............."
RESULT=`make distchk | grep -i "error"`
if [ -n "$RESULT" ]
then
    echo "There are FAILS in distribution check"
    cd -
    exit;
fi

make -s dist_bin
make -s dist_src

sudo make -s install

cd -
