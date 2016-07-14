SCALA_BIN="/Users/roncoleman125/programs/scala-2.11.6/bin"
PATH=$SCALA_BIN:$PATH

CLASSPATH=".:../antlr-4.5.3-complete.jar:../boldt-cgrammar.jar:../readability.jar"

DAY=`date +%Y%m%d`
OUTPUT_FILE=~/tmp/cc-results-${DAY}.txt

rm $OUTPUT_FILE

# Assumes we're in util folder
cd ../bin
INPUT_DIR="/Users/roncoleman125/Marist/Research/Pretty/meths"

#scala -cp $CLASSPATH pretty.Cyclomatic $INPUT_DIR/*.c | awk '!/file/ { print $0 }' | tee -a $OUTPUT_FILE
scala -cp $CLASSPATH pretty.Cyclomatic $INPUT_DIR/*.c > $OUTPUT_FILE

