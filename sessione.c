#include "sessione.h"

session* newSession(const char *user, int id){
    int i;
    session* new_session;

    // Allocazione della memoria per la nuova sessione 
    new_session = (session*)malloc(sizeof(session));
    if (new_session == NULL) {
        return NULL;
    }

    // Inizializzazione dei campi della sessione 
    strcpy(new_session->username, user);
    new_session->id = id;
    new_session->token = 0;
    new_session->token_remaining = 0;
    new_session->time = 0;
    new_session->actual_inventory_size = 0;

    // Inizializzazione dell'inventario 
    for (i = 0; i < MAX_INVENTORY_SIZE; i++) {
        memset(new_session->inventory[i], 0, MAX_ITEM_LENGTH);
    }

    // Inizializzazione dei riddles risolti 
    for (i = 0; i < MAX_OBJECT_NUMBER; i++) {
        new_session->riddle_solved[i] = false;
    }
    for (i = 0; i < MAX_OBJECT_NUMBER; i++) {
        new_session->now_visible[i] = false;
    }

    new_session->next = NULL;
    new_session->online = true;
    new_session->spy = false;
    new_session->spy_sd = -1;
    new_session->spied_by = -1;
    new_session->room = 0;
    new_session->session_init = false;

    return new_session;
}

session* checkSession(session **ses_list, const char *user){
    session *temp; 

    if (ses_list == NULL || *ses_list == NULL) {
        return NULL;
    }

    temp = *ses_list;

    while (temp != NULL) {
        if (strcmp(temp->username, user) == 0) {
            printf("Utente trovato\n");
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

session* checkSessionById(session **ses_list, int id){
    session *temp;

    if (ses_list == NULL || *ses_list == NULL) {
        return NULL;
    }

    temp = *ses_list;

    while (temp != NULL) {
        if (temp->id == id) {
            return temp; 
        }
        temp = temp->next;
    }
    return NULL;
}

bool checkActiveSession(session** ses_list){
    session *temp;

    if (ses_list == NULL || *ses_list == NULL) {
        return false;
    }

    temp = *ses_list;

    while (temp != NULL) {
        if (temp->online == true) {
            return true;
        }
        temp = temp->next;
    }
    return false;
}

void insertSession(session** ses_list, session* ses){
    if (ses_list == NULL) {
        return;
    }

    if (*ses_list == NULL) {
        *ses_list = ses; 
    } else {
        ses->next = *ses_list; 
        *ses_list = ses;
    }
}

void deleteSession(session** ses_list, session* ses){
    session *temp, *prev = NULL;

    if (ses_list == NULL || *ses_list == NULL || ses == NULL) {
        return;
    }

    temp = *ses_list;

    while (temp != NULL) {
        if (strcmp(temp->username, ses->username) == 0) {
            if (prev == NULL) {
                *ses_list = temp->next; 
            } else {
                prev->next = temp->next; 
            }
            free(temp); 
            return;
        }
        prev = temp;          
        temp = temp->next;  
    }
}