// Federico Giacomelli, matricola 578016, progetto di Reti Informatiche 2022/23
// FILE: function/server_cli.c

#include "./../header/server_cli.h"
#include "./../header/server_liste.h"
#include "./../header/lib.h"
#include "./../header/utility.h"

//Variabili globali
extern tavolo* lista_tavoli;
extern sala* lista_sale;
extern prenotazione* lista_prenotazioni;
extern int contatore_prenotazioni;
extern int id_progressivo;

//Ritorna un codice causale ma certamente diverso da quelli già inseriti per una nuova prenotazione
int cli_genera_codice(){
    FILE* fd;
    int codice=0, i=0, totali=0, k=0;
    int dd_x, mo_x, yy_x, hh_x, n_persone_x, tavolo_prenotato, codice_x;
    int* lista_codici = (int*) malloc(MAX_LEN);
    char* cognome_x = (char*) malloc(1024);
    char* buffer_file = (char*) malloc(MAX_LEN);
 
    //Recupero tutti i codici
    fd = fopen("./storage/book.txt", "r");
    while(fgets(buffer_file, MAX_LEN, fd) != NULL){  
        sscanf(buffer_file, "%d-%d-%d %d %s %d %d %d", &dd_x, &mo_x, &yy_x, &hh_x, cognome_x,  &n_persone_x, &tavolo_prenotato, &codice_x);
        lista_codici[i] = codice_x;
        i++;
    }
    totali = i;
    fclose(fd);
    //Genero un codice sicuramente diverso da quelli già inseriti
    while(1){
        srand(time(NULL));
        codice = rand() % 10000 + 1;
        for(i=0; i<totali; i++){
            if(codice == lista_codici[i])
                k++;
        }
        if(k==0)
            break;
    }
    return codice;
}

//Preparo le sale e i tavoli disponibli
void cli_prepara_tavoli(){
    
    FILE* fd;
    int last_sala=0, conta_tavoli=0, conta_posti=0;;
    int tavolo_x, sala_x;
    char* ubicazione_x = (char*) malloc(GOOD_LEN);
    char* buffer_file = (char*) malloc(MAX_LEN);

    lista_sale = NULL;
    lista_tavoli = NULL;
    fd = fopen("./storage/tavoli.txt", "r");
    while(fgets(buffer_file, MAX_LEN, fd) != NULL){  
        sscanf(buffer_file, "%d %d %s", &tavolo_x, &sala_x, ubicazione_x);
        if(last_sala != sala_x){
            inserisci_sala(&lista_sale, last_sala, conta_tavoli, conta_posti);
            conta_tavoli = 0;
            conta_posti = 0;
            last_sala = sala_x;
        }
        inserisci_tavolo(&lista_tavoli, tavolo_x, POSTI_TAVOLO, sala_x, 0, 0, ubicazione_x);
        conta_posti+=2;
        conta_tavoli++;
    }

    inserisci_sala(&lista_sale, last_sala, conta_tavoli, conta_posti);
    fclose(fd);
}

//Preparo la prenotazione temporanea, discriminando se devo accorpare più tavoli
void cli_controllo_disponibilita(int n_persone, int dd, int mo, int yy, int hh){
    FILE* fd;
    int dd_x, mo_x, yy_x, hh_x, n_persone_x, n_persone_tot, tavolo_prenotato;
    char* cognome_x = (char*) malloc(1024);
    char* buffer_file = (char*) malloc(MAX_LEN);
 
    n_persone_tot = n_persone;
    fd = fopen("./storage/book.txt", "r");
    // Guardo le prenotazioni GIA' effettuate e tolgo i tavoli già occupati
    while(fgets(buffer_file, MAX_LEN, fd) != NULL){  
        sscanf(buffer_file, "%d-%d-%d %d %s %d %d", &dd_x, &mo_x, &yy_x, &hh_x, cognome_x,  &n_persone_x, &tavolo_prenotato);
        if(dd_x == dd && mo_x == mo && yy_x == yy && ((hh_x == hh)||(hh_x+1 == hh))){
            tavolo* l_tav = lista_tavoli;
            sala* l_sala = lista_sale;
            
            while (l_tav->id != tavolo_prenotato)
                l_tav = l_tav->next;
            l_tav->posti = 0;
            l_tav->accorpabile = 0;
            l_tav->disponibile = 0;

            while (l_sala->id != l_tav->sala)
                l_sala = l_sala->next;
            l_sala->posti_disponibili-=2;
        }
    }
    fclose(fd);

    // Cerco tavoli sufficientemente capienti, se ho una richiesta per meno di 2 persone, basta un tavolo
    if(n_persone <= 2){
        tavolo* lista = lista_tavoli;
        while (lista != NULL){
            if(lista->posti > 0)
                lista->disponibile = 1;
            lista = lista->next;
        }
    }
    // Devo accorpare più tavoli
    else{
        tavolo* l_tav = lista_tavoli;
        sala* l_sal = lista_sale;

        while (l_sal != NULL){
            n_persone_tot = n_persone;
            if(l_sal->posti_disponibili < n_persone_tot){
                l_sal->posti_disponibili = 0;
                l_sal = l_sal->next;
                continue;
            }
            l_tav = lista_tavoli;
            while(l_tav != NULL){
                if(l_tav->sala == l_sal->id){
                    if(l_tav->posti == 0)
                        l_sal->posti_disponibili -= 2;
                    if(l_tav->posti > 0 && n_persone_tot > 0){
                        l_tav->accorpabile = 1;
                        n_persone_tot -= 2;
                    }
                }
                l_tav = l_tav->next;
            }
            l_sal = l_sal->next;
        }
    }
    return;
}

