# Measured 2011 MacBook Pro: 5h 23m to process 1,043 files.

# Installing gindent on Mac
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

#INPUT_DIR=../coreutils/methods-only/data/
INPUT_DIR=../data/coreutils/methods-only
#INPUT_DIR=/Users/roncoleman125/marist/research/pretty/linux/linux-master/arch/alpha/kernel
#INPUT_DIR=/Users/roncoleman125/marist/research/Pretty/linux/meths

# Assumes we're in util folder
cd ../bin
for FILE in $INPUT_DIR/*.c
do
  START=`date +%T`
  NAME=`basename $FILE`
  echo $NAME
  LC=`wc -l $FILE|awk '{print$1}'`
  RESULTS="$NAME $LC "
  for STYLE in "$GNU" "$KR" "$BERKELEY" "$LINUX"
  do
    BASENAME=`basename ${FILE}`
    INDENTED=~/tmp/${BASENAME}.indent
    /usr/local/bin/gindent $STYLE $FILE -o $INDENTED
    FD=`scala -cp ".:../fractop-0.3c.jar" pretty.Analyze $INDENTED base ../util/c.config | awk '/FD =/{print $3}'`
    RESULTS="$RESULTS $FD"
    rm $INDENTED
  done
  END=`date +%T`
  echo $RESULTS | tee -a $OUTPUT_FILE
done