# Installing gindent
# See https://www.topbug.net/blog/2013/04/14/install-and-use-gnu-command-line-tools-in-mac-os-x/
# See http://forums.macrumors.com/showthread.php?t=597546

SCALA_BIN="/Users/roncoleman125/programs/scala-2.11.6/bin"
PATH=$SCALA_BIN:$PATH

DAY=`date +%Y%m%d`
OUTPUT_FILE=~/tmp/decom-${DAY}.txt
#INPUT_DIR="/Users/roncoleman125/Marist/Research/Pretty/meths"
INPUT_DIR=/Users/roncoleman125/marist/research/pretty/linux/meths/kernel/testbed2

rm $OUTPUT_FILE

# Assumes we're in util folder
cd ../bin
for FILE in $INPUT_DIR/*.c
do
  START=`date +%T`
  NAME=`basename $FILE`
#  echo $NAME
  
  STRIPPED=${FILE}.stripped
  #scala -cp "." pretty.Decomment $FILE base boxes > $STRIPPED
  ../util/decom < $FILE > $STRIPPED
  
  LC=`wc -l $STRIPPED | awk '{print$1}'`
  RESULTS="$NAME $LC "
  
  # Run the stripped WITHOUT indent styles
  FD=`JAVA_OPTS='-Xmx6144M -d64' scala -cp ".:../fractop-0.3c.jar" pretty.Analyze $STRIPPED BASE ../util/c.config | awk '/FD =/{print $3}'`
  RESULTS="$RESULTS $FD"

#  # Run the stripped with indent styles
#  for STYLE in "$GNU" "$KR" "$BERKELEY" "$LINUX"
#  do
#    INDENTED=${STRIPPED}.indent
#    
#    /usr/local/bin/gindent $STYLE $STRIPPED -o $INDENTED
#    
#    FD=`JAVA_OPTS='-Xmx6144M -d64' scala -cp ".:../fractop-0.3c.jar" pretty.Analyze $INDENTED base boxes | awk '/FD =/{print $3}'`
#    
#    RESULTS="$RESULTS $FD"
#    
#    rm $INDENTED
#  done
  
  rm $STRIPPED
  
  END=`date +%T`
  echo $RESULTS | tee -a $OUTPUT_FILE
done