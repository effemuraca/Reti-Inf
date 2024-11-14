#ifndef SOCKET_FUN_H
#define SOCKET_FUN_H

#include "general.h"

/**
 * @brief Configura l'indirizzo del socket.
 * 
 * @param addr Puntatore alla struttura sockaddr_in da configurare.
 * @param port Numero di porta da utilizzare.
 */
void buildAddress(struct sockaddr_in* addr, uint16_t port);

/**
 * @brief Connette un client al server.
 * 
 * @param sd Descrittore del socket.
 * @param addr Puntatore alla struttura sockaddr_in del server.
 */
void connectToServer(int sd, struct sockaddr_in* addr);

/**
 * @brief Avvia l'ascolto su un socket.
 * 
 * @param sd Descrittore del socket.
 * @param queue_size Numero massimo di connessioni in coda.
 */
void listenToSocket(int sd, int queue_size);

/**
 * @brief Accetta una connessione in entrata.
 * 
 * @param sd Descrittore del socket in ascolto.
 * @param addr Puntatore alla struttura sockaddr_in del client.
 * @return int Descriptor del nuovo socket della connessione accettata.
 */
int acceptConnection(int sd, struct sockaddr_in* addr);

/**
 * @brief Chiude un socket.
 * 
 * @param sd Descrittore del socket.
 */
void closeSocket(int sd);

/**
 * @brief Associa un socket a un indirizzo specifico.
 * 
 * @param sd Descrittore del socket.
 * @param addr Puntatore alla struttura sockaddr_in da associare.
 */
void bindSocket(int sd, struct sockaddr_in* addr);

/**
 * @brief Crea e configura un socket server.
 * 
 * @param addr Puntatore alla struttura sockaddr_in da configurare.
 * @param port Numero di porta su cui il server ascolterà.
 * @return int Descriptor del socket server creato.
 */
int buildServerSocket(struct sockaddr_in* addr, uint16_t port);

/**
 * @brief Crea e configura un socket client.
 * 
 * @param addr Puntatore alla struttura sockaddr_in del server.
 * @param port Numero di porta del server.
 * @return int Descriptor del socket client creato.
 */
int buildClientSocket(struct sockaddr_in* addr, uint16_t port);

/**
 * @brief Invia dati tramite un socket.
 * 
 * @param sd Descrittore del socket.
 * @param buffer Puntatore ai dati da inviare.
 * @param size Dimensione dei dati in byte.
 */
void sendToSocket(int sd, const void* buffer, int size);

/**
 * @brief Riceve dati tramite un socket.
 * 
 * @param sd Descrittore del socket.
 * @param buffer Puntatore al buffer dove salvare i dati ricevuti.
 * @param size Dimensione massima dei dati da ricevere in byte.
 */
void recvFromSocket(int sd, void* buffer, int size);

#endif