// Federico Giacomelli, matricola 578016, progetto di Reti Informatiche 2022/23
// FILE: function/server_liste.c

#include "./../header/server_liste.h"
#include "./../header/lib.h"
#include "./../header/utility.h"
#include "./../header/flow.h"
 
//Variabili globali
extern int contatore_prenotazioni_attive;
extern prenotazione_servire* lista_prenotazioni_attive;
extern int id_progressivo;

//Sono tutte funzioni autoesplicative: inseriscono i dati nelle liste di riferimento
void inserisci_tavolo(struct tavolo **testa, int id, int posti, int sala, int disponibile, int accorpabile, char* ubicazione){
    struct tavolo *prossimoNodo = (struct tavolo *) malloc (sizeof (struct tavolo));

    prossimoNodo->id = id;
    prossimoNodo->posti = posti;
    prossimoNodo->sala = sala;
    prossimoNodo->disponibile = disponibile;
    prossimoNodo->accorpabile = accorpabile;
    memcpy(prossimoNodo->ubicazione, ubicazione, GOOD_LEN);
    prossimoNodo->next = NULL;

    if (*testa == NULL){
        *testa = prossimoNodo;
        return;
    }
    struct tavolo *temp = *testa;
    while (temp->next != NULL)
        temp = temp->next;

    temp->next = prossimoNodo;
}
void inserisci_sala(struct sala **testa, int id, int tavoli, int posti_disponibili){
    struct sala *prossimoNodo = (struct sala *) malloc (sizeof (struct sala));
    prossimoNodo->id = id;
    prossimoNodo->tavoli = tavoli;
    prossimoNodo->posti_disponibili = posti_disponibili;
    prossimoNodo->next = NULL;

    if (*testa == NULL){
        *testa = prossimoNodo;
        return;
    }
    struct sala *temp = *testa;
    while (temp->next != NULL)
        temp = temp->next;

    temp->next = prossimoNodo;
}
void inserisci_prenotazione(struct prenotazione **testa, int id, char* data, int ora, char* cognome, int n_posti, int sala, int n_tavoli, int codice, int* tav){
    int i=0;
    struct prenotazione *prossimoNodo = (struct prenotazione *) malloc (sizeof (struct prenotazione));

    prossimoNodo->id = id;
    prossimoNodo->id_progressivo = id_progressivo;
    prossimoNodo->data = data;
    prossimoNodo->ora = ora;
    prossimoNodo->cognome = cognome;
    prossimoNodo->n_posti = n_posti;
    prossimoNodo->sala = sala;
    prossimoNodo->codice = codice;
    prossimoNodo->n_tavoli = n_tavoli;
    for(i=0;i<n_tavoli;i++)
        prossimoNodo->tavoli[i] = tav[i];

    prossimoNodo->next = NULL;
    id_progressivo++;

    if (*testa == NULL){
        *testa = prossimoNodo;
        return;
    }
    struct prenotazione *temp = *testa;
    while (temp->next != NULL)
        temp = temp->next;

    temp->next = prossimoNodo;
}
void inserisci_menu(struct menu **testa, int id, char* portata, char* piatto, int costo){
    struct menu *prossimoNodo = (struct menu *) malloc (sizeof (struct menu));

    prossimoNodo->id = id;
    memcpy(prossimoNodo->piatto, piatto, DIM_PIATTO);
    prossimoNodo->costo = costo;
    memcpy(prossimoNodo->portata, portata, DIM_PORTATA);
    prossimoNodo->next = NULL;

    if (*testa == NULL){
        *testa = prossimoNodo;
        return;
    }
    struct menu *temp = *testa;
    while (temp->next != NULL)
        temp = temp->next;

    temp->next = prossimoNodo;
}
void inserisci_prenotazione_attiva(struct prenotazione_servire **testa, int codice, int n_tavoli, int* tav){
    struct prenotazione_servire *prossimoNodo;
    int i=0;
    prenotazione_servire* scorri =  lista_prenotazioni_attive;
    while (scorri != NULL){
        if(scorri->codice == codice)
            return;
        scorri = scorri->next;
    }
    
    prossimoNodo = (struct prenotazione_servire *) malloc (sizeof (struct prenotazione_servire));
    prossimoNodo->id = contatore_prenotazioni_attive;
    contatore_prenotazioni_attive++;
    prossimoNodo->codice = codice;
    prossimoNodo->n_tavoli = n_tavoli;
    for(i=0; i<n_tavoli; i++)
        prossimoNodo->tavoli[i] = tav[i];
    prossimoNodo->next = NULL;

    if (*testa == NULL){
        *testa = prossimoNodo;
        return;
    }
    struct prenotazione_servire *temp = *testa;
    while (temp->next != NULL)
        temp = temp->next;

    temp->next = prossimoNodo;
}
int inserisci_comanda(struct prenotazione_servire **testa, int codice, char* tipo, int* numero, int* quanto, int k){

    FILE* fd;
    char* buffer = (char*) malloc(MAX_LEN);
    int i=1;
    int j=0;
    int z=0;
    int trovato = 0;

    int codice_y, id_y, stato_y, numero_y, quanto_y, quante_y, codice_tavoli;
    char tipo_y;
    char* orario_y = (char*) malloc(GOOD_LEN);
    char* tavoli_prenotazione = (char*) malloc(GOOD_LEN);
    char* tavoli_prenotazione_x = (char*) malloc(GOOD_LEN);
    char* buffer_file = (char*) malloc(MAX_LEN);
    char* scambio = (char*) malloc(GOOD_LEN);

    // Recupero la giusta prenotazione
    prenotazione_servire* prenotazione_giusta = *testa;
    while (prenotazione_giusta->codice != codice)
        prenotazione_giusta = prenotazione_giusta->next;

    for(j=0;j<prenotazione_giusta->n_tavoli;j++){
        strcpy(scambio, tavoli_prenotazione);
        codice_tavoli = prenotazione_giusta->tavoli[j];
        codice_tavoli++;
        sprintf(tavoli_prenotazione, "%sT%d", scambio, codice_tavoli);
    }

    // Preparo la nuova comanda
    comanda** comanda_giusta = &prenotazione_giusta->comande_prenotazione;
    struct comanda *prossimoNodo = (struct comanda *) malloc (sizeof (struct comanda));

    for(j=0;j<k;j++){
        prossimoNodo->tipo[j] = tipo[j];
        prossimoNodo->numero[j] = numero[j];
        prossimoNodo->quanto[j] = quanto[j];
    }
    prossimoNodo->n_portate = k;
    prossimoNodo->next = NULL;

    // Recupero l'ultima comanda
    fd = fopen("./storage/book_attive.txt", "r");
    while(fgets(buffer_file, MAX_LEN, fd) != NULL){
        sscanf(buffer_file, "%d %d %d %s %d %c %d %d %s", &codice_y, &id_y, &quante_y, tavoli_prenotazione_x, &stato_y, &tipo_y, &numero_y,  &quanto_y, orario_y);
        if(codice == codice_y){
            i = id_y;
            trovato = 1;
        }
    }
    fclose(fd);
    if(trovato == 1)
        i++;
    // Se non ho ancora comande
    if (*comanda_giusta == NULL){
        prossimoNodo->id=i;
        *comanda_giusta = prossimoNodo;
        fd = fopen("./storage/book_attive.txt", "a+");            
        for(z=0;z<k;z++){
            // codice prenotazione | numero comanda | quante comande | tavoli | stato | tipo | numero | quantita | timestamp
            sprintf(buffer, "%d %d %d %s %d %c %d %d %s\n", codice, i, k, tavoli_prenotazione, 0, tipo[z], numero[z], quanto[z], orario());
            fprintf(fd, "%s", buffer);
        }
        fclose(fd);
        return 0;
    }
    
    // Se ho già comande
    prossimoNodo->id = i;
    fd = fopen("./storage/book_attive.txt", "a+");  
    for(z=0;z<k;z++){
        // codice prenotazione | numero comanda | stato | tipo | numero | quantita | timestamp
        sprintf(buffer, "%d %d %d %s %d %c %d %d %s\n", codice, i, k, tavoli_prenotazione, 0, tipo[z], numero[z], quanto[z], orario());
        fprintf(fd, "%s", buffer);
    }
    fclose(fd);
    return 0;

}
