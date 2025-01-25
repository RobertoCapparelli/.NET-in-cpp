#define SDL_MAIN_HANDLED
#include "../SDL2-2.30.11/include/SDL.h"
#include "../SDL2-2.30.11/include/SDL_test_font.h"
#include "SocketManager.h"
#include "Packet.h"
#include <iostream>

// Costanti della finestra
const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 600;

// Prototipi
void drawInitialMenu(SDL_Renderer* renderer);
void drawGrid(SDL_Renderer* renderer);
bool isMouseOver(int mouseX, int mouseY, int x, int y, int w, int h);

int main(int argc, char* argv[]) {
    // Inizializza SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Crea finestra
    SDL_Window* window = SDL_CreateWindow(
        "Multiplayer Client",
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

    // Crea renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Crea socket per il client
    SocketManager clientSocket;
    if (!clientSocket.createSocket()) {
        std::cerr << "Errore nella creazione della socket client." << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Variabile per il ciclo principale
    bool running = true;
    bool showGrid = false; // Determina se mostrare la griglia
    SDL_Event e;

    while (running) {
        // Gestione degli eventi
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            } else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                int mouseX = e.button.x;
                int mouseY = e.button.y;

                if (!showGrid) {
                    // Controlla se il clic è su "Create a Room"
                    if (isMouseOver(mouseX, mouseY, 200, 200, 200, 50)) {
                        // Invia pacchetto per creare una stanza
                        Packet packet(Command::CREATE_ROOM, 101, "Host");
                        if (clientSocket.sendPacket(packet, "127.0.0.1", 9999)) {
                            std::cout << "Pacchetto inviato correttamente al server." << std::endl;
                            showGrid = true; // Mostra la griglia
                        } else {
                            std::cerr << "Errore nell'invio del pacchetto al server." << std::endl;
                        }
                    }
                }
            }
        }

        // Disegna contenuto
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Sfondo nero
        SDL_RenderClear(renderer);

        if (showGrid) {
            drawGrid(renderer); // Disegna la griglia
        } else {
            drawInitialMenu(renderer); // Disegna il menu iniziale
        }

        SDL_RenderPresent(renderer);
    }

    // Pulisci risorse
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

// Disegna il menu iniziale
void drawInitialMenu(SDL_Renderer* renderer) {
    // Disegna il pulsante "Create a Room"
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Colore bianco per il pulsante
    SDL_Rect createRoomButton = {200, 200, 200, 50};
    SDL_RenderFillRect(renderer, &createRoomButton);

    // Imposta il colore del testo (nero)
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Nero per il testo

    // Disegna il testo "Create a Room" direttamente sul renderer
    SDLTest_DrawString(renderer, 220, 220, "Create a Room");
}

// Disegna la griglia 3x3
void drawGrid(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Bianco

    for (int i = 1; i < 3; ++i) {
        SDL_RenderDrawLine(renderer, SCREEN_WIDTH / 3 * i, 0, SCREEN_WIDTH / 3 * i, SCREEN_HEIGHT); // Linee verticali
        SDL_RenderDrawLine(renderer, 0, SCREEN_HEIGHT / 3 * i, SCREEN_WIDTH, SCREEN_HEIGHT / 3 * i); // Linee orizzontali
    }

    std::cout << "[DEBUG]: Disegna la griglia 3x3" << std::endl;
}

// Controlla se il mouse è sopra un rettangolo
bool isMouseOver(int mouseX, int mouseY, int x, int y, int w, int h) {
    return mouseX >= x && mouseX <= x + w && mouseY >= y && mouseY <= y + h;
}