// Federico Giacomelli, matricola 578016, progetto di Reti Informatiche 2022/23
// FILE: function/flow.c

#include "./../header/lib.h"
#include "./../header/flow.h"

uint32_t mex_len;                               
uint32_t mex_len_n;
uint16_t lmsg;

int ret;

// Ritorna l'orario corrente in una forma leggibile
char* orario(){
    time_t rawtime;
    struct tm* timeinfo;
    char* buffer = (char*) malloc(50);
    int hh, mi, ss, dd, mo, yy;
    
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    hh = timeinfo->tm_hour;
    mi = timeinfo->tm_min;
    ss = timeinfo->tm_sec;
    
    dd = timeinfo->tm_mday;
    mo = timeinfo->tm_mon;
    mo++;
    yy = timeinfo->tm_year;
    yy += 1900;
    sprintf(buffer, "%d:%d:%d,%d-%d-%d", hh, mi, ss, dd, mo, yy);
    return buffer;
}

//Gestione error detection
void error_detection(int ret, int sd){
    if(ret == -1){
        perror("ERRORE nella comunicazione con il server");
        printf("ARRESTO IN CORSO...\n");
        fflush(stdout);
        exit(1);
    }
    else if (ret == 0){
        printf("AVVISO: il server ha chiuso il socket remoto.\nARRESTO IN CORSO...\n");
        fflush(stdout);
        exit(1);
    }
}

//Invio messaggi
void send_msg(int sd, char* msg){
    mex_len = strlen(msg)+1;
    mex_len_n = htonl(mex_len);
    ret = send(sd, &mex_len_n, sizeof(uint32_t), 0);  
    error_detection(ret, sd);
    ret = send(sd, msg, mex_len, 0);
    error_detection(ret, sd);
}

// Ricevo messaggi
recv_str* recv_msg(int sd){
    
    struct recv_str* risposta = (struct recv_str*) malloc (sizeof (struct recv_str));
    risposta->ret = 0;
    risposta->msg = (char*) malloc(MAX_LEN);

    risposta->ret = recv(sd, &mex_len_n, sizeof(uint32_t), 0);  
    error_detection(ret, sd);                        
    mex_len = ntohl(mex_len_n);  
    risposta->ret = recv(sd, risposta->msg, mex_len, 0);
    error_detection(ret, sd);

    return risposta;
}