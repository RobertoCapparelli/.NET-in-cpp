#define SDL_MAIN_HANDLED
#include "SDL2-2.30.11/include/SDL.h"
#include "SocketManager.h"
#include "Room.h"
#include <iostream>

const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 600;

int main(int argc, char* argv[]) {
    // Inizializza SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Multiplayer Game",
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
        }

        SDL_Delay(16); // Mantieni ~60 FPS
    }

    // Pulisci risorse
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}