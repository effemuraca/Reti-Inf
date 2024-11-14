#ifndef GAME_H 
#define GAME_H

#include "general.h"

/* Viene attuato un typedef per le strutture utilizzate
   per fare in modo da poterle utilizzare come se si fosse in C++ */

typedef struct location{
    char name[MAX_NAME_LENGTH];                   // Nome della location
    char description[MAX_DESCRIPTION_LENGTH];     // Descrizione dettagliata della location
} location;

typedef struct object{
    char name[MAX_NAME_LENGTH];                    // Nome dell'oggetto
    char description[MAX_DESCRIPTION_LENGTH];      // Descrizione dettagliata dell'oggetto
    bool takeable;                                 // Indica se l'oggetto può essere raccolto
    bool visible;                                  // Indica se l'oggetto è visibile
    // Non c'è bool usable perché le condizioni per l'usabilità sono le stesse della visibilità (se un oggetto è visibile è anche usabile)
    bool riddle;                                   // Indica se l'oggetto ha un enigma associato
    char rdl_description[MAX_DESCRIPTION_LENGTH];  // Descrizione dell'enigma
    char rdl_answer[MAX_RIDDLE_ANS_LENGTH];       // Risposta all'enigma
    bool token;                                    // Indica se l'oggetto fornisce un token
    char used_with[MAX_NAME_LENGTH];               // Nome dell'oggetto con cui può essere utilizzato
    char new_description[MAX_DESCRIPTION_LENGTH];  // Nuova descrizione dopo l'uso
    char description_after_usage_alone[MAX_DESCRIPTION_LENGTH]; // Descrizione dopo l'uso dell'oggetto da solo
    char description_after_usage[MAX_DESCRIPTION_LENGTH];       // Descrizione dopo l'uso dell'oggetto con un altro
    uint8_t item_unlocked[MAX_OBJECT_UNLOCKABLE];  // Posizione (nell'array objs) degli oggetti sbloccati dopo l'uso
} object;

typedef struct room{
    uint16_t room_id;                              // Identificativo unico della stanza
    char name[MAX_NAME_LENGTH];                    // Nome della stanza
    char description[MAX_DESCRIPTION_LENGTH];      // Descrizione dettagliata della stanza
    uint8_t token;                                 // Numero di token associati alla stanza
    uint8_t inventory_size;                        // Dimensione dell'inventario della stanza
    uint16_t time;                                 // Tempo totale per completare la stanza
    struct location loc[MAX_LOCATION_NUMBER];      // Array delle location presenti nella stanza
    struct object obj[MAX_OBJECT_NUMBER];          // Array degli oggetti presenti nella stanza
} room;

#endif