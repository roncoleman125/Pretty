SCALA_BIN="/Users/roncoleman125/programs/scala-2.11.6/bin"
PATH=$SCALA_BIN:$PATH

DAY=`date +%Y%m%d`
OUTPUT_FILE=~/tmp/results-${DAY}.txt

rm $OUTPUT_FILE

# Assumes we're in util folder
cd ../bin
INPUT_DIR="/Users/roncoleman125/Marist/Research/Pretty/meths"
for FILE in $INPUT_DIR/*.c
do
  START=`date +%T`
  NAME=`basename $FILE`
  echo $NAME
  LC=`wc -l $FILE|awk '{print$1}'`
  RESULTS="$NAME $LC "
  for METHOD in em base 0 20 40 nm beau
  do
    FDM=`scala -cp ".:../fractop-0.3b.jar:../antlr-4.5.1-complete.jar:../cg4.jar" pretty.Analyze $FILE $METHOD ../util/c.config |
    	awk '/FD =/||/M =/{print $3}'`
    RESULTS="$RESULTS $FDM"
  done
  END=`date +%T`
  echo $RESULTS | tee -a $OUTPUT_FILE
done
