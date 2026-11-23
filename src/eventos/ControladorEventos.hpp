#pragma once

#include "../estado/EstadoApp.hpp"
#include "../ui/MiniWinCapa.hpp"

namespace eventos {

// Capa D: Procesa clics y teclado sin depender directamente de MINIWIN.
void procesarEventos(estado::EstadoApp& app, const ui::EntradaUsuario& entrada);

} // namespace eventos
