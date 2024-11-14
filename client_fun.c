#include "client_fun.h"

void sendCredentials(int sd, char* user, char* pwd, char *cmd){
    char command[9];                
    char user_buf[MAX_USERNAME_LENGTH]; 
    char pwd_buf[MAX_PASSWORD_LENGTH]; 

    // Inizializzazione dei buffer
    memset(command, 0, sizeof(command));
    memset(user_buf, 0, sizeof(user_buf));
    memset(pwd_buf, 0, sizeof(pwd_buf));

    // Copia i dati nei buffer
    strncpy(command, cmd, sizeof(command) - 1);
    command[sizeof(command) - 1] = '\0';   
    strncpy(user_buf, user, sizeof(user_buf) - 1);
    user_buf[sizeof(user_buf) - 1] = '\0';
    strncpy(pwd_buf, pwd, sizeof(pwd_buf) - 1);
    pwd_buf[sizeof(pwd_buf) - 1] = '\0';

    // Invia i dati al server
    sendToSocket(sd, command, sizeof(command));
    sendToSocket(sd, user_buf, sizeof(user_buf));
    sendToSocket(sd, pwd_buf, sizeof(pwd_buf));
}

// Alcune print sono state commentate per evitare che il client stampi troppi messaggi (sono però utili per il debug)
void switchCode(uint8_t code, bool *loop_ended){
    switch(code){
        // Login, Registrazione e Logout
        case LOGIN_SUCCESS_PREVIOUS:
            printf("Login effettuato con successo, caricamento della sessione precedente\n");
            *loop_ended = true;
            break;
        case LOGIN_SUCCESS:
            printf("Login effettuato con successo \n");
            *loop_ended = true;
            break;
        case USER_ALREADY_LOGGED:
            printf("L'utente è già loggato \n");
            break;
        case WRONG_PASSWORD:
            printf("La password inserita è errata \n");
            break;
        case USERNAME_NOT_EXIST:
            printf("L'username inserito non esiste \n");
            break;
        case USERNAME_ALREADY_EXISTS:
            printf("L'username inserito esiste già \n");
            break;
        case REGISTRATION_SUCCESS:
            printf("Registrazione effettuata con successo \n");
            printf("Effettua il login per iniziare a giocare \n");
            break;
        case LOGOUT_SUCCESS:
            printf("Logout effettuato con successo \n");
            *loop_ended = true;
            break;
        
        // Sessioni
        case SESSION_NOT_FOUND_OR_OFFLINE:
            printf("La sessione dell'utente non è stata trovata o è offline \n");
            break;
        case SESSION_END_NO_GAME:
            printf("Fine sessione in cui non era stata avviata la partita \n");
            *loop_ended = true;
            break;
        
        // Gestione Stanze
        case ROOM_SET_SUCCESS:
            //printf("Stanza impostata correttamente \n");
            *loop_ended = true;
            break;
        case ROOM_ALREADY_SET:
            printf("Stanza già impostata \n");
            break;
        case ROOMS_LIST_SENT_SUCCESS:
            //printf("Lista delle stanze inviata correttamente \n");
            break;
        case ROOM_NOT_PRESENT:
            printf("Stanza non presente \n");
            break;

        // Miscellanea
        case COMMAND_NOT_RECOGNIZED:
            printf("Comando non riconosciuto \n");
            break;
        case GAME_END:
            printf("Fine partita \n");
            *loop_ended = true;
            break;
        case DESCRIPTION_SENT_SUCCESS:
            //printf("Descrizione inviata correttamente \n");
            break;
        case VICTORY:
            printf("Vittoria \n");
            *loop_ended = true;
            break;
        case TOKEN_PICKED_SUCCESS:
            printf("Token preso correttamente \n");
            break;
        case INVENTORY_FULL:
            printf("Inventario pieno \n");
            break;

        // Gestione Oggetti
        case OBJECTS_SENT_SUCCESS:
            //printf("Oggetti inviati correttamente \n");
            break;
        case OBJECT_DROPPED_SUCCESS:
            printf("Oggetto lasciato correttamente \n");
            break;
        case OBJECT_NOT_IN_INVENTORY:
            printf("Oggetto non presente in inventario \n");
            break;
        case OBJECT_ALREADY_IN_INVENTORY:
            printf("Oggetto già presente in inventario \n");
            break;
        case OBJECT_PICKED_SUCCESS:
            printf("Oggetto preso correttamente \n");
            break;
        case OBJECT_NOT_PICKABLE:
            printf("Oggetto non prendibile \n");
            break;
        case OBJECT_NOT_PRESENT:
            printf("Oggetto non presente \n");
            break;
        case OBJECT_NOT_VISIBLE:
            printf("Oggetto non visibile \n");
            break;
        case OBJECT_NOT_IN_GAME:
            printf("Oggetto non presente nel gioco \n");
            break;
        case OBJECT_USED_SUCCESS:
            printf("Oggetto usato correttamente \n");
            break;
        case OBJECTS_NOT_COMPATIBLE:
            printf("Oggetti non compatibili \n");
            break;
        case OBJECTS_MATCH:
            printf("Oggetti uguali tra loro \n");
            break;

        // Enigmi
        case RIDDLE_TO_SOLVE:
            //printf("Enigma da risolvere \n");
            break;
        case RIDDLE_NOT_SOLVED:
            printf("Enigma non risolto \n");
            break;
        case OBJECT_BLOCKED_BY_RIDDLE:
            printf("Oggetto bloccato da un enigma, prova ad interagire con altri oggetti nella stanza per risolverlo \n");
            break;
        case RIDDLE_SOLVED_SUCCESS:
            printf("Enigma risolto correttamente \n");
            break;

        // Lista e Utenti Online
        case LIST_SENT_SUCCESS:
            //printf("Lista inviata correttamente \n");
            break;
        case NO_USERS_ONLINE:
            printf("Nessun utente online \n");
            break;

        // Spionaggio
        case SPY_MODE_ACTIVATED:
            printf("Spy mode attivata \n");
            *loop_ended = true;
            break;
        case SPY_USER_FOUND:
            printf("Utente trovato: Modalità spia avviata \n");
            break;
        case SPY_USER_NOT_FOUND:
            printf("Utente da spiare non trovato \n");
            break;
        case SPY_ALREADY_ACTIVE:
            printf("Spionaggio già attivo \n");
            break;

        // Default
        default:
            printf("Codice sconosciuto %hhu\n", code); 
            break;
    }
}

