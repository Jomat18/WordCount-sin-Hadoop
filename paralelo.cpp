#include <string.h>
#include <stdio.h> 
#include <stdlib.h>
#include <pthread.h>
#include <iostream>
#include <unordered_map> 
#include <cmath> 
#include <sstream>

using namespace std;

const char * filename = "file.txt";
FILE *file; 


typedef struct thread {   
	pthread_t thread_id;       
	FILE *output;
	long int offset;
	long int start;
	long int chunk_size;
	unordered_map<string, int> umap; 
} ThreadData;

void *wordCount(void *thread)
{
  ThreadData *data  = (ThreadData*)thread;	
  //struct to_read *data = (struct to_read *)arg;

  char *buffer = (char*) malloc (sizeof(char)*data->chunk_size);

  fseek(file, data->start, data->offset);
  fread(buffer, data->chunk_size, 1, file);

  char *token = strtok(buffer, " "); 
  
  while (token != NULL) 
  {  
      if (data->umap.find(token) == data->umap.end()) 
        data->umap[token] = 1;
          
      else
        data->umap.at(token) += 1;

      token = strtok(NULL, " "); 
  } 

  free(buffer);
  free(token);
  //free(word);
  //pthread_exit(NULL);
  return NULL;

}

int main() 
{ 

    file = fopen(filename, "r"); 

    if (file==NULL)
    {
      perror("Could not open file.");
      exit(1);
    }

    long int size, chunk_size;
    int nthreads, ret_val, tid;

    printf("Enter the number of threads: ");
	scanf("%d",&nthreads);
	//threads = (pthread_t *)malloc(nthreads*sizeof(pthread_t));

	fseek(file, 0L, SEEK_END); //OL
    size = ftell(file);  

    chunk_size = ceil((1.0*size)/nthreads);

   	printf ("Size of myfile.txt: %ld bytes.\n",size);
   	printf ("Size of chunk_size: %ld bytes.\n\n",chunk_size);
   

    fseek(file, 0, SEEK_SET); 
    int output_files = 1;
  	string temp;

  	ThreadData thread[nthreads]; 

	for(tid = 0; tid < nthreads; tid++)
    {
    	FILE *output;
	    stringstream ss;
	    ss << output_files;
	    
	    temp = "output_" + ss.str() + ".txt";
	    output = fopen(temp.c_str(),"w");

	    thread[tid].output = output;
		thread[tid].chunk_size = chunk_size;
		thread[tid].start = tid*chunk_size;
		thread[tid].offset = thread[tid].start + chunk_size;

		ret_val = pthread_create(&(thread[tid].thread_id), NULL, wordCount, (void *)(thread+tid));
      
		if(ret_val!= 0) {
			printf ("Create pthread error!\n");
			exit (1);
      	}
      	output_files++;
    }


	//wait for results (all threads)
	for (tid = 0; tid < nthreads; tid++){
		pthread_join(thread[tid].thread_id, NULL);
	}  
	
    for (tid = 0; tid < nthreads; tid++){

		  if(thread[tid].output == NULL)
		  {
		    printf("Error!");   
		    exit(1);             
		  }


		  char *word;

		  for (auto x : thread[tid].umap) {

		    word = (char*) malloc (sizeof(char)*(x.first.length()+1));
		    strcpy(word, x.first.c_str());  

		    fprintf(thread[tid].output, "%s %d\n",word,x.second);
		  }

		free(word); 	
        fclose(thread[tid].output);
    }

	fclose(file);
	//free(thread);

	//pthread_exit(NULL);
  
    return 0; 
} 

//g++ paralelo.cpp -o wordcount -lpthread

