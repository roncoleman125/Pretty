SCALA_BIN="/Users/roncoleman125/programs/scala-2.11.6/bin"
PATH=$SCALA_BIN:$PATH

DAY=`date +%Y%m%d`
OUTPUT_FILE=~/tmp/results-${DAY}.txt

rm $OUTPUT_FILE

# Assumes we're in util folder
cd ../bin
for FILE in ../data/*.c
do
  START=`date +%T`
  NAME=`basename $FILE`
  echo $NAME
  LC=`wc -l $FILE|awk '{print$1}'`
  RESULTS="$NAME $LC "
  for METHOD in nc # base 0 20 40 nm
  do
    FD=`scala -cp ".:../fractop-0.3b.jar" pretty.Analyze $FILE $METHOD boxes | awk '/FD =/{print $3}'`
    RESULTS="$RESULTS $FD"
  done
  END=`date +%T`
  echo $RESULTS | tee -a $OUTPUT_FILE
done
