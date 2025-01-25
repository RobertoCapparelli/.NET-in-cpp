#include "SocketManager.h"
#include <iostream>
#include <cstring> // Per memset

#ifdef _WIN32
#pragma comment(lib, "ws2_32.lib") // Necessario per Winsock su Windows
#endif

SocketManager::SocketManager() : sockfd(-1) {
#ifdef _WIN32
    // Inizializza Winsock su Windows
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Errore inizializzazione Winsock." << std::endl;
    }
#endif
}

SocketManager::~SocketManager() {
    if (sockfd >= 0) {
#ifdef _WIN32
        closesocket(sockfd);
        WSACleanup();
#else
        close(sockfd);
#endif
    }
}

bool SocketManager::createSocket() {
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Errore creazione socket");
        return false;
    }

#ifdef _WIN32
    u_long mode = 1; // Modalità non bloccante
    if (ioctlsocket(sockfd, FIONBIO, &mode) != 0) {
        perror("Errore configurazione modalità non bloccante");
        return false;
    }
#else
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags < 0) {
        perror("Errore lettura flag socket");
        return false;
    }
    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) < 0) {
        perror("Errore configurazione modalità non bloccante");
        return false;
    }
#endif

    return true;
}
bool SocketManager::bindSocket(int port) {
    memset(&localAddr, 0, sizeof(localAddr));
    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = INADDR_ANY;
    localAddr.sin_port = htons(port);

    if (bind(sockfd, reinterpret_cast<sockaddr*>(&localAddr), sizeof(localAddr)) < 0) {
        perror("Errore binding socket");
        return false;
    }
    return true;
}

bool SocketManager::sendPacket(const Packet& packet, const std::string& ip, int port) {
    sockaddr_in destAddr{};
    memset(&destAddr, 0, sizeof(destAddr));
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip.c_str(), &destAddr.sin_addr) <= 0) {
        perror("Errore conversione indirizzo IP");
        return false;
    }

    std::string serialized = packet.serialize();
    int sentBytes = sendto(sockfd, serialized.c_str(), serialized.size(), 0,
                           reinterpret_cast<sockaddr*>(&destAddr), sizeof(destAddr));

    if (sentBytes < 0) {
        perror("Errore invio pacchetto");
        return false;
    }

    return true;
}

Packet SocketManager::receivePacket() {
    char buffer[4096];
    sockaddr_in senderAddr{};
    socklen_t senderLen = sizeof(senderAddr);

    int receivedBytes = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0,
                                 reinterpret_cast<sockaddr*>(&senderAddr), &senderLen);
    if (receivedBytes < 0) {
#ifdef _WIN32
        int error = WSAGetLastError();
        if (error == WSAEWOULDBLOCK) {
            return Packet(); // Nessun pacchetto disponibile
        }
        std::cerr << "Errore socket (WSAGetLastError): " << error << std::endl;
#else
        int error = errno;
        if (error == EWOULDBLOCK || error == EAGAIN) {
            return Packet(); // Nessun pacchetto disponibile
        }
        std::cerr << "Errore socket (errno): " << error << std::endl;
#endif
        perror("Errore ricezione pacchetto");
        return Packet();
    }

    buffer[receivedBytes] = '\0'; // Termina la stringa
    return Packet::deserialize(buffer);
}
void SocketManager::removeClient(const std::string& ip, int port) {
    clients.erase(ip + ":" + std::to_string(port));
}