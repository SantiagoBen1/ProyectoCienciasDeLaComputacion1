#include "algoritmos.hpp"

#include <algorithm>
#include <functional>
#include <iostream>
#include <limits>
#include <map>
#include <queue>
#include <unordered_set>

namespace algoritmos {

ColaFIFO::ColaFIFO() : frente_(nullptr), final_(nullptr), tamano_(0) {}

ColaFIFO::~ColaFIFO() {
  int valor = 0;
  while (desencolar(valor)) {
  }
}

void ColaFIFO::encolar(int valor) {
  ::nodo* nuevo = new ::nodo(valor);
  if (final_ == nullptr) {
    frente_ = final_ = nuevo;
  } else {
    final_->setNext(nuevo);
    final_ = nuevo;
  }
  ++tamano_;
}

bool ColaFIFO::desencolar(int& valor) {
  if (frente_ == nullptr) {
    return false;
  }

  ::nodo* eliminado = frente_;
  valor = eliminado->getDato();
  frente_ = frente_->getNext();
  if (frente_ == nullptr) {
    final_ = nullptr;
  }
  delete eliminado;
  --tamano_;
  return true;
}

bool ColaFIFO::consultarFrente(int& valor) const {
  if (frente_ == nullptr) {
    return false;
  }
  valor = frente_->getDato();
  return true;
}

bool ColaFIFO::estaVacia() const {
  return frente_ == nullptr;
}

std::size_t ColaFIFO::tamano() const {
  return tamano_;
}

std::vector<int> ColaFIFO::aVector() const {
  std::vector<int> resultado;
  resultado.reserve(tamano_);
  for (::nodo* actual = frente_; actual != nullptr;
       actual = actual->getNext()) {
    resultado.push_back(actual->getDato());
  }
  return resultado;
}

ColaPrioridad::ColaPrioridad() : siguienteOrden_(0) {}

bool ColaPrioridad::precede(const Elemento& a, const Elemento& b) {
  if (a.prioridad != b.prioridad) {
    return a.prioridad < b.prioridad;
  }
  return a.orden < b.orden;
}

void ColaPrioridad::subir(std::size_t indice) {
  while (indice > 0) {
    const std::size_t padre = (indice - 1) / 2;
    if (!precede(monticulo_[indice], monticulo_[padre])) {
      break;
    }
    std::swap(monticulo_[indice], monticulo_[padre]);
    indice = padre;
  }
}

void ColaPrioridad::bajar(std::size_t indice) {
  while (true) {
    const std::size_t izquierdo = indice * 2 + 1;
    const std::size_t derecho = indice * 2 + 2;
    std::size_t mejor = indice;

    if (izquierdo < monticulo_.size() &&
        precede(monticulo_[izquierdo], monticulo_[mejor])) {
      mejor = izquierdo;
    }
    if (derecho < monticulo_.size() &&
        precede(monticulo_[derecho], monticulo_[mejor])) {
      mejor = derecho;
    }
    if (mejor == indice) {
      break;
    }

    std::swap(monticulo_[indice], monticulo_[mejor]);
    indice = mejor;
  }
}

bool ColaPrioridad::encolar(int valor, int prioridad) {
  monticulo_.push_back({valor, prioridad, siguienteOrden_++});
  subir(monticulo_.size() - 1);
  return true;
}

bool ColaPrioridad::desencolar(int& valor, int& prioridad) {
  if (monticulo_.empty()) {
    return false;
  }

  valor = monticulo_.front().valor;
  prioridad = monticulo_.front().prioridad;
  if (monticulo_.size() == 1) {
    monticulo_.pop_back();
    return true;
  }

  monticulo_.front() = monticulo_.back();
  monticulo_.pop_back();
  bajar(0);
  return true;
}

bool ColaPrioridad::consultar(int& valor, int& prioridad) const {
  if (monticulo_.empty()) {
    return false;
  }
  valor = monticulo_.front().valor;
  prioridad = monticulo_.front().prioridad;
  return true;
}

bool ColaPrioridad::estaVacia() const {
  return monticulo_.empty();
}

std::size_t ColaPrioridad::tamano() const {
  return monticulo_.size();
}

std::vector<std::pair<int, int>> ColaPrioridad::aVectorOrdenado() const {
  std::vector<Elemento> copia = monticulo_;
  std::sort(copia.begin(), copia.end(), precede);
  std::vector<std::pair<int, int>> resultado;
  resultado.reserve(copia.size());
  for (const auto& elem : copia) {
    resultado.emplace_back(elem.valor, elem.prioridad);
  }
  return resultado;
}

namespace {

void combinar(std::vector<int>& valores, std::vector<int>& auxiliar,
              std::size_t inicio, std::size_t mitad, std::size_t fin,
              std::vector<PasoMerge>* pasos) {
  std::size_t izquierda = inicio;
  std::size_t derecha = mitad;
  std::size_t salida = inicio;

  while (izquierda < mitad && derecha < fin) {
    if (valores[izquierda] <= valores[derecha]) {
      auxiliar[salida++] = valores[izquierda++];
    } else {
      auxiliar[salida++] = valores[derecha++];
    }
  }
  while (izquierda < mitad) {
    auxiliar[salida++] = valores[izquierda++];
  }
  while (derecha < fin) {
    auxiliar[salida++] = valores[derecha++];
  }
  for (std::size_t i = inicio; i < fin; ++i) {
    valores[i] = auxiliar[i];
  }

  if (pasos != nullptr) {
    pasos->push_back({inicio, mitad, fin, valores});
  }
}

void mergeSortRecursivo(std::vector<int>& valores,
                        std::vector<int>& auxiliar,
                        std::size_t inicio, std::size_t fin,
                        std::vector<PasoMerge>* pasos) {
  if (fin - inicio <= 1) {
    return;
  }

  const std::size_t mitad = inicio + (fin - inicio) / 2;
  mergeSortRecursivo(valores, auxiliar, inicio, mitad, pasos);
  mergeSortRecursivo(valores, auxiliar, mitad, fin, pasos);
  combinar(valores, auxiliar, inicio, mitad, fin, pasos);
}

} // namespace

std::vector<int> mergeSort(const std::vector<int>& valores,
                           std::vector<PasoMerge>* pasos) {
  if (pasos != nullptr) {
    pasos->clear();
  }
  std::vector<int> resultado = valores;
  std::vector<int> auxiliar(resultado.size());
  mergeSortRecursivo(resultado, auxiliar, 0, resultado.size(), pasos);
  return resultado;
}

int busquedaBinaria(const std::vector<int>& valores, int objetivo,
                    int* comparaciones) {
  if (comparaciones != nullptr) {
    *comparaciones = 0;
  }
  std::size_t inicio = 0;
  std::size_t fin = valores.size();

  while (inicio < fin) {
    const std::size_t mitad = inicio + (fin - inicio) / 2;
    if (comparaciones != nullptr) {
      ++(*comparaciones);
    }
    if (valores[mitad] == objetivo) {
      return static_cast<int>(mitad);
    }
    if (valores[mitad] < objetivo) {
      inicio = mitad + 1;
    } else {
      fin = mitad;
    }
  }
  return -1;
}

int busquedaLineal(const std::vector<int>& valores, int objetivo,
                   int* comparaciones) {
  if (comparaciones != nullptr) {
    *comparaciones = 0;
  }
  for (std::size_t i = 0; i < valores.size(); ++i) {
    if (comparaciones != nullptr) {
      ++(*comparaciones);
    }
    if (valores[i] == objetivo) {
      return static_cast<int>(i);
    }
  }
  return -1;
}

ListaEnlazada::ListaEnlazada()
    : cabeza_(nullptr), cola_(nullptr), tamano_(0) {}

ListaEnlazada::~ListaEnlazada() {
  while (cabeza_ != nullptr) {
    ::nodo* eliminado = cabeza_;
    cabeza_ = cabeza_->getNext();
    delete eliminado;
  }
  cola_ = nullptr;
  tamano_ = 0;
}

void ListaEnlazada::insertarAlInicio(int valor) {
  ::nodo* nuevo = new ::nodo(valor);
  nuevo->setNext(cabeza_);
  cabeza_ = nuevo;
  if (cola_ == nullptr) {
    cola_ = nuevo;
  }
  ++tamano_;
}

void ListaEnlazada::insertarAlFinal(int valor) {
  ::nodo* nuevo = new ::nodo(valor);
  if (cola_ == nullptr) {
    cabeza_ = cola_ = nuevo;
  } else {
    cola_->setNext(nuevo);
    cola_ = nuevo;
  }
  ++tamano_;
}

bool ListaEnlazada::eliminarNodo(int valor) {
  ::nodo* actual = cabeza_;
  ::nodo* anterior = nullptr;
  while (actual != nullptr && actual->getDato() != valor) {
    anterior = actual;
    actual = actual->getNext();
  }
  if (actual == nullptr) {
    return false;
  }

  if (anterior == nullptr) {
    cabeza_ = actual->getNext();
  } else {
    anterior->setNext(actual->getNext());
  }
  if (actual == cola_) {
    cola_ = anterior;
  }

  delete actual;
  --tamano_;
  return true;
}

bool ListaEnlazada::contiene(int valor) const {
  for (::nodo* actual = cabeza_; actual != nullptr;
       actual = actual->getNext()) {
    if (actual->getDato() == valor) {
      return true;
    }
  }
  return false;
}

std::vector<int> ListaEnlazada::aVector() const {
  std::vector<int> resultado;
  resultado.reserve(tamano_);
  for (::nodo* actual = cabeza_; actual != nullptr;
       actual = actual->getNext()) {
    resultado.push_back(actual->getDato());
  }
  return resultado;
}

bool ListaEnlazada::estaVacia() const {
  return cabeza_ == nullptr;
}

std::size_t ListaEnlazada::tamano() const {
  return tamano_;
}

Pila::Pila() : tope_(nullptr), tamano_(0) {}

Pila::~Pila() {
  int valor = 0;
  while (pop(valor)) {
  }
}

void Pila::push(int valor) {
  ::nodo* nuevo = new ::nodo(valor);
  nuevo->setNext(tope_);
  tope_ = nuevo;
  ++tamano_;
}

bool Pila::pop(int& valor) {
  if (tope_ == nullptr) {
    return false;
  }

  ::nodo* eliminado = tope_;
  valor = eliminado->getDato();
  tope_ = tope_->getNext();
  delete eliminado;
  --tamano_;
  return true;
}

bool Pila::consultarTope(int& valor) const {
  if (tope_ == nullptr) {
    return false;
  }
  valor = tope_->getDato();
  return true;
}

bool Pila::estaVacia() const {
  return tope_ == nullptr;
}

std::size_t Pila::tamano() const {
  return tamano_;
}

std::vector<int> Pila::aVector() const {
  std::vector<int> resultado;
  resultado.reserve(tamano_);
  for (::nodo* actual = tope_; actual != nullptr;
       actual = actual->getNext()) {
    resultado.push_back(actual->getDato());
  }
  return resultado;
}

Grafo::Grafo(bool dirigido) : dirigido_(dirigido) {}

Grafo::~Grafo() {
  for (::Nodo* nodo : nodos_) {
    delete nodo;
  }
}

::Nodo* Grafo::agregarNodo(int valor) {
  ::Nodo* existente = buscarNodo(valor);
  if (existente != nullptr) {
    return existente;
  }

  ::Nodo* nuevo = new ::Nodo(valor);
  nodos_.push_back(nuevo);
  return nuevo;
}

bool Grafo::agregarAristaPonderada(int origen, int destino, int peso) {
  ::Nodo* nodoOrigen = buscarNodo(origen);
  ::Nodo* nodoDestino = buscarNodo(destino);
  if (nodoOrigen == nullptr || nodoDestino == nullptr) {
    return false;
  }

  nodoOrigen->agregarAdyacente(nodoDestino, peso);
  if (!dirigido_ && nodoOrigen != nodoDestino) {
    nodoDestino->agregarAdyacente(nodoOrigen, peso);
  }
  return true;
}

bool Grafo::eliminarArista(int origen, int destino) {
  ::Nodo* nodoOrigen = buscarNodo(origen);
  ::Nodo* nodoDestino = buscarNodo(destino);
  if (nodoOrigen == nullptr || nodoDestino == nullptr) {
    return false;
  }

  bool eliminada = nodoOrigen->eliminarAdyacente(nodoDestino);
  if (!dirigido_ && nodoOrigen != nodoDestino) {
    eliminada = nodoDestino->eliminarAdyacente(nodoOrigen) || eliminada;
  }
  return eliminada;
}

::Nodo* Grafo::buscarNodo(int valor) const {
  for (::Nodo* nodo : nodos_) {
    if (nodo->getValor() == valor) {
      return nodo;
    }
  }
  return nullptr;
}

const std::vector<::Nodo*>& Grafo::getNodos() const {
  return nodos_;
}

void Grafo::mostrarGrafo() const {
  for (const ::Nodo* nodo : nodos_) {
    std::cout << nodo->getValor() << " -> ";
    const auto& adyacentes = nodo->getAdyacentes();
    for (std::size_t i = 0; i < adyacentes.size(); ++i) {
      if (i > 0) {
        std::cout << ", ";
      }
      std::cout << adyacentes[i].first->getValor()
                << " (peso: " << adyacentes[i].second << ")";
    }
    std::cout << '\n';
  }
}

ResultadoRuta Grafo::Dijkstra(int origen, int destino) const {
  ResultadoRuta res;

  if (buscarNodo(origen) == nullptr || buscarNodo(destino) == nullptr) {
    res.mensaje =
        "No se puede calcular la ruta: el nodo de origen o destino no existe.";
    return res;
  }

  for (const ::Nodo* nodo : nodos_) {
    for (const auto& arista : nodo->getAdyacentes()) {
      if (arista.second < 0) {
        res.tienePesosNegativos = true;
        res.mensaje =
            "Dijkstra no es valido con pesos negativos. Use Bellman-Ford o "
            "cambie a red solo positiva.";
        return res;
      }
    }
  }

  using Entrada = std::pair<long long, int>;
  const long long infinito = std::numeric_limits<long long>::max();
  std::map<int, long long> distancias;
  std::map<int, int> predecesores;
  std::priority_queue<Entrada, std::vector<Entrada>, std::greater<Entrada>>
      pendientes;

  for (const ::Nodo* nodo : nodos_) {
    distancias[nodo->getValor()] = infinito;
  }
  distancias[origen] = 0;
  pendientes.push({0, origen});

  while (!pendientes.empty()) {
    const auto actual = pendientes.top();
    pendientes.pop();
    const long long distanciaActual = actual.first;
    const int valorActual = actual.second;

    if (distanciaActual != distancias[valorActual]) {
      continue;
    }
    if (valorActual == destino) {
      break;
    }

    ::Nodo* nodoActual = buscarNodo(valorActual);
    for (const auto& arista : nodoActual->getAdyacentes()) {
      const int vecino = arista.first->getValor();
      const long long peso = arista.second;
      if (distanciaActual > infinito - peso) {
        continue;
      }

      const long long nuevaDistancia = distanciaActual + peso;
      if (nuevaDistancia < distancias[vecino]) {
        distancias[vecino] = nuevaDistancia;
        predecesores[vecino] = valorActual;
        pendientes.push({nuevaDistancia, vecino});
      }
    }
  }

  if (distancias[destino] == infinito) {
    res.inalcanzable = true;
    res.mensaje = "No existe una ruta desde el sector " +
                  std::to_string(origen) + " hasta el sector " +
                  std::to_string(destino) + ".";
    return res;
  }

  std::vector<int> ruta;
  for (int actual = destino; actual != origen;) {
    ruta.push_back(actual);
    const auto anterior = predecesores.find(actual);
    if (anterior == predecesores.end()) {
      res.mensaje = "No se pudo reconstruir la ruta encontrada.";
      return res;
    }
    actual = anterior->second;
  }
  ruta.push_back(origen);
  std::reverse(ruta.begin(), ruta.end());

  res.exito = true;
  res.costoTotal = distancias[destino];
  res.camino = std::move(ruta);
  res.mensaje = "Dijkstra calculo la ruta optima con costo total " +
                std::to_string(res.costoTotal) + ".";
  return res;
}

ResultadoRuta Grafo::BellmanFord(int origen, int destino) const {
  ResultadoRuta res;

  if (buscarNodo(origen) == nullptr || buscarNodo(destino) == nullptr) {
    res.mensaje = "El nodo inicial o de destino no existe.";
    return res;
  }

  const long long infinito = std::numeric_limits<long long>::max();
  std::map<int, long long> distancias;
  std::map<int, int> predecesores;
  for (const ::Nodo* nodo : nodos_) {
    distancias[nodo->getValor()] = infinito;
  }
  distancias[origen] = 0;

  for (std::size_t i = 1; i < nodos_.size(); ++i) {
    bool huboCambios = false;
    for (const ::Nodo* nodo : nodos_) {
      const int valorActual = nodo->getValor();
      if (distancias[valorActual] == infinito) {
        continue;
      }

      for (const auto& arista : nodo->getAdyacentes()) {
        const int vecino = arista.first->getValor();
        const long long nuevaDistancia =
            distancias[valorActual] + arista.second;
        if (nuevaDistancia < distancias[vecino]) {
          distancias[vecino] = nuevaDistancia;
          predecesores[vecino] = valorActual;
          huboCambios = true;
        }
      }
    }
    if (!huboCambios) {
      break;
    }
  }

  std::unordered_set<int> afectados;
  std::queue<::Nodo*> porPropagar;
  for (const ::Nodo* nodo : nodos_) {
    const int valorActual = nodo->getValor();
    if (distancias[valorActual] == infinito) {
      continue;
    }
    for (const auto& arista : nodo->getAdyacentes()) {
      const int vecino = arista.first->getValor();
      if (distancias[valorActual] + arista.second < distancias[vecino] &&
          afectados.insert(vecino).second) {
        porPropagar.push(arista.first);
      }
    }
  }

  while (!porPropagar.empty()) {
    ::Nodo* actual = porPropagar.front();
    porPropagar.pop();
    for (const auto& arista : actual->getAdyacentes()) {
      if (afectados.insert(arista.first->getValor()).second) {
        porPropagar.push(arista.first);
      }
    }
  }

  if (!afectados.empty()) {
    res.hayCicloNegativo = true;
    for (const ::Nodo* nodo : nodos_) {
      if (afectados.count(nodo->getValor()) > 0) {
        res.nodosAfectadosCiclo.push_back(nodo->getValor());
      }
    }
    res.mensaje =
        "Alerta: Bellman-Ford detecto un ciclo negativo alcanzable desde el "
        "origen (nodos afectados con distancia -infinito).";
    return res;
  }

  if (distancias[destino] == infinito) {
    res.inalcanzable = true;
    res.mensaje = "No existe ruta alcanzable desde el sector " +
                  std::to_string(origen) + " hasta el sector " +
                  std::to_string(destino) + ".";
    return res;
  }

  std::vector<int> ruta;
  std::size_t limite = nodos_.size() + 1;
  for (int actual = destino; actual != origen && limite > 0; --limite) {
    ruta.push_back(actual);
    const auto anterior = predecesores.find(actual);
    if (anterior == predecesores.end()) {
      res.mensaje = "No se pudo reconstruir la ruta con Bellman-Ford.";
      return res;
    }
    actual = anterior->second;
  }
  ruta.push_back(origen);
  std::reverse(ruta.begin(), ruta.end());

  res.exito = true;
  res.costoTotal = distancias[destino];
  res.camino = std::move(ruta);
  res.mensaje = "Bellman-Ford calculo la ruta de menor costo neto: " +
                std::to_string(res.costoTotal) + ".";
  return res;
}

} // namespace algoritmos
