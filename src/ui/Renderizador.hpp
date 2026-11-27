// ============================================================================
// Archivo: src/ui/Renderizador.hpp
// Capa: Capa C — Renderizador de Pantallas y Geometria Compartida.
// Proposito: Declara las funciones que construyen la geometria de controles de
//            cada modulo (compartida con ControladorEventos para que los hitboxes
//            coincidan al pixel con lo dibujado) y la funcion maestra de dibujo.
// ============================================================================

#pragma once

#include "ComponentesUI.hpp"
#include "../estado/EstadoApp.hpp"

#include <array>

namespace ui {

// --- Controles de la barra superior (pestanas 1-4 y boton Reset [F5]) ---
std::array<Boton, 4> obtenerBotonesPestanas(dominio::ModuloActivo activo);
Boton obtenerBotonReiniciar();

// Geometria de controles del Modulo 1 (Cola FIFO y Cola de Prioridad).
struct ControlesModulo1 {
  CajaTexto cajaId;
  CajaTexto cajaDestino;
  Boton btnPesoMenos;
  Boton btnPesoMas;
  Boton btnPrioMedico;
  Boton btnPrioUrgente;
  Boton btnPrioRegular;
  Boton btnIngresar;
  Boton btnDespachar;
};

// Geometria de controles del Modulo 2 (Bodega: Merge Sort, Busqueda Binaria y Lineal).
struct ControlesModulo2 {
  Boton btnOrdenarMerge;
  Boton btnToggleAnim;
  Boton btnRestaurarDesorden;
  CajaTexto cajaConsulta;
  Boton btnBuscarBinaria;
  Boton btnBuscarLinealId;
  Boton btnBuscarLinealDest;
};

// Geometria de controles del Modulo 3 (Lista Enlazada de Repartidores y Pila LIFO).
struct ControlesModulo3 {
  CajaTexto cajaRepId;
  CajaTexto cajaRepNombre;
  CajaTexto cajaRepZona;
  Boton btnInsertarInicio;
  Boton btnInsertarFinal;
  Boton btnEliminarRep;

  CajaTexto cajaEntregaPaqId;
  CajaTexto cajaEntregaDestino;
  Boton btnPushEntrega;
  Boton btnPopEntrega;
  Boton btnAuditarPila;
};

// Geometria de controles del Modulo 4 (Rutas Urbanas: Grafo, Dijkstra y Bellman-Ford).
struct ControlesModulo4 {
  Boton btnFijarOrigen;
  Boton btnFijarDestino;
  Boton btnIntercambiar;
  Boton btnModoBonif;
  Boton btnModoPositivo;
  Boton btnModoCicloNeg;
  Boton btnDijkstra;
  Boton btnBellmanFord;
  float radioNodo = 24.0f; // Radio visual y de hit-test de cada circulo del grafo
};

ControlesModulo1 obtenerControlesModulo1(estado::EstadoApp& app);
ControlesModulo2 obtenerControlesModulo2(estado::EstadoApp& app);
ControlesModulo3 obtenerControlesModulo3(estado::EstadoApp& app);
ControlesModulo4 obtenerControlesModulo4(const estado::EstadoApp& app);

// Dibuja un cuadro completo (fondo, barra superior/guia, modulo activo y barra de estado).
void renderizarAplicacion(estado::EstadoApp& app, float ratonX, float ratonY);

} // namespace ui
