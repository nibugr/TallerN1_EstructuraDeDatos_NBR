#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include "../data_structures/LinkedList.h"
#include "../data_structures/Stack.h"
#include "../data_structures/Queue.h"
#include "Song.h"

// Estados posibles del reproductor
enum PlayState {
    STOPPED,   // Detenido
    PLAYING,   // Reproduciendo
    PAUSED     // En pausa
};

// Modos de repeticion
enum RepeatMode {
    REPEAT_OFF,  // Sin repeticion
    REPEAT_ONE,  // Repetir una
    REPEAT_ALL   // Repetir todas
};

class Player {
public:
    // --- Catalogo de canciones disponibles ---
    LinkedList<Song> catalog;

    // --- Lista de reproduccion pendiente (cola) ---
    Queue<Song> playQueue;

    // --- Historial para volver atras (pila) ---
    Stack<Song> history;

    // --- Estado actual ---
    Song     currentSong;       // Cancion que suena ahora
    bool     hasCurrent;        // Si hay cancion cargada
    PlayState  playState;       // STOPPED / PLAYING / PAUSED
    RepeatMode repeatMode;      // OFF / ONE / ALL
    bool     shuffleOn;         // Modo aleatorio activado
    int      nextId;            // Proximo ID correlativo

    // Constructor: valores por defecto al iniciar
    Player() :
        hasCurrent(false),
        playState(STOPPED),
        repeatMode(REPEAT_OFF),
        shuffleOn(false),
        nextId(1)
    {}

    // --- Helpers de estado en texto ---

    // Devuelve el texto del estado para mostrar en pantalla
    std::string getStateLabel() const {
        switch (playState) {
            case PLAYING: return "Reproduciendo";
            case PAUSED:  return "En Pausa";
            default:      return "Detenido";
        }
    }

    // Devuelve los indicadores S y R entre parentesis
    // Ejemplo: "(S-R1)" o "(RA)" o "" segun estado
    std::string getModeLabel() const {
        std::string s = shuffleOn ? "S" : "";
        std::string r = "";
        if (repeatMode == REPEAT_ONE) r = "R1";
        if (repeatMode == REPEAT_ALL) r = "RA";

        if (s.empty() && r.empty()) return "";
        if (s.empty()) return "(" + r + ")";
        if (r.empty()) return "(" + s + ")";
        return "(" + s + "-" + r + ")";
    }
};

#endif // PLAYER_H