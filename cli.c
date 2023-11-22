// Federico Giacomelli, matricola 578016, progetto di Reti Informatiche 2022/23
// FILE: cli.h

#include "header/lib.h"
#include "header/flow.h"

// Info da stampare all'avvio del client
void begin_msg(){
    printf("***************************** BENVENUTO *****************************\n");
    printf("Digita un comando:\n");
    printf("find --> ricerca la disponibilità per una prenotazione\n");
    printf("book --> invia una prenotazione\n");
    printf("esc --> termina il client\n\n");
    fflush(stdout);
}

// Funzione di utilità: verifica se le info temporale della prenotazione sono fattibili
int check_data_ora(int giorno, int mese, int anno, int ora){
    time_t rawtime;
    struct tm* timeinfo;
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
    yy %= 100;

    //Vedo intanto se il mese e l'anno tornano
    if(mese < mo || anno < yy)
        return 1;
    //Quindi controllo se, nel mese corretto, il giorno è valido
    if(giorno < dd)
        return 1;

    //Controllo data - FUTURA
    int bisestile = anno%4;
    if(mese > 12)
        return 3;
    switch (mese){
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
            if(giorno > 31)
                return 3;
            break;
        case 4:
        case 6:
        case 9:
        case 11:
            if(giorno > 30)
                return 3;
            break;
        default:
            if (bisestile == 0 && giorno > 29)
                return 3;
            else if (giorno > 28)
                return 3;
            break;
    }

    //Controllo ora - OGGI
    if(giorno == dd && mese == mo && anno == yy){
        if(ora < hh)
            return 2;
    }

    // Controllo ora - GENERALE
    if(ora < 11)
        return 2;
    if(ora > 14 && ora < 19)
        return 2;
    if(ora > 22)
        return 2;

    // Tutto OK
    return 0;
}

int main(int argc, char* argv[]){
    
    // Variabili per la gestione del socket
    fd_set master, read_fds; 
    struct sockaddr_in cl_addr, sv_addr;
    in_port_t porta = htons(atoi(argv[1]));
    int ret, sd, len, fdmax;

    int n_persone,dd,mo,yy,hh,mm,n_opzione;
    char* cognome;
    char* comando = (char*) malloc(MAX_LEN);
    char* buffer = (char*) malloc(MAX_LEN);
    char* input = (char*) malloc(MAX_LEN);
    struct recv_str* risposta;
    uint32_t mex_len;                               
    uint32_t mex_len_n;
    uint16_t lmsg;

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
    ret = send(sd, "cl", 3, 0);
    error_detection(ret, sd);
    ret = recv(sd, buffer, 4, 0);
    if(strcmp(buffer, "NO")==0){
        printf("AVVISO: ci sono già troppi cli attivi.\nARRESTO IN CORSO...\n");
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

            // FIND - l'utente cerca slot liberi
            if(strcmp(comando,"find")==0){
                cognome = (char*) malloc(MAX_LEN);
                sscanf(buffer, "%s %s %d %d-%d-%d %d", comando, cognome, &n_persone, &dd, &mo, &yy, &hh);

                //Prima di inviare il messaggio, controllo se il timestamp sia valido
                if(check_data_ora(dd,mo,yy,hh)==1){
                    printf("Data errata: inserire una data almeno da oggi in poi!\n");
                    fflush(stdout);
                    continue;
                }
                else if(check_data_ora(dd,mo,yy,hh)==2){
                    printf("Ora errata: orari ammessi 11-14 e 19-22!\n");
                    fflush(stdout);
                    continue;
                }
                else if(check_data_ora(dd,mo,yy,hh)==3){
                    printf("Data errata: non esiste nel calendario!\n");
                    fflush(stdout);
                    continue;
                }
                buffer[strcspn(buffer, "\n")] = 0;
                send_msg(sd, buffer);
            }

            // BOOK - l'utente effettua una prenotazione
            else if(strcmp(comando,"book")==0){
                buffer[strcspn(buffer, "\n")] = 0;
                sprintf(buffer, "%s -%s", buffer, cognome);
                send_msg(sd, buffer);
            }
            
            // ESC - l'utente esce e si chiude il server
            else if(strcmp(comando,"esc")==0){
                printf("CLIENT - Arresto in corso...\n");
                fflush(stdout);
                close(sd);
                exit(0);
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
