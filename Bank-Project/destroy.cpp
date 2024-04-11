#include <iostream>
#include <cstdlib>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>

#include "bank.h"

int main()
{
    const int n = 10;
    const char* shm_name = "/bank_shared_mem";
    int shm_fd = shm_open(shm_name, O_RDWR, 0666);
    if(shm_fd == -1)
    {
        std::cerr << "sem shm open" << std::endl;
        exit(errno);
    }
    std::size_t size = sizeof(Bank) + n * sizeof(BankCell) + n * sizeof(sem_t);
    void* ptr = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(ptr == MAP_FAILED)
    {
        std::cerr << "init mmap" << std::endl;
        exit(EXIT_FAILURE);
    }

    Bank* bank_ptr = (Bank*)ptr;

    for(int i = 0; i < bank_ptr->bankSize; ++i)
    {
        std::string name= "sem" + std::to_string(i + 1);

        int shmm = shm_open(name.c_str(), O_CREAT | O_RDWR | O_TRUNC, 0666);
        if(shmm == -1)
        {
            std::cerr <<"sem shm open"<<std::endl;
            exit(errno);
        }
        (bank_ptr->cells[i]).sem = (sem_t*)mmap(nullptr, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, shmm, 0);
        std::cout<<"init "<<__LINE__<<std::endl;
        if(sem_destroy((bank_ptr->cells[i]).sem) == -1)
        {
            std::cerr << "sem_destroy" <<std::endl;
            exit(errno);
        }
        std::cout<<"init "<<__LINE__<<std::endl;

        if(shm_unlink(name.c_str()) == -1)
        {
            std::cerr << "shm_unlink" <<std::endl;
            exit(EXIT_FAILURE);
        }
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

    if(shm_unlink(shm_name) == -1)
    {
        std::cerr << "shm_unlink" << std::endl;
        exit(EXIT_FAILURE);
    }

    if(close(shm_fd) == -1)
    {
        std::cerr << "init close" <<std::endl;
        exit(EXIT_FAILURE);
    }


    return 0;
}