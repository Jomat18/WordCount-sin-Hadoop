# creating an 7,1 GB (7.059.975.655 bytes) file size 
g++ generate_file.cpp -o gfile
./gfile

# get number of Threads
N_THREADS=$(nproc --all);  #4

# Wordcount 
g++ parallel.cpp -o wordcount -pthread
./wordcount $N_THREADS

#sudo sh -c 'echo 3 > /proc/sys/vm/drop_caches'