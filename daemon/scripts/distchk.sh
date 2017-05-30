#!/bin/sh

if [ $# -ne 3 ]; then
    echo -n "Invalid parameters. Should be: distchk.sh par1 par2 par3\n\tpar1 - path of the project archive file\n\tpar2 - directory name in the archive file\n\tpar3 - path of the installed file\n"
    exit
fi

PREV_DIR=$PWD

ARC_FILE=`echo $1 | sed 's/.*\///'`
DIR=$2
TMP=/tmp
INSTALLED_FILE=$3

cp $1 $TMP
cd $TMP
tar xf $ARC_FILE
cd $DIR
make -s all
sudo make install

if [ -e $INSTALLED_FILE ]; then
    sudo make uninstall    
else 
    echo "ERROR: Installation failed. The file $INSTALLED_FILE hasn't copied"
fi

[ -e $INSTALLED_FILE ] && echo "ERROR: Uninstallation failed. The file $INSTALLED_FILE hasn't deleted"

rm -rf $TMP/$ARC_FILE
rm -rf $TMP/$DIR