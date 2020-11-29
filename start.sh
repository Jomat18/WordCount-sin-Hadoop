#!/bin/bash

FILE=input/file.txt

N_LINES="$(wc -l $FILE | awk '{ print $1 }')";

let N_LINES=N_LINES+1

echo $N_LINES

split -l 4 input/file.txt chunks/chunk_.txt

counter=1; 

for file in chunks/*; do 
	[[ -f $file ]] && mv -i "$file" "chunks/input"_$((counter))."txt" && ((counter++)); 
done

NUMBER_FILES="$(ls chunks | wc -l)";

N_THREADS=4

g++ paralelo.cpp -o wordcount -lpthread
./wordcount $NUMBER_FILES $N_THREADS