#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <string>
#include "../classes/Player.h"

class FileManager {
public:
    static bool loadCatalog(Player& player, const std::string& filename = "music_source.txt");
    static bool loadStatus(Player& player, const std::string& filename = "status.cfg");
    static void saveStatus(const Player& player, const std::string& filename = "status.cfg");
    static void saveCatalog(const Player& player, const std::string& filename = "music_source.txt");

private:
    static char detectDelimiter(const std::string& line);

    // Ahora retorna cantidad de partes, llena arreglo dinamico
    static int splitLine(const std::string& line, char delimiter,
                         std::string* parts, int maxParts);
};

#endif // FILEMANAGER_H