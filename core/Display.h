#ifndef DISPLAY_H
#define DISPLAY_H

#include "../classes/Player.h"

class Display {
public:
    // Limpia la consola (Windows)
    static void clearScreen();

    // Muestra el encabezado con la cancion actual
    static void showHeader(const Player& player);

    // Muestra el menu principal completo
    static void showMainMenu(const Player& player);

    // Muestra el submenu de lista de reproduccion (A)
    static void showQueueMenu(const Player& player);

    // Muestra el submenu de catalogo de canciones (L)
    static void showCatalogMenu(const Player& player);

    // Muestra un mensaje de error o aviso
    static void showMessage(const std::string& msg);
};

#endif // DISPLAY_H