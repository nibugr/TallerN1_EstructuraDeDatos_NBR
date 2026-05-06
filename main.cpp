#include <iostream>
#include <string>
#include <cctype>

#include "classes/Player.h"
#include "core/FileManager.h"
#include "core/Display.h"
#include "core/PlayerActions.h"

// =============================================
// SUBMENU A — Lista de reproduccion actual
// =============================================
void menuQueue(Player& player) {
    std::string input;

    while (true) {
        Display::showQueueMenu(player);
        std::getline(std::cin, input);

        if (input.empty()) continue;

        // Convertir primera letra a mayuscula
        char cmd = toupper(input[0]);

        if (cmd == 'V') {
            break; // Volver al menu principal

        } else if (cmd == 'S' && input.size() > 1) {
            // S<num> — saltar a posicion
            try {
                int pos = std::stoi(input.substr(1));
                PlayerActions::jumpToTrack(player, pos);
                break; // Volver al menu principal automaticamente
            } catch (...) {
                Display::showMessage("Numero invalido. Intente de nuevo.");
            }

        } else {
            Display::showMessage("Opcion no reconocida. Intente de nuevo.");
        }
    }
}

// =============================================
// SUBMENU L — Catalogo de canciones
// =============================================
void menuCatalog(Player& player) {
    std::string input;

    while (true) {
        Display::showCatalogMenu(player);
        std::getline(std::cin, input);

        if (input.empty()) continue;

        char cmd = toupper(input[0]);

        if (cmd == 'V') {
            break;

        } else if (cmd == 'R' && input.size() > 1) {
            // R<num> — reproducir cancion especifica
            try {
                int pos = std::stoi(input.substr(1));
                PlayerActions::playSongFromCatalog(player, pos);
                break; // Volver al menu principal
            } catch (...) {
                Display::showMessage("Numero invalido. Intente de nuevo.");
            }

        } else if (cmd == 'A' && input.size() > 1) {
            // A<num> — agregar al final de la cola
            try {
                int pos = std::stoi(input.substr(1));
                PlayerActions::addToQueue(player, pos);
            } catch (...) {
                Display::showMessage("Numero invalido. Intente de nuevo.");
            }

        } else if (cmd == 'N') {
            // N — agregar nueva cancion al registro
            PlayerActions::addNewSong(player);

        } else if (cmd == 'D' && input.size() > 1) {
            // D<num> — eliminar cancion
            try {
                int pos = std::stoi(input.substr(1));
                PlayerActions::deleteSong(player, pos);
            } catch (...) {
                Display::showMessage("Numero invalido. Intente de nuevo.");
            }

        } else {
            Display::showMessage("Opcion no reconocida. Intente de nuevo.");
        }
    }
}

// =============================================
// MENU PRINCIPAL
// =============================================
void menuMain(Player& player) {
    std::string input;

    while (true) {
        Display::showMainMenu(player);
        std::getline(std::cin, input);

        if (input.empty()) continue;

        char cmd = toupper(input[0]);

        if (cmd == 'W') {
            // Reproducir / Pausar
            PlayerActions::togglePlay(player);

        } else if (cmd == 'Q') {
            // Pista anterior
            if (!player.history.isEmpty()) {
                PlayerActions::prevTrack(player);
            }

        } else if (cmd == 'E') {
            // Pista siguiente
            PlayerActions::nextTrack(player);

        } else if (cmd == 'S') {
            // Modo aleatorio
            PlayerActions::toggleShuffle(player);

        } else if (cmd == 'R') {
            // Ciclar repeticion
            PlayerActions::cycleRepeat(player);

        } else if (cmd == 'A') {
            // Submenu lista de reproduccion
            menuQueue(player);

        } else if (cmd == 'L') {
            // Submenu catalogo
            menuCatalog(player);

        } else if (cmd == 'X') {
            // Salir — guardar estado y terminar
            FileManager::saveStatus(player);
            Display::clearScreen();
            std::cout << "Estado guardado. Hasta luego." << std::endl;
            break;

        } else {
            Display::showMessage("Opcion no reconocida. Intente de nuevo.");
        }
    }
}

// =============================================
// INICIO DEL PROGRAMA
// =============================================
int main() {
    Player player;

    // --- Cargar catalogo de canciones ---
    bool catalogLoaded = FileManager::loadCatalog(player);

    // --- Cargar estado previo ---
    if (catalogLoaded) {
        FileManager::loadStatus(player);
    } else {
        // Sin catalogo: iniciar limpio
        player.hasCurrent  = false;
        player.playState   = STOPPED;
        player.repeatMode  = REPEAT_OFF;
        player.shuffleOn   = false;
    }

    // --- Iniciar menu principal ---
    menuMain(player);

    return 0;
}