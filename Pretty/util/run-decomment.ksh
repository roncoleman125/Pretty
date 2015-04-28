# Installing gindent
# See https://www.topbug.net/blog/2013/04/14/install-and-use-gnu-command-line-tools-in-mac-os-x/
# See http://forums.macrumors.com/showthread.php?t=597546

SCALA_BIN="/Users/roncoleman125/programs/scala-2.11.6/bin"
PATH=$SCALA_BIN:$PATH

DAY=`date +%Y%m%d`
OUTPUT_FILE=~/tmp/indents-${DAY}.txt

rm $OUTPUT_FILE

# Styles
DEFAULT=
GNU='-nbad -bap -bbo -nbc -bl -bli2 -bls -cp1 -ncdb -nce -cs -di2 -ndj -nfc1 -nfca -hnl -i2 -ip5 -lp -pcs -nprs -psl -saf -sai -saw -nsc -nsob'
KR='-nbad -bap -bbo -nbc -br -brs -c33 -cd33 -ncdb -ce -ci4 -cli0 -cp33 -cs -d0 -di1 -nfc1 -nfca -hnl -i4 -ip0 -l75 -lp -npcs -nprs -npsl -saf -sai -saw -nsc -nsob -nss'
BERKELEY='-bbo -nbad -nbap -bc -br -brs -c33 -cd33 -cdb -ce -ci4 -cli0 -cp33 -di16 -fc1 -fca -hnl -i4 -ip4 -l75 -lp -npcs -nprs -psl -saf -sai -saw -sc -nsob -nss -ts8'
LINUX='-nbad -bap -bbo -nbc -br -brs -c33 -cd33 -hnl -ncdb -ce -ci4 -cli0 -d0 -di1 -nfc1 -i8 -ip0 -l80 -lp -npcs -nprs -npsl -sai -saf -saw -ncs -nsc -sob -nfca -cp33 -ss -ts8 -il1'

# Assumes we're in util folder
cd ../bin
for FILE in ../data/ls.c #../data/*.c
do
  START=`date +%T`
  NAME=`basename $FILE`
  echo $NAME
  
  STRIPPED=${FILE}.stripped
  #scala -cp "." pretty.Decomment $FILE base boxes > $STRIPPED
  ../util/decom < $FILE > $STRIPPED
  
  LC=`wc -l $STRIPPED | awk '{print$1}'`
  RESULTS="$NAME $LC "
  
  FD=`JAVA_OPTS='-Xmx6144M -d64' scala -cp ".:../fractop-0.3b.jar" pretty.Analyze $STRIPPED base boxes | awk '/FD =/{print $3}'`
  RESULTS="$RESULTS $FD"

  for STYLE in "$GNU" "$KR" "$BERKELEY" "$LINUX"
  do
    INDENTED=${STRIPPED}.indent
    
    /usr/local/bin/gindent $STYLE $STRIPPED -o $INDENTED
    
    FD=`JAVA_OPTS='-Xmx6144M -d64' scala -cp ".:../fractop-0.3b.jar" pretty.Analyze $INDENTED base boxes | awk '/FD =/{print $3}'`
    
    RESULTS="$RESULTS $FD"
    
    rm $INDENTED
  done
  
  rm $STRIPPED
  
  END=`date +%T`
  echo $RESULTS | tee -a $OUTPUT_FILE
done