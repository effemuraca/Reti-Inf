#include "utility.h"

void mostraComandiServer(){
    printf("\n************************** CONSOLE SERVER **************************\n");
    printf("Digita un comando:\n");
    printf("1) start <port> --> avvia il server\n");
    printf("2) stop --> termina il server\n");
    printf("******************************************************************************\n");
}

void mostraComandiLogin(){
    printf("\n***************************** COMANDI LOGIN *********************************\n");
    printf("Lista dei comandi disponibili:\n");
    printf("1) login <username> <password> --> permette di accedere al gioco \n");
    printf("2) register <username> <password> --> permette di registrarsi al gioco\n");
    printf("3) end --> termina il gioco\n");
    printf("******************************************************************************\n");
} 

void mostraComandiClient(){
    printf("\n***************************** COMANDI CLIENT *********************************\n");
    printf("Lista dei comandi disponibili:\n");
    printf("1) look [obj | loc] --> dà una descrizione della stanza, di un oggetto o di una location\n");
    printf("2) take <obj> --> prende un oggetto e lo inserisce nell'inventario\n");
    printf("3) use <obj_1> [obj_2] --> usa l'oggetto 1 presente in inventario, eventualmente con l'oggetto 2\n");
    printf("4) objs --> mostra l'inventario attuale\n");
    printf("5) drop <obj> --> lascia l'oggetto presente in inventario\n");
    printf("6) logout --> effettua il logout, salvando la sessione\n");
    printf("7) end --> termina il gioco\n");
    printf("******************************************************************************\n");
}

void mostraComandiSpia(){
    printf("\n************************** COMANDI SPIA **************************\n");
    printf("1) list --> mostra la lista dei giocatori online\n");
    printf("2) spy <player> --> permette di spiare un giocatore\n");
    printf("3) end --> termina la sessione di spionaggio\n");
    printf("**********************************************************\n");
}

void mostraComandiStart(){
    printf("\n************************** COMANDI START **************************\n");
    printf("1) start --> mostra la lista delle stanze disponibili\n");
    printf("2) start <room_number> --> inizia il gioco nella stanza scelta\n");
    printf("3) spymode --> accedi ai comandi da spia\n");
    printf("4) end --> termina il gioco\n");
    printf("**********************************************************\n");
}