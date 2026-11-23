#pragma once

// Renombrado desde nodo.hpp -> NodoLista.hpp para evitar colision de nombres
// con Nodo.hpp en sistemas de archivos insensibles a mayusculas (Windows).
// Se removio #include <bits/stdc++.h> y using namespace std para compatibilidad
// estandar C++17 (g++ / MSVC). Logica de la clase nodo sin cambios.

class nodo {
private:
  int dato;
  nodo* next;

public:
  nodo(int dato);

  // Getters
  int getDato();
  nodo* getNext();

  // Setters
  void setDato(int dato);
  void setNext(nodo* nxt);
};
