#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <memory>

class Player {
public:
    int id;                   // ID unico del giocatore
    std::string name;         // Nome del giocatore
    bool isConnected;         // Stato della connessione del giocatore

    // Costruttori
    Player(const std::string& playerName, int playerId);
    Player() = delete; // Disabilita il costruttore di default

    // Metodi
    void disconnect(); // Imposta lo stato del giocatore come disconnesso
    void reconnect();  // Imposta lo stato del giocatore come connesso
};

#endif // PLAYER_H 