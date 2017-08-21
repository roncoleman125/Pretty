#!/bin/ksh
#set -x

if [ $# -ne 3 ]
then
  echo usage: strip-bulk.ksh include-file in-dir out-dir
  exit 1
fi

INCL=$1
IDIR=$2
ODIR=$3

for F in `cat $INCL` 
do
  awk -f strip-cpright.awk $F > tmp.c

  DIR=`dirname $F`

  TARGET_DIR="$ODIR/$DIR"

  mkdir -p $TARGET_DIR 2> /dev/null

  NAME=`basename $F`

  TARGET_FILE="$TARGET_DIR/$NAME"

  mv tmp.c $TARGET_FILE
  echo processed $F
done