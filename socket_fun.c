#include "socket_fun.h"

void buildAddress(struct sockaddr_in* addr, uint16_t port){
    uint16_t net_port = htons(port);
    memset(addr, 0, sizeof(*addr));
    addr->sin_family = AF_INET; 
    addr->sin_port = net_port;
    inet_pton(AF_INET, "127.0.0.1", &(addr->sin_addr));
}

void connectToServer(int sd, struct sockaddr_in* addr){
    int i, ret;
    // Si tenta la connessione al server per 3 volte
    for (i = 0; i < 3; i++){
        ret = connect(sd, (struct sockaddr*) addr, sizeof(*addr));
        if (ret == 0) {
            break;
        }
        sleep(3);
    }
    if (ret < 0) {
        perror("Errore nella connessione al server");
        exit(EXIT_FAILURE);
    }
    printf("Connessione al server avvenuta con successo\n");
}

void listenToSocket(int sd, int queue_size){
    int ret = listen(sd, queue_size);
    if (ret < 0) {
        perror("Errore nella listen del socket");
        exit(EXIT_FAILURE);
    }
    printf("Il server è in ascolto\n");
}

int acceptConnection(int sd, struct sockaddr_in* addr){
    socklen_t len = sizeof(*addr);
    int new_sd = accept(sd, (struct sockaddr*) addr, &len);
    if (new_sd < 0) {
        perror("Errore nell'accettazione della connessione");
        exit(EXIT_FAILURE);
    }
    printf("Connessione accettata\n");
    return new_sd;
}

void closeSocket(int sd){
    int ret = close(sd);
    if (ret < 0) {
        perror("Errore nella chiusura del socket");
        exit(EXIT_FAILURE);
    }
    printf("Il socket è stato chiuso correttamente\n");
}

void bindSocket(int sd, struct sockaddr_in* addr){
    int ret = bind(sd, (struct sockaddr*) addr, sizeof(*addr));
    if (ret < 0) {
        perror("Errore nella bind del socket");
        exit(EXIT_FAILURE);
    }
    printf("Il socket è stato assegnato correttamente all'indirizzo\n");
}

int buildServerSocket(struct sockaddr_in* addr, uint16_t port){
    int sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0) {
        perror("Errore nella creazione del socket");
        exit(EXIT_FAILURE);
    }
    printf("Il socket è stato creato correttamente\n");
    buildAddress(addr, port);
    bindSocket(sd, addr);
    return sd;
}

int buildClientSocket(struct sockaddr_in* addr, uint16_t port){
    int sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0) {
        perror("Errore nella creazione del socket");
        exit(EXIT_FAILURE);
    }
    printf("Il socket è stato creato correttamente\n");
    buildAddress(addr, port);
    return sd; 
}

void sendToSocket(int sd, const void* buffer, int size){    
    int ret = send(sd, buffer, size, 0);
    if (ret < 0) {
        perror("Errore nell'invio del messaggio");
        exit(EXIT_FAILURE);
    }
}

void recvFromSocket(int sd, void* buffer, int size){
    int ret = recv(sd, buffer, size, 0);
    if (ret < 0) {
        perror("Errore nella ricezione del messaggio");
        exit(EXIT_FAILURE);
    }
}