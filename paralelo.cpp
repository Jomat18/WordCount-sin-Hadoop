#include <string.h>
#include <stdio.h> 
#include <stdlib.h>
#include <pthread.h>
#include <unordered_map> 
#include <cmath> 
#include <sstream>

#define NTHREADS 4

using namespace std;

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
  char *buffer = (char*) malloc (sizeof(char)*data->chunk_size);

  fseek(file, data->start, data->offset);
  fread(buffer, data->chunk_size, 1, file);
  //printf("%ld %ld %ld %s\n",data->start ,data->offset ,data->chunk_size, filename);

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
  //pthread_exit(NULL);
  return NULL;

}

int main(int argc, char *argv[])
{ 

    if(argc<4)
      exit(1);	

    file = fopen(argv[1], "r");

    if (!file)
    {
      perror("Could not open file.");
      exit(1);
    }

    long int size, chunk_size;
    int ret_val, tid;

    //printf("Enter the number of threads: ");
	  //scanf("%d",&nthreads);
	//threads = (pthread_t *)malloc(nthreads*sizeof(pthread_t));

	//fseek(file, 0L, SEEK_END); 
    //size = ftell(file);  
    size = atoll(argv[2]);

    chunk_size = ceil((1.0*size)/NTHREADS);

   	printf ("File of %ld lines\n",size);
   	printf ("Size of chunk_size %ld lines\n",chunk_size);
    printf ("Name of file %s\n",argv[1]);
    printf ("Number of threads: %d\n", NTHREADS);

    fseek(file, 0, SEEK_SET); 
  	string temp;

  	ThreadData thread[NTHREADS]; 

    int output_files = atoi(argv[3]);

	for(tid = 0; tid < NTHREADS; tid++)
    {
    	FILE *output;
	    stringstream ss;
	    ss << output_files;
	    
	    temp = "../output/output_" + ss.str() + ".txt";
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
	for (tid = 0; tid < NTHREADS; tid++){
		pthread_join(thread[tid].thread_id, NULL);
	}  
	
    for (tid = 0; tid < NTHREADS; tid++){

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
  printf("\n");
  
    return 0; 
} 

//cd dataset -> bash ../start.sh