#include "server_fun.h"

bool checkUsername(const char* user) {
    // Apertura del file in lettura binaria
    FILE *f = fopen("account.bin", "rb");
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];

    if (!f) {
        perror("Errore nell'apertura del file");
        return false;
    }

    // Legge il file fino alla fine, controllando se l'username è presente
    while (fread(username, sizeof(char), MAX_USERNAME_LENGTH, f) == MAX_USERNAME_LENGTH) {
        username[MAX_USERNAME_LENGTH - 1] = '\0';
        if (fread(password, sizeof(char), MAX_PASSWORD_LENGTH, f) != MAX_PASSWORD_LENGTH) {
            perror("Errore nella lettura del file");
            fclose(f);
            return false;
        }
        if (strcmp(username, user) == 0) {
            fclose(f);
            return true;
        }
    }

    fclose(f);
    return false;
}

bool checkPassword(const char* user, const char* pwd) {
    // Apertura del file in lettura binaria
    FILE *f = fopen("account.bin", "rb");
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];

    if (!f) {
        perror("Errore nell'apertura del file");
        return false;
    }

    // Legge il file fino alla fine, controllando se l'username e la password sono corretti
    while (fread(username, sizeof(char), MAX_USERNAME_LENGTH, f) == MAX_USERNAME_LENGTH) {
        username[MAX_USERNAME_LENGTH - 1] = '\0';
        if (fread(password, sizeof(char), MAX_PASSWORD_LENGTH, f) != MAX_PASSWORD_LENGTH) {
            perror("Errore nella lettura del file");
            fclose(f);
            return false;
        }
        password[MAX_PASSWORD_LENGTH - 1] = '\0';
        if (strcmp(username, user) == 0 && strcmp(password, pwd) == 0) {
            fclose(f);
            return true;
        }
    }

    fclose(f);
    return false;
}

bool saveToFile(const char* user, const char* pwd) {
    // Apertura del file in append binario
    FILE *f = fopen("account.bin", "ab");
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];

    if (!f) {
        perror("Errore nell'apertura del file");
        return false;
    }

    // Pulisce i buffer
    memset(username, 0, sizeof(username));
    memset(password, 0, sizeof(password));

    // Copia i dati negli array di caratteri
    strncpy(username, user, sizeof(username) - 1);
    strncpy(password, pwd, sizeof(password) - 1);

    // Scrive i dati nel file
    if (fwrite(username, sizeof(char), MAX_USERNAME_LENGTH, f) != MAX_USERNAME_LENGTH) {
        perror("Errore nella scrittura del file");
        fclose(f);
        return false;
    }

    if (fwrite(password, sizeof(char), MAX_PASSWORD_LENGTH, f) != MAX_PASSWORD_LENGTH) {
        perror("Errore nella scrittura del file");
        fclose(f);
        return false;
    }

    fclose(f);
    return true;
}

void recvCredentials(int sd, char* user, char* pwd) {
    recvFromSocket(sd, user, MAX_USERNAME_LENGTH);
    recvFromSocket(sd, pwd, MAX_PASSWORD_LENGTH);
    user[MAX_USERNAME_LENGTH - 1] = '\0';
    pwd[MAX_PASSWORD_LENGTH - 1] = '\0';
}

void setRoom(int sd, session* ses, uint8_t n_room, room* container) {
    uint16_t time;
    // Adatta l'ID della stanza da 1-n a 0-(n-1)
    ses->room = n_room - 1;
    ses->token = container[ses->room].token;
    ses->token_remaining = container[ses->room].token;
    ses->session_init = true;

    time = htons(container[ses->room].time);
    sendToSocket(sd, container[ses->room].description, sizeof(container[ses->room].description));
    sendToSocket(sd, &time, sizeof(time));
}

