# This script runs ALL 11 treatments plus the baseline recursively in a directory.
# It outputs 21 fields to be processed be analyzed by beauty-1.awk.
# R. Coleman
# version 0.2, 26 Aug 2017
#!/bin/ksh
#set -x

if [ $# -ne 2 ]
then
  echo usage: run-all.ksh input-dir output-dir
  exit 1
fi

INPUT_DIR=$1
OUTPUT_DIR=$2

#SCALA_BIN="/Users/roncoleman/setups/scala-2.12.3/bin"
SCALA_BIN=~/Programs/scala-2.12.3/bin
PATH=$SCALA_BIN:$PATH

# To a classpath, replace the jar file with a path, typically the workspace path.
PRETTYJAR=~/Marist/research/Pretty2/pretty/pretty.jar 

export JAVA_OPTS='-Xmx6144M -d64'

DAY=`date +%Y%m%d`

OUTPUT_FILE=$OUTPUT_DIR/sci-results-${DAY}.txt_

# Styles
GNU='-nbad -bap -bbo -nbc -bl -bli2 -bls -cp1 -ncdb -nce -cs -di2 -ndj -nfc1 -nfca -hnl -i2 -ip5 -lp -pcs -nprs -psl -saf -sai -saw -nsc -nsob'
KR='-nbad -bap -bbo -nbc -br -brs -c33 -cd33 -ncdb -ce -ci4 -cli0 -cp33 -cs -d0 -di1 -nfc1 -nfca -hnl -i4 -ip0 -l75 -lp -npcs -nprs -npsl -saf -sai -saw -nsc -nsob -nss'
BERKELEY='-bbo -nbad -nbap -bc -br -brs -c33 -cd33 -cdb -ce -ci4 -cli0 -cp33 -di16 -fc1 -fca -hnl -i4 -ip4 -l75 -lp -npcs -nprs -psl -saf -sai -saw -sc -nsob -nss -ts8'
LINUX='-nbad -bap -bbo -nbc -br -brs -c33 -cd33 -hnl -ncdb -ce -ci4 -cli0 -d0 -di1 -nfc1 -i8 -ip0 -l80 -lp -npcs -nprs -npsl -sai -saf -saw -ncs -nsc -sob -nfca -cp33 -ss -ts8 -il1'

rm $OUTPUT_FILE

#INPUT_DIR="/Users/roncoleman125/Marist/Research/Pretty/meths-0-comments"
#INPUT_DIR=/Users/roncoleman125/marist/research/pretty/linux/meths/kernel/testbed2

# Assumes we're in util folder
cd ../bin
for FILE in `find $INPUT_DIR -type f -name "*.c"`
do
  START=`date +%T`
  NAME=`basename $FILE`
  DIRN=`dirname $FILE`
  #echo $NAME
  
  # Compute the baseline line count
  LC=`wc -l $FILE | awk '{print$1}'`
  RESULTS="$DIRN $NAME $LC"
  
  # Get the baseline measurement
  for METHOD in BASE 0 2 5 NON MNE
  do
    FD=`scala -cp "$PRETTYJAR:fractop-0.3c.jar" pretty.Analyze "$FILE" $METHOD ../util/c-BAM.config | awk '/FD =/{print $3}'`
    RESULTS="$RESULTS $FD"
  done
  
  # Recomment the file
  RECOM=${FILE}.recom
  scala -cp "$PRETTYJAR" pretty.Recomment "$FILE" ../data/coreutils/comments-db/comdb.zip > $RECOM

  # Compute line count for recommented file
  LC=`wc -l $RECOM | awk '{print$1}'`
  RESULTS="$RESULTS $LC"
  
  # Analyze the re-commented file's D
  FD2=`scala -cp "$PRETTYJAR:fractop-0.3c.jar" pretty.Analyze $RECOM BASE ../util/c-BAM.config | awk '/FD =/{print $3}'`
  RESULTS="$RESULTS $FD2"
  
  rm $RECOM
  
  # Decomment the file
  STRIPPED="${FILE}.stripped"
  #scala -cp "." pretty.Decomment $FILE base boxes > $STRIPPED
  ../util/decom < "$FILE" > $STRIPPED
  
  LC=`wc -l $STRIPPED | awk '{print$1}'`
  RESULTS="$RESULTS $LC "
  
  # Run the stripped WITHOUT indent styles
  FD3=`scala -cp "$PRETTYJAR:fractop-0.3c.jar" pretty.Analyze $STRIPPED BASE ../util/c-BAM.config | awk '/FD =/{print $3}'`
  RESULTS="$RESULTS $FD3"

  # Run the styles
  for STYLE in "$GNU" "$KR" "$BERKELEY" "$LINUX"
  do
    INDENTED=${FILE}.indent
    /usr/local/bin/gindent $STYLE "$FILE" -o $INDENTED
    
    LC=`wc -l $INDENTED|awk '{print$1}'`
    
    FD=`scala -cp "$PRETTYJAR:fractop-0.3b.jar" pretty.Analyze $INDENTED BASE ../util/c-BAM.config | awk '/FD =/{print $3}'`
    
    RESULTS="$RESULTS $LC $FD "
    
    rm $INDENTED
  done

  END=`date +%T`
  echo $RESULTS | tee -a $OUTPUT_FILE
done
