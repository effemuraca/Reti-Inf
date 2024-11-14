#ifndef CLIENT_FUN_H
#define CLIENT_FUN_H

#include "general.h"       
#include "socket_fun.h"   

/**
 * @brief Invia le credenziali al server.
 *
 * @param sd Descrittore del socket utilizzato per la comunicazione.
 * @param user Puntatore alla stringa contenente l'username dell'utente.
 * @param pwd Puntatore alla stringa contenente la password dell'utente.
 * @param cmd Puntatore alla stringa contenente il comando da inviare (es. "login", "register").
 */
void sendCredentials(int sd, char* user, char* pwd, char *cmd);

/**
 * @brief Gestisce e interpreta i codici di risposta dal server.
 *
 * @param code Codice di risposta ricevuto dal server.
 * @param loop_ended Puntatore a una variabile booleana che indica se il loop principale deve terminare.
 */
void switchCode(uint8_t code, bool *loop_ended);

/**
 * @brief Invia il comando per selezionare una stanza al server.
 *
 * @param sd Descrittore del socket utilizzato per la comunicazione.
 * @param room Numero della stanza da selezionare.
 * @param loop_ended Puntatore a una variabile booleana che indica se il loop principale deve terminare.
 * @return true Se si è verificato un errore durante l'invio o la ricezione dei dati.
 * @return false Se si è verificato un errore durante l'invio o la ricezione dei dati.
 */
bool sendRoom(int sd, uint8_t room, bool *loop_ended);

/**
 * @brief Gestisce alcuni codici di risposta che richiedono operazioni specifiche.
 * 
 * @param sd Descrittore del socket.
 * @param code Codice di risposta ricevuto dal server.
 * @param start_time Tempo di inizio del gioco.
 * @param room_time Tempo massimo per completare la stanza.
 * @param game_ended Puntatore a una variabile booleana che indica se il gioco è terminato. 
 */
void handleServerResponse(int sd, uint8_t code, time_t start_time, uint16_t room_time, bool *game_ended);

#endif