rm results.txt
for FILE in ../data/*.c
do
  START=`date +%T`
  NAME=`basename $FILE`
  echo $NAME
  LC=`wc -l $FILE|awk '{print$1}'`
  RESULTS="$NAME $LC "
  for METHOD in base 0 20 40 nm
  do
    FD=`scala -cp ".:../fractop-0.3b.jar" pretty.Analyze $FILE $METHOD boxes | awk '/FD =/{print $3}'`
    RESULTS="$RESULTS $FD"
  done
  END=`date +%T`
  echo $RESULTS | tee -a results.txt
done
