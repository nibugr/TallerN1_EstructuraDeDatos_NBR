#include "FileManager.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <ctime>

// ---------- Eliminar \r al final de strings de Windows ----------
static void trimCR(std::string& s) {
    if (!s.empty() && s[s.size() - 1] == '\r') {
        s.erase(s.size() - 1);
    }
}

// ---------- detectDelimiter ----------
char FileManager::detectDelimiter(const std::string& line) {
    char delimiters[] = {',', '.', ':', ';', '-'};
    int maxCount = 0;
    char chosen = ',';

    for (int d = 0; d < 5; d++) {
        int count = 0;
        for (int c = 0; c < (int)line.size(); c++) {
            if (line[c] == delimiters[d]) count++;
        }
        if (count > maxCount) {
            maxCount = count;
            chosen = delimiters[d];
        }
    }
    return chosen;
}

// ---------- splitLine (sin vector, usa arreglo dinamico) ----------
int FileManager::splitLine(const std::string& line, char delimiter,
                           std::string* parts, int maxParts) {
    int count = 0;
    std::string token = "";

    for (int i = 0; i <= (int)line.size(); i++) {
        if (i == (int)line.size() || line[i] == delimiter) {
            if (count < maxParts) {
                // Trim espacios inicio
                int start = 0, end = (int)token.size() - 1;
                while (start <= end && token[start] == ' ') start++;
                while (end >= start && token[end] == ' ') end--;
                parts[count] = (start <= end)
                    ? token.substr(start, end - start + 1)
                    : "";
                count++;
            }
            token = "";
        } else {
            token += line[i];
        }
    }
    return count;
}

// ---------- loadCatalog ----------
bool FileManager::loadCatalog(Player& player, const std::string& filename) {
    std::ifstream file(filename.c_str());
    if (!file.is_open()) return false;

    player.catalog.clear();
    std::string line;
    char delimiter = ',';
    bool firstLine = true;

    while (std::getline(file, line)) {
        trimCR(line);
        if (line.empty()) continue;

        if (firstLine) {
            delimiter = detectDelimiter(line);
            firstLine = false;
        }

        // Maximo 7 campos
        std::string parts[7];
        int count = splitLine(line, delimiter, parts, 7);
        if (count < 7) continue;

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
                if (song.id >= player.nextId) {
                    player.nextId = song.id + 1;
                }
            }
        } catch (...) {
            continue;
        }
    }

    file.close();
    return true;
}

// ---------- loadStatus ----------
bool FileManager::loadStatus(Player& player, const std::string& filename) {
    std::ifstream file(filename.c_str());
    if (!file.is_open()) return false;

    std::string line;
    while (std::getline(file, line)) {
        trimCR(line);
        if (line.empty()) continue;

        size_t eq = line.find('=');
        if (eq == std::string::npos) continue;

        std::string key   = line.substr(0, eq);
        std::string value = line.substr(eq + 1);
        trimCR(key);
        trimCR(value);

        if (key == "NEXT_ID") {
            try { player.nextId = std::stoi(value); } catch (...) {}

        } else if (key == "CURRENT_SONG_ID") {
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
            if (value == "PLAYING")     player.playState = PLAYING;
            else if (value == "PAUSED") player.playState = PAUSED;
            else                        player.playState = STOPPED;

        } else if (key == "SHUFFLE") {
            player.shuffleOn = (value == "1");

        } else if (key == "REPEAT") {
            if (value == "ONE")         player.repeatMode = REPEAT_ONE;
            else if (value == "ALL")    player.repeatMode = REPEAT_ALL;
            else                        player.repeatMode = REPEAT_OFF;

        } else if (key == "QUEUE") {
            player.playQueue.clear();
            if (!value.empty() && value != "empty") {
                // Separar IDs por |
                std::string idStr = "";
                for (int i = 0; i <= (int)value.size(); i++) {
                    if (i == (int)value.size() || value[i] == '|') {
                        if (!idStr.empty()) {
                            int qid = 0;
                            try { qid = std::stoi(idStr); } catch (...) {}
                            if (qid > 0) {
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
                        idStr = "";
                    } else {
                        idStr += value[i];
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

    file << "NEXT_ID=" << player.nextId << "\n";

    if (player.hasCurrent) {
        file << "CURRENT_SONG_ID=" << player.currentSong.id << "\n";
    } else {
        file << "CURRENT_SONG_ID=0\n";
    }

    std::string stateStr = "STOPPED";
    if (player.playState == PLAYING)     stateStr = "PLAYING";
    else if (player.playState == PAUSED) stateStr = "PAUSED";
    file << "PLAY_STATE=" << stateStr << "\n";

    file << "SHUFFLE=" << (player.shuffleOn ? "1" : "0") << "\n";

    std::string repeatStr = "OFF";
    if (player.repeatMode == REPEAT_ONE)     repeatStr = "ONE";
    else if (player.repeatMode == REPEAT_ALL) repeatStr = "ALL";
    file << "REPEAT=" << repeatStr << "\n";

    // Guardar cola como IDs separados por |
    std::string queueStr = "";
    Queue<Song>& q = const_cast<Queue<Song>&>(player.playQueue);
    Node<Song>* node = q.getFront();
    while (node != nullptr) {
        if (!queueStr.empty()) queueStr += "|";
        queueStr += std::to_string(node->data.id);
        node = node->next;
    }
    file << "QUEUE=" << (queueStr.empty() ? "empty" : queueStr) << "\n";

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