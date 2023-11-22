// Federico Giacomelli, matricola 578016, progetto di Reti Informatiche 2022/23
// FILE: header/utility.h

#ifndef UTILITY_H
#define UTILITY_H

#define POSTI_TOTALI 40
#define TAVOLI_TOTALI 20
#define POSTI_TAVOLO 2
#define SALE_TOTALI 4
#define PORTATE_TOTALI 8
#define DIM_PORTATA 2
#define DIM_PIATTO 50
#define DIM_DATA 8

#define MAX_KD 5
#define MAX_CLI 10
#define MAX_TD 20

#define MAX_LEN 1024
#define GOOD_LEN 512

typedef struct tavolo{
    int id;
    int posti;
    int sala;
    char ubicazione[GOOD_LEN];
    int disponibile;
    int accorpabile;
    struct tavolo* next;
} tavolo;

typedef struct sala{
    int id;
    int tavoli;
    int posti_disponibili;
    struct sala* next;
} sala;

typedef struct prenotazione{
    int id;
    int id_progressivo;
    char* data;
    int ora;
    char* cognome;
    int n_posti;
    int sala;
    int n_tavoli;
    int codice;
    int tavoli[TAVOLI_TOTALI];
    struct prenotazione* next;
} prenotazione;

typedef struct menu{
    int id;
    char portata[DIM_PORTATA];
    int costo;
    char piatto[DIM_PIATTO];
    struct menu* next;
} menu;

typedef struct comanda{
    int id;
    int n_portate;
    char tipo[TAVOLI_TOTALI];
    int numero[TAVOLI_TOTALI];
    int quanto[TAVOLI_TOTALI];
    struct comanda* next;
} comanda;

typedef struct prenotazione_servire{
    int id;
    int codice;
    int tavoli[TAVOLI_TOTALI];
    int n_tavoli;
    struct comanda* comande_prenotazione;
    struct prenotazione_servire* next;
} prenotazione_servire;

void error_detection(int ret, int sd);

#endif