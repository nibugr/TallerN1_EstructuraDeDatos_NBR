#include "Display.h"
#include <iostream>
#include <cstdlib>

// ---------- clearScreen ----------
void Display::clearScreen() {
    system("cls");  // Windows
}

// ---------- showHeader ----------
void Display::showHeader(const Player& player) {
    std::string modeLabel = player.getModeLabel();

    if (!player.hasCurrent) {
        std::cout << "Reproduccion Detenida" << std::endl;
    } else {
        // Ejemplo: "Reproduciendo (S-R1): Fire Dance"
        std::string line = player.getStateLabel();
        if (!modeLabel.empty()) line += " " + modeLabel;
        line += ": " + player.currentSong.name;
        std::cout << line << std::endl;
        std::cout << "Artista: " << player.currentSong.artist << std::endl;
        std::cout << "Album: "   << player.currentSong.album
                  << " ["        << player.currentSong.year << "]" << std::endl;
        std::cout << "Duracion: " << player.currentSong.getDurationFormatted() << std::endl;
    }
    std::cout << std::endl;
}

// ---------- showMainMenu ----------
void Display::showMainMenu(const Player& player) {
    clearScreen();
    showHeader(player);

    std::cout << "Opciones:" << std::endl;
    std::cout << "W - Reproducir/Pausar" << std::endl;

    // Q solo se muestra si hay historial previo
    if (!player.history.isEmpty()) {
        std::cout << "Q - Pista Anterior" << std::endl;
    }

    std::cout << "E - Pista Siguiente" << std::endl;
    std::cout << "S - Activar/Desactivar modo aleatorio" << std::endl;
    std::cout << "R - Repeticion (";

    switch (player.repeatMode) {
        case REPEAT_OFF: std::cout << "Desactivado"; break;
        case REPEAT_ONE: std::cout << "Repetir una"; break;
        case REPEAT_ALL: std::cout << "Repetir todas"; break;
    }
    std::cout << ")" << std::endl;

    std::cout << "A - Ver lista de reproduccion actual" << std::endl;
    std::cout << "L - Listado de canciones" << std::endl;
    std::cout << "X - Salir" << std::endl;
    std::cout << std::endl;
    std::cout << "Ingrese Opcion: ";
}

// ---------- showQueueMenu ----------
void Display::showQueueMenu(const Player& player) {
    clearScreen();

    // Encabezado con cancion actual
    std::string modeLabel = player.getModeLabel();
    std::string headerLine = "";
    if (player.hasCurrent) {
        if (!modeLabel.empty()) headerLine += modeLabel + " ";
        headerLine += player.currentSong.name + " - " + player.currentSong.artist;
    } else {
        headerLine = "Sin cancion actual";
    }
    std::cout << "Actual " << (modeLabel.empty() ? "" : modeLabel + " ")
              << ": " << (player.hasCurrent
                          ? player.currentSong.name + " - " + player.currentSong.artist
                          : "Ninguna")
              << std::endl;

    std::cout << std::endl;
    std::cout << "Lista de reproduccion actual:" << std::endl;

    // Recorrer la cola sin destruirla usando getFront()
    Queue<Song>& q = const_cast<Queue<Song>&>(player.playQueue);
    Node<Song>* node = q.getFront();

    if (node == nullptr) {
        std::cout << "Vacia" << std::endl;
    } else {
        int i = 1;
        while (node != nullptr) {
            std::cout << i << ". " << node->data.name
                      << " - "    << node->data.artist << std::endl;
            node = node->next;
            i++;
        }
    }

    std::cout << std::endl;
    std::cout << "Opciones:" << std::endl;

    if (q.getFront() != nullptr) {
        std::cout << "S<num> - Saltar a la cancion seleccionada" << std::endl;
    }
    std::cout << "V - Volver al menu principal" << std::endl;
    std::cout << std::endl;
    std::cout << "Ingrese Opcion: ";
}

// ---------- showCatalogMenu ----------
void Display::showCatalogMenu(const Player& player) {
    clearScreen();

    // Encabezado
    std::string modeLabel = player.getModeLabel();
    std::cout << "Actual"
              << (modeLabel.empty() ? "" : " " + modeLabel)
              << ": "
              << (player.hasCurrent
                  ? player.currentSong.name + " - " + player.currentSong.artist
                  : "Ninguna")
              << std::endl;

    std::cout << std::endl;
    std::cout << "Canciones registradas:" << std::endl;

    Node<Song>* node = player.catalog.getHead();
    int i = 1;
    while (node != nullptr) {
        std::cout << i << ". " << node->data.name
                  << " - "    << node->data.artist << std::endl;
        node = node->next;
        i++;
    }

    std::cout << std::endl;
    std::cout << "Opciones:" << std::endl;
    std::cout << "R<num> - Reproducir cancion seleccionada" << std::endl;
    std::cout << "A<num> - Agregar cancion al final de la lista" << std::endl;
    std::cout << "N      - Agregar nueva cancion al registro" << std::endl;
    std::cout << "D<num> - Eliminar cancion seleccionada" << std::endl;
    std::cout << "V      - Volver al menu principal" << std::endl;
    std::cout << std::endl;
    std::cout << "Ingrese Opcion: ";
}

// ---------- showMessage ----------
void Display::showMessage(const std::string& msg) {
    std::cout << std::endl << "[!] " << msg << std::endl;
    std::cout << "Presione Enter para continuar...";
    std::cin.ignore();
    std::cin.get();
}