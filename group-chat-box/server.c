#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include "message.h"


#define PORT 8000
#define MAX_CLIENT_COUNT 10
#define MAX_BUFF_SIZE 1024

// Client socket pool
typedef int ClientPool[MAX_CLIENT_COUNT];
ClientPool client_pool = {0};

// Server socket
int srv_socket;

int start_server();
int add_sockfd(int sockfd);
int remove_sockfd(int sockfd);



int start_server()
{
    int srv_socket;
    int opt = 1;
    struct sockaddr_in srv_addr;
    
    // Create server socket (TCP)
    if ( (srv_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(-1);
    }

    // set socket option
    if ( setsockopt(srv_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
    {
        perror("setsockopt failed");
        exit(-1);
    }

    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(PORT);
    // srv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if ( bind(srv_socket, (struct sockaddr*) &srv_addr, sizeof(srv_addr)) < 0)
    {
        perror("socket bind failed");
        exit(-1);
    }

    if ((listen(srv_socket, 5)) < 0)
    {
        perror("listen failed");
        exit(-1);
    }
    return srv_socket;
}

int add_sockfd(int sockfd)
{
    for (int i = 0; i < MAX_CLIENT_COUNT; ++i)
        if (client_pool[i] == 0)
        {
            client_pool[i] = sockfd;
            return 0;
        }
    return -1;
}

int remove_sockfd(int sockfd)
{
    for (int i = 0; i < MAX_CLIENT_COUNT; ++i)
        if (client_pool[i] == sockfd)
        {
            client_pool[i] = 0;
            return 0;
        }
    fprintf(stderr, "No target socket in pool.");
    return -1;

}

void broadcast(struct Message* msg, int size)
{
    for (int i = 0; i < MAX_CLIENT_COUNT; ++i)
    {
        if (client_pool[i] != 0 && client_pool[i] != srv_socket)
        if (send(client_pool[i], (void *)msg, size, 0) == -1 )
        {
            char error_msg[1024];
            sprintf(error_msg, "Fail to broacast message to socket %d", client_pool[i]);
            perror(error_msg);
        }
    }
}


void readMsg(void *_sockfd)
{
    int sockfd = (intptr_t)_sockfd;
    struct Message msg;
    while (1)
    {
        if (recv(sockfd, (void *)&msg, sizeof(msg), 0) == -1)
        {
            printf("Client (Socket %d) halts", sockfd);
            remove_sockfd(sockfd);
        }
        else
        {
            char content[MAX_BUFF_SIZE];
            char user[MAX_BUFF_SIZE];
            char time_s[MAX_BUFF_SIZE];
            get_time(&msg, time_s);
            get_user(&msg, user);
            get_content(&msg, content);
            
            printf("%s%s: %s\n", time_s, user, content);
            if (strncmp(content, "QUIT", 4) == 0)
            {
                if (remove_sockfd(sockfd) == -1)
                {
                    perror("");
                }
                else {
                    printf("User %s (Socket %d) exits\n", user, sockfd);
                }
                pthread_exit(NULL);
            } else {
                broadcast(&msg, sizeof(msg));
                bzero((void *)&msg, sizeof(msg));
            }
        } 
    }
}

int main()
{
    srv_socket = start_server();
    int cli_socket;
    printf("Server start. Server socket: %d\n", srv_socket);

    
    struct sockaddr_in cli_addr;
    socklen_t cli_addr_len = sizeof(cli_addr);

    pthread_t t1;

    while (1)
    {
        // Get a new socket
        cli_socket = accept(srv_socket, (struct sockaddr*)&cli_addr, &cli_addr_len);

        if (cli_socket < 0)
        {
            printf("Server: accept failed\n");
        }
        else 
        {
            printf("New socket arrived: %d\n", cli_socket);
            // Add the socket to the list
            if (add_sockfd(cli_socket) == -1)
            {
                printf("Client pool overflow\n");
                // TODO
            }
            pthread_create(&t1, NULL, (void *)readMsg, (void *)(intptr_t)cli_socket);
        }
    }
}