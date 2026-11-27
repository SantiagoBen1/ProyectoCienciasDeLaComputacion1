// ============================================================================
// Archivo: src/eventos/ControladorEventos.hpp
// Capa: Capa D — Manejo de Eventos (sin llamadas directas a MINIWIN).
// Proposito: Declara el despachador central de eventos de raton y teclado que
//            traduce las acciones del usuario en operaciones sobre EstadoApp.
// ============================================================================

#pragma once

#include "../estado/EstadoApp.hpp"
#include "../ui/MiniWinCapa.hpp"

namespace eventos {

// Procesa una instantanea de entrada del cuadro actual (clics, teclado y
// atajos globales) y actualiza el estado del modulo activo.
void procesarEventos(estado::EstadoApp& app, const ui::EntradaUsuario& entrada);

} // namespace eventos
