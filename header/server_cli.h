// Federico Giacomelli, matricola 578016, progetto di Reti Informatiche 2022/23
// FILE: header/server_cli.h

int cli_genera_codice();
void cli_prepara_tavoli();
void cli_controllo_disponibilita(int n_persone, int dd, int mo, int yy, int hh);
int cli_verifica_concorrenza(char* data, int ora, int* tavoli, int n_tavoli);
void cli_rimuovi_prenotazione_temporanea(int id);
char* cli_cerca_tavoli(char* buffer);
char* cli_effettua_prenotazione(char* buffer, char* persona);