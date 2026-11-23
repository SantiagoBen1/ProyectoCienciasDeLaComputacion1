#include "Nodo.hpp"

#include <algorithm>

Nodo::Nodo(int valor) : valor(valor) {
}

int Nodo::getValor() const {
  return valor;
}

void Nodo::agregarAdyacente(Nodo* nodo, int peso) {
  if (nodo == nullptr) {
    return;
  }

  for (auto& adyacente : adyacentes) {
    if (adyacente.first == nodo) {
      adyacente.second = peso;
      return;
    }
  }

  adyacentes.emplace_back(nodo, peso);
}

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

const std::vector<std::pair<Nodo*, int>>& Nodo::getAdyacentes() const {
  return adyacentes;
}
