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

OUTPUT_DIR="/Users/roncoleman125/Marist/Research/Pretty/gnu"
INPUT_DIR="/Users/roncoleman125/Marist/Research/Pretty/meths"

# Assumes we're in util folder
cd ../bin
for FILE in $INPUT_DIR/*.c
do
    BASE=`basename $FILE`
    echo $BASE
    /usr/local/bin/gindent $GNU $FILE -o $OUTPUT_DIR/$BASE
done