bool sendRoom(int sd, uint8_t room, bool *loop_ended){
    char command[6];
    uint8_t code;

    // Inizializzazione del buffer
    memset(command, 0, sizeof(command));
    strncpy(command, "start", sizeof(command) - 1);
    command[sizeof(command) - 1] = '\0';

    // Invio del comando start <room> al server
    sendToSocket(sd, command, sizeof(command));
    sendToSocket(sd, &room, sizeof(room)); 
    recvFromSocket(sd, &code, sizeof(code));
    switchCode(code, loop_ended);

    if (code == ROOM_SET_SUCCESS){
        char buf[MAX_DESCRIPTION_LENGTH];
        memset(buf, 0, sizeof(buf));
        recvFromSocket(sd, buf, sizeof(buf));
        printf("%s\n", buf);
        //printf("Il gioco è iniziato\n");
        return true;
    }
    return false;
}

void handleServerResponse(int sd, uint8_t code, time_t start_time, uint16_t room_time, bool *game_ended){
    char buf[MAX_DESCRIPTION_LENGTH];
    time_t cur_time;
    uint16_t time_remaining;

    if (code == DESCRIPTION_SENT_SUCCESS || code == OBJECTS_SENT_SUCCESS) {
        memset(buf, 0, sizeof(buf));
        recvFromSocket(sd, buf, sizeof(buf));
        printf("%s\n", buf);
    }
    else if (code == GAME_END) {
        uint8_t token;
        uint8_t token_remaining;
        recvFromSocket(sd, &token, sizeof(token));
        recvFromSocket(sd, &token_remaining, sizeof(token_remaining));

        time(&cur_time);
        time_remaining = room_time - difftime(cur_time, start_time);
         
        printf("Hai preso %d token, hai %d token rimanenti e ti sono rimasti %d secondi\n", token - token_remaining, token_remaining, time_remaining);
        closeSocket(sd);
        *game_ended = true;
    }
    else if (code == RIDDLE_TO_SOLVE) {
        char riddle[MAX_DESCRIPTION_LENGTH];
        char riddle_ans[MAX_RIDDLE_ANS_LENGTH];
        memset(riddle, 0, sizeof(riddle));

        // Ricezione dell'enigma dal server
        recvFromSocket(sd, riddle, sizeof(riddle));
        printf("Enigma: %s\n", riddle);
        memset(riddle_ans, 0, sizeof(riddle_ans));
        fgets(riddle_ans, sizeof(riddle_ans), stdin);
        // Rimozione del carattere di newline
        if (strlen(riddle_ans) > 0 && riddle_ans[strlen(riddle_ans) - 1] == '\n') {
            riddle_ans[strlen(riddle_ans) - 1] = '\0';
        }

        // Invio della risposta dell'enigma al server
        sendToSocket(sd, riddle_ans, sizeof(riddle_ans));
        recvFromSocket(sd, &code, sizeof(code));
        switchCode(code, game_ended);
        recvFromSocket(sd, &code, sizeof(code));
        if (code != RIDDLE_SOLVED_SUCCESS && code != RIDDLE_NOT_SOLVED){
            switchCode(code, game_ended);
        }
    }
    else if (code == LOGOUT_SUCCESS){
        printf("Il logout è avvenuto con successo e ora il client terminerà, la tua sessione è stata salvata correttamente e potrai recuperarla rifacendo il login\n");
        closeSocket(sd);
        *game_ended = true;
    }
    else if (code == OBJECT_USED_SUCCESS){
        memset(buf, 0, sizeof(buf));
        recvFromSocket(sd, buf, sizeof(buf));
        printf("%s\n", buf);
        recvFromSocket(sd, &code, sizeof(code));
        if (code != OBJECT_USED_SUCCESS){
            switchCode(code, game_ended);
        }
    }

    // Gestione del codice di vittoria
    if (code == VICTORY){
        time(&cur_time);
        time_remaining = room_time - difftime(cur_time, start_time);
        printf("Tempo rimanente: %d secondi\n", time_remaining);
        closeSocket(sd);
        *game_ended = true;
    }
}