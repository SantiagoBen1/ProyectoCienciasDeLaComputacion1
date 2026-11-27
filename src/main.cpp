// ============================================================================
// Archivo: src/main.cpp
// Proposito: Punto de entrada y bucle principal de la aplicacion grafica.
//            Orquesta las 4 capas del sistema:
//              - Inicializacion de ventana (Capa C) y Bootstrapper (Capa B)
//              - Captura de entrada (Capa C) -> Procesamiento (Capa D)
//              - Animaciones (Capa B) -> Renderizado a ~60 FPS (Capa C)
// Nota: Incluye miniwin.h unicamente por la macro estandar de MINIWIN
//       (main -> _main_). Todas las llamadas graficas residen en MiniWinCapa.cpp.
// ============================================================================

#include "miniwin.h"

#include "estado/EstadoApp.hpp"
#include "eventos/ControladorEventos.hpp"
#include "ui/MiniWinCapa.hpp"
#include "ui/Paleta.hpp"
#include "ui/Renderizador.hpp"

int main() {
  // 1) Crea y dimensiona la ventana principal (1160 x 720).
  ui::iniciarVentana(ui::layout::ANCHO_VENTANA, ui::layout::ALTO_VENTANA);

  // 2) Instancia el estado global y carga los datos iniciales del Bootstrapper.
  estado::EstadoApp app;
  app.inicializarDatosPrecargados();

  // 3) Bucle principal interactivo (~60 cuadros por segundo).
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

  // 4) Cierra ordenadamente la ventana grafica (los destructores liberan la RAM).
  ui::cerrarVentana();
  return 0;
}
