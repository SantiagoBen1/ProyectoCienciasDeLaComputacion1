#pragma once

#include "Nodo.hpp"
#include "NodoLista.hpp"

#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace algoritmos {

class ColaFIFO {
private:
  ::nodo* frente_;
  ::nodo* final_;
  std::size_t tamano_;

public:
  ColaFIFO();
  ~ColaFIFO();

  ColaFIFO(const ColaFIFO&) = delete;
  ColaFIFO& operator=(const ColaFIFO&) = delete;

  void encolar(int valor);
  bool desencolar(int& valor);
  bool consultarFrente(int& valor) const;
  bool estaVacia() const;
  std::size_t tamano() const;
  // Lectura no destructiva para visualizacion grafica.
  std::vector<int> aVector() const;
};

class ColaPrioridad {
private:
  struct Elemento {
    int valor;
    int prioridad;
    std::uint64_t orden;
  };

  std::vector<Elemento> monticulo_;
  std::uint64_t siguienteOrden_;

  static bool precede(const Elemento& a, const Elemento& b);
  void subir(std::size_t indice);
  void bajar(std::size_t indice);

public:
  ColaPrioridad();

  bool encolar(int valor, int prioridad);
  bool desencolar(int& valor, int& prioridad);
  bool consultar(int& valor, int& prioridad) const;
  bool estaVacia() const;
  std::size_t tamano() const;
  // Lectura no destructiva en orden real de despacho (valor, prioridad).
  std::vector<std::pair<int, int>> aVectorOrdenado() const;
};

// Registro de un paso de mezcla para animacion visual de Merge Sort.
struct PasoMerge {
  std::size_t inicio = 0;
  std::size_t mitad = 0;
  std::size_t fin = 0;
  std::vector<int> estado;
};

std::vector<int> mergeSort(const std::vector<int>& valores,
                           std::vector<PasoMerge>* pasos = nullptr);
int busquedaBinaria(const std::vector<int>& valores, int objetivo,
                    int* comparaciones = nullptr,
                    std::vector<int>* historialPivotes = nullptr);
int busquedaLineal(const std::vector<int>& valores, int objetivo,
                   int* comparaciones = nullptr);

class ListaEnlazada {
private:
  ::nodo* cabeza_;
  ::nodo* cola_;
  std::size_t tamano_;

public:
  ListaEnlazada();
  ~ListaEnlazada();

  ListaEnlazada(const ListaEnlazada&) = delete;
  ListaEnlazada& operator=(const ListaEnlazada&) = delete;

  void insertarAlInicio(int valor);
  void insertarAlFinal(int valor);
  bool eliminarNodo(int valor);
  bool contiene(int valor) const;
  std::vector<int> aVector() const;
  bool estaVacia() const;
  std::size_t tamano() const;
};

class Pila {
private:
  ::nodo* tope_;
  std::size_t tamano_;

public:
  Pila();
  ~Pila();

  Pila(const Pila&) = delete;
  Pila& operator=(const Pila&) = delete;

  void push(int valor);
  bool pop(int& valor);
  bool consultarTope(int& valor) const;
  bool estaVacia() const;
  std::size_t tamano() const;
  // Lectura no destructiva desde el tope hasta la base para dibujar y auditar.
  std::vector<int> aVector() const;
};

// Resultado estructurado para entregar caminos y alertas a la interfaz grafica.
struct ResultadoRuta {
  bool exito = false;
  bool tienePesosNegativos = false;
  bool hayCicloNegativo = false;
  bool inalcanzable = false;
  long long costoTotal = 0;
  int iteraciones = 0;
  int relajaciones = 0;
  std::vector<int> camino;
  std::vector<int> pesosTramos;
  std::vector<int> nodosAfectadosCiclo;
  std::string mensaje;
};

class Grafo {
private:
  std::vector<::Nodo*> nodos_;
  bool dirigido_;

public:
  explicit Grafo(bool dirigido = false);
  ~Grafo();

  Grafo(const Grafo&) = delete;
  Grafo& operator=(const Grafo&) = delete;

  ::Nodo* agregarNodo(int valor);
  bool agregarAristaPonderada(int origen, int destino, int peso);
  bool eliminarArista(int origen, int destino);
  ::Nodo* buscarNodo(int valor) const;
  const std::vector<::Nodo*>& getNodos() const;
  void mostrarGrafo() const;

  ResultadoRuta Dijkstra(int origen, int destino) const;
  ResultadoRuta BellmanFord(int origen, int destino) const;
};

} // namespace algoritmos
