#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <error.h>
#include "message.h"

#define PORT 8000
#define MAX_BUFF_SIZE 1024

char username[MAX_BUFF_SIZE];
int cli_socket;

int startup();
void talk(int sockfd);
void recv_msg(int sockfd);
void client_exit();
void INThandler(int);


int startup()
{
    int cli_socket;
    if ( (cli_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Client: socket creation failed");
        exit(-1);
    }

    struct sockaddr_in srv_addr;
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(PORT);
    srv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    socklen_t srv_addr_len = sizeof(srv_addr);

    // Connect to server
    if (connect(cli_socket, (struct sockaddr*)&srv_addr, srv_addr_len) != 0)
    {
        perror("Client: Connect to server failed");
        exit(-1);
    }
    else
        printf("Client: Successfully connect to server\n");

    return cli_socket;
}

void recv_msg(int sockfd)
{
    struct Message msg;
    while (1)
    {
        if (recv(sockfd, (void *)&msg, sizeof(msg), 0) == -1)
        {
            perror("Server halts");
            exit(-1);
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
            fflush(stdout);
        }
    }
}

void talk(int sockfd)
{
    char buff[MAX_BUFF_SIZE];
    int n;
    while (1)
    {
        bzero(buff, sizeof(buff));
        n = 0;
        while ((buff[n++] = getchar()) != '\n')
            ;
        buff[--n] = 0; // Remove the newline character
        printf("Send message: %s", buff);
        
        struct Message* msg = Message_new(username, buff);
        
        send(sockfd, (void *) msg, sizeof(*msg), 0);
        if (strncmp(buff, "QUIT", 4) == 0)
            client_exit();
    }
}

void client_exit()
{
    printf("\nClient exit\n");
    struct Message msg;
    set_user(&msg, username);
    set_content(&msg, "QUIT");
    send(cli_socket, (void *)&msg, sizeof(msg), 0);
    close(cli_socket);
    exit(0);
}

void INThandler(int sig)
{
    signal(sig, SIG_IGN);
    client_exit(cli_socket);
    // getchar(); // Get new line character
}

int main(int argc, char *argv[])
{
    if (argc > 1)
        strcpy(username, argv[1]);
    else
        strcpy(username, "Anonymous");
    
    cli_socket = startup();
    printf("%s login.\n", username);
    
    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = INThandler;
    sa.sa_flags = 0; 
    sigaction(SIGINT, &sa, NULL);


    pthread_t talk_thread_id, recv_thread_id;
    
    if (pthread_create(&talk_thread_id, NULL, 
            (void *)talk, 
            (void *)(intptr_t)cli_socket) != 0)
        perror("Client: talk pthread_create");

    if (pthread_create(&recv_thread_id, NULL,
            (void *)recv_msg,
            (void *)(intptr_t)cli_socket) != 0)
        perror("Client: recv thread_create");
    
    while(1);

    return 0;
}