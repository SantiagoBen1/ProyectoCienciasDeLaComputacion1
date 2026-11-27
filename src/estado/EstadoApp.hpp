// ============================================================================
// Archivo: src/estado/EstadoApp.hpp
// Capa: Capa B — Dominio y Estado de la Aplicacion (sin dependencias graficas).
// Proposito: Declara el estado persistente en memoria durante toda la sesion
//            (EstadoApp), los sub-estados de los 4 modulos y las operaciones
//            de negocio que conectan la interfaz con las estructuras de Capa A.
// ============================================================================

#pragma once

#include "Modelos.hpp"
#include "../algoritmos/algoritmos.hpp"

#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

namespace estado {

// Estado del formulario de recepcion del Modulo 1 (Cola FIFO y Cola de Prioridad).
struct FormularioModulo1 {
  std::string idTexto = "113";
  std::string destinoTexto = "CLINICA NORTE";
  double pesoKg = 2.5;
  dominio::Prioridad prioridad = dominio::Prioridad::Medico;
  bool focoId = false;
  bool focoDestino = false;
};

// Estado operativo del Modulo 2 (Bodega: Merge Sort animado, Busqueda Binaria y Lineal).
struct EstadoModulo2 {
  std::string consultaTexto = "109"; // ID o subcadena de destino a buscar
  bool focoConsulta = false;
  bool animarPasos = true;           // Activa/desactiva animacion paso a paso

  // Control de animacion de Merge Sort sobre la tabla de bodega.
  bool animacionActiva = false;
  std::vector<algoritmos::PasoMerge> pasosMerge;
  std::size_t indicePasoActual = 0;
  int contadorCuadros = 0;
  std::vector<int> idsOrdenadosDestino;

  // Instantaneas de trazabilidad Antes / Despues de ordenar.
  std::vector<int> snapshotAntes;
  std::vector<int> snapshotDespues;
  bool tieneSnapshots = false;

  // Metricas de comparacion O(log n) vs O(n) e historial de pivotes binarios.
  int filaEncontrada = -1;
  std::string ultimoMetodo = "Ninguno";
  std::string ultimoObjetivo;
  bool ultimoEncontrado = false;
  int compBinaria = -1;
  int compLinealId = -1;
  int compLinealDestino = -1;
  std::vector<int> pivotesBinaria;
};

// Estado operativo del Modulo 3 (Lista Enlazada de Repartidores y Pila LIFO).
struct EstadoModulo3 {
  std::string repIdTexto = "70";
  std::string repNombreTexto = "CARLOS ROJAS";
  std::string repZonaTexto = "NORTE";
  bool focoRepId = false;
  bool focoRepNombre = false;
  bool focoRepZona = false;
  int ultimoRepResaltado = -1;      // Resalta el nodo recien insertado o clicado
  int siguienteIdRepartidor = 70;

  std::string entregaPaqIdTexto = "107";
  std::string entregaDestinoTexto = "ZONA FRANCA";
  bool focoEntregaPaqId = false;
  bool focoEntregaDestino = false;

  bool auditoriaActiva = false;     // Activa numeracion inversa [1], [2], ... en la pila
  std::string resumenAuditoria;
  std::string ultimoDeshecho;
};

// Estado operativo del Modulo 4 (Rutas Urbanas: Dijkstra y Bellman-Ford).
struct EstadoModulo4 {
  int nodoOrigen = 1;
  int nodoDestino = 8;
  bool seleccionandoOrigen = true;  // Define si el proximo clic izquierdo fija Origen o Destino

  bool tieneResultado = false;
  std::string algoritmoUsado = "Ninguno";
  algoritmos::ResultadoRuta resultado;
};

// ----------------------------------------------------------------------------
// EstadoApp: Contenedor central en memoria de toda la sesion operativa.
// Almacena las estructuras de src/algoritmos/ (que operan sobre IDs enteros)
// junto con diccionarios de metadatos para mostrar informacion rica en la GUI.
// ----------------------------------------------------------------------------
struct EstadoApp {
  dominio::ModuloActivo moduloActivo = dominio::ModuloActivo::ColaYPrioridad;
  dominio::LogTrazabilidad log;
  bool ejecutando = true; // Controla la permanencia en el bucle principal

