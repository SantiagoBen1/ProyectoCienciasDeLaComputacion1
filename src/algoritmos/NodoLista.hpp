// ============================================================================
// Archivo: src/algoritmos/NodoLista.hpp
// Capa: Capa A — Logica y Estructuras de Datos (sin dependencias graficas).
// Proposito: Define el nodo simplemente enlazado (class nodo) usado por las
//            estructuras lineales dinamicas: ColaFIFO, ListaEnlazada y Pila.
//            Renombrado desde nodo.hpp para evitar colision con Nodo.hpp en Windows.
// ============================================================================

#pragma once

// Nodo basico para estructuras enlazadas simples con valor entero (ID).
class nodo {
private:
  int dato;   // Valor almacenado (identificador de paquete, repartidor o entrega)
  nodo *next; // Puntero al siguiente nodo en la secuencia (nullptr al final)

public:
  // Inicializa el nodo con su valor y puntero siguiente en nullptr.
  nodo(int dato);

  // Getters
  int getDato();
  nodo *getNext();

  // Setters
  void setDato(int dato);
  void setNext(nodo *nxt);
};
