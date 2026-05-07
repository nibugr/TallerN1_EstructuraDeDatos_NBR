#include "PlayerActions.h"
#include "Display.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

// ---------- fillQueueRandom (sin vector) ----------
void PlayerActions::fillQueueRandom(Player& player, int excludeId) {
    // Contar canciones validas
    int total = 0;
    Node<Song>* node = player.catalog.getHead();
    while (node != nullptr) {
        if (node->data.id != excludeId) total++;
        node = node->next;
    }

    // Si no hay canciones, salir
    if (total == 0) return;

    // Crear arreglo dinamico manual
    Song* songs = new Song[total];
    int idx = 0;
    node = player.catalog.getHead();
    while (node != nullptr) {
        if (node->data.id != excludeId) {
            songs[idx++] = node->data;
        }
        node = node->next;
    }

    // Mezclar con Fisher-Yates
    srand((unsigned int)time(nullptr));
    for (int i = total - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Song temp = songs[i];
        songs[i] = songs[j];
        songs[j] = temp;
    }

    // Llenar la cola
    player.playQueue.clear();
    for (int i = 0; i < total; i++) {
        player.playQueue.enqueue(songs[i]);
    }

    // Liberar memoria
    delete[] songs;
}

// ---------- shuffleQueue (sin vector) ----------
void PlayerActions::shuffleQueue(Player& player) {
    int total = player.playQueue.getSize();
    if (total == 0) return;

    // Crear arreglo dinamico manual
    Song* songs = new Song[total];
    int idx = 0;
    while (!player.playQueue.isEmpty()) {
        songs[idx++] = player.playQueue.dequeue();
    }

    // Mezclar
    srand((unsigned int)time(nullptr));
    for (int i = total - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Song temp = songs[i];
        songs[i] = songs[j];
        songs[j] = temp;
    }

    // Volver a encolar
    for (int i = 0; i < total; i++) {
        player.playQueue.enqueue(songs[i]);
    }

    // Liberar memoria
    delete[] songs;
}

// ---------- W: togglePlay ----------
void PlayerActions::togglePlay(Player& player) {
    if (player.catalog.isEmpty()) return;

    if (!player.hasCurrent) {
        player.currentSong = player.catalog.getAt(0);
        player.hasCurrent  = true;
        player.playState   = PLAYING;
        fillQueueRandom(player, player.currentSong.id);
    } else {
        if (player.playState == PLAYING) {
            player.playState = PAUSED;
        } else {
            player.playState = PLAYING;
        }
    }
    FileManager::saveStatus(player);
}

// ---------- Q: prevTrack ----------
void PlayerActions::prevTrack(Player& player) {
    if (player.catalog.isEmpty()) return;
    if (player.history.isEmpty()) return;
    if (player.repeatMode == REPEAT_ONE) return;

    // Cancion actual vuelve al frente de la cola
    if (player.hasCurrent) {
        Queue<Song> tempQueue;
        tempQueue.enqueue(player.currentSong);
        while (!player.playQueue.isEmpty()) {
            tempQueue.enqueue(player.playQueue.dequeue());
        }
        while (!tempQueue.isEmpty()) {
            player.playQueue.enqueue(tempQueue.dequeue());
        }
    }

    player.currentSong = player.history.pop();
    player.hasCurrent  = true;
    player.playState   = PLAYING;
    FileManager::saveStatus(player);
}

// ---------- E: nextTrack ----------
void PlayerActions::nextTrack(Player& player) {
    if (player.catalog.isEmpty()) return;
    if (player.repeatMode == REPEAT_ONE) return;

    if (player.hasCurrent) {
        player.history.push(player.currentSong);
    }

    if (player.playQueue.isEmpty()) {
        if (player.repeatMode == REPEAT_ALL) {
            if (player.shuffleOn) {
                fillQueueRandom(player, -1);
            } else {
                player.playQueue.clear();
                Node<Song>* node = player.catalog.getHead();
                while (node != nullptr) {
                    player.playQueue.enqueue(node->data);
                    node = node->next;
                }
            }
        } else {
            // Cola vacia sin repeticion: rellenar aleatorio
            // Si solo hay 1 cancion en catalogo, no excluir ninguna
            int excludeId = -1;
            if (player.hasCurrent && player.catalog.getSize() > 1) {
                excludeId = player.currentSong.id;
            }
            fillQueueRandom(player, excludeId);
        }
    }

    player.currentSong = player.playQueue.dequeue();
    player.hasCurrent  = true;
    player.playState   = PLAYING;
    FileManager::saveStatus(player);
}

// ---------- S: toggleShuffle ----------
void PlayerActions::toggleShuffle(Player& player) {
    if (player.catalog.isEmpty()) return;

    player.shuffleOn = !player.shuffleOn;
    if (player.shuffleOn) {
        shuffleQueue(player);
    }
    FileManager::saveStatus(player);
}

