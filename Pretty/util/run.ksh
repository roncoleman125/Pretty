SCALA_BIN="/Users/roncoleman125/programs/scala-2.11.6/bin"
PATH=$SCALA_BIN:$PATH

export CLASSPATH=".:../fractop-0.3b.jar:../commons-lang3-3.4.jar:../javahelper-0-0.jar"
export JAVA_OPTS='-Xmx6144M -d64'
export TMPDIR=~/tmp
export TABSTOP=8

DAY=`date +%Y%m%d`
OUTPUT_FILE=~/tmp/results-${DAY}.txt

rm $OUTPUT_FILE

INPUT_DIR=../data/coreutils/methods-only
#INPUT_DIR=/Users/roncoleman125/marist/research/pretty/linux/linux-master/arch/alpha/kernel
#INPUT_DIR=/Users/roncoleman125/marist/research/Pretty/linux/meths
#INPUT_DIR=/Users/roncoleman125/marist/research/pretty/linux/linux-master/kernel
# Assumes we're in util folder
cd ../bin
for FILE in $INPUT_DIR/*.c
do
  START=`date +%T`
  NAME=`basename $FILE`
  echo $NAME
  LC=`wc -l $FILE|awk '{print$1}'`
  RESULTS="$NAME $LC "
  for METHOD in BASE 0 2 5 NON MNE
  do
    FD=`scala pretty.Analyze $FILE $METHOD ../util/c.config | awk '/FD =/{ print $3 }'`
    RESULTS="$RESULTS $FD"
  done
  END=`date +%T`
  echo $RESULTS | tee -a $OUTPUT_FILE
done
