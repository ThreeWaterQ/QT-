#ifndef COMMON_H
#define COMMON_H
#include <stdio.h>
#include <string.h>
struct MSG_DT
{
    char buf[1022];
    short type;
};

struct register_node
{
    register_node(const char *name,const char *password){
        strcpy(this->name,name);
        strcpy(this->password,password);
    }
    char name[128];
    char password[128];
};
#endif // COMMON_H
