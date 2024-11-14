#ifndef SERVER_FUN_H
#define SERVER_FUN_H

#include "general.h"
#include "sessione.h"
#include "game.h"
#include "socket_fun.h"

/**
 * @brief Controlla la validità del nome utente
 *
 * @param user Nome utente da verificare
 * @return true se il nome utente è valido, false altrimenti
 */
bool checkUsername(const char* user);

/**
 * @brief Controlla la corrispondenza tra nome utente e password
 *
 * @param user Nome utente
 * @param pwd Password da verificare
 * @return true se la password è corretta, false altrimenti
 */
bool checkPassword(const char* user, const char* pwd);

/**
 * @brief Salva le credenziali dell'utente su file
 *
 * @param user Nome utente
 * @param pwd Password dell'utente
 * @return true se il salvataggio ha avuto successo, false altrimenti
 */
bool saveToFile(const char* user, const char* pwd);

/**
 * @brief Riceve le credenziali dal client
 *
 * @param sd Descrittore del socket.
 * @param user Buffer per il nome utente
 * @param pwd Buffer per la password
 */
void recvCredentials(int sd, char* user, char* pwd);

/**
 * @brief Imposta la stanza di gioco per un giocatore
 *
 * @param sd Descrittore del socket.
 * @param ses Sessione del giocatore
 * @param n_room Numero di stanze
 * @param container Puntatore al contenitore delle stanze
 */
void setRoom(int sd, session* ses, uint8_t n_room, room* container);

/**
 * @brief Verifica se un giocatore ha raggiunto la vittoria
 *
 * @param sd Descrittore del socket.
 * @param ses Sessione del giocatore
 * @param ses_list Lista delle sessioni
 */
void checkVictory(int sd, session* ses, session** ses_list);

/**
 * @brief Gestisce l'invio di un messaggio a un giocatore spiato
 * 
 * @param ses Sessione del giocatore spiato
 * @param arg Messaggio da inviare
 */
void sendToSpy(session* ses, const char* arg);

/**
 * @brief Gestisce l'invio di un codice di risposta a un giocatore spiato
 * 
 * @param ses Sessione del giocatore spiato
 * @param code Codice di risposta da inviare
 */
void sendCodeToSpy(session* ses, uint8_t* code);

/**
 * @brief Gestisce il login di un utente
 * 
 * @param username Nome utente
 * @param password Password
 * @param ses_list Lista delle sessioni
 * @param container Contenitore delle stanze
 * @param i Descrittore del socket
 * @param code Codice di risposta
 * @return true se dopo la funzione c'è da fare una continue, false altrimenti
 */
bool handleLogin(const char* username, const char* password, session** ses_list, room * container, int i, uint8_t* code);

/**
 * @brief Gestisce la registrazione di un utente
 * 
 * @param username Nome utente
 * @param password Password
 * @param code Codice di risposta
 */
void handleRegister(const char* username, const char* password, uint8_t* code);

/**
 * @brief Gestisce il logout di un utente
 * 
 * @param full_cmd Comando completo
 * @param ses_list Lista delle sessioni
 * @param i Descrittore del socket
 * @param code Codice di risposta
 */
void handleLogout(char* full_cmd, session** ses_list, int i, uint8_t* code);

/**
 * @brief Gestisce l'inizio di una partita
 * 
 * @param ses_list Lista delle sessioni
 * @param container Contenitore delle stanze
 * @param i Descrittore del socket
 * @param code Codice di risposta
 * @return true se dopo la funzione c'è da fare una continue, false altrimenti
 */
bool handleStart(session** ses_list, room* container, int i, uint8_t* code);

/**
 * @brief Gestisce il comando "look"
 * 
 * @param full_cmd Comando completo
 * @param ses_list Lista delle sessioni
 * @param container Contenitore delle stanze
 * @param i Descrittore del socket
 * @param code Codice di risposta
 * @return true se dopo la funzione c'è da fare una continue, false altrimenti
 */
bool handleLook(char* full_cmd, session** ses_list, room* container, int i, uint8_t* code);

/**
 * @brief Gestisce il comando "take"
 * 
 * @param full_cmd Comando completo
 * @param ses_list Lista delle sessioni
 * @param container Contenitore delle stanze
 * @param i Descrittore del socket
 * @param code Codice di risposta
 * @return true se dopo la funzione c'è da fare una continue, false altrimenti
 */
bool handleTake(char* full_cmd, session** ses_list, room* container, int i, uint8_t* code);

/**
 * @brief Gestisce il comando "use"
 * 
 * @param full_cmd Comando completo
 * @param ses_list Lista delle sessioni
 * @param container Contenitore delle stanze
 * @param i Descrittore del socket
 * @param code Codice di risposta
 * @return true se dopo la funzione c'è da fare una continue, false altrimenti
 */
bool handleUse(char* full_cmd, session** ses_list, room* container, int i, uint8_t* code);

/**
 * @brief Gestisce il comando "objs"
 * 
 * @param full_cmd Comando completo
 * @param ses_list Lista delle sessioni
 * @param i Descrittore del socket
 * @param code Codice di risposta
 * @return true se dopo la funzione c'è da fare una continue, false altrimenti
 */
bool handleObjs(char* full_cmd, session** ses_list, int i, uint8_t* code);

/**
 * @brief Gestisce il comando "end"
 * 
 * @param full_cmd Comando completo
 * @param ses_list Lista delle sessioni
 * @param i Descrittore del socket
 * @param code Codice di risposta
 * @return true se dopo la funzione c'è da fare una continue, false altrimenti
 */
bool handleEnd(char* full_cmd, session** ses_list, int i, uint8_t* code);

/**
 * @brief Gestisce il comando "drop"
 * 
 * @param full_cmd Comando completo
 * @param ses_list Lista delle sessioni
 * @param i Descrittore del socket
 * @param code Codice di risposta
 */
void handleDrop(char* full_cmd, session** ses_list, int i, uint8_t* code);

/**
 * @brief Gestisce il comando "list"
 * 
 * @param ses_list Lista delle sessioni
 * @param i Descrittore del socket
 * @param code Codice di risposta
 */
void handleList(session** ses_list, int i, uint8_t* code);

/**
 * @brief Gestisce il comando "spymode"
 * 
 * @param ses_list Lista delle sessioni
 * @param i Descrittore del socket
 * @param code Codice di risposta
 */
void handleSpyMode(session** ses_list, int i, uint8_t* code);

/**
 * @brief Gestisce il comando "spy"
 * 
 * @param ses_list Lista delle sessioni
 * @param i Descrittore del socket
 * @param code Codice di risposta
 */
void handleSpy(session** ses_list, int i, uint8_t* code);


#endif

