// Federico Giacomelli, matricola 578016, progetto di Reti Informatiche 2022/23
// FILE: header/server_td.h

#include <stdbool.h>

void td_prepara_menu();
int td_recupera_prezzo(char tipo, int numero);
int td_cerca_prenotazione(int codice_dato);
int td_comanda_esistente(char tipo, int numero);
int td_recuperaPrenotazione(char* codice_dato);
char* td_gestione_comanda(char* input);
bool td_fine_pasto(int codice);
char* td_gestione_conto(char* input);