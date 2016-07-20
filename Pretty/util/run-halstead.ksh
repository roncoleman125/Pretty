SCALA_BIN="/Users/roncoleman125/programs/scala-2.11.6/bin"
PATH=$SCALA_BIN:$PATH

CLASSPATH=".:../antlr-4.5.3-complete.jar:../boldt-cgrammar.jar:../readability.jar"

DAY=`date +%Y%m%d`
OUTPUT_FILE=~/tmp/hal-results-${DAY}.txt

rm $OUTPUT_FILE

# Assumes we're in util folder
cd ../bin
#INPUT_DIR="/Users/roncoleman125/Marist/Research/Pretty/meths"
INPUT_DIR=/Users/roncoleman125/marist/research/Pretty/linux/meths
for FILE in $INPUT_DIR/*.c
do
#  NAME=`basename $FILE`
#  echo $NAME
  LC=`wc -l $FILE|awk '{print$1}'`
  RESULTS="$NAME $LC "
  HAL=`scala -cp $CLASSPATH pretty.readability.Halstead $FILE |
    	awk '!/file/{print $0}'`
  echo $HAL | tee -a $OUTPUT_FILE
done
