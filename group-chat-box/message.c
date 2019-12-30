#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "message.h"
// MAX_STR_LEN = 1024;

// struct Message {
//     char user[MAX_STR_LEN];
//     char content[MAX_STR_LEN];
// };

int get_user(struct Message *msg, char *user)
{       
    strcpy(user, msg->user);
    return 0;
}

int get_content(struct Message *msg, char *content)
{
    strcpy(content, msg->content);
    return 0;
}

int set_user(struct Message *msg, const char *user)
{
    if (sizeof(msg->user) < strlen(user))
    {
        fprintf(stderr, "set_user");
        return -1;
    }
    strcpy(msg->user, user);
    return 0;
}

int set_content(struct Message *msg, const char *content)
{
    if (sizeof(msg->content) < strlen(content))
    {
        fprintf(stderr, "set_content");
        return -1;
    }
    strcpy(msg->content, content);
    return 0;
}

int get_time(struct Message *msg, char *time_s)
{
    strcpy(time_s, msg->time);
    return 0;
}

struct Message *Message_new(const char *username, const char *content) {
    struct Message *msg = malloc(sizeof(struct Message));

    struct tm* local;
    time_t t = time(NULL);
    local = localtime(&t);
    strcpy(msg->time, asctime(local));
    
    set_user(msg, username);
    set_content(msg, content);
    return msg;
}