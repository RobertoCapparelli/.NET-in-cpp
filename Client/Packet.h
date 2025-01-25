#ifndef PACKET_H
#define PACKET_H

#include <string>


enum class Command {
    INVALID = 0,
    MOVE = 1,   
    UPDATE, 
    CREATE_ROOM,    
    JOIN,       
    QUIT,       
    GAME_OVER   
};

struct Packet {
    Command command{Command::INVALID};
    int room_id{0};
    std::string data{};

    Packet(Command cmd, int id, const std::string& payload)
        : command(cmd), room_id(id), data(payload) {};
    Packet() = default;


    std::string serialize() const {
        return std::to_string(static_cast<int>(command)) + "|" +
               std::to_string(room_id) + "|" + data; //Define a packet divided by '|'
    }

    static Packet deserialize(const std::string& raw) {
        auto first = raw.find('|');
        auto second = raw.find('|', first + 1);

        Command cmd = static_cast<Command>(std::stoi(raw.substr(0, first)));    //Stoi converts string to integer
        int id = std::stoi(raw.substr(first + 1, second - first - 1)); 
        std::string payload = raw.substr(second + 1); //Extract the payload

        return Packet(cmd, id, payload);
    }
};

#endif // PACKET_H