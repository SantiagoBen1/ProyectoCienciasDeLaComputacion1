#include "NodoLista.hpp"

nodo::nodo(int dato) {
  this->dato = dato;
  this->next = nullptr;
}

int nodo::getDato() {
  return this->dato;
}

nodo* nodo::getNext() {
  return this->next;
}

void nodo::setDato(int dato) {
  this->dato = dato;
}

void nodo::setNext(nodo* nxt) {
  this->next = nxt;
}
