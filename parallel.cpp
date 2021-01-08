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
ofstream file_result;

typedef struct thread_data {   
    int thread_id;       
    ifstream input;
    ifstream input_bytes;
    ofstream output;
    char *token;
    char *buffer;
    int size;
    long int begin = 0;
    unordered_map<string, int> umap; 
} ThreadData;


void *wordCount(void *threadarg)
{
    ThreadData *data;
    data  = (ThreadData*) threadarg;

    //cout<<data->buffer<<endl;
  
    data->token = strtok(data->buffer, " \n\t"); 
  
    while (data->token != NULL) 
    {  
        if (data->umap.find(data->token) == data->umap.end()) 
            data->umap[data->token] = 1;
          
        else
            data->umap.at(data->token) += 1;

        data->token = strtok(NULL, " \n\t");
    } 

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
    cout<<"Number of files or chunks: "<<argv[1]<<"\n";
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
          
            //temp = "output/output_" + ss.str() + ".txt";
            //thread[tid].output.open(temp.c_str(), ios_base::app); 

            //temp = "chunks/input_" + ss.str() + ".txt";
            //thread[tid].input.open(temp.c_str(), ios::in);

            thread[tid].input_bytes.open("chunks/input.txt", ios::in);
            thread[tid].input.open("input/file.txt", ios::in);

            //if (!thread[tid].input | !thread[tid].output)
            if (!thread[tid].input | !thread[tid].input_bytes)
            {
                perror("Could not open file.\n");
                exit(1);
            }

            //thread[tid].input.seekg(0, thread[tid].input.end);
		    //thread[tid].size = thread[tid].input.tellg();
		    //thread[tid].input.seekg (0, thread[tid].input.beg);

			for (int i=0; i<output_files ;i++) {
				thread[tid].input_bytes >> thread[tid].size;
				thread[tid].begin += thread[tid].size;
			}

			thread[tid].begin -= thread[tid].size;

			thread[tid].input.seekg (thread[tid].begin, thread[tid].input.beg);

            thread[tid].buffer = (char*) malloc (thread[tid].size*sizeof(char));
            thread[tid].input.read(thread[tid].buffer, thread[tid].size);

            thread[tid].thread_id = tid;
          
            ret_val = pthread_create(&threads[tid], NULL, wordCount, (void *)&thread[tid]);
          
            if (ret_val) 
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

                //thread[tid].output << x.first <<" "<<x.second<<endl;
            }

            pthread_mutex_unlock(&lock);
            thread[tid].umap.clear();
    		thread[tid].input.close();
    		thread[tid].input_bytes.close();
    		thread[tid].begin = 0;
            //thread[tid].output.close();
        }    

        cout<<"Iteration number "<<it+1<<": "<<nthreads<<" threads\n";

    }

    cout<<endl<<"*********** Success!! *************"<<endl<<endl;

    file_result.open("result.txt", ios_base::out | ios_base::trunc); 
    for (auto x : ureduce) 
        file_result<<x.first<<" "<<x.second<<endl;

    file_result.close();
    pthread_exit(NULL);  
  
} 