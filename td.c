// Federico Giacomelli, matricola 578016, progetto di Reti Informatiche 2022/23
// FILE: td.c

#include "header/lib.h"
#include "header/flow.h"
 
// Info da stampare all'avvio del client
void begin_msg(){
    printf("***************************** BENVENUTO *****************************\n");
    printf("Digita un comando:\n\n");
    printf("1) help            --> mostra i dettagli dei comandi\n");
    printf("2) menu            --> mostra il menu dei piatti\n");
    printf("3) comanda         --> invia una comanda\n");
    printf("4) conto           --> chiede il conto\n\n");
    fflush(stdout);
}

int main(int argc, char* argv[]){

    // Variabili per la gestione del socket
    int ret, sd, len, fdmax;
    struct sockaddr_in cl_addr, sv_addr;
    in_port_t porta = htons(atoi(argv[1]));
    fd_set master, read_fds; 
    uint32_t mex_len;                               
    uint32_t mex_len_n;

    int n_opzione, go;
    struct recv_str* risposta;
    char* cod;
    char* comando = (char*) malloc(MAX_LEN);
    char* input = (char*) malloc(MAX_LEN);
    char* buffer = (char*) malloc(MAX_LEN);
    char* identificativo = (char*) malloc(MAX_LEN);

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
    ret = send(sd, "td", 3, 0);
    error_detection(ret, sd);
    ret = recv(sd, buffer, 4, 0);
    if(strcmp(buffer, "NO")==0){
        printf("AVVISO: ci sono già troppi td attivi.\nARRESTO IN CORSO...\n");
        close(sd);
        exit(1);
    }
    strcpy(identificativo, buffer);
    
    // Mi assicuro di inserire un codice di prenotazione esistente
    cod = (char*) malloc(MAX_LEN);
    while (1){
        printf("CODICE PRENOTAZIONE: ");
        memset(input, 0, sizeof(input));
        strcat(input, "codice_tavolo");
        fgets(buffer, MAX_LEN, stdin);              
        sprintf(comando, "%s %s %s", input, buffer, identificativo);
        
        buffer[strcspn(buffer, "\n")] = 0;
        sprintf(cod, "X%s", buffer);
        //Invio il codice inserito
        send_msg(sd, comando);
        mex_len = strlen(comando) + 1;          
        mex_len_n = htonl(mex_len);
        //Controllo l'esito, se positivo, esci
        ret = recv(sd, &mex_len_n, sizeof(uint32_t), 0);  
        error_detection(ret, sd);                    
        mex_len = ntohl(mex_len_n);  
        ret = recv(sd, buffer, mex_len, 0);
        error_detection(ret, sd);
        if(strcmp("OK", buffer)==0)
            break;
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
            risposta = recv_msg(sd);
            ret = risposta->ret;
            buffer = risposta->msg;

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

            // HELP - mostro i dettagli dei comandi
            if(strcmp(comando,"help")==0){
                printf(") menu - mostra il menu, cioè l’abbinamento fra codici, nomi dei piatti e relativi prezzi.\nSintassi: menu\n");
                printf(") comanda - invia una comanda alla cucina.\nSintassi: comanda <piatto_1-quantità_1> ... <piatto_n-quantità_n>\n\n");
                printf(") conto - invia al server la richiesta di conto.\nSintassi: conto\n");
                fflush(stdout);
            }

            // MENU - chiedo al server il menu
            else if(strcmp(comando,"menu")==0){
                send_msg(sd, buffer);
            }
            // COMANDA e CONTO - invio al server la comanda o richiedo il conto
            else if(strcmp(comando,"comanda")==0 || strcmp(comando,"conto")==0){
                buffer[strcspn(buffer, "\n")] = 0;
                sprintf(buffer, "%s %s", buffer, cod);
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