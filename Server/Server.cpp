#define SDL_MAIN_HANDLED
#include "../SDL2-2.30.11/include/SDL.h"
#include "SocketManager.h"
#include "Room.h"
#include "Player.h"
#include <iostream>
#include <vector>
#include <algorithm>

// Costanti
const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 600;

// Funzione di log per lo stato del server
void logServerState(const std::vector<Room>& rooms) {
    std::cout << "=== Stato del Server ===" << std::endl;
    for (const auto& room : rooms) {
        std::cout << "Room ID: " << room.getId() 
                  << " | Stato: " << static_cast<int>(room.getState())
                  << " | Giocatori: " << room.getPlayers().size() << std::endl;
    }
    std::cout << "=========================" << std::endl;
}

int main(int argc, char* argv[]) {
    // Inizializza SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Crea finestra (opzionale, utile per debug visuale se necessario)
    SDL_Window* window = SDL_CreateWindow(
        "Server Log",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Crea e configura SocketManager
    SocketManager socketManager;
    if (!socketManager.createSocket()) {
        std::cerr << "Errore nella creazione della socket." << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    if (!socketManager.bindSocket(9999)) {
        std::cerr << "Errore nel binding della socket." << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Elenco delle stanze del server
    std::vector<Room> rooms;
    bool stateChanged = false; // Flag per registrare modifiche

    // Ciclo principale
    bool running = true;
    SDL_Event e;

    while (running) {
        // Gestione degli eventi SDL
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            }
        }

        // Ricevi pacchetti
        Packet packet = socketManager.receivePacket();
        if (packet.command != Command::INVALID) {
            std::cout << "Received: Command = " << static_cast<int>(packet.command)
                      << ", Room ID = " << packet.room_id
                      << ", Data = " << packet.data << std::endl;

            // Gestione dei comandi
            if (packet.command == Command::JOIN) {
                // Aggiungi un giocatore a una stanza
                auto it = std::find_if(rooms.begin(), rooms.end(),
                                       [&](const Room& room) { return room.getId() == packet.room_id; });
                if (it != rooms.end()) {
                    Player newPlayer(packet.data, rooms.size() + 1); // Usa i dati del pacchetto come nome giocatore
                    it->addPlayer(std::make_unique<Player>(newPlayer));
                    stateChanged = true;
                }
            } else if (packet.command == Command::CREATE_ROOM) {
                // Crea una nuova stanza
                Room newRoom(packet.room_id, std::make_unique<Player>("Host", packet.room_id), GameType::TIC_TAC_TOE, 2);
                rooms.push_back(std::move(newRoom));
                stateChanged = true;
            }
        }

        // Log dello stato del server se c'è stato un cambiamento
        if (stateChanged) {
            logServerState(rooms);
            stateChanged = false;
        }

        SDL_Delay(16); // Mantieni ~60 FPS
    }

    // Pulisci risorse
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}