#include <string.h>
#include <stdio.h> 
#include <stdlib.h>
#include <pthread.h>
#include <iostream>
#include <unordered_map> 
#include <cmath> 

using namespace std;

const char * filename = "file2.txt";

using namespace std;

struct to_read{
  FILE *fp;
  long int offset;
  long int start;
  long int chunk_size;
  unordered_map<string, int> umap; 
};

void *wordCount(void *arg)
{
  struct to_read *data = (struct to_read *)arg;

  char *buffer = (char*) malloc (sizeof(char)*data->chunk_size);

  fseek(data->fp, data->start, data->offset);
  fread(buffer, data->chunk_size, 1, data->fp);

  char *token = strtok(buffer, " "); 
  
  while (token != NULL) 
  {  
      if (data->umap.find(token) == data->umap.end()) 
        data->umap[token] = 1;
          
      else
        data->umap.at(token) += 1;

      token = strtok(NULL, " "); 
  } 

  for (auto x : data->umap) 
		cout<<x.first<<" "<<x.second<<endl; 

  free(buffer);
  free(token);
  pthread_exit(NULL);

}

int main( int argc, char **argv )
{ 
    FILE *fp; 
    fp = fopen(filename, "r"); 

    if (fp==NULL)
    {
      perror("Could not open file.");
      exit(1);
    }

    long int size, chunk_size;
    int nthreads, ret_val, tid;
    pthread_t *threads;

    printf("Enter the number of threads: ");
	scanf("%d",&nthreads);
	threads = (pthread_t *)malloc(nthreads*sizeof(pthread_t));

	fseek(fp, 0L, SEEK_END); //OL
    size = ftell(fp);  

    chunk_size = ceil((1.0*size)/nthreads);

   	printf ("Size of myfile.txt: %ld bytes.\n",size);
   	printf ("Size of chunk_size: %ld bytes.\n\n",chunk_size);
   

    fseek(fp, 0, SEEK_SET); 

	for(tid = 0; tid < nthreads; tid++)
    {
    	struct to_read data;

		data.fp=fp;
		data.chunk_size=chunk_size;
		data.start = tid*chunk_size;
		data.offset = data.start + chunk_size;

		ret_val = pthread_create(&threads[tid], NULL, wordCount, (void*) &data);
      
		if(ret_val!= 0) {
			printf ("Create pthread error!\n");
			exit (1);
      }
    }


	//wait for results (all threads)
	for (tid = 0; tid < nthreads; tid++){
		pthread_join(threads[tid], NULL);
	}  

	fclose(fp);
	free(threads);

	pthread_exit(NULL);
  
    return 0; 
} 

//g++ paralelo.cpp -o wordcount -lpthread