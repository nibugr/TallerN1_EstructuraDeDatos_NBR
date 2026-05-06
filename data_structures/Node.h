#ifndef NODE_H
#define NODE_H

// Nodo generico con plantilla (template)
// Sirve para Lista, Pila y Cola
template <typename T>
struct Node {
    T data;        // El dato que almacena
    Node* next;    // Puntero al siguiente nodo

    // Constructor
    Node(T value) : data(value), next(nullptr) {}
};

#endif // NODE_H