#!/bin/bash

FILE=input/file.txt

#awk '/MemTotal/ {print $2}' /proc/meminfo
#cat /proc/meminfo | grep MemTotal
#awk '/MemFree/ {print $2}' /proc/meminfo
#awk '/MemAvailable/ {print $2}' /proc/meminfo
# free  // kilobytes
# free -m   //megabyte


N_LINES_CHUNK=400

g++ generate_file.cpp -o gfile
./gfile

N_LINES=$(wc -l <${FILE});

let N_LINES=N_LINES+1

echo "Number of lines of file.txt "$N_LINES

split -l $N_LINES_CHUNK -e $FILE chunks/chunk_

echo "Number of lines of chunk files "$N_LINES_CHUNK

counter=1; 

for file in chunks/*; do 
	[[ -f $file ]] && mv -i "$file" "chunks/input"_$((counter))."txt" && ((counter++)); 
done

#NUMBER_FILES="$(ls chunks | wc -l)";

let counter=counter-1;

#nproc
N_THREADS=$(nproc --all);  #4

g++ parallel.cpp -o wordcount -pthread
./wordcount $counter $N_THREADS