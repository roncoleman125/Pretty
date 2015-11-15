# Installing gindent
# See https://www.topbug.net/blog/2013/04/14/install-and-use-gnu-command-line-tools-in-mac-os-x/
# See http://forums.macrumors.com/showthread.php?t=597546

SCALA_BIN="/Users/roncoleman125/programs/scala-2.11.6/bin"
PATH=$SCALA_BIN:$PATH

DAY=`date +%Y%m%d`
OUTPUT_FILE=~/tmp/recom-${DAY}.txt

rm $OUTPUT_FILE

INPUT_DIR="/Users/roncoleman125/Marist/Research/Pretty/meths-test"

# Assumes we're in util folder
cd ../bin
for FILE in $INPUT_DIR/*.c
do
  START=`date +%T`
  NAME=`basename $FILE`
  echo $NAME
  
  # Recomment the file
  RECOM=${FILE}.recom
  JAVA_OPTS='-Xmx6144M -d64' scala -cp "." pretty.Recomment $FILE ../data/comdb.zip > $RECOM

  LC=`wc -l $RECOM | awk '{print$1}'`
  RESULTS="$NAME $LC "
  
  # Analyze the file's D
  FD=`JAVA_OPTS='-Xmx6144M -d64' scala -cp ".:../fractop-0.3c.jar" pretty.Analyze $RECOM base ../util/c.config | awk '/FD =/{print $3}'`
  RESULTS="$RESULTS $FD"
  
  rm $RECOM
  
  END=`date +%T`
  echo $RESULTS | tee -a $OUTPUT_FILE
done