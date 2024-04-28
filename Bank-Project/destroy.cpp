#include <iostream>
#include <cstdlib>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <sstream>
#include <semaphore.h>

#include "bank.h"
#include "config.h"


int main()
{

    if(sem_unlink(sem_name) == -1)
    {
        std::cerr << "sem_unlink" << std::endl;
        exit(EXIT_FAILURE);
    }

    if(shm_unlink(shm_name) == -1) {
        std::cerr << "shm_unlink" << std::endl;
        exit(EXIT_FAILURE);
    }



    return 0;
}
