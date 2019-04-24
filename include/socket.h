#ifndef SOCKET_H
#define SOCKET_H

#include     "../include/data.h"


char *MakeJsonBody(AppStruct AppData);
void httPost(char *postdata);


#endif
