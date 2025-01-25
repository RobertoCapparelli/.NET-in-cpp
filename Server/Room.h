#ifndef ROOM_H
#define ROOM_H

#include <string>
#include <vector>
#include <queue>
#include <memory>
#include "Packet.h"
#include "Player.h" 

enum class RoomState {
    OPEN,
    IN_PROGRESS,
    CLOSED
};
enum class GameType {
    TIC_TAC_TOE,
};

class Room
{
protected:
    int room_id;
    GameType game_type;
    std::queue<Packet> packet_queue; 
    RoomState state;

    std::unique_ptr<Player> owner;
    std::vector<std::unique_ptr<Player>> players; 
    int max_players;

public:
    //costructor
    Room(int id, const std::unique_ptr<Player> ownerPlayer, GameType gameType, int maxPlayers);

    bool isOwner(const std::unique_ptr<Player> &player) const;
    bool addPlayer( std::unique_ptr<Player> &&player);
    bool removePlayer( std::unique_ptr<Player> &player);
    void changeOwner( std::unique_ptr<Player> &&newOwner);

    void processPacket(const Packet& packet);
    void reset();

    // Getter
    int getId() const { return room_id; }
    const std::unique_ptr<Player>& getOwner() const { return owner; }
    const std::vector<std::unique_ptr<Player>>& getPlayers() const { return players; }
    GameType getGameType() const { return game_type; }
    RoomState getState() const { return state; }

    // Setter
    void setState(RoomState newState) { state = newState; }
};
#endif // ROOM_H