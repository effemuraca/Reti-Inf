#ifndef _GENERAL_H
#define _GENERAL_H

// Lunghezza massima delle descrizioni
#define MAX_DESCRIPTION_LENGTH 512

// Lunghezza massima delle risposta agli enigmi
#define MAX_RIDDLE_ANS_LENGTH 32

// Lunghezza massima per i nomi
#define MAX_NAME_LENGTH 32

// Numero massimo di stanze
#define MAX_ROOM_NUMBER 1

// Numero massimo di location per stanza
#define MAX_LOCATION_NUMBER 8 

// Numero massimo di oggetti nel gioco
#define MAX_OBJECT_NUMBER 64

// Lunghezza massima dell'username
#define MAX_USERNAME_LENGTH 32

// Lunghezza massima della password
#define MAX_PASSWORD_LENGTH 16

// Lunghezza massima per il nome degli oggetti
#define MAX_ITEM_LENGTH 32

// Dimensione massima dell'inventario
#define MAX_INVENTORY_SIZE 8

// Lunghezza massima degli argomenti dei comandi
#define MAX_ARG_LENGTH 32

// Numero massimo di oggetti sbloccabili da un singolo oggetto
#define MAX_OBJECT_UNLOCKABLE 4

// Sezione dedicata alla ridenominazione dei codici di risposta
typedef enum code {

    // Login, Registrazione e Logout
    LOGIN_SUCCESS_PREVIOUS = 10,
    LOGIN_SUCCESS = 11,
    USER_ALREADY_LOGGED = 12,
    WRONG_PASSWORD = 13,
    USERNAME_NOT_EXIST = 14,
    USERNAME_ALREADY_EXISTS = 15,
    REGISTRATION_SUCCESS = 16,
    LOGOUT_SUCCESS = 17,

    // Sessioni
    SESSION_NOT_FOUND_OR_OFFLINE = 20,
    SESSION_END_NO_GAME = 21,

    // Gestione Stanze
    ROOM_SET_SUCCESS = 30,
    ROOM_ALREADY_SET = 31,
    ROOMS_LIST_SENT_SUCCESS = 32,
    ROOM_NOT_PRESENT = 33,

    // Gestione Oggetti
    OBJECTS_SENT_SUCCESS = 40,
    OBJECT_DROPPED_SUCCESS = 41,
    OBJECT_NOT_IN_INVENTORY = 42,
    OBJECT_ALREADY_IN_INVENTORY = 43,
    OBJECT_PICKED_SUCCESS = 44,
    OBJECT_NOT_PICKABLE = 45,
    OBJECT_NOT_PRESENT = 46,
    OBJECT_NOT_VISIBLE = 47,
    OBJECT_NOT_IN_GAME = 48,
    OBJECT_USED_SUCCESS = 49,
    OBJECTS_NOT_COMPATIBLE = 50,
    OBJECTS_MATCH = 51,
    
    // Enigmi
    RIDDLE_TO_SOLVE = 61,
    RIDDLE_NOT_SOLVED = 62,
    OBJECT_BLOCKED_BY_RIDDLE = 63,
    RIDDLE_SOLVED_SUCCESS = 64,

    // Lista e Utenti Online
    LIST_SENT_SUCCESS = 70,
    NO_USERS_ONLINE = 71,

    // Spionaggio
    SPY_MODE_ACTIVATED = 80,
    SPY_USER_FOUND = 81,
    SPY_USER_NOT_FOUND = 82,
    SPY_ALREADY_ACTIVE = 83,
    
    // Miscellanea
    COMMAND_NOT_RECOGNIZED = 84,
    GAME_END = 85,
    DESCRIPTION_SENT_SUCCESS = 86,
    VICTORY = 87,
    TOKEN_PICKED_SUCCESS = 88,
    INVENTORY_FULL = 89,    
} code;


#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <signal.h>

typedef struct sockaddr_in sockaddr_in; 

#endif