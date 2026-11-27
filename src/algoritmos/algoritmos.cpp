// ============================================================================
// Archivo: src/algoritmos/algoritmos.cpp
// Capa: Capa A — Logica y Estructuras de Datos (sin dependencias graficas).
// Proposito: Implementacion de las estructuras dinamicas (ColaFIFO,
//            ColaPrioridad, ListaEnlazada, Pila, Grafo) y algoritmos clasicos
//            (Merge Sort, Busqueda Binaria, Busqueda Lineal, Dijkstra y
//            Bellman-Ford).
// ============================================================================

#include "algoritmos.hpp"

#include <algorithm>
#include <functional>
#include <iostream>
#include <limits>
#include <map>
#include <queue>
#include <unordered_set>

namespace algoritmos {

// ============================================================================
// 1. ColaFIFO (Cola enlazada First-In, First-Out)
// ============================================================================

ColaFIFO::ColaFIFO() : frente_(nullptr), final_(nullptr), tamano_(0) {}

// Destructor: vacia la cola liberando cada nodo dinamico para evitar fugas.
ColaFIFO::~ColaFIFO() {
  int valor = 0;
  while (desencolar(valor)) {
  }
}

// Inserta un nuevo nodo al final de la cola. Complejidad: O(1).
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

// Extrae el nodo del frente, devuelve su valor y libera su memoria. Complejidad: O(1).
bool ColaFIFO::desencolar(int& valor) {
  if (frente_ == nullptr) {
    return false;
  }

  ::nodo* eliminado = frente_;
  valor = eliminado->getDato();
  frente_ = frente_->getNext();
  if (frente_ == nullptr) {
    final_ = nullptr; // La cola quedo vacia
  }
  delete eliminado;
  --tamano_;
  return true;
}

// Consulta el frente sin extraerlo. Complejidad: O(1).
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

// Recorre la cola sin modificarla para renderizar las tarjetas en orden FIFO. O(n).
std::vector<int> ColaFIFO::aVector() const {
  std::vector<int> resultado;
  resultado.reserve(tamano_);
  for (::nodo* actual = frente_; actual != nullptr;
       actual = actual->getNext()) {
    resultado.push_back(actual->getDato());
  }
  return resultado;
}

// ============================================================================
// 2. ColaPrioridad (Min-Heap binario estable)
// ============================================================================

ColaPrioridad::ColaPrioridad() : siguienteOrden_(0) {}

// Criterio de orden del min-heap: menor prioridad primero; ante empate, FIFO.
bool ColaPrioridad::precede(const Elemento& a, const Elemento& b) {
  if (a.prioridad != b.prioridad) {
    return a.prioridad < b.prioridad;
  }
  return a.orden < b.orden;
}

// Flota un elemento recien insertado hacia la raiz mientras preceda a su padre. O(log n).
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

// Hunde la raiz hacia las hojas eligiendo el hijo con mayor precedencia. O(log n).
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

// Inserta un nuevo elemento al final del vector y restaura el monticulo. O(log n).
bool ColaPrioridad::encolar(int valor, int prioridad) {
  monticulo_.push_back({valor, prioridad, siguienteOrden_++});
  subir(monticulo_.size() - 1);
  return true;
}

// Extrae el elemento mas prioritario (raiz), mueve el ultimo a la raiz y hunde. O(log n).
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

// Consulta el elemento de mayor prioridad sin extraerlo. O(1).
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

// Devuelve una copia ordenada segun la precedencia real de despacho sin alterar
// el monticulo interno. Complejidad: O(n log n).
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

// ============================================================================
// 3. Merge Sort (Division y Conquista) + Busqueda Binaria y Lineal
// ============================================================================

namespace {

// Mezcla dos subarreglos ordenados adyacentes [inicio, mitad) y [mitad, fin)
// en orden ascendente usando un buffer auxiliar compartido. Complejidad: O(fin - inicio).
void combinar(std::vector<int>& valores, std::vector<int>& auxiliar,
              std::size_t inicio, std::size_t mitad, std::size_t fin,
              std::vector<PasoMerge>* pasos) {
  std::size_t izquierda = inicio;
  std::size_t derecha = mitad;
  std::size_t salida = inicio;

  // Selecciona siempre el menor entre la cabeza izquierda y la derecha (orden estable).
  while (izquierda < mitad && derecha < fin) {
    if (valores[izquierda] <= valores[derecha]) {
      auxiliar[salida++] = valores[izquierda++];
    } else {
      auxiliar[salida++] = valores[derecha++];
    }
  }
  // Copia los elementos remanentes de cualquiera de las dos mitades.
  while (izquierda < mitad) {
    auxiliar[salida++] = valores[izquierda++];
  }
  while (derecha < fin) {
    auxiliar[salida++] = valores[derecha++];
  }
  // Vuelca el rango ya mezclado de vuelta al arreglo principal.
  for (std::size_t i = inicio; i < fin; ++i) {
    valores[i] = auxiliar[i];
  }

  // Guarda la instantanea del paso si la interfaz solicito animacion.
  if (pasos != nullptr) {
    pasos->push_back({inicio, mitad, fin, valores});
  }
}

// Divide recursivamente el rango [inicio, fin) hasta subarreglos de tamano <= 1
// y luego los combina de forma ordenada. Complejidad: O(n log n).
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

// Punto de entrada de Merge Sort: reserva un unico vector auxiliar O(n) y ordena.
// Complejidad temporal: O(n log n) en mejor, promedio y peor caso.
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

// Busqueda Binaria sobre intervalo semiabierto [inicio, fin).
// Descarta la mitad del espacio en cada iteracion. Complejidad: O(log n).
// Precondicion: 'valores' debe estar ordenado ascendentemente.
int busquedaBinaria(const std::vector<int>& valores, int objetivo,
                    int* comparaciones,
                    std::vector<int>* historialPivotes) {
  if (comparaciones != nullptr) {
    *comparaciones = 0;
  }
  if (historialPivotes != nullptr) {
    historialPivotes->clear();
  }
  std::size_t inicio = 0;
  std::size_t fin = valores.size();

  while (inicio < fin) {
    const std::size_t mitad = inicio + (fin - inicio) / 2;
    if (comparaciones != nullptr) {
      ++(*comparaciones);
    }
    if (historialPivotes != nullptr) {
      historialPivotes->push_back(valores[mitad]);
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

// Busqueda Lineal secuencial: funciona sobre vectores ordenados o desordenados.
// Complejidad: O(n) en peor caso.
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

// ============================================================================
// 4. ListaEnlazada (Nomina dinamica de repartidores)
// ============================================================================

ListaEnlazada::ListaEnlazada()
    : cabeza_(nullptr), cola_(nullptr), tamano_(0) {}

// Destructor: recorre desde cabeza_ liberando cada nodo con delete.
ListaEnlazada::~ListaEnlazada() {
  while (cabeza_ != nullptr) {
    ::nodo* eliminado = cabeza_;
    cabeza_ = cabeza_->getNext();
    delete eliminado;
  }
  cola_ = nullptr;
  tamano_ = 0;
}

// Inserta un nuevo nodo al comienzo (HEAD). Complejidad: O(1).
void ListaEnlazada::insertarAlInicio(int valor) {
  ::nodo* nuevo = new ::nodo(valor);
  nuevo->setNext(cabeza_);
  cabeza_ = nuevo;
  if (cola_ == nullptr) {
    cola_ = nuevo;
  }
  ++tamano_;
}

// Inserta un nuevo nodo al final (TAIL) aprovechando el puntero cola_. O(1).
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

// Busca la primera ocurrencia de 'valor', re-enlaza punteros y libera memoria.
// Complejidad: O(n).
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

  // Actualiza cabeza_ si se elimino el primer nodo, o el puntero next del anterior.
  if (anterior == nullptr) {
    cabeza_ = actual->getNext();
  } else {
    anterior->setNext(actual->getNext());
  }
  // Si se elimino el ultimo nodo, actualiza cola_ al nuevo final.
  if (actual == cola_) {
    cola_ = anterior;
  }

  delete actual;
  --tamano_;
  return true;
}

// Recorre la lista para comprobar si existe un nodo con 'valor'. Complejidad: O(n).
bool ListaEnlazada::contiene(int valor) const {
  for (::nodo* actual = cabeza_; actual != nullptr;
       actual = actual->getNext()) {
    if (actual->getDato() == valor) {
      return true;
    }
  }
  return false;
}

// Exporta los valores en orden HEAD -> TAIL sin modificar la lista. O(n).
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

// ============================================================================
// 5. Pila (LIFO: Last-In, First-Out para historial y reversion de entregas)
// ============================================================================

Pila::Pila() : tope_(nullptr), tamano_(0) {}

// Destructor: desapila y libera todos los nodos dinamicos.
Pila::~Pila() {
  int valor = 0;
  while (pop(valor)) {
  }
}

// Apila un nuevo nodo sobre el tope actual. Complejidad: O(1).
void Pila::push(int valor) {
  ::nodo* nuevo = new ::nodo(valor);
  nuevo->setNext(tope_);
  tope_ = nuevo;
  ++tamano_;
}

// Extrae el nodo del tope, retorna su valor y libera memoria. Complejidad: O(1).
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

// Consulta el valor del tope sin extraerlo. Complejidad: O(1).
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

// Recorre la pila desde el tope hasta la base (orden cronologico inverso). O(n).
std::vector<int> Pila::aVector() const {
  std::vector<int> resultado;
  resultado.reserve(tamano_);
  for (::nodo* actual = tope_; actual != nullptr;
       actual = actual->getNext()) {
    resultado.push_back(actual->getDato());
  }
  return resultado;
}

// ============================================================================
// 6. Grafo Ponderado + Dijkstra + Bellman-Ford
// ============================================================================

Grafo::Grafo(bool dirigido) : dirigido_(dirigido) {}

// Destructor: libera todos los vertices dinamicos creados en el grafo.
Grafo::~Grafo() {
  for (::Nodo* nodo : nodos_) {
    delete nodo;
  }
}

// Agrega un vertice si no existia previamente y retorna su puntero. O(V).
::Nodo* Grafo::agregarNodo(int valor) {
  ::Nodo* existente = buscarNodo(valor);
  if (existente != nullptr) {
    return existente;
  }

  ::Nodo* nuevo = new ::Nodo(valor);
  nodos_.push_back(nuevo);
  return nuevo;
}

// Conecta origen -> destino con 'peso' (y la inversa si el grafo es no dirigido).
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

// Elimina la arista origen -> destino (y la inversa si es no dirigido).
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

// Localiza un vertice por su ID. Complejidad: O(V).
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

// Imprime la lista de adyacencia por consola (utilidad de depuracion).
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

// Algoritmo de Dijkstra con cola de prioridad minima.
// Precondicion verificada: todas las aristas del grafo deben tener peso >= 0.
// Complejidad: O((V + E) log V) usando min-heap.
ResultadoRuta Grafo::Dijkstra(int origen, int destino) const {
  ResultadoRuta res;

  if (buscarNodo(origen) == nullptr || buscarNodo(destino) == nullptr) {
    res.mensaje =
        "No se puede calcular la ruta: el nodo de origen o destino no existe.";
    return res;
  }

  // Validacion obligatoria: Dijkstra no garantiza optimalidad con aristas negativas.
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

  using Entrada = std::pair<long long, int>; // (distancia acumulada, idNodo)
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

  // Bucle voraz: extrae el nodo no fijado con menor distancia tentativa.
  while (!pendientes.empty()) {
    const auto actual = pendientes.top();
    pendientes.pop();
    const long long distanciaActual = actual.first;
    const int valorActual = actual.second;

    // Descarta entradas obsoletas que ya fueron mejoradas en la cola.
    if (distanciaActual != distancias[valorActual]) {
      continue;
    }
    ++res.iteraciones;
    // Al extraer el destino de la cola minima, su distancia optima ya es definitiva.
    if (valorActual == destino) {
      break;
    }

    ::Nodo* nodoActual = buscarNodo(valorActual);
    for (const auto& arista : nodoActual->getAdyacentes()) {
      const int vecino = arista.first->getValor();
      const long long peso = arista.second;
      if (distanciaActual > infinito - peso) {
        continue; // Proteccion contra desbordamiento de entero
      }

      // Relajacion de arista (valorActual -> vecino).
      const long long nuevaDistancia = distanciaActual + peso;
      if (nuevaDistancia < distancias[vecino]) {
        distancias[vecino] = nuevaDistancia;
        predecesores[vecino] = valorActual;
        pendientes.push({nuevaDistancia, vecino});
        ++res.relajaciones;
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

  // Reconstruye el camino desde 'destino' hacia 'origen' usando predecesores.
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

  // Recopila el peso de cada tramo para detallar la ruta en la interfaz.
  for (std::size_t i = 0; i + 1 < ruta.size(); ++i) {
    const ::Nodo* u = buscarNodo(ruta[i]);
    int pesoArista = 0;
    if (u != nullptr) {
      for (const auto& arista : u->getAdyacentes()) {
        if (arista.first->getValor() == ruta[i + 1]) {
          pesoArista = arista.second;
          break;
        }
      }
    }
    res.pesosTramos.push_back(pesoArista);
  }

  res.exito = true;
  res.costoTotal = distancias[destino];
  res.camino = std::move(ruta);
  res.mensaje = "Dijkstra calculo la ruta optima con costo total " +
                std::to_string(res.costoTotal) + ".";
  return res;
}

// Algoritmo de Bellman-Ford: admite pesos negativos (bonificaciones/descuentos)
// y detecta ciclos negativos alcanzables propagando los nodos afectados (-inf).
// Complejidad: O(V * E).
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

  // Fase 1: relaja todas las aristas hasta V - 1 veces (con corte temprano si converge).
  for (std::size_t i = 1; i < nodos_.size(); ++i) {
    bool huboCambios = false;
    ++res.iteraciones;
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
          ++res.relajaciones;
        }
      }
    }
    if (!huboCambios) {
      break; // Ya se alcanzo el punto fijo optimo
    }
  }

  // Fase 2 (pasada V): si alguna arista aun se puede relajar, existe un ciclo negativo.
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

  // Fase 3: BFS desde los nodos del ciclo negativo para marcar todos los alcanzables (-inf).
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

  // Fase 4: reconstruye la ruta optima desde destino hacia origen con cota de seguridad.
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

  // Registra los pesos de cada tramo recorrido para mostrarlos en la UI.
  for (std::size_t i = 0; i + 1 < ruta.size(); ++i) {
    const ::Nodo* u = buscarNodo(ruta[i]);
    int pesoArista = 0;
    if (u != nullptr) {
      for (const auto& arista : u->getAdyacentes()) {
        if (arista.first->getValor() == ruta[i + 1]) {
          pesoArista = arista.second;
          break;
        }
      }
    }
    res.pesosTramos.push_back(pesoArista);
  }

  res.exito = true;
  res.costoTotal = distancias[destino];
  res.camino = std::move(ruta);
  res.mensaje = "Bellman-Ford calculo la ruta de menor costo neto: " +
                std::to_string(res.costoTotal) + ".";
  return res;
}

} // namespace algoritmos
