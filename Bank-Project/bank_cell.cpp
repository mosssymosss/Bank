#include "bank_cell.h"
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <cstdlib>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <sstream>
#include <semaphore.h>


BankCell::BankCell()
        :current_balance(0)
{

}


int BankCell::get_min_balance() const {
    return min_amount;
}

int BankCell::get_max_balance() const {
    return max_amount;
}

int BankCell::get_curr_balance() const
{
    std::cout << __LINE__ << std::endl;
    return current_balance;
}

void BankCell::freeze() {
    frozen = true;
}

void BankCell::unfreeze() {
    frozen = false;
}

bool BankCell::is_frozen() const{
    return frozen;
}

bool BankCell::receive_amount(int amount)  {
    if(current_balance + amount <= max_amount) {
        current_balance += amount;
        return true;
    }

    return false;
}

bool BankCell::send_amount(int amount) {
    if(current_balance - amount >= min_amount) {
        current_balance -= amount;
        return true;
    }

    return false;
}

void BankCell::set_min_amount(int amount) {
    min_amount = amount;
}

void BankCell::set_max_amount(int amount) {
    max_amount = amount;
}


