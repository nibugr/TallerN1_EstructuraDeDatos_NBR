#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <string>
#include <vector>  
#include "../classes/Player.h"

class FileManager {
public:
    // Carga el catalogo desde music_source.txt
    // Retorna true si el archivo existe y se cargo bien
    static bool loadCatalog(Player& player, const std::string& filename = "music_source.txt");

    // Carga configuracion desde status.cfg
    // Retorna true si el archivo existe
    static bool loadStatus(Player& player, const std::string& filename = "status.cfg");

    // Guarda el estado actual en status.cfg
    static void saveStatus(const Player& player, const std::string& filename = "status.cfg");

    // Guarda el catalogo completo en music_source.txt
    static void saveCatalog(const Player& player, const std::string& filename = "music_source.txt");

private:
    // Detecta el delimitador del archivo (coma, punto, etc.)
    static char detectDelimiter(const std::string& line);

    // Divide una linea segun el delimitador detectado
    static std::vector<std::string> splitLine(const std::string& line, char delimiter);
};

#endif // FILEMANAGER_H