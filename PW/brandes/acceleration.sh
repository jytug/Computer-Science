# !/bin/bash

MAX_THREADS=$1
FILE=$2

OT=$(/usr/bin/time -f "%e" ./build/brandes 1 $FILE /dev/null 2>&1)
echo Brandes algorithm executed by 1 thread:
echo Time: $OT seconds
echo

for (( i=2; i<$MAX_THREADS+1; i++ ))
do
  echo Brandes algorithm executed by $i threads:
  MT=$(/usr/bin/time -f "%e" ./build/brandes $i $FILE /dev/null 2>&1)
  ACC=$(echo "$OT / $MT" | bc -l)
  echo Time: $MT seconds
  echo Acceleration: $ACC
  echo
done
