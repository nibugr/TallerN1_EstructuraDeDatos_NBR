#include "PlayerActions.h"
#include "Display.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>

// ---------- fillQueueRandom ----------
void PlayerActions::fillQueueRandom(Player& player, int excludeId) {
    // Copiar todas las canciones del catalogo a un vector temporal
    std::vector<Song> songs;
    Node<Song>* node = player.catalog.getHead();
    while (node != nullptr) {
        if (node->data.id != excludeId) {
            songs.push_back(node->data);
        }
        node = node->next;
    }

    // Mezclar el vector con Fisher-Yates
    srand((unsigned int)time(nullptr));
    for (int i = (int)songs.size() - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Song temp = songs[i];
        songs[i] = songs[j];
        songs[j] = temp;
    }

    // Vaciar cola actual y llenarla con las canciones mezcladas
    player.playQueue.clear();
    for (int i = 0; i < (int)songs.size(); i++) {
        player.playQueue.enqueue(songs[i]);
    }
}

// ---------- shuffleQueue ----------
void PlayerActions::shuffleQueue(Player& player) {
    // Sacar todo de la cola a un vector
    std::vector<Song> songs;
    while (!player.playQueue.isEmpty()) {
        songs.push_back(player.playQueue.dequeue());
    }

    // Mezclar
    srand((unsigned int)time(nullptr));
    for (int i = (int)songs.size() - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Song temp = songs[i];
        songs[i] = songs[j];
        songs[j] = temp;
    }

    // Volver a encolar
    for (int i = 0; i < (int)songs.size(); i++) {
        player.playQueue.enqueue(songs[i]);
    }
}

// ---------- W: togglePlay ----------
void PlayerActions::togglePlay(Player& player) {
    if (player.catalog.isEmpty()) return;

    if (!player.hasCurrent) {
        // No hay cancion — seleccionar la primera del catalogo
        player.currentSong = player.catalog.getAt(0);
        player.hasCurrent  = true;
        player.playState   = PLAYING;
        fillQueueRandom(player, player.currentSong.id);
    } else {
        // Alternar entre PLAYING y PAUSED
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

    // Si repetir una esta activo, no se cambia de pista
    if (player.repeatMode == REPEAT_ONE) return;

    // La cancion actual vuelve al frente de la cola
    if (player.hasCurrent) {
        // Crear una cola temporal con la cancion actual al frente
        Queue<Song> tempQueue;
        tempQueue.enqueue(player.currentSong);
        while (!player.playQueue.isEmpty()) {
            tempQueue.enqueue(player.playQueue.dequeue());
        }
        while (!tempQueue.isEmpty()) {
            player.playQueue.enqueue(tempQueue.dequeue());
        }
    }

    // Sacar la cancion anterior del historial
    player.currentSong = player.history.pop();
    player.hasCurrent  = true;
    player.playState   = PLAYING;

    FileManager::saveStatus(player);
}

// ---------- E: nextTrack ----------
void PlayerActions::nextTrack(Player& player) {
    if (player.catalog.isEmpty()) return;

    // Si repetir una esta activo, no cambia de pista
    if (player.repeatMode == REPEAT_ONE) return;

    // Guardar cancion actual en historial
    if (player.hasCurrent) {
        player.history.push(player.currentSong);
    }

    // Si la cola esta vacia, rellenar
    if (player.playQueue.isEmpty()) {
        if (player.repeatMode == REPEAT_ALL) {
            // Rellenar la cola segun modo aleatorio
            if (player.shuffleOn) {
                fillQueueRandom(player, -1);
            } else {
                // Orden normal: agregar todas las canciones del catalogo
                player.playQueue.clear();
                Node<Song>* node = player.catalog.getHead();
                while (node != nullptr) {
                    player.playQueue.enqueue(node->data);
                    node = node->next;
                }
            }
        } else {
            // Sin repeticion y cola vacia: rellenar aleatoriamente
            fillQueueRandom(player, player.hasCurrent ? player.currentSong.id : -1);
        }
    }

    // Tomar la siguiente cancion de la cola
    player.currentSong = player.playQueue.dequeue();
    player.hasCurrent  = true;
    player.playState   = PLAYING;

    FileManager::saveStatus(player);
}

// ---------- S: toggleShuffle ----------
void PlayerActions::toggleShuffle(Player& player) {
    if (player.catalog.isEmpty()) return;

    player.shuffleOn = !player.shuffleOn;

    // Al activar: mezclar la cola actual
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
    // position es 1-based
    if (position < 1 || position > player.playQueue.getSize()) {
        Display::showMessage("Posicion invalida.");
        return;
    }

    // Guardar cancion actual en historial
    if (player.hasCurrent) {
        player.history.push(player.currentSong);
    }

    // Descartar canciones hasta llegar a la posicion
    for (int i = 1; i < position; i++) {
        player.playQueue.dequeue(); // descarta sin guardar
    }

    // La cancion en la posicion indicada es la nueva actual
    player.currentSong = player.playQueue.dequeue();
    player.hasCurrent  = true;
    player.playState   = PLAYING;

    FileManager::saveStatus(player);
}

// ---------- Submenu L: playSongFromCatalog ----------
void PlayerActions::playSongFromCatalog(Player& player, int position) {
    // position es 1-based
    if (position < 1 || position > player.catalog.getSize()) {
        Display::showMessage("Posicion invalida.");
        return;
    }

    // Guardar actual en historial
    if (player.hasCurrent) {
        player.history.push(player.currentSong);
    }

    Song chosen = player.catalog.getAt(position - 1);
    player.currentSong = chosen;
    player.hasCurrent  = true;
    player.playState   = PLAYING;

    // Llenar cola con el resto aleatoriamente
    fillQueueRandom(player, chosen.id);

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

    std::cout << std::endl;
    std::cout << "=== Agregar nueva cancion ===" << std::endl;

    std::cout << "Nombre de la cancion: ";
    std::cin.ignore();
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

    // Si es la cancion actual, detener reproduccion
    if (player.hasCurrent && player.currentSong.id == toDelete.id) {
        player.hasCurrent  = false;
        player.playState   = STOPPED;
    }

    // Eliminar del catalogo
    player.catalog.removeAt(position - 1);

    // Eliminar de la cola si estaba ahi
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