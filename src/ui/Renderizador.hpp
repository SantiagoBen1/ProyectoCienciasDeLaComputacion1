#pragma once

#include "ComponentesUI.hpp"
#include "../estado/EstadoApp.hpp"

#include <array>

namespace ui {

// Geometria compartida entre Renderizador y ControladorEventos.
std::array<Boton, 4> obtenerBotonesPestanas(dominio::ModuloActivo activo);

// Geometria del Modulo 1 (Cola FIFO y Cola de Prioridad).
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

// Geometria del Modulo 2 (Bodega: Merge Sort, Busqueda Binaria y Lineal).
struct ControlesModulo2 {
  Boton btnOrdenarMerge;
  Boton btnToggleAnim;
  Boton btnRestaurarDesorden;
  CajaTexto cajaConsulta;
  Boton btnBuscarBinaria;
  Boton btnBuscarLinealId;
  Boton btnBuscarLinealDest;
};

// Geometria del Modulo 3 (Lista Enlazada de Repartidores y Pila LIFO).
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

ControlesModulo1 obtenerControlesModulo1(estado::EstadoApp& app);
ControlesModulo2 obtenerControlesModulo2(estado::EstadoApp& app);
ControlesModulo3 obtenerControlesModulo3(estado::EstadoApp& app);

void renderizarAplicacion(estado::EstadoApp& app, float ratonX, float ratonY);

} // namespace ui
