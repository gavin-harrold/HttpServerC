#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>

#define PORT 8080

int main(int argc, char const *argv[])
{
    int server_fd, new_socket;
    long valread;
    struct sockaddr_in address;
    int addr_len = sizeof(address);

    char *response = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n\n Hello world!\n";

    // socket file descriptor creation
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("in socket");
        exit(EXIT_FAILURE);
    }

    // address to bind socket to
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // create client address
    struct sockaddr_in client_addr;
    int client_addrlen = sizeof(client_addr);

    memset(address.sin_zero, '\0', sizeof(address.sin_zero));

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("in bind");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 10) < 0)
    {
        perror("in listen");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        printf("\n=====Waiting for connection=====\n");
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addr_len)) < 0)
        {
            perror("in accept");
            exit(EXIT_FAILURE);
        }

        int sockn = getsockname(new_socket, (struct sockaddr *)&client_addr, (socklen_t *)&client_addrlen);

        char buffer[30000] = {0};
        valread = read(new_socket, buffer, 30000);
        // printf("%s\n", buffer);

        // read request
        char method[30000], uri[30000], version[30000];
        sscanf(buffer, "%s %s %s", method, uri, version);
        // client connection information
        printf("[%s:%u] %s %s %s:\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), method, version, uri);
        write(new_socket, response, strlen(response));
        printf("=====Message Sent=====\n");
        close(new_socket);
    }
    return 0;
}
