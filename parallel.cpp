#include <string.h>
#include <stdio.h> 
#include <stdlib.h>
#include <pthread.h>
#include <unordered_map> 
#include <sstream>
#include <math.h>
#include <fstream>
#include <iostream>

using namespace std;

pthread_mutex_t lock;
unordered_map<string, int> ureduce; 

typedef struct thread {   
	pthread_t thread_id;       
  FILE *input;
  ofstream output;
  char *token;
  char *buffer;
	long size;
	unordered_map<string, int> umap; 
} ThreadData;


void *wordCount(void *thread)
{

  ThreadData *data  = (ThreadData*)thread;	

  //printf("%s %ld %li\n", data->buffer, data->thread_id, data->size);

  data->token = strtok(data->buffer, " \n\t"); 
  
  while (data->token != NULL) 
  {  
      if (data->umap.find(data->token) == data->umap.end()) 
        data->umap[data->token] = 1;
          
      else
        data->umap.at(data->token) += 1;

      data->token = strtok(NULL, " \n\t");
  } 

  free(data->buffer);
  free(data->token);
  return NULL;

}

int main(int argc, char *argv[])
{ 

    if(argc<3){
      perror("./wordcount number_files nthreads\n");
      exit(1);	
    }

    int nthreads, residue, n_iterations, ret_val, tid, output_files = 1;
    string temp;

    nthreads = atoi(argv[2]);

    residue = atoi(argv[1])%nthreads;

    n_iterations = ceil(atof(argv[1])/nthreads);    

    printf ("\nNumber of threads: %d\n", nthreads);
    printf ("Number of files: %i\n", atoi(argv[1]));
    printf ("Number of iterations: %i\n\n", n_iterations);

  for (int it = 0; it < n_iterations; it++)
  {   
      if (it == n_iterations-1 && residue)
        nthreads = residue;

      ThreadData thread[nthreads]; 

    	for(tid = 0; tid < nthreads; tid++)
      {

    	    stringstream ss;
    	    ss << output_files;
    	    
    	    temp = "output/output_" + ss.str() + ".txt";
    	    thread[tid].output.open(temp.c_str(), ios_base::app); 

          temp = "chunks/input_" + ss.str() + ".txt";
          thread[tid].input = fopen(temp.c_str(), "rb");

          if (!thread[tid].input | !thread[tid].output)
          {
            perror("Could not open file.\n");
            exit(1);
          }

          fseek(thread[tid].input, 0, SEEK_END); 
          thread[tid].size = ftell(thread[tid].input);  

          fseek(thread[tid].input, 0, SEEK_SET); 


          thread[tid].buffer = (char*) malloc (sizeof(char)*thread[tid].size);
          fread(thread[tid].buffer, 1, thread[tid].size, thread[tid].input); 

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

          fclose(thread[tid].input);

          pthread_mutex_lock(&lock);  

    		  for (auto x : thread[tid].umap) {

            if (ureduce.find(x.first) == ureduce.end()) 
              ureduce[x.first] = x.second;

            else
              ureduce.at(x.first) += x.second;

            thread[tid].output << x.first <<" "<<x.second<<endl;
    		  }

          pthread_mutex_unlock(&lock);
          thread[tid].output.close();
      }

      printf("Iteration number %i: %i threads\n", it+1, nthreads);
    }  

    cout<<endl<<"*********** Results *************"<<endl<<endl;

    for (auto x : ureduce) {

      cout<<x.first<<" "<<x.second<<endl;
    }  

    //pthread_exit(NULL);  
  
    return 0; 
} 
