#!/bin/bash

g++ generate_file.cpp -o gfile
./gfile

FILE=input/file.txt

N_LINES="$(wc -l $FILE | awk '{ print $1 }')";

let N_LINES=N_LINES+1

echo "Number of lines of file.txt "$N_LINES

split -l 100 input/file.txt chunks/chunk_.txt

counter=1; 

for file in chunks/*; do 
	[[ -f $file ]] && mv -i "$file" "chunks/input"_$((counter))."txt" && ((counter++)); 
done

NUMBER_FILES="$(ls chunks | wc -l)";

N_THREADS=4

g++ parallel.cpp -o wordcount -lpthread
./wordcount $NUMBER_FILES $N_THREADS