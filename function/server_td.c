// Federico Giacomelli, matricola 578016, progetto di Reti Informatiche 2022/23
// FILE: function/server_td.c

#include "./../header/server_td.h"
#include "./../header/server_liste.h"
#include "./../header/lib.h"
#include "./../header/utility.h"

//Variabili globali
extern prenotazione* lista_prenotazioni;
extern menu* lista_menu;
extern prenotazione_servire* lista_prenotazioni_attive;

//Verifica l'esistanza di una data prenotazione
int td_cerca_prenotazione(int codice_dato){
    FILE* fd;
    int codice, tavolo_prenotato, n_persone_x, dd_x, mo_x, yy_x, hh_x;
    int k=0; 
    int* tavoli_x = (int*)malloc(TAVOLI_TOTALI * sizeof(int));
    char* cognome_x = (char*) malloc(MAX_LEN);
    char* buffer_file = (char*) malloc(MAX_LEN);
    fd = fopen("./storage/book.txt", "r");
    while(fgets(buffer_file, MAX_LEN, fd) != NULL){      
        sscanf(buffer_file, "%d-%d-%d %d %s %d %d %d", &dd_x, &mo_x, &yy_x, &hh_x, cognome_x,  &n_persone_x, &tavolo_prenotato, &codice);
        if(codice == codice_dato){
            tavoli_x[k] = tavolo_prenotato;
            k++;    
        }
    } 
    fclose(fd);

    if(k==0)    //La prenotazione non esiste
        return 1;
    else{
        inserisci_prenotazione_attiva(&lista_prenotazioni_attive, codice_dato, k, tavoli_x);
        return 0;
    }
}

//Legge il menù e inserisce le portate nella lista
void td_prepara_menu(){
    FILE* fd;
    int costo;
    int i=0;
    char* portata = (char*) malloc(DIM_PORTATA);
    char* piatto = (char*) malloc(DIM_PIATTO);
    char* buffer_file = (char*) malloc(MAX_LEN);
    //Scorre il file del menù e legge tutte le portate
    fd = fopen("./storage/menu.txt", "r");
    while(fgets(buffer_file, MAX_LEN, fd) != NULL){      
        sscanf(buffer_file, "%s - %s  %d", portata, piatto, &costo);
        for(i=0;i<DIM_PIATTO;i++){
            if(piatto[i] == '_')
                piatto[i] = ' ';
        }
        inserisci_menu(&lista_menu, i, portata, piatto, costo);
    }
    fclose(fd);
}

//Scorre il menù fintanto che trova la portata chiesta, quindi restituisce il costo
int td_recupera_prezzo(char tipo, int numero){
    menu* scorri = lista_menu;
    char* portata_chiesta = (char*) malloc(GOOD_LEN);
    sprintf(portata_chiesta, "%c%d", tipo, numero);
    while (scorri != NULL){
        if(strcmp(scorri->portata, portata_chiesta)==0)
            return scorri->costo;
        scorri = scorri->next;
    }
    return 1;
}

//Scorre il menù per verificare se il dato codice esiste o meno
int td_comanda_esistente(char tipo, int numero){
    menu* scorri = lista_menu;
    char* portata_chiesta = (char*) malloc(GOOD_LEN);
    sprintf(portata_chiesta, "%c%d", tipo, numero);
    while (scorri != NULL){
        if(strcmp(scorri->portata, portata_chiesta)==0)
            return 0;
        scorri = scorri->next;
    }
    return 1;
}

//Recupera il codice specifico della portata
int td_recuperaPrenotazione(char* codice_dato){
    memmove(&codice_dato[0], &codice_dato[0+1], strlen(codice_dato) - 0);
    return atoi(codice_dato);
}

