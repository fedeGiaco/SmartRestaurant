// Federico Giacomelli, matricola 578016, progetto di Reti Informatiche 2022/23
// FILE: header/flow.h

#ifndef FLOW_H
#define FLOW_H

#define MAX_LEN 1024

typedef struct recv_str{
    int ret;
    char* msg;
} recv_str;

char* orario();
void error_detection(int ret, int sd);
void send_msg(int sd, char* msg);
recv_str* recv_msg(int sd);

#endif