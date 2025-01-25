#include "Player.h"

// Costruttore
Player::Player(const std::string& playerName, int playerId)
    : id(playerId), name(playerName), isConnected(true) {}

// Disconnette il giocatore
void Player::disconnect() {
    isConnected = false;
}

// Riconnette il giocatore
void Player::reconnect() {
    isConnected = true;
}