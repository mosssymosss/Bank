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
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "bank.h"

sem_t* sem;
Bank* ptr;

std::string logic(std::string input);

void* handle_client(void* data)
{
    int client_socket = *(static_cast<int*>(data));
    char buffer[3001];
    // Receive message from client
    while(true)
    {
        int rs = recv(client_socket, buffer, 3000, 0);
        if (rs < 0)
        {
            std::perror("client socket connection error");
            close(client_socket);
            return nullptr;
        }
        else if (rs > 0)
        {
            if(sem_wait(sem) == -1)
            {
                std::cerr << "sem_wait" <<std::endl;
                exit(EXIT_FAILURE);
            }
            buffer[rs] = '\0';
            std::cout<<logic(buffer) <<std::endl;
            if(sem_post(sem) == -1)
            {
                std::cerr << "sem_post" <<std::endl;
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            std::cout<<"Disconnected client \n";
            break;
        }
    }
    close(client_socket);
    return nullptr;
}


int main()
{
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(server_socket == -1)
    {
        std::perror("socket");
        exit(errno);
    }

    struct sockaddr_in server_address;

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(9888);

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        std::perror("bind failed");
        exit(errno);
    }

    if (listen(server_socket, 1024) < 0)
    {
        std::perror("listen failed");
        exit(errno);
    }
    std::cout << "Waiting for connection\n";


    const char* sem_name = "/sem_shared_mem";
    sem = sem_open(sem_name,  O_CREAT, 0666, 1);

    const char* shm_name = "/bank_shared_mem";

    int shm_fd = shm_open(shm_name, O_RDWR, 0666);
    if(shm_fd == -1)
    {
        std::cerr << "shm_open" <<std::endl;
        exit(errno);
    }
    const int n = 10;
    std::size_t size = sizeof(Bank) + n * sizeof(BankCell);

    ptr = (Bank*)mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(ptr == MAP_FAILED)
    {
        std::cerr << "init mmap" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::vector<pthread_t> threads;

    while(true)
    {
        int client_socket;
        struct sockaddr_in client_address;
        unsigned int client_addr_len = sizeof(client_address);
        // Accept incoming connection
        if ((client_socket = accept(server_socket, (struct sockaddr*) &client_address, &client_addr_len)) < 0)
        {
            std::perror("accept failed");
            exit(errno);
        }
        std::cout << "Connected client with address: " << inet_ntoa(client_address.sin_addr) << "\n";

        pthread_t thread;
        if(pthread_create(&thread, nullptr, handle_client, &client_socket) != 0)
        {
            std::perror("pthread");
            exit(EXIT_FAILURE);
        }
        threads.push_back(thread);
    }
    for(auto& x : threads)
    {
        if(pthread_join(x, nullptr) != 0)
        {
            std::perror("join");
            exit(EXIT_FAILURE);
        }
    }


    if(munmap(ptr, size) == -1)
    {
        std::cerr << "munmap" <<std::endl;
        exit(EXIT_FAILURE);
    }

    if(close(shm_fd) == -1)
    {
        std::cerr << "close" <<std::endl;
        exit(EXIT_FAILURE);
    }
    if(sem_close(sem) == -1)
    {
        std::cerr << "sem close" <<std::endl;
        exit(EXIT_FAILURE);
    }

    if(close(server_socket) == -1)
    {
        std::cerr << "close" << std::endl;
        exit(EXIT_FAILURE);
    }

    return 0;
}

std::string logic(std::string input)
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

            int res = ptr->get_cell_curr_balance(std::stoi(in[1]) - 1);
            if(res == -1)
                str = str + "invalid id";
            else {
                str = str + "Current balance for cell number ";
                str = str + in[1];
                str = str + ": ";
                str = str + std::to_string(res);
            }
        }
        else if(in[0] == "minimum")
        {
            int res = ptr->get_cell_min_balance(std::stoi(in[1]) - 1);
            if(res == -1)
                str = str + "invalid id";
            else {
                str = str + "Minimum balance for cell number ";
                str = str + in[1];
                str = str + ": ";
                str = str + std::to_string(res);
            }
        }
        else if(in[0] == "maximum")
        {
            int res = ptr->get_cell_max_balance(std::stoi(in[1]) - 1);
            if(res == -1)
                str = str + "invalid id";
            else {
                str = str + "Maximum balance for cell number ";
                str = str + in[1];
                str = str + ": ";
                str = str + std::to_string(res);
            }
        }
        else if(in[0] == "freeze")
        {
            bool res = ptr->freeze_cell(std::stoi(in[1]) - 1);
            if(!res)
                str = str + "invalid id";
            else
                str = str + "Cell number " + in[1]+" successfully frozen";
        }
        else if(in[0] == "unfreeze")
        {
            bool res = ptr->unfreeze_cell(std::stoi(in[1]) - 1);
            if(!res)
                str = str + "invalid id";
            else
                str = str + "Cell number "+in[1]+" successfully unfrozen";
        }
        else if(in[0] == "addall")
        {

            bool res = ptr->add_to_all(std::stoi(in[1]));
            if(!res)
                str = str + "invalid op";
            else
                str = str + "Successfully added " +in[1] +" to all cells";

        }
        else if(in[0] == "suball")
        {
            bool res = ptr->sub_from_all(std::stoi(in[1]));
            if(!res)
                str = str + "invalid op";
            else
                str = str + "Successfully subbed " + in[1] +" from all cells";

        }
        else if(in[0] == "info")
        {
            std::string res = ptr->get_info(std::stoi(in[1]));
            if(res == "")
                str = str + "invalid id";
            else
                str = str + res;
        }
        else
        {
            str = str + "invalid input";
        }
    }
    else if(in.size() == 3)
    {
        if(in[0] == "setmin")
        {
            bool res = ptr->set_cell_min_amount(std::stoi(in[1]) - 1, std::stoi(in[2]));
            if(!res)
                str = str + "invalid op";
            else
                str = str +"Minimum for cell "+in[1]+" is set to "+in[2];
        }
        else if(in[0] == "setmax")
        {
            bool res = ptr->set_cell_max_amount(std::stoi(in[1]) - 1, std::stoi(in[2]));
            if(!res)
                str = str + "invalid op";
            else
                str = str +"Maximum for cell "+in[1]+" is set to "+in[2];
        }
        else
        {
            str = str + "invalid input";
        }
    }
    else if(in.size() == 4)
    {
        if(in[0] == "transfer")
        {
            bool res = ptr->transfer(std::stoi(in[1]) - 1, std::stoi(in[2]) - 1, std::stoi(in[3]));
            if(!res)
                str = str + "invalid op";
            else
                str = str +"Successfully transferred "+in[3] +" from " +in[1]+" to "+in[2];
        }
        else
        {
            str = str + "invalid input";
        }
    }
    else
    {
        str = str + "invalid input";
    }

    return str;
}