void sendToSpy(session* ses, const char* arg) {
    char buf[MAX_DESCRIPTION_LENGTH];
    uint16_t net_size;
    uint16_t size = strlen(arg) + 1;

    memset(buf, 0, sizeof(buf));
    strncpy(buf, arg, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    net_size = htons(size);
    sendToSocket(ses->spied_by, &net_size, sizeof(net_size));
    sendToSocket(ses->spied_by, buf, size);
}

void sendCodeToSpy(session* ses, uint8_t* code) {
    int16_t net_size;
    uint16_t size = sizeof(*code);
    net_size = htons(size);
    sendToSocket(ses->spied_by, &net_size, sizeof(net_size));
    sendToSocket(ses->spied_by, code, size);
}

void checkVictory(int sd, session* ses, session** ses_list) {
    uint8_t code;
    // Se il giocatore ha raccolto tutti i token, invia il codice di vittoria
    if (ses->token_remaining == 0) {
        code = VICTORY;
        sendToSocket(sd, &code, sizeof(code));

        // Gestione della sessione spiata, se presente
        if (ses->spied_by != -1) {
            sendCodeToSpy(ses, &code);
        }

        deleteSession(ses_list, ses);
    }
    else {
        code = TOKEN_PICKED_SUCCESS;
        sendToSocket(sd, &code, sizeof(code));

        // Gestione della sessione spiata, se presente
        if (ses->spied_by != -1) {
            sendCodeToSpy(ses, &code);
        }
    }
}

bool handleLogin(const char* username, const char* password, session** ses_list, room* container, int i, uint8_t* code) {
    session *ses;
    if (checkUsername(username) && checkPassword(username, password)) {
        ses = checkSession(ses_list, username);
        if (ses != NULL && ses->online) {
            // Utente già loggato
            *code = USER_ALREADY_LOGGED;
        }
        else if (ses != NULL && !ses->online) {
            uint16_t net_time;
            // Riprende una sessione precedente
            ses->online = true;
            *code = LOGIN_SUCCESS_PREVIOUS;
            sendToSocket(i, code, sizeof(*code));
            sendToSocket(i, &ses->room, sizeof(ses->room));
            net_time = htons(ses->time);
            sendToSocket(i, &net_time, sizeof(net_time));
            sendToSocket(i, container[ses->room].description, sizeof(container[ses->room].description));
            return true;
        }
        else {   
            // Crea una nuova sessione per l'utente
            session *new_session = newSession(username, i);
            if (new_session == NULL) {
                perror("Errore nella creazione della sessione");
                exit(1);
            }
            insertSession(ses_list, new_session);
            *code = LOGIN_SUCCESS;
        }
    }
    else if (checkUsername(username) && !checkPassword(username, password)) {
        // Password errata
        *code = WRONG_PASSWORD; 
    }
    else {
       // Username inesistente
        *code = USERNAME_NOT_EXIST;
    }
    return false;
}

void handleRegister(const char* username, const char* password, uint8_t* code) {
    if (checkUsername(username)) {
        // Username già esistente
        *code = USERNAME_ALREADY_EXISTS;
    }
    else {
        // Registrazione effettuata con successo
        *code = REGISTRATION_SUCCESS;
        if(!saveToFile(username, password)){
            perror("Errore nel salvataggio delle credenziali");
            exit(1);
        }
    }
}

void handleLogout(char* full_cmd, session** ses_list, int i, uint8_t* code) {
    session *ses;
    ses = checkSessionById(ses_list, i);
    uint16_t time, net_time;
    
    // Se l'utente è spiato, invia full_cmd al socket del client spia
    if (ses != NULL && ses->spied_by != -1) {
        sendToSpy(ses, full_cmd);
    }  
    
    recvFromSocket(i, &net_time, sizeof(net_time));
    time = ntohs(net_time);
    
    if (ses != NULL && ses->online) {
        ses->online = false;
        ses->time = time;
        // Logout effettuato con successo
        *code = LOGOUT_SUCCESS;
    }
    else {
        // Sessione non trovata o utente già offline
        *code = SESSION_NOT_FOUND_OR_OFFLINE;
    }
}

bool handleStart(session** ses_list, room* container, int i, uint8_t* code) {
    uint8_t n_room;
    // Riceve il numero della stanza dal client
    recvFromSocket(i, &n_room, sizeof(n_room));

    if (n_room == MAX_ROOM_NUMBER + 1) {
        char buf[MAX_DESCRIPTION_LENGTH];
        int j;
        // Codice per indicare che la lista è stata inviata correttamente
        *code = ROOMS_LIST_SENT_SUCCESS;   
        memset(buf, 0, sizeof(buf));

        // Invio della lista delle stanze nel formato "numero stanza) nome stanza"
        // Prima genera il buffer e poi effettua una singola send
        for (j = 0; j < MAX_ROOM_NUMBER; j++) {
            if (strcmp(container[j].name, "") != 0) {
                snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), "%d) %s\n", j + 1, container[j].name);
            }
        }
        sendToSocket(i, code, sizeof(*code));
        sendToSocket(i, buf, sizeof(buf));
        return true; 
    }
    else {
        if (strcmp(container[n_room - 1].name, "") == 0) {
            // Stanza non presente
            *code = ROOM_NOT_PRESENT;
        }
        else {
            session *ses = checkSessionById(ses_list, i);
            if (ses != NULL && ses->session_init == false) {
                // Stanza impostata correttamente
                *code = ROOM_SET_SUCCESS;
                sendToSocket(i, code, sizeof(*code));
                setRoom(i, ses, n_room, container);
            }
            else if (ses != NULL && ses->session_init == true) {
                // Stanza già impostata
                *code = ROOM_ALREADY_SET;
            }
            else {
                // Sessione dell'utente non trovata o offline
                *code = SESSION_NOT_FOUND_OR_OFFLINE;
            }
        }
    }
    return false;
}

