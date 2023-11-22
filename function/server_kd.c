// Federico Giacomelli, matricola 578016, progetto di Reti Informatiche 2022/23
// FILE: function/server_kd.c

#include "./../header/server_kd.h"
#include "./../header/server_liste.h"
#include "./../header/lib.h"
#include "./../header/utility.h"

//Variabili globali
extern tavolo* lista_tavoli;
extern sala* lista_sale;
extern prenotazione* lista_prenotazioni;
extern int contatore_prenotazioni;
extern int id_progressivo;
extern int codice_interessato;
extern int codice_interessato_servizio;

//Mette in preparazione la prima comanda temporalmente ancora non in quello stato
char* kd_take(){
    
    FILE* fd;
    int f_codice, f_id, f_quante, f_stato, f_numero, f_quanto, i, id;
    int conta_righe = 0, uscita = 0, k=0;
    int* numero = (int*) malloc(MAX_LEN);
    int* quanto = (int*) malloc(MAX_LEN);
    char f_tipo;
    char* buffer_file = (char*) malloc(MAX_LEN);
    char* risposta = (char*) malloc(MAX_LEN);
    char* next_risposta = (char*) malloc(MAX_LEN);
    char* f_data = (char*) malloc(MAX_LEN);
    char* f_tavoli = (char*) malloc(MAX_LEN);
    char* tavoli = (char*) malloc(MAX_LEN);
    char* tipo = (char*) malloc(MAX_LEN);
    char* data_i = (char*) malloc(MAX_LEN);
    char** riscrivi = (char**) malloc(MAX_LEN);

    //Leggo il file e mi salvo riga per riga il contenuto, sovrascrivendo eventuali comande non ancora in preparazione
    fd = fopen("./storage/book_attive.txt", "r");
    while(fgets(buffer_file, MAX_LEN, fd) != NULL){      
        sscanf(buffer_file, "%d %d %d %s %d %c %d %d %s", &f_codice, &f_id, &f_quante, f_tavoli, &f_stato, &f_tipo, &f_numero, &f_quanto, f_data);
        if(f_stato!=0 || uscita==1){
            riscrivi[k] = (char*) malloc(MAX_LEN);
            strcpy(riscrivi[k], buffer_file);
        }

        //Se la comanda non era in prepazione, la riscrivo tale
        if(f_stato == 0 && uscita==0){
            id = f_id;
            strcpy(tavoli, f_tavoli);
            tipo[conta_righe] = f_tipo;
            numero[conta_righe] = f_numero;
            quanto[conta_righe] = f_quanto;
            strcpy(data_i, f_data);
            conta_righe++;
            
            riscrivi[k] = (char*) malloc(MAX_LEN);
            sprintf(riscrivi[k], "%d %d %d %s %d %c %d %d %s\n", f_codice, f_id, f_quante, f_tavoli, 1, f_tipo, f_numero, f_quanto, f_data);
            codice_interessato = f_codice;
            if(conta_righe == f_quante)
                uscita=1;
        }
        k++;
    }
    fclose(fd);

    //Se almeno una comanda l'ho mandata in preparazione, riscrivo il file
    if(conta_righe!=0){
        sprintf(risposta, "com%d %s\n", id, tavoli);
        fd = fopen("./storage/book_attive.txt", "w");
        for(i=0;i<k;i++){
            fprintf(fd, "%s", riscrivi[i]);
            sprintf(next_risposta, "%s%c%d %d\n", risposta,tipo[i], numero[i], quanto[i]);
            memcpy(risposta, next_risposta, MAX_LEN);
        }
        fclose(fd);
    }
    else
        sprintf(risposta, "Nessuna comanda da mandare in preparazione\n");

    if(conta_righe == 0)
        codice_interessato = 0;
    return risposta;
}

