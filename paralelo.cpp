#include <string.h>
#include <stdio.h> 
#include <stdlib.h>
#include <pthread.h>
#include <unordered_map> 
#include <sstream>
#include <math.h>

using namespace std;

typedef struct thread {   
	pthread_t thread_id;       
  FILE *input;
	FILE *output;
  char *token;
	int size;
	unordered_map<string, int> umap; 
} ThreadData;

void *wordCount(void *thread)
{

  ThreadData *data  = (ThreadData*)thread;	

  char *buffer = (char*) malloc (sizeof(char)*data->size);

  fread(buffer, data->size, 1, data->input);

  data->token = strtok(buffer, " \n\t"); 
  
  while (data->token != NULL) 
  {  
      if (data->umap.find(data->token) == data->umap.end()) 
        data->umap[data->token] = 1;
          
      else
        data->umap.at(data->token) += 1;

      data->token = strtok(NULL, " \n\t");
  } 

  free(buffer);
  free(data->token);
  //pthread_exit(NULL);
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

    printf ("Number of threads: %d\n", nthreads);
    printf ("Number of residue: %i\n", residue);
    printf ("Number of itearions: %i\n", n_iterations);

  for (int it = 0; it < n_iterations; it++)
  {   
      if (it == n_iterations-1 && residue)
        nthreads = residue;

      ThreadData thread[nthreads]; 

    	for(tid = 0; tid < nthreads; tid++)
      {
        	FILE *output;
          FILE *file; 

    	    stringstream ss;
    	    ss << output_files;
    	    
    	    temp = "output/output_" + ss.str() + ".txt";
    	    output = fopen(temp.c_str(),"w");

          temp = "chunks/input_" + ss.str() + ".txt";
          file = fopen(temp.c_str(), "r");

          if (!file | !output)
          {
            perror("Could not open file.\n");
            exit(1);
          }

          fseek(file, 0L, SEEK_END); 
          thread[tid].size = ftell(file);  

          fseek(file, 0L, SEEK_SET); 

          thread[tid].input = file;
    	    thread[tid].output = output;

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

    		  for (auto x : thread[tid].umap) {

    		    thread[tid].token = (char*) malloc (sizeof(char)*(x.first.length()+1));
    		    strcpy(thread[tid].token, x.first.c_str());  

    		    fprintf(thread[tid].output, "%s %d\n",thread[tid].token,x.second);
    		  }

    		  free(thread[tid].token); 	
          fclose(thread[tid].output);
      }

	//free(thread);
	//pthread_exit(NULL);
      printf("\nIteration number %i , %i threads\n", it+1, nthreads);
    }  
  
    return 0; 
} 
