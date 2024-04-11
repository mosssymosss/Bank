#include "bank.h"
#include <iostream>
#include <semaphore.h>
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


int Bank::get_cell_curr_balance(int num) const
{
    std::cout << __LINE__ << std::endl;
    int res;
    if(num < 0 || num >= bankSize)
    {
        std::cout << __LINE__ << std::endl;
        return -1;
    }
    std::cout << __LINE__ << std::endl;
    if(sem_wait(cells[num].sem) == -1)
    {
        std::cerr << "sem_wait" << std::endl;
        exit(errno);
    }
    std::cout << __LINE__ << std::endl;
    res = cells[num].get_curr_balance();
    std::cout << __LINE__ << std::endl;
    sem_post(cells[num].sem);
    std::cout << __LINE__ << std::endl;
    return res;
}
int Bank::get_cell_min_balance(int num) const
{
    int res;

    if(num < 0 || num >= bankSize) {
        return -1;
    }
    sem_wait(cells[num].sem);
    res = cells[num].get_min_balance();
    sem_post(cells[num].sem);
    return res;
}

int Bank::get_cell_max_balance(int num) const
{
    int res;

    if(num < 0 || num >= bankSize) {
        return -1;
    }
    sem_wait(cells[num].sem);
    res = cells[num].get_max_balance();
    sem_post(cells[num].sem);
    return res;
}

bool Bank::freeze_cell(int num)
{
    bool res;

    if(num < 0 || num >= bankSize)
    {
        return false;
    }
    sem_wait(cells[num].sem);
    cells[num].freeze();
    res = true;
    sem_post(cells[num].sem);
    return res;
}

bool Bank::unfreeze_cell(int num)
{
    bool res;
    if(num < 0 || num >= bankSize) {
        return false;
    }

    sem_wait(cells[num].sem);
    cells[num].unfreeze();
    res = true;
    sem_post(cells[num].sem);
    return res;
}

bool Bank::transfer(int a, int b, int amount)
{
    bool res = false;
    if(a < 0 || a >= bankSize || b < 0 || b >= bankSize || amount < 0)
    {
        return false;
    }
    sem_wait(cells[a].sem);
    sem_wait(cells[b].sem);
    if(cells[a].is_frozen() || cells[b].is_frozen())
    {
        sem_post(cells[a].sem);
        sem_post(cells[b].sem);
        return false;
    }

    if(cells[a].send_amount(amount)) {
        if(cells[b].receive_amount(amount)) {
            res = true;
        }

        cells[a].receive_amount(amount);
    }
    sem_post(cells[a].sem);
    sem_post(cells[b].sem);
    return res;
}

bool Bank::add_to_all(int amount)
{
    for(int i = 0; i < bankSize; ++i)
    {
        sem_wait(cells[i].sem);
    }
    if(amount < 0)
    {
        for(int i = 0; i < bankSize; ++i)
        {
            sem_post(cells[i].sem);
        }
        return false;
    }

    int index = 0;
    bool res = true;
    for( ; index < bankSize; ++index) {
        if(!cells[index].receive_amount(amount) || cells[index].is_frozen()) {
            for(int i = 0; i < index; ++i) {
                cells[i].send_amount(amount);
            }

            res = false;
        }
    }
    for(int i = 0; i < bankSize; ++i)
    {
        sem_post(cells[i].sem);
    }
    return res;
}

bool Bank::sub_from_all(int amount)
{
    for(int i = 0; i < bankSize; ++i)
    {
        sem_wait(cells[i].sem);
    }
    if(amount < 0)
    {
        for(int i = 0; i < bankSize; ++i)
        {
            sem_post(cells[i].sem);
        }
        return false;
    }

    int index = 0;
    bool res = true;
    for( ; index < bankSize; ++index) {
        if(!cells[index].send_amount(amount) || cells[index].is_frozen()) {
            for(int i = 0; i < index; ++i) {
                cells[i].receive_amount(amount);
            }

            res = false;
        }
    }

    for(int i = 0; i < bankSize; ++i)
    {
        sem_post(cells[i].sem);
    }
    return res;
}

bool Bank::set_cell_min_amount(int num, int amount)
{
    if(num < 0 || num >= bankSize) {
        return false;
    }
    sem_wait(cells[num].sem);
    if(amount > cells[num].get_curr_balance() || amount >= cells[num].get_max_balance() || cells[num].is_frozen()) {
        sem_post(cells[num].sem);
        return false;
    }

    cells[num].set_min_amount(amount);
    sem_post(cells[num].sem);
    return true;
}

bool Bank::set_cell_max_amount(int num, int amount) {
    if(num < 0 || num >= bankSize) {
        return false;
    }
    sem_wait(cells[num].sem);
    if(amount < cells[num].get_curr_balance() || amount <= cells[num].get_min_balance() || cells[num].is_frozen()) {
        sem_post(cells[num].sem);
        return false;
    }

    cells[num].set_max_amount(amount);
    sem_post(cells[num].sem);
    return true;
}

BankCell& Bank::operator[](unsigned int ind)
{
    return cells[ind];
}