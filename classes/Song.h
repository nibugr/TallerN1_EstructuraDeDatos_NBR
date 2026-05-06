#ifndef SONG_H
#define SONG_H

#include <string>

class Song {
public:
    int    id;           // Identificador unico correlativo
    std::string name;    // Nombre de la cancion
    std::string artist;  // Nombre del artista
    std::string album;   // Nombre del album
    int    year;         // Anno de lanzamiento
    int    duration;     // Duracion en segundos
    std::string path;    // Ubicacion del archivo en disco

    // Constructor por defecto
    Song() : id(0), year(0), duration(0) {}

    // Constructor completo
    Song(int id, std::string name, std::string artist,
         std::string album, int year, int duration, std::string path)
        : id(id), name(name), artist(artist),
          album(album), year(year), duration(duration), path(path) {}

    // Devuelve duracion formateada como mm:ss
    std::string getDurationFormatted() const {
        int min = duration / 60;
        int sec = duration % 60;
        std::string s = (sec < 10) ? "0" + std::to_string(sec) : std::to_string(sec);
        return std::to_string(min) + ":" + s;
    }

    // Devuelve true si la cancion tiene datos validos
    bool isValid() const {
        return id > 0 && !name.empty() && !artist.empty();
    }
};

#endif // SONG_H