//Verifico se in quella data e ora, il tavolo non è stato nel mentre prenotato
int cli_verifica_concorrenza(char* data, int ora, int* tavoli, int n_tavoli){
    FILE* fd;
    int persone_prenotazione,ora_prenotazione, tavolo_prenotazione;
    int i=0;
    char data_prenotazione[GOOD_LEN];
    char nome[GOOD_LEN];
    char* buffer_file = (char*) malloc(MAX_LEN);

    fd = fopen("./storage/book.txt", "r");
    while (fgets(buffer_file, MAX_LEN, fd) != NULL){
        sscanf(buffer_file, "%s %d %s %d %d", data_prenotazione, &ora_prenotazione, nome, &persone_prenotazione, &tavolo_prenotazione);
        for(;i<n_tavoli;i++){
            if(strcmp(data_prenotazione, data)==0 && ((ora_prenotazione == ora)||(ora_prenotazione == ora+1)||(ora_prenotazione == ora-1)) && tavolo_prenotazione == tavoli[i]){
                fclose(fd);
                return 1;
            }
        }
    }
    fclose(fd);  
    return 0;
}

//Svuota la lista delle prenotazioni temporanee
void cli_rimuovi_prenotazione_temporanea(int id){
    struct prenotazione** l_preno = &lista_prenotazioni;
    struct prenotazione* l_temp;
    while (*l_preno != NULL){
        if((*l_preno)->id == id){
            l_temp = *l_preno;
            *l_preno = (*l_preno)->next;
            free(l_temp);
        }
    }
}

