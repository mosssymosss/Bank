#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>

#include "config.h"

int main() {

    // server address
    struct sockaddr_in server_address;

    // create a socket
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        std::perror("socket creation error");
        exit(errno);
    }

    // fill the server ip and port address
    server_address.sin_addr.s_addr = inet_addr(ip_address);
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(hostshort);

    // connect
    int connected = connect(client_socket, (struct sockaddr *) &server_address, sizeof(server_address));

    if (connected == -1) {
        std::perror("connection failed");
        exit(errno);
    }
    system("clear");
    std::cout << "Successfully connected to server." << std::endl;
    // send message to server
    std::string mess;
    char buffer[3001];
    while (true) {
        std::getline(std::cin, mess);
        if (mess == "exit") {
            break;
        }
        int sent = send(client_socket, mess.c_str(), mess.size(), 0);
        if (sent == -1) {
            std::perror("send");
            exit(errno);
        }
        int rs = recv(client_socket, buffer, 3000, 0);
        if (rs < 0) {
            std::perror("client socket connection error");
            close(client_socket);
            exit(EXIT_FAILURE);
        }
        system("clear");
        buffer[rs] = '\0';
        if(std::string(buffer) == "shutdown")
        {
            close(client_socket);
            exit(EXIT_SUCCESS);

        }
        std::cout << buffer << std::endl;
    }
    close(client_socket);
    return 0;
}