#!/bin/bash

#FILE=$1
#[ -f "$FILE" ] || exit

FILE=input/file.txt
FILE_CHUNK=chunks/input.txt

#awk '/MemTotal/ {print $2}' /proc/meminfo
#cat /proc/meminfo | grep MemTotal

#awk '/MemFree/ {print $2}' /proc/meminfo
#awk '/MemAvailable/ {print $2}' /proc/meminfo
# free  // kilobytes
# free -m   //megabyte

g++ generate_file.cpp -o gfile
./gfile

N_LINES=$(wc -l <${FILE});

let N_LINES=N_LINES+1

echo "Number of lines of file.txt "$N_LINES

BYTES=0;
counter=0; 
N_LINES_CHUNK=200;

echo "Number of lines of chunk "$N_LINES_CHUNK

> $FILE_CHUNK

for LINE in $(seq 1 $N_LINES); do
	let BYTES+=$(sed -n "$LINE"p "$FILE" | wc -c)
	
    if [ $(($LINE%$N_LINES_CHUNK)) -eq 0 ] || [ "$LINE" -eq $N_LINES ]
    then
		echo $BYTES >> $FILE_CHUNK
		BYTES=0
		((counter++))
	fi	
done



#split -l $N_LINES_CHUNK -e $FILE chunks/chunk_

#counter=1; 

#for file in chunks/*; do 
#	[[ -f $file ]] && mv -i "$file" "chunks/input"_$((counter))."txt" && ((counter++)); 
#done

## NUMBER_FILES="$(ls chunks | wc -l)";

#let counter=counter-1;

#nproc
N_THREADS=$(nproc --all);  #4

g++ parallel.cpp -o wordcount -pthread
./wordcount $counter $N_THREADS