  // Catalogo maestro de paquetes indexado por ID unico.
  std::unordered_map<int, dominio::Paquete> paquetesPorId;
  int siguienteIdPaquete = 113;

  // --- Modulo 1: Colas de recepcion y despacho ---
  algoritmos::ColaFIFO colaRegular;
  algoritmos::ColaPrioridad colaPrioridad;
  FormularioModulo1 formM1;
  bool tieneUltimoDespachado = false;
  dominio::Paquete ultimoDespachado;
  std::string origenUltimoDespacho;
  int totalDespachados = 0;

  // --- Modulo 2: Bodega (Merge Sort, Busqueda Binaria y Lineal) ---
  std::vector<int> bodegaIds;          // Orden actual visible en la tabla
  std::vector<int> bodegaIdsIniciales; // Respaldo del desorden inicial del Bootstrapper
  bool bodegaOrdenada = false;
  EstadoModulo2 m2;

  // --- Modulo 3: Flota y Auditoria (Lista Enlazada y Pila LIFO) ---
  algoritmos::ListaEnlazada listaRepartidores;
  std::unordered_map<int, dominio::Repartidor> repartidoresPorId;
  algoritmos::Pila pilaEntregas;
  std::unordered_map<int, dominio::RegistroEntrega> entregasPorId;
  int siguienteIdEntrega = 1;
  EstadoModulo3 m3;

  // --- Modulo 4: Grafo urbano dirigido y ponderado (Dijkstra y Bellman-Ford) ---
  algoritmos::Grafo grafoUrbano{true};
  std::vector<dominio::Sector> sectores;
  dominio::ModoRed modoRed = dominio::ModoRed::ConBonificaciones;
  EstadoModulo4 m4;

  // --- Operaciones generales del ciclo de vida ---
  void inicializarDatosPrecargados(); // Implementado en Bootstrapper.cpp
  void reiniciarSimulacion();         // Limpia estructuras dinamicas y recarga Bootstrapper [F5]
  void rotarFocoModulo();             // Cicla el foco de campos del modulo activo [Tab]
  void construirAristasGrafoSegunModo(dominio::ModoRed modo);
  void registrarLog(dominio::NivelLog nivel, const std::string& titulo,
                    const std::string& antes, const std::string& despues);
  void limpiarFocos();
  bool algunCampoEnfocado() const;
  void cambiarModulo(dominio::ModuloActivo nuevoModulo);
  void actualizarAnimaciones();

  // --- Operaciones de negocio del Modulo 1 ---
  bool ingresarPedidoM1();
  bool despacharSiguienteM1();

  // --- Operaciones de negocio del Modulo 2 ---
  void ordenarBodegaMergeSortM2();
  void finalizarAnimacionMergeSortM2();
  void restaurarDesordenBodegaM2();
  bool buscarBinariaIdM2();
  bool buscarLinealIdM2();
  bool buscarLinealDestinoM2();

  // --- Operaciones de negocio del Modulo 3 ---
  bool insertarRepartidorM3(bool alInicio);
  bool eliminarRepartidorPorIdM3();
  bool registrarEntregaPushM3();
  bool deshacerUltimaEntregaPopM3();
  bool auditarHistorialM3();

  // --- Operaciones de negocio del Modulo 4 ---
  void seleccionarNodoGrafoM4(int idSector, bool forzarDestino = false);
  void intercambiarOrigenDestinoM4();
  void cambiarModoRedM4(dominio::ModoRed nuevoModo);
  bool calcularRutaDijkstraM4();
  bool calcularRutaBellmanFordM4();

  // --- Consultas y utilidades de formato ---
  const dominio::Paquete* obtenerPaquete(int id) const;
  const dominio::Repartidor* obtenerRepartidor(int id) const;
  const dominio::RegistroEntrega* obtenerEntrega(int idRegistro) const;
  const dominio::Sector* obtenerSector(int id) const;
  bool grafoTienePesosNegativos() const;
  std::string formatearPeso(double pesoKg) const;
  std::string formatearSecuenciaSectores(const std::vector<int>& camino) const;
  static std::string formatearListaIds(const std::vector<int>& ids);
  static std::string nombreModoRed(dominio::ModoRed modo);
};

// Devuelve el titulo corto de un modulo para cabeceras y registros de log.
std::string nombreModulo(dominio::ModuloActivo modulo);

} // namespace estado
