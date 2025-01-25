#include "Room.h"
#include "Player.h"
#include <algorithm> // For std::remove
#include <iostream>

Room::Room(int id, std::unique_ptr<Player> ownerPlayer, GameType gameType, int maxPlayers)
    : room_id(id), owner(std::move(ownerPlayer)), game_type(gameType), state(RoomState::OPEN), max_players(maxPlayers) {
    players.push_back(std::make_unique<Player>(*owner)); //Copies the owner
}

bool Room::isOwner(const std::unique_ptr<Player>& player) const {
    return player->id == owner->id; // Check if the player is the owner with ID
}

bool Room::addPlayer(std::unique_ptr<Player>&& player) {
    if (players.size() >= max_players) {
        return false; // La stanza è piena
    }
    auto it = std::find_if(players.begin(), players.end(),
                           [&](const std::unique_ptr<Player>& p) {
                               return p->id == player->id;
                           });
    if (it != players.end()) {
        return false; // Giocatore già presente nella stanza
    }
    players.push_back(std::move(player)); // Trasferisce la proprietà
    return true;
}

bool Room::removePlayer(std::unique_ptr<Player>& player) {
    if (player == owner) {
        if (players.size() > 1) {
            for (auto& potentialOwner : players) {
                if (potentialOwner != player) {
                    changeOwner(std::move(potentialOwner)); // Cambia il proprietario
                    break;
                }
            }
        } else {
            reset(); // Resetta la stanza se non ci sono altri giocatori
            return true;
        }
    }

    auto it = std::remove_if(players.begin(), players.end(),
                             [&](const std::unique_ptr<Player>& p) {
                                 return p->id == player->id;
                             });
    if (it != players.end()) {
        players.erase(it, players.end());
        return true;
    }
    return false; // Giocatore non trovato
}

void Room::changeOwner(std::unique_ptr<Player>&& newOwner) {
    auto it = std::remove_if(players.begin(), players.end(),
                             [&](const std::unique_ptr<Player>& p) {
                                 return p->id == owner->id;
                             });
    players.erase(it, players.end());
    owner = std::move(newOwner); // Transfer ownership
}

void Room::processPacket(const Packet& packet) {
    packet_queue.push(packet);

    switch (packet.command) {
        case Command::MOVE:
            std::cout << "Processing MOVE command for Room " << room_id << std::endl;
            break;
        case Command::UPDATE:
            std::cout << "Processing UPDATE command for Room " << room_id << std::endl;
            break;
        case Command::JOIN:
            std::cout << "Processing JOIN command for Room " << room_id << std::endl;
            break;
        case Command::QUIT:
            std::cout << "Processing QUIT command for Room " << room_id << std::endl;
            break;
        case Command::GAME_OVER:
            std::cout << "Processing GAME_OVER command for Room " << room_id << std::endl;
            break;
        default:
            std::cout << "Unknown command in Room " << room_id << std::endl;
            break;
    }
}

void Room::reset() {
    state = RoomState::OPEN;
    players.clear();
    if (owner) {
        players.push_back(std::make_unique<Player>(*owner)); // Crea una copia dell'owner
    }
    packet_queue = std::queue<Packet>();
}
