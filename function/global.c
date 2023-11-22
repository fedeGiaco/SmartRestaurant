// Federico Giacomelli, matricola 578016, progetto di Reti Informatiche 2022/23
// FILE: function/global.c

#include "./../header/utility.h"

tavolo* lista_tavoli;
sala* lista_sale;
prenotazione* lista_prenotazioni;
menu* lista_menu;
prenotazione_servire* lista_prenotazioni_attive;
int contatore_prenotazioni_attive;
int contatore_prenotazioni = 0;
int id_progressivo = 1;
int codice_interessato;
int codice_interessato_servizio;