bool handleLook(char* full_cmd, session** ses_list, room* container, int i, uint8_t* code) {
    char arg1[MAX_ARG_LENGTH];
    // Recupera la sessione dell'utente
    session *ses = checkSessionById(ses_list, i);
    // Riceve l'argomento della comando "look" dal socket
    recvFromSocket(i, arg1, sizeof(arg1));
    // Aggiunge l'argomento appena ricevuto a full_cmd
    if (strcmp(arg1, "") != 0){
        snprintf(full_cmd + strlen(full_cmd), sizeof(full_cmd) - strlen(full_cmd), " %s", arg1);
    }
    if (ses != NULL && ses->online == true) {
        char buf[MAX_DESCRIPTION_LENGTH];
        bool found = false;
        // Se l'utente è sotto spionaggio, invia il comando completo al client spia
        if (ses->spied_by != -1) {
            sendToSpy(ses, full_cmd);
        }   
        memset(buf, 0, sizeof(buf));
        if (strcmp(arg1, "") == 0) {
            // Se nessun argomento è fornito, invia la descrizione della stanza
            strncpy(buf, container[ses->room].description, MAX_DESCRIPTION_LENGTH - 1);
            buf[MAX_DESCRIPTION_LENGTH - 1] = '\0';
            found = true;
        }
        else {
            int j;
            // Cerca la descrizione di una location nella stanza
            for (j = 0; j < MAX_LOCATION_NUMBER; j++) {
                if (strcmp(container[ses->room].loc[j].name, arg1) == 0) {
                    strncpy(buf, container[ses->room].loc[j].description, MAX_DESCRIPTION_LENGTH - 1);
                    buf[MAX_DESCRIPTION_LENGTH - 1] = '\0';
                    found = true;
                    break;
                }
            }
            if (found == false) {
                // Se non trova la location, cerca un oggetto nella stanza
                for (j = 0; j < MAX_OBJECT_NUMBER; j++) {
                    if (strcmp(container[ses->room].obj[j].name, arg1) == 0) {
                        if (container[ses->room].obj[j].visible == true || ses->now_visible[j] == true) {
                            // Se l'oggetto è visibile o è stato risolto un enigma relativo
                            if (strcmp(container[ses->room].obj[j].new_description, "") != 0 && ses->now_visible[j] == true) {
                                strncpy(buf, container[ses->room].obj[j].new_description, MAX_DESCRIPTION_LENGTH - 1);
                            }
                            else {
                                strncpy(buf, container[ses->room].obj[j].description, MAX_DESCRIPTION_LENGTH - 1);
                            }
                            buf[MAX_DESCRIPTION_LENGTH - 1] = '\0';
                            found = true;
                            break;
                        }
                        else {
                            // Oggetto non visibile
                            *code = OBJECT_NOT_PRESENT;
                            break;
                        }
                    }
                }
            }
            if (found == false) {
                // Oggetto o location non presente
                *code = OBJECT_NOT_PRESENT;
            }
        }
        if (found == true) {
            // Descrizione inviata correttamente
            *code = DESCRIPTION_SENT_SUCCESS;
            sendToSocket(i, code, sizeof(*code));
            if (ses->spied_by != -1) {
                sendCodeToSpy(ses, code);
            } 
            sendToSocket(i, buf, sizeof(buf));
            if (ses->spied_by != -1) {
                sendToSpy(ses, buf);
            } 
            return true;
        }   
    }
    else {
        // Sessione dell'utente non trovata o offline
        *code = SESSION_NOT_FOUND_OR_OFFLINE;
    }
    return false;
}

