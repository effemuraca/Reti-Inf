#ifndef SESSIONE_H
#define SESSIONE_H

#include "general.h"

/**
 * @struct session
 * @brief Struttura che rappresenta una sessione di gioco per un utente.
 */
typedef struct session{
    int id;                                      // Identificativo unico della sessione
    char username[MAX_USERNAME_LENGTH];          // Nome utente dell'utente
    char password[MAX_PASSWORD_LENGTH];          // Password dell'utente
    uint8_t token;                               // Numero di token della stanza
    uint8_t token_remaining;                     // Numero di token rimanenti
    uint16_t time;                               // Tempo totale associato alla partita
    char inventory[MAX_INVENTORY_SIZE][MAX_ITEM_LENGTH]; // Inventario dell'utente con gli oggetti
    uint8_t actual_inventory_size;               // Dimensione attuale dell'inventario
    bool online;                                 // Stato dell'utente

    // Indica se il giocatore è in modalità spia e contiene il socket descriptor del giocatore spiato
    bool spy;
    int spy_sd;                                  // Socket descriptor del giocatore spiato
    int spied_by;                                // Socket descriptor del giocatore che sta spiando                            

    uint8_t room;                                // ID della stanza corrente in cui si trova l'utente
    bool session_init;                           // Indica se la sessione è stata inizializzata
    bool riddle_solved[MAX_OBJECT_NUMBER];       // Stato di risoluzione degli enigmi associati agli oggetti
    bool now_visible[MAX_OBJECT_NUMBER];        // Stato di visibilità degli oggetti
    struct session *next;                        // Puntatore alla prossima sessione nella lista
} session;

/**
 * @brief Crea una nuova sessione per un utente.
 * 
 * @param user Nome utente.
 * @param id Identificativo unico della sessione.
 * @return session* Puntatore alla nuova sessione creata.
 */
session* newSession(const char *user, int id);

/**
 * @brief Verifica se esiste una sessione attiva per un dato utente.
 * 
 * @param ses_list Puntatore alla lista delle sessioni.
 * @param user Nome utente da verificare.
 * @return session* Puntatore alla sessione se trovata, NULL altrimenti.
 */
session* checkSession(session **ses_list, const char *user);

/**
 * @brief Verifica se esiste una sessione attiva per un dato ID.
 * 
 * @param ses_list Puntatore alla lista delle sessioni.
 * @param id Identificativo della sessione da verificare.
 * @return session* Puntatore alla sessione se trovata, NULL altrimenti.
 */
session* checkSessionById(session **ses_list, int id);

/**
 * @brief Controlla se ci sono sessioni attive.
 * 
 * @param ses_list Puntatore alla lista delle sessioni.
 * @return true Se ci sono sessioni attive.
 * @return false Altrimenti.
 */
bool checkActiveSession(session **ses_list);

/**
 * @brief Inserisce una nuova sessione nella lista delle sessioni.
 * 
 * @param ses_list Puntatore alla lista delle sessioni.
 * @param session Puntatore alla sessione da inserire.
 */
void insertSession(session **ses_list, session* session);

/**
 * @brief Elimina una sessione dalla lista delle sessioni.
 * 
 * @param ses_list Puntatore alla lista delle sessioni.
 * @param session Puntatore alla sessione da eliminare.
 */
void deleteSession(session **ses_list, session* session);

#endif