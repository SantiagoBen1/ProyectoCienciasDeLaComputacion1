// ============================================================================
// Archivo: src/algoritmos/Nodo.cpp
// Capa: Capa A — Logica y Estructuras de Datos (sin dependencias graficas).
// Proposito: Implementacion del vertice de grafo y gestion de su lista de
//            adyacencia ponderada.
// ============================================================================

#include "Nodo.hpp"

#include <algorithm>

// Construye un vertice sin aristas salientes. Complejidad: O(1).
Nodo::Nodo(int valor) : valor(valor) {
}

// Retorna el ID del vertice. Complejidad: O(1).
int Nodo::getValor() const {
  return valor;
}

// Registra o actualiza la arista (this -> nodo) con el peso indicado.
// Complejidad: O(grado de salida del vertice).
void Nodo::agregarAdyacente(Nodo* nodo, int peso) {
  if (nodo == nullptr) {
    return;
  }

  // Evita aristas paralelas duplicadas sobrescribiendo el peso si ya existia.
  for (auto& adyacente : adyacentes) {
    if (adyacente.first == nodo) {
      adyacente.second = peso;
      return;
    }
  }

  adyacentes.emplace_back(nodo, peso);
}

// Elimina la arista hacia 'nodo' usando el idioma erase-remove.
// Retorna true si la arista existia y fue removida. Complejidad: O(grado de salida).
bool Nodo::eliminarAdyacente(Nodo* nodo) {
  auto it = std::remove_if(adyacentes.begin(), adyacentes.end(),
                           [nodo](const std::pair<Nodo*, int>& par) {
                             return par.first == nodo;
                           });
  if (it == adyacentes.end()) {
    return false;
  }
  adyacentes.erase(it, adyacentes.end());
  return true;
}

// Devuelve una referencia constante a las aristas salientes. Complejidad: O(1).
const std::vector<std::pair<Nodo*, int>>& Nodo::getAdyacentes() const {
  return adyacentes;
}
