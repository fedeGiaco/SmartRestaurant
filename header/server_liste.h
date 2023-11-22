// Federico Giacomelli, matricola 578016, progetto di Reti Informatiche 2022/23
// FILE: header/server_liste.h

#include "utility.h"

void inserisci_tavolo(struct tavolo **testa, int id, int posti, int sala, int disponibile, int accorpabile, char* ubicazione);
void inserisci_sala(struct sala **testa, int id, int tavoli, int posti_disponibili);
void inserisci_prenotazione(struct prenotazione **testa, int id, char* data, int ora, char* cognome, int n_posti, int sala, int n_tavoli, int codice, int* tav);
void inserisci_menu(struct menu **testa, int id, char* portata, char* piatto, int costo);
void inserisci_prenotazione_attiva(struct prenotazione_servire **testa, int codice, int n_tavoli, int* tav);
int inserisci_comanda(struct prenotazione_servire **testa, int codice, char* tipo, int* numero, int* quanto, int k);