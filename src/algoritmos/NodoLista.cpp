// ============================================================================
// Archivo: src/algoritmos/NodoLista.cpp
// Capa: Capa A — Logica y Estructuras de Datos (sin dependencias graficas).
// Proposito: Implementacion de los metodos de acceso y mutacion de class nodo.
// ============================================================================

#include "NodoLista.hpp"

// Construye un nodo aislado sin sucesor. Complejidad: O(1).
nodo::nodo(int dato) {
  this->dato = dato;
  this->next = nullptr;
}

// Retorna el dato entero almacenado en el nodo. Complejidad: O(1).
int nodo::getDato() {
  return this->dato;
}

// Retorna el puntero al siguiente nodo (o nullptr si es el ultimo). Complejidad: O(1).
nodo* nodo::getNext() {
  return this->next;
}

// Actualiza el valor entero contenido en el nodo. Complejidad: O(1).
void nodo::setDato(int dato) {
  this->dato = dato;
}

// Enlaza este nodo con el nodo sucesor indicado. Complejidad: O(1).
void nodo::setNext(nodo* nxt) {
  this->next = nxt;
}
