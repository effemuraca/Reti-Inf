#include "general.h"     
#include "socket_fun.h"   
#include "utility.h"      
#include "client_fun.h"   

int main(int argc, char* argv[]){ 
    char io_client[127];                    
    char cmd_client[9];                    
    uint8_t room = 0;                      
    uint8_t code;                            
    bool spy = false;                      
    sockaddr_in addr;                      
    int sd;                                
    int ret;                              
    bool login_ended = false;              
    char buf[MAX_DESCRIPTION_LENGTH];       
    bool game_ended = false;               
    time_t start_time, cur_time;           
    uint16_t time_remaining, room_time;    

    // Verifica degli argomenti della riga di comando
    if (argc != 2) {
        printf("Errore nell'esecuzione del file client \n");
        printf("La sintassi corretta è './client <porta>' \n");
        return 0;
    }

    // Costruzione dell'indirizzo del server e inizializzazione del socket
    buildAddress(&addr, atoi(argv[1]));
    memset(io_client, 0, sizeof(io_client));
    memset(cmd_client, 0, sizeof(cmd_client));
    sd = buildClientSocket(&addr, atoi(argv[1]));
    connectToServer(sd, &addr);

    printf("Il client è stato inizializzato correttamente \n");
    mostraComandiLogin(); 

    // Fase di login o registrazione
    while(login_ended == false){
        char username[MAX_USERNAME_LENGTH];
        char password[MAX_PASSWORD_LENGTH];

        // Inizializzazione dei buffer per ogni iterazione
        memset(io_client, 0, sizeof(io_client));
        memset(cmd_client, 0, sizeof(cmd_client));
        memset(username, 0, sizeof(username));
        memset(password, 0, sizeof(password));

        // Lettura dell'input dell'utente
        fgets(io_client, sizeof(io_client), stdin);
        ret = sscanf(io_client, "%8s %31s %15s", cmd_client, username, password);

        if (strcmp(cmd_client, "login") == 0 || strcmp(cmd_client, "register") == 0){
            if (ret == 3){
                sendCredentials(sd, username, password, cmd_client); 
                recvFromSocket(sd, &code, sizeof(code));             
                switchCode(code, &login_ended);                     
            }
            else {
                printf("Errore nel formato del comando\n");
                mostraComandiLogin();
            }
        }
        else if (strcmp(cmd_client, "end" ) == 0){
            closeSocket(sd);
            return 0;
        }
        else {
            printf("Comando non riconosciuto \n");
            mostraComandiLogin(); 
        }
    }    
    //printf("La fase di login è terminata correttamente\n");

    // Inizializzazione dei buffer per le fasi successive
    memset(io_client, 0, sizeof(io_client));
    memset(cmd_client, 0, sizeof(cmd_client));    
    memset(buf, 0, sizeof(buf));

    // Verifica se si sta ripristinando una sessione precedente o si deve selezionare una stanza
    if (code != LOGIN_SUCCESS_PREVIOUS){
        bool start_ended = false;
        mostraComandiStart();

        // Fase di selezione della stanza
        while (start_ended == false){        
            fgets(io_client, sizeof(io_client), stdin);
            ret = sscanf(io_client, "%8s %hhu", cmd_client, &room);

            // Gestione del comando "start"
            if (!strcmp(cmd_client, "start")){
                if (ret == 1){
                    // Comando "start" senza numero di stanza
                    char buf[MAX_DESCRIPTION_LENGTH];
                    memset(buf, 0, sizeof(buf));
                    sendToSocket(sd, cmd_client, sizeof(cmd_client));

                    // MAX_ROOM_NUMBER + 1 indica la richiesta della lista delle stanze
                    room = MAX_ROOM_NUMBER + 1; 
                    sendToSocket(sd, &room, sizeof(room));
                    recvFromSocket(sd, &code, sizeof(code));
                    switchCode(code, &start_ended);
                    recvFromSocket(sd, buf, sizeof(buf));
                    printf("Lista delle stanze disponibili: \n");
                    printf("%s\n", buf);
                    continue;
                }
                else if (ret == 2){
                    // Comando "start" con numero di stanza
                    //printf("Hai selezionato la stanza %d\n", room);
                    if (room == 0 || room > MAX_ROOM_NUMBER){
                        printf("Stanza non presente \n");
                    }
                    else if (sendRoom(sd, room, &start_ended) == true){
                        uint16_t net_time;
                        // Fa partire il timer del gioco e riceve il tempo totale della stanza
                        time(&start_time); 
                        recvFromSocket(sd, &net_time, sizeof(net_time));
                        room_time = ntohs(net_time);
                    }
                    continue;
                }
            }  
            // Gestione della modalità spia
            else if (!strcmp(cmd_client, "spymode") && ret == 1){
                sendToSocket(sd, cmd_client, sizeof(cmd_client));
                spy = true;
                break;
            }
            // Gestione del comando "end" per terminare la connessione
            else if (!strcmp(cmd_client, "end") && ret == 1){
                sendToSocket(sd, cmd_client, sizeof(cmd_client));
                recvFromSocket(sd, &code, sizeof(code));
                switchCode(code, &start_ended);
                closeSocket(sd);
                return 0;
            }
            else {
                printf("Comando non riconosciuto \n");
                mostraComandiStart();
                continue;
            }
        }        
        recvFromSocket(sd, &code, sizeof(code));
        switchCode(code, &start_ended);
    }
    else {
        // Ripristino di una sessione precedente senza selezionare una stanza
        uint8_t room;
        uint16_t net_time;
        time(&start_time);
        
        recvFromSocket(sd, &room, sizeof(room));
        printf("Hai selezionato la stanza %d\n", room);
        // usare htons
        recvFromSocket(sd, &net_time, sizeof(net_time));
        room_time = ntohs(net_time);
        recvFromSocket(sd, buf, sizeof(buf));
        printf("%s\n", buf);
    }

    // Gestione della modalità spia se attivata
    if (spy == true){
        bool spy_ended = false;
        char user_to_spy[MAX_USERNAME_LENGTH];
        char buf[MAX_DESCRIPTION_LENGTH];
        mostraComandiSpia(); 
        
        // Loop principale per la modalità spia
        while(spy_ended == false){
            memset(io_client, 0, sizeof(io_client));
            memset(cmd_client, 0, sizeof(cmd_client));
            memset(user_to_spy, 0, sizeof(user_to_spy));
            memset(buf, 0, sizeof(buf));

            fgets(io_client, sizeof(io_client), stdin);
            ret = sscanf(io_client, "%5s %31s", cmd_client, user_to_spy);
            
            // Gestione del comando "list" per ottenere la lista degli utenti online
            if (!strcmp(cmd_client, "list") && ret == 1){
                sendToSocket(sd, cmd_client, sizeof(cmd_client));
                recvFromSocket(sd, &code, sizeof(code));
                switchCode(code, &spy_ended);
                recvFromSocket(sd, buf, sizeof(buf));
                printf("%s\n", buf);

            }
            // Gestione del comando "spy" per avviare lo spionaggio su un utente specifico
            else if (!strcmp(cmd_client, "spy") && ret == 2){
                uint16_t net_size;
                uint16_t size;
                sendToSocket(sd, cmd_client, sizeof(cmd_client));
                sendToSocket(sd, user_to_spy, sizeof(user_to_spy));
                recvFromSocket(sd, &code, sizeof(code));
                switchCode(code, &spy_ended);
                memset(buf, 0, sizeof(buf));

                // Loop per ricevere continuamente i dati dello spionaggio
                if (code == SPY_USER_FOUND){
                    while(1){
                        recvFromSocket(sd, &net_size, sizeof(net_size));
                        size = ntohs(net_size);
                        recvFromSocket(sd, buf, size);
                        if (size == 1){
                            // Se riceve un codice di terminazione
                            switchCode(buf[0], &spy_ended);
                            if (spy_ended) 
                                break;
                        }
                        else {
                            buf[size] = '\0'; 
                            printf("%s\n", buf);
                        }
                    }
                }
            }
            // Gestione del comando "end" per terminare lo spionaggio
            else if (!strcmp(cmd_client, "end") && ret == 1){
                sendToSocket(sd, cmd_client, sizeof(cmd_client));
                recvFromSocket(sd, &code, sizeof(code));
                switchCode(code, &spy_ended);
                closeSocket(sd);
                return 0;
            }
            else {
                printf("Comando non riconosciuto \n");
                mostraComandiSpia(); 
            }
        }
    }

    // Loop principale del gioco finché non è terminato
    while (game_ended == false) {
        char arg1[MAX_ARG_LENGTH];
        char arg2[MAX_ARG_LENGTH]; 
        time(&cur_time);
        time_remaining = room_time - difftime(cur_time, start_time);
        
        // Verifica se il tempo è scaduto
        if(time_remaining <= 0){
            printf("Tempo scaduto, hai perso!\n");
            break;
        }

        // Inizializzazione dei buffer per ogni iterazione
        memset(io_client, 0, sizeof(io_client));
        memset(cmd_client, 0, sizeof(cmd_client));
        fgets(io_client, sizeof(io_client), stdin);
        memset(arg1, 0, sizeof(arg1));
        memset(arg2, 0, sizeof(arg2));

        // Parsing del comando e dei suoi argomenti
        ret = sscanf(io_client, "%8s %31s %31s", cmd_client, arg1, arg2);

        // Gestione dei comandi non disponibili in questa fase del gioco
        if (strcmp("login", cmd_client) == 0 || strcmp("register", cmd_client) == 0 || strcmp("start", cmd_client) == 0) {
            printf("Comando non disponibile in questa fase del gioco \n");
            continue;
        }
        // Gestione del comando "logout"
        else if (strcmp("logout", cmd_client) == 0) {
            if (ret == 1){
                uint16_t net_time;
                sendToSocket(sd, cmd_client, sizeof(cmd_client));
                time(&cur_time);
                time_remaining = room_time - difftime(cur_time , start_time);
                net_time = htons(time_remaining);
                sendToSocket(sd, &net_time, sizeof(net_time));
            }
            else {
                printf("Errore nell'inserimento dei dati \n");
                printf("La sintassi corretta è logout\n");
                continue;
            }
        }
        // Gestione del comando "look"
        else if (strcmp("look", cmd_client) == 0) {
            if (ret == 1 || ret == 2){
                sendToSocket(sd, cmd_client, sizeof(cmd_client));
                sendToSocket(sd, arg1, sizeof(arg1));
            }
            else {
                printf("Errore nell'inserimento dei dati \n");
                printf("La sintassi corretta è look [obj]\n");
                continue;
            }
        }
        // Gestione del comando "take"
        else if (strcmp("take", cmd_client) == 0) {
            if (ret == 2){
                sendToSocket(sd, cmd_client, sizeof(cmd_client));
                sendToSocket(sd, arg1, sizeof(arg1));
            }
            else {
                printf("Errore nell'inserimento dei dati \n");
                printf("La sintassi corretta è take <obj>\n");
                continue;
            }
        }
        // Gestione del comando "use"
        else if (strcmp("use", cmd_client) == 0) {
            if (ret == 2 || ret == 3){
                sendToSocket(sd, cmd_client, sizeof(cmd_client));
                sendToSocket(sd, arg1, sizeof(arg1));
                sendToSocket(sd, arg2, sizeof(arg2));
            }
            else {
                printf("Errore nell'inserimento dei dati \n");
                printf("La sintassi corretta è use <obj_1> [obj_2]\n");
                continue;
            }
        }
        // Gestione del comando "objs"
        else if (strcmp("objs", cmd_client) == 0) {
            if (ret == 1){
                sendToSocket(sd, cmd_client, sizeof(cmd_client));
            }
            else {
                printf("Errore nell'inserimento dei dati \n");
                printf("La sintassi corretta è objs\n");
                continue;
            }
        }
        // Gestione del comando "drop"
        else if (strcmp("drop", cmd_client) == 0) {
            if (ret == 2){
                sendToSocket(sd, cmd_client, sizeof(cmd_client));
                sendToSocket(sd, arg1, sizeof(arg1));
            }
            else {
                printf("Errore nell'inserimento dei dati \n");
                printf("La sintassi corretta è drop <obj>\n");
                continue;
            }
        }
        // Gestione del comando "end"
        else if (strcmp("end", cmd_client) == 0) {
            if (ret == 1){
                sendToSocket(sd, cmd_client, sizeof(cmd_client));
            }
            else {
                printf("Errore nell'inserimento dei dati \n");
                printf("La sintassi corretta è end\n");
                continue;
            }
        }
        else {
            printf("Comando non riconosciuto \n");
            mostraComandiClient(); 
            continue;   
        }

        // Ricezione del codice di risposta dal server
        recvFromSocket(sd, &code, sizeof(code));
        switchCode(code, &game_ended); 
        handleServerResponse(sd, code, start_time, room_time, &game_ended);
    }

    return 0;
}