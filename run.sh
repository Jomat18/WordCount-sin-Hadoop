# Creating an 10,6 GB (10.589.982.570 bytes) file size 
g++ generate_file.cpp -o gfile
./gfile

# get number of Threads
N_THREADS=$(nproc --all);  #4

# Wordcount 
g++ parallel.cpp -o wordcount -pthread
time ./wordcount $N_THREADS

# Clear cache
#sudo sh -c 'echo 3 > /proc/sys/vm/drop_caches'