// ---------- R: cycleRepeat ----------
void PlayerActions::cycleRepeat(Player& player) {
    if (player.catalog.isEmpty()) return;

    switch (player.repeatMode) {
        case REPEAT_OFF: player.repeatMode = REPEAT_ONE; break;
        case REPEAT_ONE: player.repeatMode = REPEAT_ALL; break;
        case REPEAT_ALL: player.repeatMode = REPEAT_OFF; break;
    }
    FileManager::saveStatus(player);
}

// ---------- Submenu A: jumpToTrack ----------
void PlayerActions::jumpToTrack(Player& player, int position) {
    if (position < 1 || position > player.playQueue.getSize()) {
        Display::showMessage("Posicion invalida.");
        return;
    }

    if (player.hasCurrent) {
        player.history.push(player.currentSong);
    }

    for (int i = 1; i < position; i++) {
        player.playQueue.dequeue();
    }

    player.currentSong = player.playQueue.dequeue();
    player.hasCurrent  = true;
    player.playState   = PLAYING;
    FileManager::saveStatus(player);
}

// ---------- Submenu L: playSongFromCatalog ----------
void PlayerActions::playSongFromCatalog(Player& player, int position) {
    if (position < 1 || position > player.catalog.getSize()) {
        Display::showMessage("Posicion invalida.");
        return;
    }

    if (player.hasCurrent) {
        player.history.push(player.currentSong);
    }

    Song chosen = player.catalog.getAt(position - 1);
    player.currentSong = chosen;
    player.hasCurrent  = true;
    player.playState   = PLAYING;

    // Si solo hay 1 cancion, no excluir ninguna
    int excludeId = (player.catalog.getSize() > 1) ? chosen.id : -1;
    fillQueueRandom(player, excludeId);

    FileManager::saveStatus(player);
}

// ---------- Submenu L: addToQueue ----------
void PlayerActions::addToQueue(Player& player, int position) {
    if (position < 1 || position > player.catalog.getSize()) {
        Display::showMessage("Posicion invalida.");
        return;
    }

    Song chosen = player.catalog.getAt(position - 1);
    player.playQueue.enqueue(chosen);

    FileManager::saveStatus(player);
    Display::showMessage("Cancion agregada a la lista de reproduccion.");
}

// ---------- Submenu L: addNewSong ----------
void PlayerActions::addNewSong(Player& player) {
    Song newSong;
    newSong.id = player.nextId;

    // Limpiar buffer antes de leer
    std::cin.sync();

    std::cout << std::endl;
    std::cout << "=== Agregar nueva cancion ===" << std::endl;

    std::cout << "Nombre de la cancion: ";
    std::getline(std::cin, newSong.name);

    std::cout << "Nombre del artista: ";
    std::getline(std::cin, newSong.artist);

    std::cout << "Nombre del album: ";
    std::getline(std::cin, newSong.album);

    std::cout << "Anno de lanzamiento: ";
    std::string yearStr;
    std::getline(std::cin, yearStr);
    try { newSong.year = std::stoi(yearStr); } catch (...) { newSong.year = 0; }

    std::cout << "Duracion en segundos: ";
    std::string durStr;
    std::getline(std::cin, durStr);
    try { newSong.duration = std::stoi(durStr); } catch (...) { newSong.duration = 0; }

    std::cout << "Ruta del archivo: ";
    std::getline(std::cin, newSong.path);

    if (!newSong.isValid()) {
        Display::showMessage("Datos invalidos. No se agrego la cancion.");
        return;
    }

    player.catalog.pushBack(newSong);
    player.nextId++;

    FileManager::saveCatalog(player);
    FileManager::saveStatus(player);
    Display::showMessage("Cancion agregada correctamente.");
}

// ---------- Submenu L: deleteSong ----------
void PlayerActions::deleteSong(Player& player, int position) {
    if (position < 1 || position > player.catalog.getSize()) {
        Display::showMessage("Posicion invalida.");
        return;
    }

    Song toDelete = player.catalog.getAt(position - 1);

    if (player.hasCurrent && player.currentSong.id == toDelete.id) {
        player.hasCurrent = false;
        player.playState  = STOPPED;
    }

    player.catalog.removeAt(position - 1);

    // Eliminar de la cola si estaba
    Queue<Song> tempQueue;
    while (!player.playQueue.isEmpty()) {
        Song s = player.playQueue.dequeue();
        if (s.id != toDelete.id) {
            tempQueue.enqueue(s);
        }
    }
    while (!tempQueue.isEmpty()) {
        player.playQueue.enqueue(tempQueue.dequeue());
    }

    FileManager::saveCatalog(player);
    FileManager::saveStatus(player);
    Display::showMessage("Cancion eliminada correctamente.");
}