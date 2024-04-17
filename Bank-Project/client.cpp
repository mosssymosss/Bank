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

std::string logic(Bank* ptr, sem_t* sem, std::string input);



int main()
{

    const char* sem_name = "/sem_shared_mem";
    sem_t* sem = sem_open(sem_name,  O_CREAT, 0666, 1);

    const char* shm_name = "/bank_shared_mem";

    int shm_fd = shm_open(shm_name, O_RDWR, 0666);
    if(shm_fd == -1)
    {
        std::cerr << "shm_open" <<std::endl;
        exit(errno);
    }
    const int n = 10;
    std::size_t size = sizeof(Bank) + n * sizeof(BankCell);

    Bank* ptr = (Bank*)mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(ptr == MAP_FAILED)
    {
        std::cerr << "init mmap" << std::endl;
        exit(EXIT_FAILURE);
    }
    while(true)
    {
        std::string input;
        std::getline(std::cin, input);
        if (input == "exit")
            break;
        std::cout << logic(ptr, sem, input)<<std::endl;
    }


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
    if(sem_close(sem) == -1)
    {
        std::cerr << "sem close" <<std::endl;
        exit(EXIT_FAILURE);
    }
    
    return 0;
}

std::string logic(Bank* ptr, sem_t* sem, std::string input)
{
    std::string str;
    std::stringstream strr(input);
    std::string temp;
    std::vector<std::string> in;
    while(strr >> temp) {
        in.push_back(temp);
    }
    //current 5
    //minimum 5
    //maximum 5
    //freeze 5
    //unfreeze 5
    //transfer 5 6 50
    //addall 50
    //suball 50
    //setmin 5 50
    //setmax 5 50
    if(in.size() == 2)
    {
        if(in[0] == "current")
        {
            sem_wait(sem);
            int res = ptr->get_cell_curr_balance(std::stoi(in[1]) - 1);
            if(res == -1)
                str = str + "invalid id";
            else {
                str = str + "Current balance for cell number ";
                str = str + in[1];
                str = str + ": ";
                str = str + std::to_string(res);
            }
            sem_post(sem);
        }
        else if(in[0] == "minimum")
        {
            sem_wait(sem);
            int res = ptr->get_cell_min_balance(std::stoi(in[1]) - 1);
            if(res == -1)
                str = str + "invalid id";
            else {
                str = str + "Minimum balance for cell number ";
                str = str + in[1];
                str = str + ": ";
                str = str + std::to_string(res);
            }
            sem_post(sem);
        }
        else if(in[0] == "maximum")
        {
            sem_wait(sem);
            int res = ptr->get_cell_max_balance(std::stoi(in[1]) - 1);
            if(res == -1)
                str = str + "invalid id";
            else {
                str = str + "Maximum balance for cell number ";
                str = str + in[1];
                str = str + ": ";
                str = str + std::to_string(res);
            }
            sem_post(sem);
        }
        else if(in[0] == "freeze")
        {
            sem_wait(sem);
            bool res = ptr->freeze_cell(std::stoi(in[1]) - 1);
            if(!res)
                str = str + "invalid id";
            else
                str = str + "Cell number " + in[1]+" successfully frozen";
            sem_post(sem);
        }
        else if(in[0] == "unfreeze")
        {
            sem_wait(sem);
            bool res = ptr->unfreeze_cell(std::stoi(in[1]) - 1);
            if(!res)
                str = str + "invalid id";
            else
                str = str + "Cell number "+in[1]+" successfully unfrozen";
            sem_post(sem);
        }
        else if(in[0] == "addall")
        {
            sem_wait(sem);
            bool res = ptr->add_to_all(std::stoi(in[1]));
            if(!res)
                str = str + "invalid op";
            else
                str = str + "Successfully added " +in[1] +" to all cells";
            sem_post(sem);
        }
        else if(in[0] == "suball")
        {
            sem_post(sem);
            bool res = ptr->sub_from_all(std::stoi(in[1]));
            if(!res)
                str = str + "invalid op";
            else
                str = str + "Successfully subbed " + in[1] +" from all cells";
            sem_post(sem);
        }
        else
        {
            sem_wait(sem);
            str = str + "invalid input";
            sem_post(sem);
        }
    }
    else if(in.size() == 3)
    {
        if(in[0] == "setmin")
        {
            sem_wait(sem);
            bool res = ptr->set_cell_min_amount(std::stoi(in[1]) - 1, std::stoi(in[2]));
            if(!res)
                str = str + "invalid op";
            else
                str = str +"Minimum for cell "+in[1]+" is set to "+in[2];
            sem_post(sem);
        }
        else if(in[0] == "setmax")
        {
            sem_wait(sem);
            bool res = ptr->set_cell_max_amount(std::stoi(in[1]) - 1, std::stoi(in[2]));
            if(!res)
                str = str + "invalid op";
            else
                str = str +"Maximum for cell "+in[1]+" is set to "+in[2];
            sem_post(sem);
        }
        else
        {
            sem_wait(sem);
            str = str + "invalid input";
            sem_post(sem);
        }
    }
    else if(in.size() == 4)
    {
        if(in[0] == "transfer")
        {
            sem_wait(sem);
            bool res = ptr->transfer(std::stoi(in[1]) - 1, std::stoi(in[2]) - 1, std::stoi(in[3]));
            if(!res)
                str = str + "invalid op";
            else
                str = str +"Successfully transferred "+in[3] +" from " +in[1]+" to "+in[2];
            sem_post(sem);
        }
        else
        {
            sem_wait(sem);
            str = str + "invalid input";
            sem_post(sem);
        }
    }
    else
    {
        sem_wait(sem);
        str = str + "invalid input";
        sem_post(sem);
    }

    return str;
}