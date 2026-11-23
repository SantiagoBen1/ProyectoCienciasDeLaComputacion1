// Punto de entrada de la aplicacion.
// Incluye miniwin.h unicamente por la macro estandar de MINIWIN (main -> _main_).
// Todas las llamadas graficas y de eventos a MINIWIN residen en src/ui/MiniWinCapa.cpp.

#include "miniwin.h"

#include "estado/EstadoApp.hpp"
#include "eventos/ControladorEventos.hpp"
#include "ui/MiniWinCapa.hpp"
#include "ui/Paleta.hpp"
#include "ui/Renderizador.hpp"

int main() {
  ui::iniciarVentana(ui::layout::ANCHO_VENTANA, ui::layout::ALTO_VENTANA);

  estado::EstadoApp app;
  app.inicializarDatosPrecargados();

  while (app.ejecutando) {
    const ui::EntradaUsuario entrada = ui::leerEntrada();
    eventos::procesarEventos(app, entrada);
    if (!app.ejecutando) {
      break;
    }
    app.actualizarAnimaciones();
    ui::renderizarAplicacion(app, entrada.ratonX, entrada.ratonY);
    ui::esperarMs(16);
  }

  ui::cerrarVentana();
  return 0;
}
