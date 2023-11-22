// Federico Giacomelli, matricola 578016, progetto di Reti Informatiche 2022/23
// FILE: header/server_utility.h

void begin_msg();
void write_log(int tipo, char* msg);
char* converti_stato (int tipo);
char converti_stato_to_char (int tipo);
void stampa_tutto(char* parametro, int tipo);