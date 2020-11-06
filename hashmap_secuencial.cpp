#include <iostream> 
#include <unordered_map> 
#include <bits/stdc++.h>  //#include<fstream>
using namespace std; 
  
int main() 
{  
    unordered_map<string, int> umap; 
    fstream file; 
    string word, filename; 
  
    filename = "file.txt"; 
  
    file.open(filename.c_str()); 
  
    while (file >> word) {
    	if (umap.find(word) == umap.end()) 
    		umap[word] = 1;
    			
    	else
    		umap.at(word) += 1;
        
    }
    	

    file.close();
  
    for (auto x : umap) 
        cout << x.first << " " << x.second << endl; 
    
    return 0;
} 
