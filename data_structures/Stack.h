#ifndef STACK_H
#define STACK_H

#include "Node.h"

template <typename T>
class Stack {
private:
    Node<T>* top;  // Tope de la pila
    int size;

public:
    Stack();
    ~Stack();

    void push(T value);   // Agregar al tope
    T pop();              // Sacar del tope
    T peek();             // Ver el tope sin sacar
    bool isEmpty() const;       // Verificar si está vacía
    int getSize() const;        // Cantidad de elementos
    void clear();         // Vaciar la pila
};

template <typename T>
Stack<T>::Stack() : top(nullptr), size(0) {}

template <typename T>
Stack<T>::~Stack() {
    clear();
}

template <typename T>
void Stack<T>::push(T value) {
    Node<T>* newNode = new Node<T>(value);
    newNode->next = top;
    top = newNode;
    size++;
}

template <typename T>
T Stack<T>::pop() {
    if (isEmpty()) return T();
    Node<T>* temp = top;
    T value = temp->data;
    top = top->next;
    delete temp;
    size--;
    return value;
}

template <typename T>
T Stack<T>::peek() {
    if (isEmpty()) return T();
    return top->data;
}

template <typename T>
bool Stack<T>::isEmpty() const {
    return top == nullptr;
}

template <typename T>
int Stack<T>::getSize() const {
    return size;
}

template <typename T>
void Stack<T>::clear() {
    while (!isEmpty()) pop();
}

#endif // STACK_H