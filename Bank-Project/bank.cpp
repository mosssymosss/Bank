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
    int res;
    if(num < 0 || num >= bankSize)
    {
        return -1;
    }
    res = cells[num].get_curr_balance();
    return res;
}
int Bank::get_cell_min_balance(int num) const
{
    int res;

    if(num < 0 || num >= bankSize) {
        return -1;
    }
    res = cells[num].get_min_balance();
    return res;
}

int Bank::get_cell_max_balance(int num) const
{
    int res;
    if(num < 0 || num >= bankSize) {
        return -1;
    }
    res = cells[num].get_max_balance();
    return res;
}

bool Bank::freeze_cell(int num)
{
    bool res;

    if(num < 0 || num >= bankSize)
    {
        return false;
    }
    cells[num].freeze();
    res = true;
    return res;
}

bool Bank::unfreeze_cell(int num)
{
    bool res;
    if(num < 0 || num >= bankSize) {
        return false;
    }

    cells[num].unfreeze();
    res = true;
    return res;
}

bool Bank::transfer(int a, int b, int amount)
{
    bool res = false;
    if(a < 0 || a >= bankSize || b < 0 || b >= bankSize || amount < 0)
    {
        return false;
    }
    if(cells[a].is_frozen() || cells[b].is_frozen())
    {
        return false;
    }

    if(cells[a].send_amount(amount)) {
        if(cells[b].receive_amount(amount)) {
            res = true;
        }

        cells[a].receive_amount(amount);
    }
    return res;
}

bool Bank::add_to_all(int amount)
{

    if(amount < 0)
    {
        return false;
    }

    int index = 0;
    bool res = true;
    for( ; index < bankSize; ++index) {
        if(cells[index].is_frozen() || !cells[index].receive_amount(amount)) {
            for(int i = 0; i < index; ++i) {
                cells[i].send_amount(amount);
            }

            res = false;
        }
    }
    return res;
}

bool Bank::sub_from_all(int amount)
{

    if(amount < 0)
    {

        return false;
    }

    int index = 0;
    bool res = true;
    for( ; index < bankSize; ++index) {
        if(cells[index].is_frozen() || !cells[index].send_amount(amount)) {
            for(int i = 0; i < index; ++i) {
                cells[i].receive_amount(amount);
            }

            res = false;
        }
    }

    return res;
}

bool Bank::set_cell_min_amount(int num, int amount)
{
    if(num < 0 || num >= bankSize) {
        return false;
    }

    if(amount > cells[num].get_curr_balance() || amount >= cells[num].get_max_balance() || cells[num].is_frozen()) {

        return false;
    }

    cells[num].set_min_amount(amount);
    return true;
}

bool Bank::set_cell_max_amount(int num, int amount) {
    if(num < 0 || num >= bankSize) {
        return false;
    }

    if(amount < cells[num].get_curr_balance() || amount <= cells[num].get_min_balance() || cells[num].is_frozen()) {

        return false;
    }

    cells[num].set_max_amount(amount);
    return true;
}

std::string Bank::get_info(int num) const
{
    if(num < 0 || num >= bankSize) {
        return "";
    }
    std::string res = "curr: ";
    res = res + std::to_string(cells[num].get_curr_balance()) + " | min: ";
    res = res + std::to_string(cells[num].get_min_balance()) + " | max: ";
    res = res + std::to_string(cells[num].get_max_balance()) + " | frozen: ";
    if(cells[num].is_frozen())
    {
        res += "true";
    }
    else
    {
        res += "false";
    }
    return res;
}

BankCell& Bank::operator[](unsigned int ind)
{
    return cells[ind];
}