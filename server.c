#include "general.h"
#include "utility.h"
#include "sessione.h"
#include "socket_fun.h"
#include "server_fun.h"
#include "game.h"

int main(int argc, char* argv[]) { 
    char io_server[127];
    extern room container[MAX_ROOM_NUMBER];
    
    char cmd_server[9]; 
    uint16_t port;
    sockaddr_in addr;
    fd_set master;      
    fd_set read_fds;   
    int fdmax; 
    session *ses_list = NULL;
    bool server_on = false;
    int ret, i;
    int listener, newfd;

    // Verifica del numero corretto di argomenti
    if (argc != 2) {
        printf("Errore nell'esecuzione del file server\n");
        printf("La sintassi corretta è './server <porta>'\n");
        return 0;
    }

    // Inizializzazione dei set di descrittori di file
    FD_ZERO(&master);
    FD_ZERO(&read_fds);
    
    // Aggiunge lo standard input al set principale
    FD_SET(STDIN_FILENO, &master);

    printf("Il server è stato inizializzato correttamente\n");
    mostraComandiServer();
  
    // Ciclo principale per monitorare i descrittori di file
    while(1) {
        memset(io_server, 0, sizeof(io_server));
        memset(cmd_server, 0, sizeof(cmd_server));

        read_fds = master; // Copia il set principale in read_fds per select()
        
        // Attende attività su uno qualsiasi dei descrittori di file
        ret = select(fdmax + 1, &read_fds, NULL, NULL, NULL);
        if (ret == -1) {
            perror("Errore nella select");
            exit(1);
        }

        // Itera attraverso i descrittori di file per verificare quali sono pronti
        for (i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) {
                if (i == STDIN_FILENO) {
                    // Gestisce i comandi del server dallo standard input
                    fgets(io_server, sizeof(io_server), stdin);

                    // Analizza il comando e il numero di porta 
                    ret = sscanf(io_server, "%6s %hu", cmd_server, &port);
                    
                    // Gestisce il comando "start"
                    if (strcmp("start", cmd_server) == 0 && ret == 2) {
                        if (!server_on) {
                            buildAddress(&addr, atoi(argv[1]));

                            // Valida la porta fornita
                            if (port != atoi(argv[1])) {
                                printf("La porta inserita non è valida\n");
                                continue;
                            }

                            // Crea e configura il socket del server
                            listener = buildServerSocket(&addr, atoi(argv[1]));
                            listenToSocket(listener, 10);
                            
                            // Aggiunge il listener al set principale e aggiorna fdmax
                            FD_SET(listener, &master);
                            fdmax = listener;
                            server_on = true;
                            
                            printf("Il server è stato avviato correttamente\n");
                        }
                        else {
                            printf("Il server è già in esecuzione sulla porta %d\n", port);
                        }
                    }
                    // Gestisce il comando "stop"
                    else if (strcmp("stop", cmd_server) == 0 && ret == 1) {
                        if (server_on) {
                            // Verifica se ci sono sessioni attive prima di chiudere
                            if (checkActiveSession(&ses_list)) {
                                printf("Non è possibile chiudere il server con delle sessioni attive\n");
                                continue;
                            }

                            // Chiude il socket listener e aggiorna lo stato del server
                            closeSocket(listener);
                            server_on = false;
                            printf("Il server è stato chiuso correttamente\n");
                            return 0;
                        }
                        else {
                            printf("Il server non è in esecuzione\n");
                        }
                    }
                    else {
                        printf("Comando non riconosciuto\n");
                    }
                }
                else {
                    if (i == listener) {
                        // Gestisce nuove connessioni in arrivo
                        fflush(stdout);
                        newfd = acceptConnection(listener, &addr);
                        if (newfd == -1) {
                            perror("Errore nell'accettazione della connessione");
                            continue;
                        }
                        printf("Nuovo client connesso: %d\n", newfd);
                        
                        // Aggiunge il nuovo socket al set principale e aggiorna fdmax
                        FD_SET(newfd, &master);
                        if (newfd > fdmax) { 
                            fdmax = newfd;
                        }
                    }
                    else {
                        // Gestisce i dati provenienti da un client connesso
                        ret = recv(i, cmd_server, sizeof(cmd_server), 0);
                        if (ret <= 0) {
                            if (ret == 0) {
                                // Connessione chiusa dal client
                                printf("Un client ha chiuso la connessione\n");
                            }
                            else {
                                perror("Errore nella recv");
                            }

                            // Pulisce la sessione e chiude il socket
                            session *ses = checkSessionById(&ses_list, i);
                            if (ses != NULL && ses->online) {
                                deleteSession(&ses_list, ses);
                            }
                            closeSocket(i);
                            FD_CLR(i, &master);
                        }
                        else {
                            // Termina il comando ricevuto
                            cmd_server[ret] = '\0';
                            printf("Comando ricevuto dal client %d: %s\n", i, cmd_server);

                            char username[MAX_USERNAME_LENGTH];
                            char password[MAX_PASSWORD_LENGTH];
                            char full_cmd[128];
                            uint8_t code;
                            session *ses;

                            memset(username, 0, sizeof(username));
                            memset(password, 0, sizeof(password));
                            memset(full_cmd, 0, sizeof(full_cmd));

                            // Copia il comando ricevuto in full_cmd
                            snprintf(full_cmd, sizeof(full_cmd), "%s", cmd_server);

                            // Gestisce i comandi "login" e "register"
                            if (strcmp("login", cmd_server) == 0 || strcmp("register", cmd_server) == 0) {
                                recvCredentials(i, username, password);
                            }

                            if (strcmp("login", cmd_server) == 0) {
                                if (handleLogin(username, password, &ses_list, container, i, &code)) {
                                    continue;
                                }
                            }
                            else if (strcmp("register", cmd_server) == 0) {
                                handleRegister(username, password, &code);
                            }
                            else if (strcmp("logout", cmd_server) == 0) {
                                handleLogout(full_cmd, &ses_list, i, &code);
                            }
                            else if (strcmp("start", cmd_server) == 0) {
                                if (handleStart(&ses_list, container, i, &code)){
                                    continue;
                                }
                            }
                            else if (strcmp("look", cmd_server) == 0) {
                                if (handleLook(full_cmd, &ses_list, container, i, &code)) {
                                    continue;
                                }
                            }
                            else if (strcmp("take", cmd_server) == 0) {
                                if (handleTake(full_cmd, &ses_list, container, i, &code)) {
                                    continue;
                                }
                            }
                            else if (strcmp("use", cmd_server) == 0) {
                                if (handleUse(full_cmd, &ses_list, container, i, &code)) {
                                    continue;
                                }
                            }
                            else if (strcmp("objs", cmd_server) == 0) {
                                if (handleObjs(full_cmd, &ses_list, i, &code)) {
                                    continue;
                                }
                            }
                            else if (strcmp("end", cmd_server) == 0) {
                                if (handleEnd(full_cmd, &ses_list, i, &code)) {
                                    continue;
                                }
                            }
                            else if (strcmp("drop", cmd_server) == 0) {
                                handleDrop(full_cmd, &ses_list, i, &code);
                            }
                            // Invia la lista di tutti i giocatori online
                            else if (strcmp("list", cmd_server) == 0) {
                                handleList(&ses_list, i, &code);
                                continue;
                            }
                            else if (strcmp("spymode", cmd_server) == 0){
                                handleSpyMode(&ses_list, i, &code);
                                continue;
                            }
                            else if (strcmp("spy", cmd_server) == 0){
                                handleSpy(&ses_list, i, &code);
                                continue;
                            }
                            else {
                                printf("Comando non riconosciuto \n");
                                code = COMMAND_NOT_RECOGNIZED;
                            }
                            printf("Comando: %s\n", cmd_server);

                            ses = checkSessionById(&ses_list, i);
                            sendToSocket(i, &code, sizeof(code));
                            if (ses != NULL && ses->spied_by != -1) {
                                sendCodeToSpy(ses, &code);
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}