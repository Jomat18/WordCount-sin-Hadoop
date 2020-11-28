#!/bin/bash

g++ ../paralelo.cpp -o .././wordcount.o -lpthread

THREADS=4
COUNT=1

for file in *; do 
	LINES="$(wc -l $file | awk '{ print $1 }')";
	let LINES=LINES+1
	.././wordcount.o $file $LINES $COUNT
	let COUNT=COUNT+THREADS
done

