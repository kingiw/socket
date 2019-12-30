#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#define MAX_STR_LEN 1024
struct Message {
    char user[MAX_STR_LEN];
    char content[MAX_STR_LEN];
    char time[MAX_STR_LEN];
};

int get_user(struct Message *msg, char *user);
int get_content(struct Message *msg, char *content);
int set_user(struct Message *msg, const char *user);
int set_content(struct Message *msg, const char *content);
int get_time(struct Message *msg, char *time_s);
struct Message *Message_new(const char *user, const char *content);

#endif