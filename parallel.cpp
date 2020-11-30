#include <string.h>
#include <unordered_map> 
#include <sstream>
#include <math.h>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

pthread_mutex_t lock;
unordered_map<string, int> ureduce; 

typedef struct thread_data {   
    int thread_id;       
    FILE *input;
    ofstream output;
    char *token;
    char *buffer;
    int size;
    unordered_map<string, int> umap; 
} ThreadData;


void *wordCount(void *threadarg)
{
    ThreadData *data;
    data  = (ThreadData*) threadarg;
  
    data->token = strtok(data->buffer, " \n\t"); 
  
    while (data->token != NULL) 
    {  
        if (data->umap.find(data->token) == data->umap.end()) 
            data->umap[data->token] = 1;
          
        else
            data->umap.at(data->token) += 1;

        data->token = strtok(NULL, " \n\t");
    } 

	free(data->token);   
	free(data->buffer);
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    if(argc<3)
    {
        perror("./wordcount number_files nthreads\n");
        exit(1);  
    }

    int nthreads, residue, n_iterations, ret_val, tid, output_files = 1;
    string temp;

    nthreads = atoi(argv[2]);
    residue = atoi(argv[1])%nthreads;
    n_iterations = ceil(atof(argv[1])/nthreads);    
    ThreadData thread[nthreads]; 

    printf ("\nNumber of threads: %d\n", nthreads);
    printf ("Number of files: %i\n", atoi(argv[1]));
    printf ("Number of iterations: %i\n\n", n_iterations);

    pthread_t threads[nthreads];

    for (int it = 0; it < n_iterations; it++)
    {   
        if (it == n_iterations-1 && residue)
            nthreads = residue;

        for(tid = 0; tid < nthreads; tid++)
        {
            stringstream ss;
            ss << output_files;
          
            temp = "output/output_" + ss.str() + ".txt";
            thread[tid].output.open(temp.c_str(), ios_base::app); 

            temp = "chunks/input_" + ss.str() + ".txt";
            thread[tid].input = fopen(temp.c_str(), "r");

            if (!thread[tid].input | !thread[tid].output)
            {
                perror("Could not open file.\n");
                exit(1);
            }

            fseek(thread[tid].input, 0, SEEK_END); 
            thread[tid].size = ftell(thread[tid].input);  

            fseek(thread[tid].input, 0, SEEK_SET); 

            thread[tid].buffer = (char*) malloc (thread[tid].size*sizeof(char));
            fread(thread[tid].buffer, 1, thread[tid].size, thread[tid].input); 

            thread[tid].thread_id = tid;

            //printf("%s %d %li\n", thread[tid].buffer, thread[tid].thread_id, thread[tid].size);
          
            ret_val = pthread_create(&threads[tid], NULL, wordCount, (void *)&thread[tid]);
          
            if(ret_val) 
            {
               printf("ERROR; return code from pthread_create() is %d\n", ret_val);
               exit(-1);
            }

            output_files++;
        }

    
        for (tid = 0; tid < nthreads; tid++)
        {

            ret_val = pthread_join(threads[tid],  NULL);

            if (ret_val) 
            {
               printf("ERROR; return code from pthread_join() is %d\n", ret_val);
               exit(-1);
            }

            pthread_mutex_lock(&lock);  

            for (auto x : thread[tid].umap) 
            {

                if (ureduce.find(x.first) == ureduce.end()) 
                    ureduce[x.first] = x.second;

                else
                    ureduce.at(x.first) += x.second;

                thread[tid].output << x.first <<" "<<x.second<<endl;
            }

            pthread_mutex_unlock(&lock);
    		fclose(thread[tid].input);
            thread[tid].output.close();
        }    

        printf("Iteration number %i: %i threads\n", it+1, nthreads);

    }

    cout<<endl<<"*********** Results *************"<<endl<<endl;

    for (auto x : ureduce) 
        cout<<x.first<<" "<<x.second<<endl;

    pthread_exit(NULL);  
  
} 