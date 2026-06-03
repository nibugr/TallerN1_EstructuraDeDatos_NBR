# TallerN1_EstructuraDeDatos_NBR
# 🎵 MusicPlayer "Ucenin"
```
*Estudiante: Nicolás Ignacio Bugueño Rementería 
*Carrera: Ingeniería Civil en Computación e informática 
*Asignatura: Estructuras de Datos — 1° Semestre 2026
*Profesor: Bastian Ruiz Garay | Ayudante: Pablo Guzmán Vargas
```
## Instrucciones de compilación

### Requisitos
- Compilador GCC con soporte C++14 o superior
- Windows 10/11

### Con Dev-C++ (TDM-GCC)
1. Abrir el archivo `TallerN1_EstructuraDeDatos_NBR.dev`
2. Presionar F11 para compilar y ejecutar

### Con g++ desde terminal
```bash
g++ -std=c++14 -o reproductor main.cpp core/FileManager.cpp core/Display.cpp core/PlayerActions.cpp
./reproductor
```

## Archivos necesarios
Asegúrate de tener en la misma carpeta del ejecutable:
- `music_source.txt` — listado de canciones
- `status.cfg` — configuración del reproductor (se genera automáticamente)

## Funcionalidades
- W: Reproducir / Pausar la canción actual
- Q: Volver a la pista anterior
- E: Avanzar a la pista siguiente
- S: Activar/desactivar modo aleatorio
- R: Ciclar modo de repetición (Off / Una / Todas)
- A: Ver y gestionar la lista de reproducción
- L: Ver catálogo, agregar y eliminar canciones
- X: Salir guardando el estado
