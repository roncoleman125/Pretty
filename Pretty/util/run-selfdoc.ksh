#!/bin/ksh
set -x

if [ $# -ne 2 ]
then
  echo usage: run-selfdoc.ksh input-dir output-dir
  exit 1
fi

INPUT_DIR=$1
OUTPUT_DIR=$2

SCALA_BIN="/Users/roncoleman/setups/scala-2.12.3/bin"
PATH=$SCALA_BIN:$PATH

DAY=`date +%Y%m%d`
OUTPUT_FILE=$OUTPUT_DIR/selfdoc-${DAY}.txt

rm $OUTPUT_FILE

#INPUT_DIR="/Users/roncoleman125/Marist/Research/Pretty/meths-0-comments"
#INPUT_DIR=/Users/roncoleman125/marist/research/pretty/linux/meths/kernel/testbed2

# Assumes we're in util folder
cd ../bin
for FILE in `find $INPUT_DIR -name "*.c"`
do
  START=`date +%T`
  NAME=`basename $FILE`
  #echo $NAME
  
  # Compute the baseline line count
  LC=`wc -l $FILE | awk '{print$1}'`
  RESULTS="$NAME $LC"
  
  # Get the baseline measurement
  for METHOD in BASE NON MNE
  do
    FD=`JAVA_OPTS='-Xmx6144M -d64' scala -cp ".:../fractop-0.3c.jar" pretty.Analyze $FILE $METHOD ../util/c-BAM.config | awk '/FD =/{print $3}'`
    RESULTS="$RESULTS $FD"
  done
  
  # Recomment the file
  RECOM=${FILE}.recom
  JAVA_OPTS='-Xmx6144M -d64' scala -cp "." pretty.Recomment $FILE ../data/coreutils/comments-db/comdb.zip > $RECOM

  # Compute line count for recommented file
  LC=`wc -l $RECOM | awk '{print$1}'`
  RESULTS="$RESULTS $LC"
  
  # Analyze the re-commented file's D
  FD2=`JAVA_OPTS='-Xmx6144M -d64' scala -cp ".:../fractop-0.3c.jar" pretty.Analyze $RECOM BASE ../util/c-BAM.config | awk '/FD =/{print $3}'`
  RESULTS="$RESULTS $FD2"
  
  rm $RECOM
  
  # Decomment the file
  STRIPPED=${FILE}.stripped
  #scala -cp "." pretty.Decomment $FILE base boxes > $STRIPPED
  ../util/decom < $FILE > $STRIPPED
  
  LC=`wc -l $STRIPPED | awk '{print$1}'`
  RESULTS="$RESULTS $LC "
  
  # Run the stripped WITHOUT indent styles
  FD3=`JAVA_OPTS='-Xmx6144M -d64' scala -cp ".:../fractop-0.3c.jar" pretty.Analyze $STRIPPED BASE ../util/c-BAM.config | awk '/FD =/{print $3}'`
  RESULTS="$RESULTS $FD3"
  
  END=`date +%T`
  echo $RESULTS | tee -a $OUTPUT_FILE
  exit 0
done