//Una volta identificata la comanda, restituisce il messaggio dell'esito
char* td_gestione_comanda(char* input){

    int ret,j;
    int k=0,z=0,x=1;
    int* numero_x = (int*) malloc(GOOD_LEN);
    int* quanto_x = (int*) malloc(GOOD_LEN);
    char* risposta = (char*) malloc(MAX_LEN);    
    char* tipo = (char*) malloc(DIM_PIATTO);
    char* numero = (char*) malloc(DIM_PIATTO);
    char* tipo_x = (char*) malloc(GOOD_LEN);
    //Per ogni portata nella comanda, discrimino ciascuna di queste
    while(input != NULL){
        if(input[0] == 'X')
            x = td_recuperaPrenotazione(input);
        
        else if(input[0] == 'A' || input[0] == 'P' || input[0] == 'S' || input[0] == 'D'){
            strcpy(tipo, input);

            memmove(&tipo[2], &tipo[2+1], strlen(tipo) - 2);
            memmove(&tipo[2], &tipo[2+1], strlen(tipo) - 2);
            
            strcpy(numero, tipo);
            memmove(&tipo[1], &tipo[1+1], strlen(tipo) - 1);
            tipo_x[k] = tipo[0];
            
            memmove(&numero[0], &numero[0+1], strlen(numero) - 0);
            numero_x[k] = numero[0]-'0';

            memmove(&input[0], &input[0 + 1], strlen(input) - 0);
            memmove(&input[0], &input[0 + 1], strlen(input) - 0);
            memmove(&input[0], &input[0 + 1], strlen(input) - 0);
            quanto_x[k] = input[0]-'0';
            if(quanto_x[k] < 0)
                quanto_x[k] = 0;

            k++;
        }
        input = strtok(NULL, " ");
    }
    if(k!=0){
        for(j=0;j<k;j++){
            ret = td_comanda_esistente(tipo_x[j], numero_x[j]);
            z+=ret;
        }
        if(z == 1)
            sprintf(risposta, "COMANDA ERRATA: una o più portate non sono nel menù!\n");
        else{
            inserisci_comanda(&lista_prenotazioni_attive, x, tipo_x, numero_x, quanto_x, k);
            sprintf(risposta, "Comanda ricevuta\n");
        }
    }
    else
        sprintf(risposta, "COMANDA ERRATA: sintassi errata!\n");
    return risposta;
}

//Non posso chiedere il conto se tutte le portate non state mandate in servizio, qui lo controllo
bool td_fine_pasto(int codice){
    FILE* fd;
    int f_codice, f_id, f_quante, f_stato, f_numero, f_quanto;
    int k=0, conta_righe = 0;
    char f_tipo;
    char* buffer_file = (char*) malloc(MAX_LEN);
    char* f_data = (char*) malloc(MAX_LEN);
    char* f_tavoli = (char*) malloc(MAX_LEN);
    //Scorro tutte le comande effettuate e controllo che TUTTE siano effettivamente in servizio
    fd = fopen("./storage/book_attive.txt", "r");
    while(fgets(buffer_file, MAX_LEN, fd) != NULL){      
        sscanf(buffer_file, "%d %d %d %s %d %c %d %d %s", &f_codice, &f_id, &f_quante, f_tavoli, &f_stato, &f_tipo, &f_numero, &f_quanto, f_data);
        if(f_codice == codice){
            if(f_stato == 2)
                k++;
            conta_righe++;
        }
    }
    fclose(fd);
    if(conta_righe == k)
        return true;
    else
        return false;
}

//Calcolo (e restitusco) il conto previa verifica dell'effettiva terminazione del pasto
char* td_gestione_conto(char* input){
    FILE* fd;
    int codice, codice_x, id_x, quante_x, stato_x, numero_x, quanto_x;
    int z,j=0,totale=0,parziale=0;
    char tipo_in;
    char* risposta = (char*) malloc(MAX_LEN);
    char* next_risposta = (char*) malloc(MAX_LEN);
    char* buffer = (char*) malloc(MAX_LEN);
    char* msg_send = (char*) malloc(MAX_LEN);
    char* lista_tav = (char*) malloc(GOOD_LEN);
    char* data_prenotazione = (char*) malloc(DIM_DATA);
    //Mi recupero la prenotazione di riferimento
    while(input != NULL){
        if(input[0] == 'X'){
            codice = td_recuperaPrenotazione(input);
        }
        input = strtok(NULL, " ");
    }
    //Pasto finito? Se sì, prosegui, altrimenti niente conto
    if(!td_fine_pasto(codice)){
        sprintf(msg_send, "NO CONTO: pasto ancora non terminato\n");
        return msg_send;
    }
    //Scorro tutte le portate ordinate
    fd = fopen("./storage/book_attive.txt", "r");
    memset(risposta, 0, MAX_LEN);
    while(fgets(buffer, MAX_LEN, fd) != NULL){      
        sscanf(buffer, "%d %d %d %s %d %c %d %d %s", &codice_x, &id_x, &quante_x, lista_tav, &stato_x, &tipo_in, &numero_x, &quanto_x, data_prenotazione);
        if(codice_x == codice){
            z = td_recupera_prezzo(tipo_in, numero_x);
            parziale = z*quanto_x;
            totale += parziale;
            sprintf(next_risposta, "%s%c%d %d %d\n", risposta, tipo_in, numero_x, quanto_x, parziale);
            memcpy(risposta, next_risposta, MAX_LEN);
            j++;
        }
    }
    fclose(fd);
    if(j==0)
        sprintf(msg_send, "NO CONTO: nessun ordine ancora effettuato\n");
    else{
        sprintf(msg_send, "%sTotale: %d\n\n", risposta,totale);
    }
    return msg_send;
}
