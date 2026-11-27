// ============================================================================
// Archivo: src/algoritmos/algoritmos.hpp
// Capa: Capa A — Logica y Estructuras de Datos (sin dependencias graficas).
// Proposito: Declara todas las estructuras de datos y algoritmos clasicos del
//            sistema logistico: ColaFIFO, ColaPrioridad (min-heap), Merge Sort,
//            Busqueda Binaria, Busqueda Lineal, ListaEnlazada, Pila (LIFO) y
//            Grafo ponderado con Dijkstra y Bellman-Ford.
// ============================================================================

#pragma once

#include "Nodo.hpp"
#include "NodoLista.hpp"

#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace algoritmos {

// ----------------------------------------------------------------------------
// ColaFIFO: Cola enlazada First-In, First-Out para envios regulares.
// Maneja memoria dinamica con nodos ::nodo y garantiza O(1) en encolar/desencolar.
// ----------------------------------------------------------------------------
class ColaFIFO {
private:
  ::nodo* frente_;     // Puntero al primer elemento en salir
  ::nodo* final_;      // Puntero al ultimo elemento insertado
  std::size_t tamano_; // Cantidad actual de elementos en cola

public:
  ColaFIFO();
  ~ColaFIFO(); // Libera todos los nodos dinamicos pendientes

  // Se prohibe copia para evitar doble liberacion (double free) de punteros.
  ColaFIFO(const ColaFIFO&) = delete;
  ColaFIFO& operator=(const ColaFIFO&) = delete;

  void encolar(int valor);               // Inserta al final — O(1)
  bool desencolar(int& valor);           // Extrae del frente y libera nodo — O(1)
  bool consultarFrente(int& valor) const;// Consulta el frente sin extraer — O(1)
  bool estaVacia() const;
  std::size_t tamano() const;
  // Lectura no destructiva en orden FIFO para visualizacion grafica — O(n).
  std::vector<int> aVector() const;
};

// ----------------------------------------------------------------------------
// ColaPrioridad: Min-Heap binario estable para envios urgentes y medicos.
// Un numero menor de prioridad tiene precedencia (1=Medico antes que 2=Urgente),
// desempatando por orden de llegada (FIFO dentro de la misma prioridad).
// ----------------------------------------------------------------------------
class ColaPrioridad {
private:
  struct Elemento {
    int valor;            // ID del paquete
    int prioridad;        // Menor valor = mayor urgencia
    std::uint64_t orden;  // Secuencia monotona para estabilidad ante empates
  };

  std::vector<Elemento> monticulo_; // Arreglo subyacente del monticulo binario
  std::uint64_t siguienteOrden_;    // Contador incremental de llegada

  static bool precede(const Elemento& a, const Elemento& b); // Comparador del min-heap
  void subir(std::size_t indice);   // Restaura propiedad del heap hacia la raiz — O(log n)
  void bajar(std::size_t indice);   // Restaura propiedad del heap hacia las hojas — O(log n)

public:
  ColaPrioridad();

  bool encolar(int valor, int prioridad);           // Inserta en el heap — O(log n)
  bool desencolar(int& valor, int& prioridad);      // Extrae el minimo — O(log n)
  bool consultar(int& valor, int& prioridad) const; // Inspecciona raiz — O(1)
  bool estaVacia() const;
  std::size_t tamano() const;
  // Lectura no destructiva en orden real de despacho (valor, prioridad) — O(n log n).
  std::vector<std::pair<int, int>> aVectorOrdenado() const;
};

// ----------------------------------------------------------------------------
// Modulo 2: Ordenamiento (Merge Sort) y Busquedas (Binaria y Lineal)
// ----------------------------------------------------------------------------

// Instantanea de un paso de mezcla en Merge Sort para animar [inicio..mitad) + [mitad..fin).
struct PasoMerge {
  std::size_t inicio = 0;
  std::size_t mitad = 0;
  std::size_t fin = 0;
  std::vector<int> estado; // Copia del arreglo completo tras completar esta mezcla
};

// Ordena ascendentemente mediante division y conquista — O(n log n).
// Si pasos != nullptr, registra cada operacion de mezcla para visualizacion.
std::vector<int> mergeSort(const std::vector<int>& valores,
                           std::vector<PasoMerge>* pasos = nullptr);

// Localiza 'objetivo' en un vector previamente ordenado — O(log n).
// Opcionalmente reporta numero de comparaciones y los pivotes evaluados.
int busquedaBinaria(const std::vector<int>& valores, int objetivo,
                    int* comparaciones = nullptr,
                    std::vector<int>* historialPivotes = nullptr);

// Localiza 'objetivo' secuencialmente sin requerir orden previo — O(n).
int busquedaLineal(const std::vector<int>& valores, int objetivo,
                   int* comparaciones = nullptr);

