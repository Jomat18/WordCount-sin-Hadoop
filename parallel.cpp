#include <unordered_map> 
#include <sstream>
#include <string.h>
#include <math.h>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

using namespace std;

pthread_mutex_t lock;
unordered_map<string, int> ureduce; 

typedef struct thread_data {   
    int thread_id;       
    ifstream input;
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

	//free(data->token);   
	//free(data->buffer);
	delete data->token; 
	delete data->buffer;
	data->token = NULL;
	data->buffer = NULL; 

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

    cout<<"\nNumber of threads: "<<nthreads<<"\n";
    cout<<"Number of files: "<<argv[1]<<"\n";
    cout<<"Number of iterations: "<<n_iterations<<"\n\n";

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
            thread[tid].input.open(temp.c_str(), ios::in);

            if (!thread[tid].input | !thread[tid].output)
            {
                perror("Could not open file.\n");
                exit(1);
            }

            thread[tid].input.seekg(0, thread[tid].input.end);
		    thread[tid].size = thread[tid].input.tellg();
		    thread[tid].input.seekg (0, thread[tid].input.beg);

            thread[tid].buffer = (char*) malloc (thread[tid].size*sizeof(char));
            thread[tid].input.read(thread[tid].buffer, thread[tid].size);

            thread[tid].thread_id = tid;
          
            ret_val = pthread_create(&threads[tid], NULL, wordCount, (void *)&thread[tid]);
          
            if(ret_val) 
            {
               cout<<"ERROR; return code from pthread_create() is "<<ret_val<<"\n";
               exit(-1);
            }

            output_files++;
        }

    
        for (tid = 0; tid < nthreads; tid++)
        {

            ret_val = pthread_join(threads[tid],  NULL);

            if (ret_val) 
            {
               cout<<"ERROR; return code from pthread_join() is "<<ret_val<<"\n";
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
            thread[tid].umap.clear();
    		thread[tid].input.close();
            thread[tid].output.close();
        }    

        cout<<"Iteration number "<<it+1<<": "<<nthreads<<" threads\n";

    }

    cout<<endl<<"*********** Results *************"<<endl<<endl;

    for (auto x : ureduce) 
        cout<<x.first<<" "<<x.second<<endl;

    pthread_exit(NULL);  
  
} 