# This scripts runs the no-indent sensitivity test.
# Change the fraction in noin.awk.
export SCALA_BIN="/Users/roncoleman125/programs/scala-2.11.6/bin"
export PATH=$SCALA_BIN:$PATH

FILE=$1

echo $FILE

OUTPUT_FILE=`pwd`/${FILE}.output
rm $OUTPUT_FILE

cd ../bin
for SAMPLE in 1 2 3 4 5 6 7 8 9 10
do
  echo -n $SAMPLE " "
  awk -f ../util/noin.awk ../data/$FILE > $FILE.noin

  FD=`JAVA_OPTS='-Xmx6144M -d64' scala -cp ".:../fractop-0.3b.jar" pretty.Analyze ${FILE}.noin base boxes | awk '/FD =/{print $3}'`

  echo $FD | tee -a $OUTPUT_FILE
done
rm $FILE.noin