//Mostra le sole comande in preparazione
char* kd_show(){
    FILE* fd;

    int f_codice, f_id, f_quante, f_stato, f_numero, f_quanto;
    int inserito = 0, k=0, old_codice=0, old_id=0;
    char f_tipo;
    char* buffer_file = (char*) malloc(MAX_LEN);
    char* risposta = (char*) malloc(MAX_LEN);
    char* next_risposta = (char*) malloc(MAX_LEN);
    char* f_data = (char*) malloc(MAX_LEN);
    char* f_tavoli = (char*) malloc(MAX_LEN);

    //Scorro tutto il file e mi salvo le comande in preparazione
    fd = fopen("./storage/book_attive.txt", "r");
    while(fgets(buffer_file, MAX_LEN, fd) != NULL){      
        sscanf(buffer_file, "%d %d %d %s %d %c %d %d %s", &f_codice, &f_id, &f_quante, f_tavoli, &f_stato, &f_tipo, &f_numero, &f_quanto, f_data);
        if(f_stato==1){
            if(old_codice != f_codice || old_id != f_id)
                inserito=0;
            old_codice = f_codice;
            old_id = f_id;
            if(inserito == 0){
                sprintf(next_risposta, "%scom%d %s\n", risposta, f_id, f_tavoli);
                memcpy(risposta, next_risposta, MAX_LEN);
                inserito++;
            }
            sprintf(next_risposta, "%s%c%d %d\n", risposta, f_tipo, f_numero, f_quanto);
            memcpy(risposta, next_risposta, MAX_LEN);
            k++;
        }
    }
    fclose(fd);

    if(k==0)
        sprintf(risposta, "Nessuna comanda in preparazione\n");
    
    return risposta;
}

//Mette in servizio la comanda specificata
int kd_ready(char* input){

    FILE* fd;
    int f_codice, f_id, f_quante, f_stato, f_numero, f_quanto, num_comanda, i;
    int k=0, trovato=0;
    char f_tipo;
    char* buffer_file = (char*) malloc(MAX_LEN);
    char* f_data = (char*) malloc(MAX_LEN);
    char* f_tavoli = (char*) malloc(MAX_LEN);
    char* comando = (char*) malloc(MAX_LEN);
    char* tavoli = (char*) malloc(MAX_LEN);
    char** riscrivi = (char**) malloc(MAX_LEN);

    //Identifico la comanda
    comando = strtok(input, " ");
    while(comando != NULL){
        if(comando[0]=='c'){
            memmove(&comando[0], &comando[0 + 1], strlen(comando) - 0);
            memmove(&comando[0], &comando[0 + 1], strlen(comando) - 0);
            memmove(&comando[0], &comando[0 + 1], strlen(comando) - 0);
            num_comanda = comando[0] - '0';

            memmove(&comando[0], &comando[0 + 1], strlen(comando) - 0);
            memmove(&comando[0], &comando[0 + 1], strlen(comando) - 0);
            strcpy(tavoli, comando);
            break;
        }
        comando = strtok(NULL, " ");
    }

    //Scorro il file e qualora trovassi la comanda in questione, la metto in servizio
    fd = fopen("./storage/book_attive.txt", "r");
    while(fgets(buffer_file, MAX_LEN, fd) != NULL){      
        sscanf(buffer_file, "%d %d %d %s %d %c %d %d %s", &f_codice, &f_id, &f_quante, f_tavoli, &f_stato, &f_tipo, &f_numero, &f_quanto, f_data);
        riscrivi[k] = (char*) malloc(MAX_LEN);

        if(f_id == num_comanda && strcmp(tavoli, f_tavoli)==0 && (f_stato==1)){
            sprintf(riscrivi[k], "%d %d %d %s %d %c %d %d %s\n", f_codice, f_id, f_quante, f_tavoli, 2, f_tipo, f_numero, f_quanto, f_data);
            trovato++;
            codice_interessato_servizio = f_codice;
        }
        else
            strcpy(riscrivi[k], buffer_file);
        k++;
    }
    fclose(fd);

    //Se almeno ho messo una comdanda in servizio, sovrascrivo opportunamente il file
    if(trovato!=0){
        fd = fopen("./storage/book_attive.txt", "w");
        for(i=0;i<k;i++)
            fprintf(fd, "%s", riscrivi[i]);
        fclose(fd);
        return 0;
    }
    codice_interessato_servizio = 0;
    return 1;
}