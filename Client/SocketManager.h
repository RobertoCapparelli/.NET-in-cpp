#ifndef SOCKETMANAGER_H
#define SOCKETMANAGER_H

#include "Packet.h"
#include <string>
#include <unordered_map>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h> // Per sockaddr_in, inet_pton
#include <sys/socket.h> // Per socket, bind, sendto, recvfrom
#include <netinet/in.h> // Per struct sockaddr_in
#include <fcntl.h>      // Per O_NONBLOCK su Linux/macOS
#include <unistd.h>     // Per close
#endif

class SocketManager {
private:
    int sockfd; // File descriptor della socket
    sockaddr_in localAddr; // Indirizzo del server locale
    std::unordered_map<std::string, sockaddr_in> clients; // Mappa dei client (IP:PORT -> sockaddr_in)

public:
    // Costruttore e Distruttore
    SocketManager();
    ~SocketManager();

    // Configurazione e gestione della socket
    bool createSocket();
    bool bindSocket(int port);

    // Invio e ricezione
    bool sendPacket(const Packet& packet, const std::string& ip, int port);
    Packet receivePacket();

    // Gestione dei client
    void addClient(const std::string& ip, int port);
    void removeClient(const std::string& ip, int port);

    // Accesso al file descriptor della socket
    int getSocketFd() const { return sockfd; }
};

#endif // SOCKETMANAGER_H