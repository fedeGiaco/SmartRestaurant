// Federico Giacomelli, matricola 578016, progetto di Reti Informatiche 2022/23
// FILE: server.c

#include "header/lib.h"
#include "header/utility.h"
#include "header/flow.h"
#include "header/server_td.h"
#include "header/server_cli.h"
#include "header/server_liste.h"
#include "header/server_kd.h"
#include "header/server_debug.h"
#include "header/server_utility.h"

extern tavolo* lista_tavoli;
extern tavolo* lista_tavoli;
extern sala* lista_sale;
extern prenotazione* lista_prenotazioni;
extern menu* lista_menu;
extern prenotazione_servire* lista_prenotazioni_attive;
extern int contatore_prenotazioni_attive;
extern int contatore_prenotazioni;
extern int id_progressivo;
extern int codice_interessato;
extern int codice_interessato_servizio;

int main(int argc, char* argv[]){

    fd_set master, read_fds;
    struct sockaddr_in sv_addr, cl_addr;
    in_port_t porta = htons(atoi(argv[1]));
    uint32_t len;
    uint32_t mex_len;
    uint32_t mex_len_n;

    int ret, sd_server, new_sd, fdmax;
    int i,j, td_interessato;
    int* kd_connessi = (int*) malloc(MAX_LEN);
    int* cli_connessi = (int*) malloc(MAX_LEN);
    int* td_connessi = (int*) malloc(MAX_LEN);
    int* td_interessati = (int*) malloc(MAX_LEN);
    int* stato_connessioni = (int*) malloc(MAX_LEN);
    
    char* buffer;
    char* comando;
    char* input;
    char* msg_log = (char*) malloc(MAX_LEN);
    menu* l_menu;

    // Messaggio di benvenuto
    begin_msg();

    sd_server = socket(AF_INET, SOCK_STREAM, 0);
    
    // Socket di ascolto
    memset(&sv_addr, 0, sizeof(sv_addr));
    memset(&cl_addr, 0, sizeof(cl_addr));
    sv_addr.sin_family = AF_INET;
    sv_addr.sin_port = porta;
    sv_addr.sin_addr.s_addr = INADDR_ANY;

    // Aggancio del socket all'indirizzo
    ret = bind(sd_server, (struct sockaddr*)&sv_addr, sizeof(sv_addr));
    if(ret < 0){
        perror("Errore in fase di bind: \n");
        printf("ARRESTO IN CORSO...\n");
        fflush(stdout);
        exit(1);
    }
    ret = listen(sd_server, 10);
    if (ret == -1){
        perror("ERRORE nella listen()");
        printf("ARRESTO IN CORSO...\n");
        fflush(stdout);
        exit(1);
    }
    write_log(0, "Server operativo");
    
    // Creazione set
    FD_ZERO(&master);
    FD_ZERO(&read_fds);
    FD_SET(sd_server, &master);
    FD_SET(0, &master);
    fdmax = sd_server;

    //Preparativi
    td_prepara_menu();
    cli_prepara_tavoli();
    
    while (1){
        comando = (char*) malloc(MAX_LEN);
        buffer = (char*) malloc(MAX_LEN);
        j=0;

        read_fds = master;
        ret = select(fdmax+1, &read_fds, NULL, NULL, NULL);
        if(ret < 0){
            perror("ERROR SELECT");
            exit(1);
        }

        for(i=0;i<=fdmax;i++){
            if(FD_ISSET(i, &read_fds)){
                //Input da terminale
                if(i == 0){
                    fgets(buffer, MAX_LEN, stdin);            
                    sscanf(buffer, "%s", comando);
                    //Inserimento comando 'stat'
                    if(strcmp(comando, "stat")==0){ 
                        buffer[strcspn(buffer, "\n")] = 0;
                        input = strtok(buffer, "  ");
                        input = strtok(NULL, " ");
                        if(input == NULL)
                            stampa_tutto(input, 0);
                        else if(input[0]=='T')
                            stampa_tutto(input, 1);
                        else if(input[0]=='a' || input[0]=='p' || input[0]=='s')
                            stampa_tutto(input, 2);
                        else{
                            printf("Comando non consetito!\n");
                            fflush(stdout);
                        }
                    }
                    //Inserimento comando 'stop'
                    else if(strcmp(comando, "stop")==0){
                        printf("SERVER - Arresto in corso...\n");
                        fflush(stdout);
                        write_log(4, "quit");
                        write_log(3, "Server spento\n");

                        //Avviso tutti i collegati
                        strcpy(comando, "quit");
                        for(j=0;j<stato_connessioni[0];j++)
                            send_msg(td_connessi[j], comando);
                        
                        for(j=0;j<stato_connessioni[1];j++)
                            send_msg(kd_connessi[j], comando);
                        
                        for(j=0;j<stato_connessioni[2];j++)
                            send_msg(cli_connessi[j], comando);
                        
                        close(sd_server);
                        exit(0);
                    }
                    //Inserimento comando non riconosciuto
                    else{
                        printf("Comando non consentito\n");
                        fflush(stdout);
                    }
                }

                // Socket di ascolto
                else if (i == sd_server){
                    len = sizeof(cl_addr);
                    new_sd = accept(sd_server, (struct sockaddr*) &cl_addr, &len);
                    if (new_sd == -1){
                        perror("ERRORE nell'accept()");
                        continue;
                    }
                    //Recupero la provenenienza della connessione
                    ret = recv(new_sd, buffer, 3, 0);
                    if (ret == 0 || ret == -1){
                        printf("ERRORE nell'identificazione di un nuovo socket remoto.\n");
                        fflush(stdout);
                        close(new_sd); 
                        continue;
                    }
                    //Controllo se ho ancora spazio per aprire la connessione opportuna
                    if(stato_connessioni[0] == MAX_TD || stato_connessioni[1] == MAX_KD || stato_connessioni[2] == MAX_CLI){
                        ret = send(new_sd, "NO", 3, 0);
                        error_detection(ret, i);
                    }
                    else{
                        //Aggiorno i contatori e scrivo su log/server
                        if(strcmp(buffer, "td")==0){
                            td_connessi[stato_connessioni[0]] = new_sd;
                            stato_connessioni[0]++;
                            sprintf(msg_log, "td%d", stato_connessioni[0]);
                            ret = send(new_sd, msg_log, 4, 0);
                            error_detection(ret, new_sd);
                            write_log(0, msg_log);
                        }
                        else if(strcmp(buffer, "kd")==0){
                            kd_connessi[stato_connessioni[1]] = new_sd;
                            stato_connessioni[1]++;
                            sprintf(msg_log, "kd%d", stato_connessioni[1]);
                            ret = send(new_sd, msg_log, 4, 0);
                            error_detection(ret, new_sd);
                            write_log(0, msg_log);
                        }
                        else if(strcmp(buffer, "cl")==0){
                            cli_connessi[stato_connessioni[2]] = new_sd;
                            stato_connessioni[2]++;
                            sprintf(msg_log, "cl%d", stato_connessioni[2]);
                            ret = send(new_sd, msg_log, 4, 0);
                            error_detection(ret, new_sd);
                            write_log(0, msg_log);               
                        }
                    }
                    
                    FD_SET(new_sd, &master);
                    if(new_sd > fdmax)
                        fdmax = new_sd;
                }

                // Socket di comunicazione
                else{                   
                    ret = recv(i, &mex_len_n, sizeof(uint32_t), 0);
                    mex_len = ntohl(mex_len_n);
                    ret = recv(i, buffer, mex_len, 0);
                    sscanf(buffer, "%s", comando);
                    //Controllo se un client si è autonomamente disconnesso
                    if(ret == 0){
                        close(i);
                        FD_CLR(i, &master);
                        write_log(3, "Client disconnesso");
                    }
                    else if(ret < 0){
                        perror("ERRORE! \n");
                        close(i);
                        FD_CLR(i, &master);
                    }
                    else{
                        // CLIENT - FIND - Cerca una slot per effettuare una prenotazione
                        if(strcmp("find", comando)==0){
                            comando = cli_cerca_tavoli(buffer);
                            send_msg(i, comando);
                            write_log(1, "Tavoli per prenotazione");
                        }
                        // CLIENT - BOOK - Tenta di confermare una prenotazione scelta fra quelle disponibili
                        else if(strcmp("book", comando)==0){
                            strcpy(comando, buffer);
                            input = strtok(buffer, " ");
                            while(input != NULL){
                                if(input[0] == '-'){
                                    memmove(&input[0], &input[0 + 1], strlen(input) - 0);
                                    break;
                                }
                                input = strtok(NULL, " ");
                            }
                            comando = cli_effettua_prenotazione(comando, input);
                            send_msg(i, comando);
                        }
                        // TD - CODICE - Autentico il tavolo
                        else if(strcmp("codice_tavolo", comando)==0){
                            j=0;
                            sscanf(buffer, "%s %d td%d", comando, &j, &td_interessato);
                            //Traccio i td connessi con questo codice
                            td_interessato--;
                            td_interessati[td_interessato] = j;
                            ret = td_cerca_prenotazione(j);
                            if(ret == 0)
                                sprintf(comando, "OK");
                            else
                                sprintf(comando, "NO");

                            send_msg(i, comando);
                            write_log(1, "Check prenotazione esistente");
                        }
                        // TD - MENU - Invio il menu
                        else if(strcmp("menu", comando)==0){
                            l_menu = lista_menu;
                            while (l_menu != NULL){
                                sprintf(buffer, "%s - %s    %d\n", l_menu->portata, l_menu->piatto, l_menu->costo);
                                send_msg(i, buffer);
                                l_menu = l_menu->next;
                            }
                            write_log(1, "Menù");
                        }
                        // TD - COMANDA - Ricevo la comanda
                        else if(strcmp("comanda", comando)==0){
                            input = strtok(buffer, " ");
                            comando = td_gestione_comanda(input);
                            send_msg(i, comando);

                            //Avviso tutti i kitchen device connessi
                            sprintf(comando, "*");
                            mex_len = strlen(comando) + 1;
                            mex_len_n = htonl(mex_len);
                            
                            for (j=0; j<stato_connessioni[1]; j++){
                                ret = send(kd_connessi[j], &mex_len_n, sizeof(uint32_t), 0);
                                error_detection(ret, kd_connessi[j]);
                                ret = send(kd_connessi[j], comando, mex_len, 0);
                                error_detection(ret, kd_connessi[j]);
                            }
                            write_log(1, "Avviso i kitchen device");
							
                        }
                        // TD - COMANDA - Calcolo il conto
                        else if(strcmp("conto", comando)==0){
                            input = strtok(buffer, " ");
                            comando = td_gestione_conto(input);
                            send_msg(i, comando);
                            write_log(1, "Conto");
                        }
                        // KD - CUCINA - Accetta la comanda nello stato di attesa da più tempo
                        else if(strcmp("take", comando)==0){
                            comando = kd_take();
                            send_msg(i, comando);
                            //Avviso i td che hanno effettuato la comanda
                            if(codice_interessato != 0){
                                for(j=0; j<stato_connessioni[0]; j++){
                                    if(codice_interessato == td_interessati[j])
                                        send_msg(td_connessi[j], "Comanda in preparazione!\n");
                                }
                                write_log(1, "Accettazione comanda");
                            }
                        }
                        // KD - CUCINA - Mostra le comande in attesa
                        else if(strcmp("show", comando)==0){
                            comando = kd_show();
                            send_msg(i, comando);
                            write_log(1, "Stampa comande in attesa");
                        }
                        // KD - CUCINA - Setta le comande in preparazione
                        else if(strcmp("ready", comando)==0){
                            ret = kd_ready(buffer);
                            if(ret == 0){
                                strcpy(comando, "Comanda in servizio!\n");
                                for(j=0; j<stato_connessioni[0]; j++){
                                    if(codice_interessato_servizio == td_interessati[j])
                                        send_msg(td_connessi[j], comando);
                                }
                            }
                            else
                                strcpy(comando, "Comanda non valida\n");

                            send_msg(i, comando);
                            write_log(1, "Inserisco comande in preparazione");
                        }
                    }
                }
            }
        }
    }
    //Chiudo il server, scrivendo preventivamente su log/server
    printf("Server spento!\n");
    fflush(stdout);
    write_log(3, "Server spento");
    close(sd_server);
}
