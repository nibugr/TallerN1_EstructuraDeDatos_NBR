#ifndef PLAYERACTIONS_H
#define PLAYERACTIONS_H

#include "../classes/Player.h"
#include "FileManager.h"

class PlayerActions {
public:
    // W - Reproducir / Pausar
    static void togglePlay(Player& player);

    // Q - Pista anterior
    static void prevTrack(Player& player);

    // E - Pista siguiente
    static void nextTrack(Player& player);

    // S - Modo aleatorio
    static void toggleShuffle(Player& player);

    // R - Modo repeticion
    static void cycleRepeat(Player& player);

    // Submenu A: saltar a posicion en la cola
    static void jumpToTrack(Player& player, int position);

    // Submenu L: reproducir cancion especifica del catalogo
    static void playSongFromCatalog(Player& player, int position);

    // Submenu L: agregar cancion del catalogo al final de la cola
    static void addToQueue(Player& player, int position);

    // Submenu L: agregar nueva cancion al catalogo
    static void addNewSong(Player& player);

    // Submenu L: eliminar cancion del catalogo
    static void deleteSong(Player& player, int position);

private:
    // Llena la cola con todas las canciones del catalogo
    // de forma aleatoria (excepto la cancion indicada)
    static void fillQueueRandom(Player& player, int excludeId = -1);

    // Mezcla la cola actual (shuffle)
    static void shuffleQueue(Player& player);
};

#endif // PLAYERACTIONS_H