bool handleTake(char* full_cmd, session** ses_list, room* container, int i, uint8_t* code) {
    char arg1[MAX_ARG_LENGTH];
    // Recupera la sessione dell'utente
    session *ses = checkSessionById(ses_list, i);
    // Riceve l'argomento del comando "take" dal socket
    recvFromSocket(i, arg1, sizeof(arg1));
    // Aggiunge l'argomento al comando completo
    if (strcmp(arg1, "") != 0){
        snprintf(full_cmd + strlen(full_cmd), sizeof(full_cmd) - strlen(full_cmd), " %s", arg1);
    }
     
    if (ses != NULL && ses->online == true) {  
        bool in_inventory = false;
        int j; 
        bool takeable = true;
        bool riddle_solved = false;
        // Se l'utente è sotto spionaggio, invia una copia del comando al client spia
        if (ses->spied_by != -1) {
            sendToSpy(ses, full_cmd);
        }          

        // Verifica se l'oggetto è già presente nell'inventario
        for (j = 0; j < MAX_INVENTORY_SIZE; j++) {
            if (strcmp(ses->inventory[j], arg1) == 0) {
                // Oggetto già presente in inventario
                *code = OBJECT_ALREADY_IN_INVENTORY;
                in_inventory = true;
                break;
            }
        }

        if (in_inventory == false){
            bool found = false;
            // Cerca l'oggetto nella stanza
            for (j = 0; j < MAX_OBJECT_NUMBER; j++) {
                if (strcmp(container[ses->room].obj[j].name, arg1) == 0) {
                    if (container[ses->room].obj[j].riddle == true && ses->riddle_solved[j] == false) {
                        if (strcmp(container[ses->room].obj[j].rdl_description, "") != 0) {
                            char riddle_answer[MAX_RIDDLE_ANS_LENGTH];
                            found = true;
                            // Enigma da risolvere
                            *code = RIDDLE_TO_SOLVE;
                            sendToSocket(i, code, sizeof(*code));
                            if (ses->spied_by != -1) {
                                sendCodeToSpy(ses, code);
                            } 
                            // Invia la descrizione dell'enigma al client
                            sendToSocket(i, container[ses->room].obj[j].rdl_description, sizeof(container[ses->room].obj[j].rdl_description));
                            if (ses->spied_by != -1) {
                                sendToSpy(ses, container[ses->room].obj[j].rdl_description);
                            } 
                            // Riceve la risposta dell'enigma dal client
                            recvFromSocket(i, riddle_answer, sizeof(riddle_answer));
                            // Verifica la risposta
                            if (strcmp(container[ses->room].obj[j].rdl_answer, riddle_answer) == 0) {
                                int k;
                                ses->now_visible[j] = true;
                                riddle_solved = true;

                                // Sblocca gli oggetti associati all'enigma
                                for (k = 0; k < MAX_OBJECT_UNLOCKABLE; k++) {
                                    if (container[ses->room].obj[j].item_unlocked[k] != j) {
                                        ses->now_visible[container[ses->room].obj[j].item_unlocked[k]] = true;
                                    }
                                }
                                // Enigma risolto correttamente
                                *code = RIDDLE_SOLVED_SUCCESS;
                            }
                            else {
                                // Enigma non risolto
                                *code = RIDDLE_NOT_SOLVED;
                            }
                            break;
                        }
                        else {
                            // Oggetto bloccato da un enigma
                            *code = OBJECT_BLOCKED_BY_RIDDLE;
                            takeable = false;  
                            break;
                        }
                    }
                    else if (container[ses->room].obj[j].takeable == true && (container[ses->room].obj[j].visible == true || ses->now_visible[j] == true)) {
                        int k;
                        // Aggiunge l'oggetto all'inventario se c'è spazio
                        for (k = 0; k < MAX_INVENTORY_SIZE; k++) {
                            if (ses->actual_inventory_size < container[ses->room].inventory_size - 1 && ses->inventory[k][0] == '\0') {
                                strncpy(ses->inventory[k], arg1, MAX_ITEM_LENGTH - 1);
                                ses->inventory[k][MAX_ITEM_LENGTH - 1] = '\0';
                                ses->actual_inventory_size++;
                                found = true;
                                // Oggetto preso correttamente
                                *code = OBJECT_PICKED_SUCCESS;
                                break;
                            }
                        }
                        if (found == false) {
                            // Inventario pieno
                            *code = INVENTORY_FULL;
                            takeable = false;
                            break;
                        }
                    }
                    else {
                        // Oggetto non prendibile
                        *code = OBJECT_NOT_PICKABLE;
                        takeable = false;
                        break;
                    }
                }
            }
            if (takeable == true && found == false) {
                // Oggetto non presente 
                *code = OBJECT_NOT_PRESENT;
            }
            if (*code == RIDDLE_NOT_SOLVED || *code == RIDDLE_SOLVED_SUCCESS) {
                sendToSocket(i, code, sizeof(*code));
                if (ses->spied_by != -1) {
                    sendCodeToSpy(ses, code);
                } 
                // Se l'oggetto è un token, decrementa il conteggio e verifica la vittoria
                
                if (riddle_solved == true && ses->riddle_solved[j] == false) {
                    ses->riddle_solved[j] = true;
                    if (container[ses->room].obj[j].token == true){
                        ses->token_remaining--;
                        checkVictory(i, ses, ses_list);
                    }
                    else {
                        *code = RIDDLE_SOLVED_SUCCESS;
                        sendToSocket(i, code, sizeof(*code));
                        if (ses->spied_by != -1) {
                            sendCodeToSpy(ses, code);
                        }
                    }
                }
                else {
                    sendToSocket(i, code, sizeof(*code));
                    if (ses->spied_by != -1) {
                        sendCodeToSpy(ses, code);
                    } 
                }

                return true; 
            }
        }
    }
    else {
        // Sessione dell'utente non trovata o offline
        *code = SESSION_NOT_FOUND_OR_OFFLINE;
    }
    return false;
}

