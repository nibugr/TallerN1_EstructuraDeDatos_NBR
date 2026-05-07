#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "Node.h"
#include <iostream>

template <typename T>
class LinkedList {
private:
    Node<T>* head;  // Primer nodo
    int size;       // Cantidad de elementos

public:
    LinkedList();
    ~LinkedList();

    void pushBack(T value);        // Agregar al final
    void pushFront(T value);       // Agregar al inicio
    bool removeAt(int index);      // Eliminar por posicion
    T getAt(int index);            // Obtener elemento por posicion
    int getSize() const;           // Retorna cantidad de elementos
    bool isEmpty() const;          // Verifica si esta vacia
    void clear();                  // Vaciar la lista
    void print();                  // Mostrar contenido (debug)
    Node<T>* getHead() const;      // Acceder al nodo inicial
};

// --- IMPLEMENTACION INLINE (necesario para templates) ---

template <typename T>
LinkedList<T>::LinkedList() : head(nullptr), size(0) {}

template <typename T>
LinkedList<T>::~LinkedList() {
    clear();
}

template <typename T>
void LinkedList<T>::pushBack(T value) {
    Node<T>* newNode = new Node<T>(value);
    if (head == nullptr) {
        head = newNode;
    } else {
        Node<T>* current = head;
        while (current->next != nullptr) {
            current = current->next;
        }
        current->next = newNode;
    }
    size++;
}

template <typename T>
void LinkedList<T>::pushFront(T value) {
    Node<T>* newNode = new Node<T>(value);
    newNode->next = head;
    head = newNode;
    size++;
}

template <typename T>
bool LinkedList<T>::removeAt(int index) {
    if (index < 0 || index >= size || head == nullptr) return false;

    Node<T>* toDelete = nullptr;
    if (index == 0) {
        toDelete = head;
        head = head->next;
    } else {
        Node<T>* current = head;
        for (int i = 0; i < index - 1; i++) {
            current = current->next;
        }
        toDelete = current->next;
        current->next = toDelete->next;
    }
    delete toDelete;
    size--;
    return true;
}

template <typename T>
T LinkedList<T>::getAt(int index) {
    // Validacion de rango
    if (index < 0 || index >= size || head == nullptr) return T();
    Node<T>* current = head;
    for (int i = 0; i < index; i++) {
        current = current->next;
    }
    return current->data;
}

template <typename T>
int LinkedList<T>::getSize() const {
    return size;
}

template <typename T>
bool LinkedList<T>::isEmpty() const {
    return size == 0;
}

template <typename T>
void LinkedList<T>::clear() {
    Node<T>* current = head;
    while (current != nullptr) {
        Node<T>* next = current->next;
        delete current;
        current = next;
    }
    head = nullptr;
    size = 0;
}

template <typename T>
Node<T>* LinkedList<T>::getHead() const {
    return head;
}

#endif // LINKEDLIST_H