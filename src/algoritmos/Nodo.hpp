#pragma once
#include <utility>
#include <vector>

class Nodo {
private:
  int valor;
  std::vector<std::pair<Nodo*, int>> adyacentes;

public:
  Nodo(int valor);
  int getValor() const;
  void agregarAdyacente(Nodo* nodo, int peso = 1);
  // Ajuste minimo aprobado: permite activar/desactivar aristas de demostracion.
  bool eliminarAdyacente(Nodo* nodo);
  const std::vector<std::pair<Nodo*, int>>& getAdyacentes() const;
};
