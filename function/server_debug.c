// Federico Giacomelli, matricola 578016, progetto di Reti Informatiche 2022/23
// FILE: function/server_debug.c

// Queste funzioni non sono chiamate al momento della consegna del progetto.
// Le ho utilizzate per debug.

#include "./../header/server_debug.h"
#include "./../header/lib.h"
#include "./../header/utility.h"

//Variabili globali
extern tavolo* lista_tavoli;
extern sala* lista_sale;
extern prenotazione* lista_prenotazioni;
extern prenotazione_servire* lista_prenotazioni_attive;

//Sono tutte funzioni autoesplicative, non utilizzate per l'esecuzione ma invocabili dal server per debug
void stampa_tavoli(){
    tavolo* scorri = lista_tavoli;
    while (scorri != NULL){
        printf("id: %d, posti: %d, sala: %d, disp: %d, acc: %d, ub: %s\n", scorri->id, scorri->posti, scorri->sala, scorri->disponibile, scorri->accorpabile, scorri->ubicazione);
        scorri = scorri->next;
    }
}
void stampa_sale(){
    sala* scorri = lista_sale;
    while (scorri != NULL){
        printf("id: %d, posti: %d, tavoli: %d\n", scorri->id, scorri->posti_disponibili, scorri->tavoli);
        scorri = scorri->next;
    }
}
void stampa_prenotazioni(){
    prenotazione* scorri = lista_prenotazioni;
    if(!scorri){
        printf("Lista vuota\n");
        return;
    }
    while (scorri != NULL){
        printf("R: %s, id: %d, id_p: %d, data: %s, cognome: %s, posti: %d, sala: %d, n_tavoli: %d, cod: %d\n", scorri->cognome, scorri->id, scorri->id_progressivo, scorri->data, scorri->cognome, scorri->n_posti, scorri->sala, scorri->n_tavoli, scorri->codice);
        scorri = scorri->next;
    }
    printf("\n");
}
void stampa_prenotazioni_attive(){
    int i=0,j=0;
    char* buffer = (char*) malloc(GOOD_LEN);

    prenotazione_servire* scorri = lista_prenotazioni_attive;
    comanda* scorri_comanda = NULL;
    while (scorri != NULL){
        for(i=0;i<scorri->n_tavoli;i++){
            printf("%d",scorri->tavoli[i]);
        }
        printf(" id: %d, n_tav: %d\n", scorri->id, scorri->n_tavoli);
        free(buffer);
        scorri_comanda = scorri->comande_prenotazione;
        while (scorri_comanda != NULL){
            for(j=0;j<scorri_comanda->n_portate;j++)
                printf("id: %d, tipo: %c, numero: %d, quanto: %d\n", scorri_comanda->id, scorri_comanda->tipo[j], scorri_comanda->numero[j], scorri_comanda->quanto[j]); 
            scorri_comanda = scorri_comanda->next;
        }
        scorri = scorri->next;
    }
}