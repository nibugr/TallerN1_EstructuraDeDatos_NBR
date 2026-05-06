#ifndef QUEUE_H
#define QUEUE_H

#include "Node.h"

template <typename T>
class Queue {
private:
    Node<T>* front;  // Frente (se saca de aqui)
    Node<T>* back;   // Final (se agrega aqui)
    int size;

public:
    Queue();
    ~Queue();

    void enqueue(T value);   // Agregar al final
    T dequeue();             // Sacar del frente
    T peek();                // Ver el frente sin sacar
    bool isEmpty() const ;          // Verificar si está vacía
    int getSize() const ;           // Cantidad de elementos
    
	Node<T>* getFront() const {
    return front;
}
    void clear();            // Vaciar la cola
};

template <typename T>
Queue<T>::Queue() : front(nullptr), back(nullptr), size(0) {}

template <typename T>
Queue<T>::~Queue() {
    clear();
}

template <typename T>
void Queue<T>::enqueue(T value) {
    Node<T>* newNode = new Node<T>(value);
    if (back == nullptr) {
        front = back = newNode;
    } else {
        back->next = newNode;
        back = newNode;
    }
    size++;
}

template <typename T>
T Queue<T>::dequeue() {
    if (isEmpty()) return T();
    Node<T>* temp = front;
    T value = temp->data;
    front = front->next;
    if (front == nullptr) back = nullptr;
    delete temp;
    size--;
    return value;
}

template <typename T>
T Queue<T>::peek() {
    if (isEmpty()) return T();
    return front->data;
}

template <typename T>
bool Queue<T>::isEmpty() const {
    return front == nullptr;
}

template <typename T>
int Queue<T>::getSize() const {
    return size;
}

template <typename T>
void Queue<T>::clear() {
    while (!isEmpty()) dequeue();
}

#endif // QUEUE_H