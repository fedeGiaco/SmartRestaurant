// Federico Giacomelli, matricola 578016, progetto di Reti Informatiche 2022/23
// FILE: kd.c

#include "header/lib.h"
#include "header/flow.h"

// Info da stampare all'avvio del client
void begin_msg(){
    printf("Digita un comando:\n\n");
    printf("1) take            --> accetta una comanda\n");
    printf("2) show            --> mostra le comande accettate (in preparazione)\n");
    printf("3) ready           --> imposta lo stato della comanda\n\n");
    fflush(stdout);
}

int main(int argc, char* argv[]){

    // Variabili per la gestione del socket
    fd_set master, read_fds; 
    int ret, sd, len, fdmax;
    struct sockaddr_in cl_addr, sv_addr;
    in_port_t porta = htons(atoi(argv[1]));
    uint32_t mex_len;                               
    uint32_t mex_len_n;
    uint16_t lmsg;

    FILE* fd;
    int n_opzione, go;
    char* cod;
    struct recv_str* risposta;
    char* comando = (char*) malloc(MAX_LEN);
    char* input = (char*) malloc(MAX_LEN);
    char* buffer = (char*) malloc(MAX_LEN);

    // Pulisco aree di memoria
    memset((void*)&cl_addr, 0, sizeof(cl_addr));
    memset((void*)&sv_addr, 0, sizeof(sv_addr));
    sd = socket(AF_INET, SOCK_STREAM, 0);

    // Lato client
    cl_addr.sin_family = AF_INET;
    cl_addr.sin_port = porta;
    cl_addr.sin_addr.s_addr = INADDR_ANY;
    ret = bind(sd, (struct sockaddr*)&cl_addr, sizeof(cl_addr));
    if(ret == -1){
        perror("Errore bind()\n");
        fflush(stdout);
        exit(1);
    }

    // Lato server
    sv_addr.sin_family = AF_INET;
    sv_addr.sin_port = htons(4242);
    sv_addr.sin_addr.s_addr = INADDR_ANY;
    ret = connect(sd, (struct sockaddr*)&sv_addr, sizeof(sv_addr));
    if(ret == -1){
        perror("Errore connect()\n");
        fflush(stdout);
        exit(1);
    }

    // Benvenuto
    begin_msg();
    
    // Preparativi con il server
    ret = send(sd, "kd", 3, 0);
    error_detection(ret, sd);
    ret = recv(sd, buffer, 4, 0);
    if(strcmp(buffer, "NO")==0){
        printf("AVVISO: ci sono già troppi kd attivi.\nARRESTO IN CORSO...\n");
        close(sd);
        exit(1);
    }

    FD_ZERO(&master);
    FD_ZERO(&read_fds);
    FD_SET(0, &master);
    FD_SET(sd, &master);
    fdmax = sd;

    while (1){
        memset(comando, 0, sizeof(comando));                   
        read_fds = master;
        select(fdmax + 1, &read_fds, NULL, NULL, NULL);

        // RICEVO dal server
        if(FD_ISSET(sd, &read_fds)){    

            ret = recv(sd, &mex_len_n, sizeof(uint32_t), 0);  
            error_detection(ret, sd);                        
            mex_len = ntohl(mex_len_n);  
            ret = recv(sd, buffer, mex_len, 0);
            error_detection(ret, sd);
            
            if(strcmp(buffer, "quit")==0){
                printf("AVVISO: il server ha chiuso il socket remoto.\nARRESTO IN CORSO...\n");
                close(sd);
                exit(1);
            }
            
            printf("%s", buffer);
            fflush(stdout);
        }
        else{
            // Ricevo l'input
            fgets(buffer, MAX_LEN, stdin);              
            sscanf(buffer, "%s", comando);  

            // Invio al server se ho un inserito un comando corretto
            if((strcmp(comando,"take")==0) || (strcmp(comando,"show")==0) || (strcmp(comando,"ready")==0)){
                buffer[strcspn(buffer, "\n")] = 0;
                send_msg(sd, buffer);
            }
            // Altrimenti ha sbagliato a scrivere
            else{
                printf("ERRORE: Comando non valido.\n");
                fflush(stdout);
            }                    
        }
    }
    // Chiudo il client
    close(sd);
}