//Inserisco le prenotazioni temporanee, discriminando se devo accorpare più tavoli
char* cli_cerca_tavoli(char* buffer){
    
    int n_persone, dd,mo,yy,hh;
    int j=0,z=0,tav_temp=0, sala_temp=0, codice_prenotazione = 0;
    int* tav = (int*) malloc(1024);
    char* comando = (char*) malloc(MAX_LEN);
    char* input = (char*) malloc(MAX_LEN);
    char* next_input = (char*) malloc(MAX_LEN);
    char* cognome = (char*) malloc(MAX_LEN);
    char* data_prenotazione = (char*) malloc(DIM_DATA);
    char* risposta = (char*) malloc(MAX_LEN);

    cli_prepara_tavoli();

    tavolo* l_tav = lista_tavoli;
    sala* l_sala = lista_sale;
    j=0;
    z=1;
    
    sscanf(buffer, "%s %s %d %d-%d-%d %d", comando, cognome, &n_persone, &dd, &mo, &yy, &hh);   // recupero i campi della ricerca dello slot
    if(n_persone % 2 != 0)  // se ho un numero dispari di persone, lo incremento
        n_persone++;
    cli_controllo_disponibilita(n_persone, dd, mo, yy, hh);
    codice_prenotazione = cli_genera_codice();

    // Se mi occorre un solo tavolo
    id_progressivo = 1;
    if(n_persone == 2){
        while (l_tav != NULL){
            if(l_tav->disponibile == 1){
                tav_temp = l_tav->id;
                tav_temp++;
                sala_temp = l_tav->sala;
                sala_temp++;
                sprintf(comando, "%d) T%d SALA%d %s\n", z,  tav_temp, sala_temp, l_tav->ubicazione);
                sprintf(data_prenotazione, "%d-%d-%d", dd,mo,yy);
                tav[0] = l_tav->id;
                inserisci_prenotazione(&lista_prenotazioni, contatore_prenotazioni, data_prenotazione, hh, cognome, n_persone, l_tav->sala, 1, codice_prenotazione, tav);                
                memcpy(risposta, strcat(risposta, comando), MAX_LEN);
                z++;
            }
            l_tav = l_tav->next;
        }
        if(z > 1)   // ho almeno un tavolo disponibile
            contatore_prenotazioni++;
        return risposta;
    }    
    // Se mi occorre più di un tavolo, devo accorparli              
    else{
        while (l_sala != NULL){
            if(l_sala->posti_disponibili > 0){
                l_tav = lista_tavoli;
                while (l_tav != NULL){
                    if(l_tav->sala == l_sala->id){
                        if(l_tav->accorpabile == 1){
                            sprintf(next_input, "%sT%d-%s ", input, ++l_tav->id, l_tav->ubicazione);
                            memcpy(input, next_input, MAX_LEN);
                            l_tav->id--;
                            tav[j] = l_tav->id;
                            j++;
                        }
                    }
                    l_tav = l_tav->next;
                }
                sprintf(comando, "%d) %s SALA%d\n", z, next_input, ++l_sala->id);
                sprintf(data_prenotazione, "%d-%d-%d", dd,mo,yy);
                inserisci_prenotazione(&lista_prenotazioni, contatore_prenotazioni, data_prenotazione, hh, cognome, n_persone, l_sala->id, j, codice_prenotazione, tav);
                memcpy(risposta, strcat(risposta, comando), MAX_LEN);
                z++;
            }
            j=0;
            memset(tav, 0, MAX_LEN);
            memset(input, 0, MAX_LEN);
            l_sala = l_sala->next;
        }
        if(z > 1)   // ho almeno un tavolo disponibile
            contatore_prenotazioni++;
        else
            sprintf(risposta,"NON ho posti a sufficienza\n");
        return risposta;
    }
}

//Confermo la prenotazione
char* cli_effettua_prenotazione(char* buffer, char* persona){
    FILE* fd;
    int ret, z;
    int opzione=0;
    char* comando = (char*) malloc(MAX_LEN);

    sscanf(buffer, "%s %d", comando, &opzione);   // recupero il numero di opzione di prenotazione che intente confermare

    //Recupero la prenotazione temporanea
    prenotazione* l_preno = lista_prenotazioni;
    if(!l_preno){
        sprintf(comando, "Opzione non disponibile\n");
        return comando;
    }
    while(1){
        if((l_preno->id_progressivo == opzione) && strcmp(l_preno->cognome, persona)==0)
            break;
        if(!l_preno->next){
            sprintf(comando, "Opzione non disponibile\n");
            return comando;
        }
        l_preno = l_preno->next;
    }
    if(!l_preno){
        sprintf(comando, "Opzione non disponibile\n");
        return comando;
    }

    //Controllo se nel mentre è stata effettuata una prenotazione con quei tavoli nello stesso giorno e ora
    ret = cli_verifica_concorrenza(l_preno->data, l_preno->ora, l_preno->tavoli, l_preno->n_tavoli);
    if(ret == 1){
        sprintf(comando, "Opzione non più disponibile, tavolo nel mentre prenotato da altri\n");
        return comando;
    }

    //Procedo a confermare la prenotazione
    fd = fopen("./storage/book.txt", "a+");    
    for(z=0;z<l_preno->n_tavoli;z++){
        sprintf(buffer, "%s %d %s %d %d %d\n", l_preno->data, l_preno->ora, l_preno->cognome, l_preno->n_posti, l_preno->tavoli[z], l_preno->codice);
        ret = fprintf(fd, "%s", buffer);
    }
    fclose(fd);
    sprintf(comando, "PRENOTAZIONE CONFERMATA - Il tuo numero di prenotazione è: %d\n\n", l_preno->codice);
    cli_rimuovi_prenotazione_temporanea(l_preno->id);
    return comando;
}