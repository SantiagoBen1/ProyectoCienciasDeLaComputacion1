// ============================================================================
// Archivo: src/algoritmos/Nodo.hpp
// Capa: Capa A — Logica y Estructuras de Datos (sin dependencias graficas).
// Proposito: Define el vertice (class Nodo) del grafo ponderado mediante lista
//            de adyacencia (pares destino-peso).
// ============================================================================

#pragma once
#include <utility>
#include <vector>

// Representa un vertice del grafo urbano junto con sus aristas salientes.
class Nodo {
private:
  int valor; // Identificador numerico unico del vertice (sector urbano)
  std::vector<std::pair<Nodo*, int>> adyacentes; // Lista de adyacencia: (vecino, peso)

public:
  // Crea un vertice con el identificador especificado.
  Nodo(int valor);

  // Devuelve el identificador del vertice.
  int getValor() const;

  // Agrega una arista dirigida hacia 'nodo' con 'peso'; si ya existe, actualiza su peso.
  void agregarAdyacente(Nodo* nodo, int peso = 1);

  // Elimina la arista dirigida hacia 'nodo' (usado al cambiar el escenario de la red).
  bool eliminarAdyacente(Nodo* nodo);

  // Retorna en solo lectura la lista de aristas salientes (vecino, peso).
  const std::vector<std::pair<Nodo*, int>>& getAdyacentes() const;
};
