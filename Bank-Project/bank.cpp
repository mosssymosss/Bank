#include "bank.h"
#include <iostream>

/*
std::string Bank::get_cell_curr_balance(int num) const {
    if(num < 0 || num >= bankSize)
    {
        return "invalid cell id";
    }

    int res = cells[num].get_curr_balance();
    std::string str;
    str = str + "Current balance for cell number ";
    str = str + std::to_string(num + 1);
    str = str + ": ";
    str = str + std::to_string(res);
    return str;
}
std::string Bank::get_cell_min_balance(int num) const {
    if(num < 0 || num >= bankSize)
    {
        return "invalid cell id";
    }

    int res = cells[num].get_min_balance();
    std::string str;
    str = str + "Minimum balance for cell number ";
    str = str + std::to_string(num + 1);
    str = str + ": ";
    str = str + std::to_string(res);
    return str;
}

std::string Bank::get_cell_max_balance(int num) const {
    if(num < 0 || num >= bankSize)
    {
        return "invalid cell id";
    }

    int res = cells[num].get_max_balance();
    std::string str;
    str = str + "Maximum balance for cell number ";
    str = str + std::to_string(num + 1);
    str = str + ": ";
    str = str + std::to_string(res);
    return str;
}
*/
std::string Bank::freeze_cell(int num)
{
    if(num < 0 || num >= bankSize)
    {
        return "invalid cell id";
    }

    cells[num].freeze();
    return "Cell number " + std::to_string(num + 1) + " successfully frozen";
}

std::string Bank::unfreeze_cell(int num)
{
    if(num < 0 || num >= bankSize)
    {
        return "invalid cell id";
    }

    cells[num].unfreeze();
    return "Cell number " + std::to_string(num + 1) + " successfully unfrozen";
}

std::string Bank::transfer(int a, int b, int amount) {
    if(a < 0 || a >= bankSize || b < 0 || b >= bankSize)
    {
        return "invalid cell id";
    }

    if(amount < 0)
    {
        return "can't transfer negative amount";
    }
    if(cells[a].is_frozen() || cells[b].is_frozen()) {
        return "one of the cells is frozen, unable to transfer";
    }

    if(cells[a].send_amount(amount)) {
        if(cells[b].receive_amount(amount)) {
            std::string str = "Successfully transferred " + std::to_string(amount);
            str = str +" from " + std::to_string(a) + " to " + std::to_string(b);
            return str;
        }

        cells[a].receive_amount(amount);
    }
    return "one of the cell is at the limit, operation not done";
}

std::string Bank::add_to_all(int amount)
{
    if(amount < 0)
    {
        return "can't add negative amount";
    }

    int index = 0;
    for( ; index < bankSize; ++index) {
        if(cells[index].is_frozen() || !cells[index].receive_amount(amount)) {
            for(int i = 0; i < index; ++i) {
                cells[i].send_amount(amount);
            }
            if(cells[index].is_frozen())
                return "cell " + std::to_string(index + 1) + " is frozen, operation not done";
            else
                return "cell " + std::to_string(index + 1) + " is at the limit, operation not done";
        }
    }

    return "Successfully added " + std::to_string(amount) +" to all cells";
}

std::string Bank::sub_from_all(int amount)
{
    if(amount < 0)
    {
        return "can't add negative amount";
    }

    int index = 0;
    for( ; index < bankSize; ++index) {
        if(cells[index].is_frozen() || !cells[index].send_amount(amount)) {
            for(int i = 0; i < index; ++i) {
                cells[i].receive_amount(amount);
            }
            if(cells[index].is_frozen())
                return "cell " + std::to_string(index + 1) + " is frozen, operation not done";
            else
                return "cell " + std::to_string(index + 1) + " can't go any lower, operation not done";
        }
    }

    return "Successfully subbed " + std::to_string(amount) +" from all cells";
}

std::string Bank::set_cell_min_amount(int num, int amount)
{
    if(num < 0 || num >= bankSize)
    {
        return "invalid cell id";
    }

    if(amount > cells[num].get_curr_balance())
    {
        return "cell current balance is greater than the new minimum, operation not done";
    }
    if(amount >= cells[num].get_max_balance())
    {
        return "new minimum balance greater than cell maximum, operation not done";
    }
    if(cells[num].is_frozen())
    {
        return "cell frozen, operation not done";
    }

    cells[num].set_min_amount(amount);
    return "Minimum for cell "+std::to_string(num)+" is set to "+std::to_string(amount);
}

std::string Bank::set_cell_max_amount(int num, int amount) {
    if(num < 0 || num >= bankSize)
    {
        return "invalid cell id";
    }

    if(amount < cells[num].get_curr_balance())
    {
        return "cell current balance is less than the new maximum, operation not done";
    }
    if(amount <= cells[num].get_min_balance())
    {
        return "new maximum balance less than cell minimum, operation not done";
    }
    if(cells[num].is_frozen())
    {
        return "cell frozen, operation not done";
    }

    cells[num].set_max_amount(amount);
    return "Maximum for cell "+std::to_string(num)+" is set to "+std::to_string(amount);
}

std::string Bank::get_info(int num) const
{
    if(num < 0 || num >= bankSize) {
        return "invalid cell id";
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