bool handleUse(char* full_cmd, session** ses_list, room* container, int i, uint8_t* code) {
    char arg1[MAX_ARG_LENGTH];
    char arg2[MAX_ARG_LENGTH];
    // Recupera la sessione dell'utente
    session *ses = checkSessionById(ses_list, i);
    // Riceve il primo argomento del comando "use" dal socket
    recvFromSocket(i, arg1, sizeof(arg1));
    // Aggiunge il primo argomento al comando completo
    if (strcmp(arg1, "") != 0){
        snprintf(full_cmd + strlen(full_cmd), sizeof(full_cmd) - strlen(full_cmd), " %s", arg1);
    }  
    // Riceve il secondo argomento del comando "use" dal socket
    recvFromSocket(i, arg2, sizeof(arg2));
    // Aggiunge il secondo argomento al comando completo
    if (strcmp(arg2, "") != 0){
        snprintf(full_cmd + strlen(full_cmd), sizeof(full_cmd) - strlen(full_cmd), " %s", arg2);
    }
    if (ses != NULL && ses->online == true) {
        bool found_obj1 = false;
        bool found_obj2 = false;
        bool is_obj2_null = false;
        bool is_obj2_in_inventory = false;
        int index_obj1 = -1;
        int index_obj2 = -1;
        int j;
        bool riddle_solved = false;
        char buf[MAX_DESCRIPTION_LENGTH];
        // Se l'utente è sotto spionaggio, invia una copia del comando al client spia
        if (ses->spied_by != -1) {
            sendToSpy(ses, full_cmd);
        }  
        memset(buf, 0, sizeof(buf));

        // Verifica se i due oggetti sono uguali
        if (strcmp(arg1, arg2) == 0) {
            // Oggetti uguali
            *code = OBJECTS_MATCH;
            sendToSocket(i, code, sizeof(*code));
            if (ses->spied_by != -1) {
                sendCodeToSpy(ses, code);
            } 
            return true;
        }

        // Verifica se il secondo oggetto è nullo
        if (strcmp(arg2, "") == 0) {
            is_obj2_null = true;
        }

        // Cerca gli oggetti nell'inventario
        for (j = 0; j < MAX_INVENTORY_SIZE; j++) {
            if (strcmp(ses->inventory[j], arg1) == 0) {
                found_obj1 = true;
            }
            if (is_obj2_null == false && strcmp(ses->inventory[j], arg2) == 0) {
                found_obj2 = true;
                index_obj2 = j;
                is_obj2_in_inventory = true;
            }
        }
        if (found_obj1 == true){
            // Associa all'oggetto l'indice dell'oggetto nella stanza
            for (j = 0; j < MAX_OBJECT_NUMBER; j++) {
                if (strcmp(container[ses->room].obj[j].name, arg1) == 0) {
                    if (container[ses->room].obj[j].visible == true || ses->now_visible[j] == true) {
                        index_obj1 = j;
                        break;
                    }
                }
            }
            // Se il secondo oggetto non è stato trovato, lo cerca nella stanza
            if (found_obj2 == false && is_obj2_null == false){
                for (j = 0; j < MAX_OBJECT_NUMBER; j++) {
                    if (strcmp(container[ses->room].obj[j].name, arg2) == 0) {
                        if (container[ses->room].obj[j].visible == true || ses->now_visible[j] == true) {
                            found_obj2 = true;
                            index_obj2 = j;
                            break;
                        }
                    }
                }
            } 
            if (found_obj2 == false && is_obj2_null == false){
                // Oggetto non presente nella stanza
                *code = OBJECT_NOT_PRESENT;
                sendToSocket(i, code, sizeof(*code));
                if (ses->spied_by != -1) {
                    sendCodeToSpy(ses, code);
                } 
                return true;
            }

            if (is_obj2_null == true){
                if (container[ses->room].obj[index_obj1].riddle == true && ses->riddle_solved[index_obj1] == false) {
                    int k;
                    ses->now_visible[index_obj1] = true;
                    // Sblocca gli oggetti associati all'enigma
                    for (k = 0; k < MAX_OBJECT_UNLOCKABLE; k++) {
                        if (container[ses->room].obj[index_obj1].item_unlocked[k] != index_obj1) {
                            ses->now_visible[container[ses->room].obj[index_obj1].item_unlocked[k]] = true;
                        }
                    }            
                    riddle_solved = true;
                }
                // Copia la descrizione dopo l'uso singolo dell'oggetto
                strncpy(buf, container[ses->room].obj[index_obj1].description_after_usage_alone, MAX_DESCRIPTION_LENGTH - 1);
                buf[MAX_DESCRIPTION_LENGTH - 1] = '\0';
            }   
            else {
                // Controlla se i due oggetti sono compatibili
                if (strcmp(container[ses->room].obj[index_obj1].used_with, arg2) == 0){
                    object *obj2;
                    if (is_obj2_in_inventory == true){
                        obj2 = &container[ses->room].obj[index_obj2];
                    }
                    else {
                        obj2 = &container[ses->room].obj[index_obj2];
                    }

                    // Verifica se uno dei due oggetti ha un enigma associato
                    if ((container[ses->room].obj[index_obj1].riddle == true || obj2->riddle == true) && 
                    (ses->riddle_solved[index_obj1] == false || ses->riddle_solved[index_obj2] == false)) {
                        int k;
                        ses->now_visible[index_obj1] = true;
                        ses->now_visible[index_obj2] = true;

                        // Sblocca gli oggetti associati agli enigmi
                        for (k = 0; k < MAX_OBJECT_UNLOCKABLE; k++) {
                            if (container[ses->room].obj[index_obj1].item_unlocked[k] != index_obj1) {
                                ses->now_visible[container[ses->room].obj[index_obj1].item_unlocked[k]] = true;
                            }
                            if (obj2->item_unlocked[k] != index_obj2) {
                                ses->now_visible[obj2->item_unlocked[k]] = true;
                            }
                        }  

                        riddle_solved = true;
                    }
                    // Copia la descrizione dopo l'uso combinato degli oggetti
                    strncpy(buf, container[ses->room].obj[index_obj1].description_after_usage, MAX_DESCRIPTION_LENGTH - 1);
                    buf[MAX_DESCRIPTION_LENGTH - 1] = '\0';
                }
                else {
                    // Oggetti non compatibili
                    *code = OBJECTS_NOT_COMPATIBLE;
                    sendToSocket(i, code, sizeof(*code));
                    if (ses->spied_by != -1) {
                        sendCodeToSpy(ses, code);
                    } 
                    return true;
                }
            }

            // Rimuove l'oggetto usato dall'inventario
            for (j = 0; j < MAX_INVENTORY_SIZE; j++) {
                if (strcmp(ses->inventory[j], container[ses->room].obj[index_obj1].name) == 0) {
                    memset(ses->inventory[j], 0, sizeof(ses->inventory[j]));
                    ses->actual_inventory_size--;
                    break;
                }
            }
            // Oggetto usato correttamente
            *code = OBJECT_USED_SUCCESS;
            sendToSocket(i, code, sizeof(*code));
            if (ses->spied_by != -1) {
                sendCodeToSpy(ses, code);
            }
            // Invia la descrizione dell'uso dell'oggetto
            sendToSocket(i, buf, sizeof(buf));
            if (ses->spied_by != -1) {
                sendToSpy(ses, buf);
            } 

            // Verifica se l'oggetto è un token e aggiorna lo stato di vittoria
            if (riddle_solved == true && (ses->riddle_solved[index_obj1] == false || ses->riddle_solved[index_obj2] == false)) {
                ses->riddle_solved[index_obj1] = true;
                if (found_obj2 == true) {
                    ses->riddle_solved[index_obj2] = true;
                }
                if (container[ses->room].obj[index_obj1].token == true){
                    ses->token_remaining--;
                    checkVictory(i, ses, ses_list);
                }
                else {
                    *code = RIDDLE_SOLVED_SUCCESS;
                    sendToSocket(i, code, sizeof(*code));
                    if (ses->spied_by != -1) {
                        sendCodeToSpy(ses, code);
                    }
                }
            }
            else {
                sendToSocket(i, code, sizeof(*code));
                if (ses->spied_by != -1) {
                    sendCodeToSpy(ses, code);
                } 
            }
            return true; 
        }
        else {
            // Oggetto non presente nell'inventario
            *code = OBJECT_NOT_IN_INVENTORY;
        }
    }
    else {
        // Sessione dell'utente non trovata o offline
        *code = SESSION_NOT_FOUND_OR_OFFLINE;
    }
    return false;
}

