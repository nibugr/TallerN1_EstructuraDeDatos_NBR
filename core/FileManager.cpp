#include "FileManager.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <cstdlib>   // para rand()
#include <ctime>

// ---------- detectDelimiter ----------
char FileManager::detectDelimiter(const std::string& line) {
    // Prueba cada delimitador posible segun el enunciado
    // Elige el que aparezca mas veces (mas confiable)
    char delimiters[] = {',', '.', ':', ';', '-'};
    int maxCount = 0;
    char chosen = ',';

    for (char d : delimiters) {
        int count = 0;
        for (char c : line) if (c == d) count++;
        if (count > maxCount) {
            maxCount = count;
            chosen = d;
        }
    }
    return chosen;
}

// ---------- splitLine ----------
std::vector<std::string> FileManager::splitLine(const std::string& line, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream stream(line);
    while (std::getline(stream, token, delimiter)) {
        // Eliminar espacios al inicio y final
        int start = 0, end = (int)token.size() - 1;
        while (start <= end && token[start] == ' ') start++;
        while (end >= start && token[end] == ' ') end--;
        tokens.push_back(token.substr(start, end - start + 1));
    }
    return tokens;
}

// ---------- loadCatalog ----------
bool FileManager::loadCatalog(Player& player, const std::string& filename) {
    std::ifstream file(filename.c_str());
    if (!file.is_open()) {
        return false;
    }

    player.catalog.clear();
    std::string line;
    char delimiter = ','; // por defecto
    bool firstLine = true;

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        // Detectar delimitador con la primera linea valida
        if (firstLine) {
            delimiter = detectDelimiter(line);
            firstLine = false;
        }

        std::vector<std::string> parts = splitLine(line, delimiter);

        // Necesitamos exactamente 7 campos
        if (parts.size() < 7) continue;

        try {
            Song song;
            song.id       = std::stoi(parts[0]);
            song.name     = parts[1];
            song.artist   = parts[2];
            song.album    = parts[3];
            song.year     = std::stoi(parts[4]);
            song.duration = std::stoi(parts[5]);
            song.path     = parts[6];

            if (song.isValid()) {
                player.catalog.pushBack(song);
                // Actualizar el proximo ID
                if (song.id >= player.nextId) {
                    player.nextId = song.id + 1;
                }
            }
        } catch (...) {
            // Si hay error en conversion, ignorar esa linea
            continue;
        }
    }

    file.close();
    return true;
}

// ---------- loadStatus ----------
bool FileManager::loadStatus(Player& player, const std::string& filename) {
    std::ifstream file(filename.c_str());
    if (!file.is_open()) {
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        // Buscar el signo = para separar PARAMETRO=VALOR
        size_t eq = line.find('=');
        if (eq == std::string::npos) continue;

        std::string key   = line.substr(0, eq);
        std::string value = line.substr(eq + 1);

        if (key == "NEXT_ID") {
            try { player.nextId = std::stoi(value); } catch (...) {}

        } else if (key == "CURRENT_SONG_ID") {
            // Buscar la cancion en el catalogo por ID
            int id = 0;
            try { id = std::stoi(value); } catch (...) {}
            if (id > 0) {
                Node<Song>* node = player.catalog.getHead();
                while (node != nullptr) {
                    if (node->data.id == id) {
                        player.currentSong = node->data;
                        player.hasCurrent  = true;
                        break;
                    }
                    node = node->next;
                }
            }

        } else if (key == "PLAY_STATE") {
            if (value == "PLAYING") player.playState = PLAYING;
            else if (value == "PAUSED") player.playState = PAUSED;
            else player.playState = STOPPED;

        } else if (key == "SHUFFLE") {
            player.shuffleOn = (value == "1");

        } else if (key == "REPEAT") {
            if (value == "ONE") player.repeatMode = REPEAT_ONE;
            else if (value == "ALL") player.repeatMode = REPEAT_ALL;
            else player.repeatMode = REPEAT_OFF;

        } else if (key == "QUEUE") {
            // La cola se guarda como IDs separados por |
            // Ejemplo: QUEUE=3|7|1|5
            player.playQueue.clear();
            if (!value.empty() && value != "empty") {
                std::istringstream ss(value);
                std::string idStr;
                while (std::getline(ss, idStr, '|')) {
                    int qid = 0;
                    try { qid = std::stoi(idStr); } catch (...) { continue; }
                    // Buscar la cancion en el catalogo
                    Node<Song>* node = player.catalog.getHead();
                    while (node != nullptr) {
                        if (node->data.id == qid) {
                            player.playQueue.enqueue(node->data);
                            break;
                        }
                        node = node->next;
                    }
                }
            }
        }
    }

    file.close();
    return true;
}

// ---------- saveStatus ----------
void FileManager::saveStatus(const Player& player, const std::string& filename) {
    std::ofstream file(filename.c_str());
    if (!file.is_open()) return;

    // NEXT_ID
    file << "NEXT_ID=" << player.nextId << "\n";

    // CURRENT_SONG_ID
    if (player.hasCurrent) {
        file << "CURRENT_SONG_ID=" << player.currentSong.id << "\n";
    } else {
        file << "CURRENT_SONG_ID=0\n";
    }

    // PLAY_STATE
    std::string stateStr = "STOPPED";
    if (player.playState == PLAYING) stateStr = "PLAYING";
    else if (player.playState == PAUSED) stateStr = "PAUSED";
    file << "PLAY_STATE=" << stateStr << "\n";

    // SHUFFLE
    file << "SHUFFLE=" << (player.shuffleOn ? "1" : "0") << "\n";

    // REPEAT
    std::string repeatStr = "OFF";
    if (player.repeatMode == REPEAT_ONE) repeatStr = "ONE";
    else if (player.repeatMode == REPEAT_ALL) repeatStr = "ALL";
    file << "REPEAT=" << repeatStr << "\n";

    // QUEUE — guardar IDs separados por |
    file << "QUEUE=";
    // Necesitamos recorrer la cola sin destruirla
    // Usamos un Queue temporal para copiarla
    Queue<Song> tempQueue;
    // No podemos copiar directo, recorremos con nodo
    // (accedemos al nodo interno de la cola original via cast)
    // Para esto usaremos un truco: guardamos en vector temporal
    std::string queueStr = "";

    // Recorremos la cola original copiando a temporal
    Queue<Song>& q = const_cast<Queue<Song>&>(player.playQueue);
    Node<Song>* node = q.getFront(); // necesitamos agregar getFront()

    while (node != nullptr) {
        if (!queueStr.empty()) queueStr += "|";
        queueStr += std::to_string(node->data.id);
        node = node->next;
    }

    file << (queueStr.empty() ? "empty" : queueStr) << "\n";
    file.close();
}

// ---------- saveCatalog ----------
void FileManager::saveCatalog(const Player& player, const std::string& filename) {
    std::ofstream file(filename.c_str());
    if (!file.is_open()) return;

    Node<Song>* node = player.catalog.getHead();
    while (node != nullptr) {
        Song& s = node->data;
        file << s.id       << ","
             << s.name     << ","
             << s.artist   << ","
             << s.album    << ","
             << s.year     << ","
             << s.duration << ","
             << s.path     << "\n";
        node = node->next;
    }
    file.close();
}