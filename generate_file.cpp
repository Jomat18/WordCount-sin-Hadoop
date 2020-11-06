#include <string.h>
#include <stdio.h> 
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <time.h> 

using namespace std;

int main() {
	long int iterations = 2000100000;
	int size;

	ofstream outfile;

  	outfile.open("file.txt", ios_base::app); 

	char str[] = "In computer science a trie also called digital tree or prefix tree is a kind of search tree an ordered tree data structure used to store a dynamic set or associative array where the keys are usually strings Unlike a binary search tree no node in the tree stores the key associated with that node instead its position in the tree defines the key with which it is associated ie the value of the key is distributed across the structure All the descendants of a node have a common prefix of the string associated with that node and the root is associated with the empty string Keys tend to be associated with leaves though some inner nodes may correspond to keys of interest Hence keys are not necessarily associated with every node For the space optimized presentation of prefix tree see compact prefix tree"; 
  
  	vector<string> input;
    
    char *token = strtok(str, " "); 
    
    while (token != NULL) 
    { 
        input.push_back(token); 
        token = strtok(NULL, " "); 
    } 

    size = input.size();
    srand (time(NULL));

    for (int i=0; i<iterations; i++) 
    	outfile << input[rand()%size]<<" ";

    outfile.close();

	return 0;
}