#include <string.h>
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <pthread.h>
#include <unordered_map> 
#include <math.h>
#include <stdio.h> 
#include <fstream>

//#include <sys/stat.h>
//struct stat st;

#define DELIMS " \n.,;()!\"#%:@$&'*+,-/[]_{}~"
#define BLOCK 1073741824 // bytes  -> 1GB

using namespace std;

pthread_mutex_t lock;
unordered_map<string, int> reduce;
//FILE* file_reduce;
ofstream file_reduce;
FILE* input;

typedef struct thread_data {   
	//FILE* chunk_input; w+
	//FILE* file_map; a+
	long long chunk_size;
	long long start;
	long long end;
} ThreadData;

void *wordCount(void *threadarg)
{
	ThreadData *data = (ThreadData *) threadarg;

	unordered_map<string, int> map; 

	char *buffer = (char*) malloc (sizeof(char)*data->chunk_size);

	fseek(input, data->start, SEEK_SET);
	fread(buffer, data->chunk_size, 1, input);

	//printf("%s\n", buffer);

	printf ("Size of chunk original: %lld bytes.\n",data->chunk_size);

	char *token = strtok(buffer, DELIMS);

	while (token != NULL) 
	{  
		if (map.find(token) == map.end()) 
			map[token] = 1;
		  
		else
			map.at(token) += 1;

		token = strtok(NULL, DELIMS);
	} 

	free(token);
	free(buffer);

	pthread_mutex_lock(&lock);  

	for (unordered_map<string, int>::iterator it = map.begin(); it != map.end(); ++it) {
		//cout<<it->first;
		//printf(" : %d\n", it->second);
		if (reduce.find(it->first) == reduce.end()) 
			reduce[it->first] = it->second;

		else
			reduce.at(it->first) += it->second;
	}

	pthread_mutex_unlock(&lock);
	//data->chunk_input.close();
	//data->file_map.close();

	return NULL;
}


int main(int argc, char *argv[])
{ 

	if(argc<2)
	{
		perror("./wordcount nthreads\n");
		exit(1);  
	}

	if (pthread_mutex_init(&lock, NULL) != 0) { 
		printf("\n mutex init has failed\n"); 
		return 1; 
	} 

	string filename = "input/file.txt";
	
	long long size, chunks_number, start = 0;
	long nthreads, residue, n_iterations, ret_val, tid, tid_, part_file = 1;
	string temp;
	char letter;

	//pthread_t *threads;

	input = fopen(filename.c_str(), "rt");

	if (!input)
		exit(EXIT_FAILURE);

	nthreads = atol(argv[1]);
	pthread_t threads[nthreads];
	ThreadData thread[nthreads];    

	//threads = (pthread_t *)malloc(nthreads*sizeof(pthread_t));

	fseek(input, 0L, SEEK_END); 
	size = ftell(input);  

	chunks_number = ceil((1.0*size)/BLOCK); 

	n_iterations = ceil((1.0*chunks_number)/nthreads);    
	residue = chunks_number%nthreads;

	// Linux/POSIX:
	//stat(filename.c_str(), &st);
	//size = st.st_size;

	fseek(input, 0L, SEEK_SET); 

	printf("\nNumber of threads: %ld\n", nthreads);
	printf("Size of file.txt: %lld bytes.\n",size);
	printf("Size of chunk initial: %d bytes.\n",BLOCK);
	printf("Number of files or chunks: %lld aprox\n", chunks_number);
	printf("Number of iterations: %ld aprox\n\n", n_iterations);

	pthread_mutex_init(&lock, NULL);

	for (int it = 0; it < n_iterations && size; it++)
	{   
		if (it == n_iterations-1 && residue)
			nthreads = residue;

		for(tid = 0; tid < nthreads && size; tid++)
		{
			//stringstream ss;
			//ss << part_file;            
			
			thread[tid].start = start;
			thread[tid].chunk_size = BLOCK;
			thread[tid].end = thread[tid].start + thread[tid].chunk_size;
			
			if (size <=thread[tid].chunk_size) {
				thread[tid].chunk_size = size;
				size = 0;
			}
			else {
				
				fseek(input, thread[tid].end, SEEK_SET); 

				letter=fgetc(input);
				ungetc(letter,input);

				while(isalpha(letter) && size> thread[tid].chunk_size) {

					thread[tid].chunk_size++;
					fseek(input, 1, SEEK_CUR);

					letter=fgetc(input);
					ungetc(letter,input);
				}
			
				start += thread[tid].chunk_size;
				size -= thread[tid].chunk_size;
	
			}

			ret_val = pthread_create(&threads[tid], NULL, wordCount, (void *)&thread[tid]);

			if(ret_val!= 0) {
				printf ("Create pthread error!, return code from pthread_create() is %ld\n", ret_val);
				exit(EXIT_FAILURE);
			}

			//part_file++;
		}


		for (tid_ = 0; tid_ < tid; tid_++){
			ret_val = pthread_join(threads[tid_], NULL);

			if (ret_val) 
			{
				printf ("ERROR, return code from pthread_join() is %ld\n", ret_val);                   
				exit(EXIT_FAILURE);
			}
		}  

		printf("\tIteration number %d : %ld threads\n",(it+1), tid);
	}

	pthread_mutex_destroy(&lock); 
	printf("\n*********** Success!! *************\n\n");

	//file_reduce = fopen("result.txt", "w+"); 
	file_reduce.open("result.txt", ios_base::out | ios_base::trunc);

	for (unordered_map<string, int>::iterator it = reduce.begin(); it != reduce.end(); ++it) {
		//cout<<it->first;
		//printf(" - %d\n", it->second);
		file_reduce << it->first << " - " << it->second <<"\n";
	}

	fclose(input);
	file_reduce.close();
	//free(threads);

	return 0;
}  