// ----------------------------------------------------------------------------
// ListaEnlazada: Lista simplemente enlazada con punteros a cabeza y cola.
// Administra la nomina dinamica de repartidores activos en turno.
// ----------------------------------------------------------------------------
class ListaEnlazada {
private:
  ::nodo* cabeza_;     // Primer nodo de la lista (HEAD)
  ::nodo* cola_;       // Ultimo nodo de la lista (TAIL) para insercion al final O(1)
  std::size_t tamano_; // Total de nodos activos

public:
  ListaEnlazada();
  ~ListaEnlazada(); // Libera cada nodo con delete

  ListaEnlazada(const ListaEnlazada&) = delete;
  ListaEnlazada& operator=(const ListaEnlazada&) = delete;

  void insertarAlInicio(int valor); // Insercion en HEAD — O(1)
  void insertarAlFinal(int valor);  // Insercion en TAIL — O(1)
  bool eliminarNodo(int valor);     // Busqueda y baja por ID con delete — O(n)
  bool contiene(int valor) const;   // Verifica existencia por ID — O(n)
  std::vector<int> aVector() const; // Copia secuencial para dibujar — O(n)
  bool estaVacia() const;
  std::size_t tamano() const;
};

// ----------------------------------------------------------------------------
// Pila: Estructura enlazada LIFO (Last-In, First-Out) para auditoria y reversion
// de entregas completadas.
// ----------------------------------------------------------------------------
class Pila {
private:
  ::nodo* tope_;       // Nodo mas reciente (TOPE de la pila)
  std::size_t tamano_; // Cantidad de registros apilados

public:
  Pila();
  ~Pila(); // Desapila y libera todos los nodos restantes

  Pila(const Pila&) = delete;
  Pila& operator=(const Pila&) = delete;

  void push(int valor);                 // Apila en el tope — O(1)
  bool pop(int& valor);                 // Desapila el tope y libera memoria — O(1)
  bool consultarTope(int& valor) const; // Inspecciona el tope sin modificar — O(1)
  bool estaVacia() const;
  std::size_t tamano() const;
  // Lectura no destructiva desde el tope hasta la base para dibujar y auditar — O(n).
  std::vector<int> aVector() const;
};

// ----------------------------------------------------------------------------
// Modulo 4: Grafo ponderado con algoritmos de caminos minimos
// ----------------------------------------------------------------------------

// Estructura de retorno que desacopla Dijkstra y Bellman-Ford de std::cout
// para que la capa grafica pueda mostrar rutas, metricas y alertas detalladas.
struct ResultadoRuta {
  bool exito = false;               // True si se hallo una ruta optima finita
  bool tienePesosNegativos = false; // True si Dijkstra detecto alguna arista < 0
  bool hayCicloNegativo = false;    // True si Bellman-Ford detecto un ciclo negativo
  bool inalcanzable = false;        // True si no existe camino dirigido al destino
  long long costoTotal = 0;         // Costo minimo acumulado hasta el destino
  int iteraciones = 0;              // Nodos extraidos (Dijkstra) o pasadas (Bellman-Ford)
  int relajaciones = 0;             // Cantidad de mejoras exitosas de distancia
  std::vector<int> camino;          // Secuencia de IDs desde origen hasta destino
  std::vector<int> pesosTramos;     // Peso individual de cada arista de 'camino'
  std::vector<int> nodosAfectadosCiclo; // Nodos con distancia -infinito por ciclo negativo
  std::string mensaje;              // Resumen descriptivo del resultado o alerta
};

// Grafo ponderado (dirigido o no dirigido) basado en nodos dinamicos ::Nodo.
class Grafo {
private:
  std::vector<::Nodo*> nodos_; // Propiedad de memoria de todos los vertices creados
  bool dirigido_;              // True para red vial dirigida (sentidos de via)

public:
  explicit Grafo(bool dirigido = false);
  ~Grafo(); // Libera cada vertice con delete

  Grafo(const Grafo&) = delete;
  Grafo& operator=(const Grafo&) = delete;

  ::Nodo* agregarNodo(int valor);
  bool agregarAristaPonderada(int origen, int destino, int peso);
  bool eliminarArista(int origen, int destino);
  ::Nodo* buscarNodo(int valor) const;
  const std::vector<::Nodo*>& getNodos() const;
  void mostrarGrafo() const; // Salida auxiliar por consola (depuracion)

  // Calcula camino minimo para pesos >= 0 — O((V + E) log V).
  ResultadoRuta Dijkstra(int origen, int destino) const;

  // Calcula camino minimo con pesos positivos/negativos y detecta ciclos negativos — O(V * E).
  ResultadoRuta BellmanFord(int origen, int destino) const;
};

} // namespace algoritmos
