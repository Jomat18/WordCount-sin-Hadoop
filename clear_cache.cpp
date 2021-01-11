#include <iostream>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <unistd.h>

using namespace std;

void clear_cache(){
    sync();
    std::ofstream ofs("/proc/sys/vm/drop_caches");

    if (!ofs)
    {
        std::cout << "could not open file" << std::endl;
        exit(EXIT_FAILURE);
    }

    ofs << "3" << std::endl;
    sync();
}


int main() {

    for(int i = 0; i < 3; i++)
        clear_cache();

    return 0;
};
