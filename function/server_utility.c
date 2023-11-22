// Federico Giacomelli, matricola 578016, progetto di Reti Informatiche 2022/23
// FILE: function/server_utility.c

#include "./../header/server_utility.h"
#include "./../header/lib.h"
#include "./../header/utility.h"
#include "./../header/flow.h"

//Messaggio di benvenuto del server
void begin_msg(){
    printf("***************************** SERVER *****************************\n");
    printf("Digita un comando o attendi una richiesta:\n");
    printf("stat table|status --> stato richieste: tutte, per tavolo|per stato\n");
    printf("stop --> termina il server\n\n");
    fflush(stdout);
}

//Scrive sia stardard output che sul file 
void write_log(int tipo, char* msg){
    FILE* fd;
    char* msg_to_write = (char*) malloc(MAX_LEN);

    fd = fopen("./storage/logServer.txt", "a+");
    if(fd == NULL){
        printf("ATTENZIONE, log assente\n");
        fflush(stdout);
        return;
    }

    if(tipo==0)
        sprintf(msg_to_write, "[%s, CONNESSIONE] - %s\n", orario(), msg);
    else if(tipo==1)
        sprintf(msg_to_write, "[%s, INVIO] - %s\n", orario(), msg);
    else if(tipo==2)
        sprintf(msg_to_write, "[%s, RICEVO] - %s\n", orario(), msg);
    else if(tipo==3)
        sprintf(msg_to_write, "[%s, DISCONNESSIONE] - %s\n", orario(), msg);
    else if(tipo==4)
        sprintf(msg_to_write, "[%s, ESEGUO] - %s\n", orario(), msg);
    
    fprintf(fd, "%s", msg_to_write);
    fclose(fd);

    printf("%s", msg_to_write);
    fflush(stdout);
    return;
}

//Converte il tipo numerico dello stato di una comanda nel corrispettivo testuale
char* converti_stato (int tipo){
    switch (tipo){
    case 0:
        return "in attesa";
    case 1:
        return "in preparazione";
    case 2:
        return "in servizio";
    }
    return "not valid";
}

//Converte lo stato nel carattere di riferimento
char converti_stato_to_char (int tipo){
    switch (tipo){
        case 0:
            return 'a';
        case 1:
            return 'p';
        case 2:
            return 's';
    }
    return 'x';
}

//Gestisce l'input del server
void stampa_tutto(char* parametro, int tipo){
    FILE* fd;
    int f_codice, f_id, f_quante, f_stato, f_numero, f_quanto;
    char f_tipo;
    char* buffer_file = (char*) malloc(MAX_LEN);
    char* risposta = (char*) malloc(MAX_LEN);
    char* f_data = (char*) malloc(MAX_LEN);
    char* f_tavoli = (char*) malloc(MAX_LEN);
    char* f_tavoli_1 = (char*) malloc(MAX_LEN);

    char* singolo = (char*) malloc(MAX_LEN);
    char** tavoli_buffer = (char**) malloc(MAX_LEN);
    char* tav = (char*) malloc(MAX_LEN);

    char* next_comanda = (char*) malloc(MAX_LEN);

    if(parametro!=NULL){
        strcpy(tav, parametro);
        memmove(&tav[0], &tav[0+1], strlen(tav) - 0);
    }
    int inserito = 0;
    int z=0,j=0;
    int old_codice=0, old_id=0;

    fd = fopen("./storage/book_attive.txt", "r");
    while(fgets(buffer_file, MAX_LEN, fd) != NULL){
        memset(risposta, 0, MAX_LEN);      
        sscanf(buffer_file, "%d %d %d %s %d %c %d %d %s", &f_codice, &f_id, &f_quante, f_tavoli, &f_stato, &f_tipo, &f_numero, &f_quanto, f_data);
        
        if(!buffer_file){
            sprintf(next_comanda, "Nessuna comanda per ora effettuata\n");
            printf("%s\n",risposta);
            fflush(stdout);
        }

        // Se non filtro
        if(tipo==0){
            if(old_codice != f_codice || old_id != f_id)
                inserito=0;
            old_codice = f_codice;
            old_id = f_id;
            if(inserito == 0){
                sprintf(risposta, "com%d %s <%s>", f_id, f_tavoli, converti_stato(f_stato));
                printf("%s\n",risposta);
                fflush(stdout);
                inserito++;
            }
            memset(risposta, 0, MAX_LEN);
            sprintf(next_comanda, "%c%d %d", f_tipo, f_numero, f_quanto);
            memcpy(risposta, next_comanda, MAX_LEN);
            printf("%s\n",risposta);
            fflush(stdout);
        }
        
        // Se filtro per tavoli
        else if(tipo==1){
            z=0;
            strcpy(f_tavoli_1, f_tavoli);
            singolo = strtok(f_tavoli, "T");
            while(singolo != NULL){
                tavoli_buffer[z] = (char*) malloc(MAX_LEN);
                tavoli_buffer[z] = singolo;
                singolo = strtok(NULL, "T");
                z++;
            }
            for(j=0; j<z; j++){
                if(strcmp(tavoli_buffer[j], tav)==0){
                    if(old_codice != f_codice || old_id != f_id)
                        inserito=0;
                    old_codice = f_codice;
                    old_id = f_id;
                    if(inserito == 0){
                        sprintf(risposta, "com%d %s <%s>", f_id, f_tavoli_1, converti_stato(f_stato));
                        printf("%s\n",risposta);
                        fflush(stdout);
                        inserito++;
                    }
                    memset(risposta, 0, MAX_LEN);
                    sprintf(next_comanda, "%c%d %d", f_tipo, f_numero, f_quanto);
                    memcpy(risposta, next_comanda, MAX_LEN);
                    printf("%s\n",risposta);
                    fflush(stdout);
                }
            }
        }

        // Se filtro per tipo
        else{
            if(parametro[0] == converti_stato_to_char(f_stato)){
                if(old_codice != f_codice || old_id != f_id)
                    inserito=0;
                old_codice = f_codice;
                old_id = f_id;
                if(inserito == 0){
                    sprintf(risposta, "com%d %s", f_id, f_tavoli);
                    printf("%s\n",risposta);
                    inserito++;
                }
                memset(risposta, 0, MAX_LEN);
                sprintf(next_comanda, "%c%d %d", f_tipo, f_numero, f_quanto);
                memcpy(risposta, next_comanda, MAX_LEN);
                printf("%s\n",risposta);
            }
        }
    }
    fclose(fd);
    printf("\n");
    fflush(stdout);
    return;
}