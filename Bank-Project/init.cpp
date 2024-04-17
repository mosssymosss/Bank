#include <iostream>
#include <cstdlib>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cerrno>
#include <semaphore.h>

#include "bank.h"

int main(int argc, char** argv)
{

    const int n = 10;

    const char* shm_name = "/bank_shared_mem";
    int shm_fd = shm_open(shm_name, O_CREAT | O_RDWR , 0666);
    if(shm_fd == -1)
    {
        std::cerr << "shm_open" <<std::endl;
        exit(errno);
    }


    std::size_t size = sizeof(Bank) + n * sizeof(BankCell);


    if(ftruncate(shm_fd, size) == -1)
    {
        std::cerr << "init ftruncate"<<std::endl;
        exit(errno);
    }

    void* ptr = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(ptr == MAP_FAILED)
    {
        std::cerr << "init mmap" << std::endl;
        exit(EXIT_FAILURE);
    }

    Bank* bank_ptr = (Bank*)ptr;

    bank_ptr->bankSize = n;

    for (int i = 0 ; i < n; ++i) {
        bank_ptr->cells[i] = BankCell(i);
        std::string name= "sem" + std::to_string(i + 1);
//    sem = sem_open(name.c_str(), O_EXCL | O_CREAT, 0, 1);
//    if(sem == SEM_FAILED)
//    {
//        std::cerr << "sem_open" << std::endl;
//        exit(errno);
//    }
        int shmm = shm_open(name.c_str(), O_CREAT | O_RDWR | O_TRUNC, 0666);
        if(shmm == -1)
        {
            std::cerr <<"sem shm open"<<std::endl;
            exit(errno);
        }
        if(ftruncate(shmm, sizeof(sem_t)) == -1)
        {
            std::cerr << "sem ftruncate" << std::endl;
            exit(errno);
        }
        (bank_ptr->cells[i]).sem = (sem_t*)mmap(nullptr, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, shmm, 0);
        std::cout<<"init "<<__LINE__<<std::endl;
        if(sem_init((bank_ptr->cells[i]).sem, 1, 1) == -1)
        {
            std::cerr << "sem_init" <<std::endl;
            exit(errno);
        }
        std::cout<<"init "<<__LINE__<<std::endl;
        if(munmap((bank_ptr->cells[i]).sem, sizeof(sem_t)) == -1)
        {
            std::cerr << "init munmap" <<std::endl;
            exit(EXIT_FAILURE);
        }

        if(close(shmm) == -1)
        {
            std::cerr << "init close" <<std::endl;
            exit(EXIT_FAILURE);
        }
    }
    std::cout<<"init "<<__LINE__<<std::endl;
    if(munmap(ptr, size) == -1)
    {
        std::cerr << "init munmap" <<std::endl;
        exit(EXIT_FAILURE);
    }

    if(close(shm_fd) == -1)
    {
        std::cerr << "init close" <<std::endl;
        exit(EXIT_FAILURE);
    }

    return 0;
}