bool handleObjs(char* full_cmd, session** ses_list, int i, uint8_t* code) {
    session *ses = checkSessionById(ses_list, i);
    if (ses != NULL && ses->online == true) {
        char buf[MAX_DESCRIPTION_LENGTH];
        int j;
        bool vuoto = true;
        // Se l'utente è sotto spionaggio, invia una copia del comando al client spia
        if (ses->spied_by != -1) {
            sendToSpy(ses, full_cmd);
        }  
        memset(buf, 0, sizeof(buf));
        // Compila la lista degli oggetti nell'inventario
        for (j = 0; j < MAX_INVENTORY_SIZE; j++) {
            if (ses->inventory[j][0] != 0) {
                strcat(buf, ses->inventory[j]);
                strcat(buf, "\n");
                vuoto = false;
            }
        }
        if (vuoto == true){
            strncpy(buf, "Inventario vuoto", MAX_DESCRIPTION_LENGTH - 1);
        }
        // Oggetti inviati correttamente
        *code = OBJECTS_SENT_SUCCESS;
        sendToSocket(i, code, sizeof(*code));
        if (ses->spied_by != -1) {
            sendCodeToSpy(ses, code);
        }
        sendToSocket(i, buf, sizeof(buf));
        if (ses->spied_by != -1) {
            sendToSpy(ses, buf);
        }
        return true;    
    }
    else {
        // Sessione dell'utente non trovata o offline
        *code = SESSION_NOT_FOUND_OR_OFFLINE;
    }
    return false;
}

bool handleEnd(char* full_cmd, session** ses_list, int i, uint8_t* code) {
    session *ses = checkSessionById(ses_list, i);
    if (ses != NULL && ses->session_init == true) {
        if (ses->spied_by != -1) {
            sendToSpy(ses, full_cmd);
        } 
        ses->online = false;
        // Fine partita
        *code = GAME_END;
        sendToSocket(i, code, sizeof(*code));
        if (ses->spied_by != -1) {
            sendCodeToSpy(ses, code);
        }
        // Invio token presi, token totali e tempo rimanente
        sendToSocket(i, &ses->token, sizeof(ses->token));
        // Trattiamo i token come codici, perche hanno la stessa dimensione
        if (ses->spied_by != -1) {
            sendCodeToSpy(ses, &(ses->token));
        } 
        sendToSocket(i, &ses->token_remaining, sizeof(ses->token_remaining));
        if (ses->spied_by != -1) {
            sendCodeToSpy(ses, &(ses->token_remaining));
        }
        deleteSession(ses_list, ses);
        return true;
    }
    else if (ses != NULL && ses->session_init == false){
        deleteSession(ses_list, ses);
        // Fine sessione in cui non è stata iniziata la partita
        *code = SESSION_END_NO_GAME;
    }
    else {
        // Sessione dell'utente non trovata o offline
        *code = SESSION_NOT_FOUND_OR_OFFLINE;
    }
    return false;
}

void handleDrop(char* full_cmd, session** ses_list, int i, uint8_t* code) {
    char arg1[MAX_ARG_LENGTH];
    session *ses = checkSessionById(ses_list, i);
    bool found = false;
    // Riceve l'argomento del comando "drop" dal socket
    recvFromSocket(i, arg1, sizeof(arg1));
    // Aggiunge l'argomento al comando completo
    if (strcmp(arg1, "") != 0){
        snprintf(full_cmd + strlen(full_cmd), sizeof(full_cmd) - strlen(full_cmd), " %s", arg1);
    }
    if (ses != NULL && ses->online == true) {
        int j;
        // Se l'utente è sotto spionaggio, invia una copia del comando al client spia
        if (ses->spied_by != -1) {
            sendToSpy(ses, full_cmd);
        }     
        // Cerca l'oggetto nell'inventario
        for (j = 0; j < MAX_INVENTORY_SIZE; j++) {
            if (strcmp(ses->inventory[j], arg1) == 0) {
                memset(ses->inventory[j], 0, sizeof(ses->inventory[j]));
                ses->actual_inventory_size--;
                found = true;
                // Oggetto lasciato correttamente
                *code = OBJECT_DROPPED_SUCCESS;
                break;
            }
        }
        if (found == false) {
            // Oggetto non presente in inventario
            *code = OBJECT_NOT_IN_INVENTORY;
        }
    }
    else {
        // Sessione dell'utente non trovata o offline
        *code = SESSION_NOT_FOUND_OR_OFFLINE;
    }
}

void handleList(session** ses_list, int i, uint8_t* code) {
    if (checkActiveSession(ses_list) == true) {
        char buf[MAX_DESCRIPTION_LENGTH];
        session *current = *ses_list;
        memset(buf, 0, sizeof(buf));    
        while (current != NULL) {
            if (current->session_init == true  && current->spy == false) {
                strcat(buf, current->username);
                strcat(buf, "\n");
            }
            current = current->next;
        }
        if (strcmp(buf, "") == 0) {
            strncpy(buf, "Nessun utente online", MAX_DESCRIPTION_LENGTH - 1);
        }

        // Lista inviata correttamente
        *code = LIST_SENT_SUCCESS;
        sendToSocket(i, code, sizeof(*code));
        sendToSocket(i, buf, sizeof(buf));
        return;
    }
    else {
        // Nessun utente online
        *code = NO_USERS_ONLINE;
    }
    sendToSocket(i, code, sizeof(*code));
}

void handleSpyMode(session** ses_list, int i, uint8_t* code) {
    session *ses = checkSessionById(ses_list, i);
    if (ses != NULL && ses->online == true){
        ses->spy = true;
        // Spymode attivata
        *code = SPY_MODE_ACTIVATED;
    }
    else {
        // Sessione dell'utente non trovata o offline
        *code = SESSION_NOT_FOUND_OR_OFFLINE;
    }
    sendToSocket(i, code, sizeof(*code));
}

void handleSpy(session** ses_list, int i, uint8_t* code) {
    char user[MAX_ARG_LENGTH];
    session *ses = checkSessionById(ses_list, i);
    // Riceve l'argomento del comando "spy" dal socket
    recvFromSocket(i, user, sizeof(user));
    if (ses != NULL && ses->online == true && ses->spy == true){
        if (ses->spy_sd == -1 && ses->spied_by == -1){
            session *to_spy = checkSession(ses_list, user);
            if (to_spy != NULL && to_spy->session_init == true){
                ses->spy_sd = to_spy->id;
                to_spy->spied_by = ses->id;
                // Utente da spiare trovato
                *code = SPY_USER_FOUND;
            }
            else {
                // Utente da spiare non trovato
                *code = SPY_USER_NOT_FOUND;
            }  
        }
        else {
            // Spionaggio già attivo
            *code = SPY_ALREADY_ACTIVE;
        }       
    }
    else {
        // Sessione dell'utente non trovata o offline
        *code = SESSION_NOT_FOUND_OR_OFFLINE;
    }
    sendToSocket(i, code, sizeof(*code));
}

