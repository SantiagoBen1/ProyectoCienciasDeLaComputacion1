#include "ControladorEventos.hpp"

#include "../ui/ComponentesUI.hpp"
#include "../ui/Renderizador.hpp"

namespace eventos {

namespace {

void procesarEventosModulo1(estado::EstadoApp& app,
                            const ui::EntradaUsuario& entrada) {
  const ui::ControlesModulo1 ctrl = ui::obtenerControlesModulo1(app);

  ui::manejarEventoCajaTexto(ctrl.cajaId, entrada);
  ui::manejarEventoCajaTexto(ctrl.cajaDestino, entrada);

  if (ui::clicEnBoton(ctrl.btnPesoMenos, entrada)) {
    app.formM1.pesoKg -= 0.5;
    if (app.formM1.pesoKg < 0.5) {
      app.formM1.pesoKg = 0.5;
    }
    return;
  }

  if (ui::clicEnBoton(ctrl.btnPesoMas, entrada)) {
    app.formM1.pesoKg += 0.5;
    if (app.formM1.pesoKg > 50.0) {
      app.formM1.pesoKg = 50.0;
    }
    return;
  }

  if (ui::clicEnBoton(ctrl.btnPrioMedico, entrada)) {
    app.formM1.prioridad = dominio::Prioridad::Medico;
    return;
  }
  if (ui::clicEnBoton(ctrl.btnPrioUrgente, entrada)) {
    app.formM1.prioridad = dominio::Prioridad::Urgente;
    return;
  }
  if (ui::clicEnBoton(ctrl.btnPrioRegular, entrada)) {
    app.formM1.prioridad = dominio::Prioridad::Regular;
    return;
  }

  if (ui::clicEnBoton(ctrl.btnIngresar, entrada)) {
    app.ingresarPedidoM1();
    return;
  }

  if (ui::clicEnBoton(ctrl.btnDespachar, entrada)) {
    app.despacharSiguienteM1();
    return;
  }

  if (app.algunCampoEnfocado()) {
    for (const auto& ev : entrada.teclas) {
      if (ev.esEnter) {
        app.ingresarPedidoM1();
        return;
      }
    }
  }
}

void procesarEventosModulo2(estado::EstadoApp& app,
                            const ui::EntradaUsuario& entrada) {
  const ui::ControlesModulo2 ctrl = ui::obtenerControlesModulo2(app);

  ui::manejarEventoCajaTexto(ctrl.cajaConsulta, entrada);

  if (ui::clicEnBoton(ctrl.btnOrdenarMerge, entrada)) {
    app.ordenarBodegaMergeSortM2();
    return;
  }

  if (ui::clicEnBoton(ctrl.btnToggleAnim, entrada)) {
    app.m2.animarPasos = !app.m2.animarPasos;
    return;
  }

  if (ui::clicEnBoton(ctrl.btnRestaurarDesorden, entrada)) {
    app.restaurarDesordenBodegaM2();
    return;
  }

  if (ui::clicEnBoton(ctrl.btnBuscarBinaria, entrada)) {
    app.buscarBinariaIdM2();
    return;
  }

  if (ui::clicEnBoton(ctrl.btnBuscarLinealId, entrada)) {
    app.buscarLinealIdM2();
    return;
  }

  if (ui::clicEnBoton(ctrl.btnBuscarLinealDest, entrada)) {
    app.buscarLinealDestinoM2();
    return;
  }

  if (app.m2.focoConsulta) {
    for (const auto& ev : entrada.teclas) {
      if (ev.esEnter) {
        if (app.bodegaOrdenada) {
          app.buscarBinariaIdM2();
        } else {
          app.buscarLinealIdM2();
        }
        return;
      }
    }
  }
}

void procesarEventosModulo3(estado::EstadoApp& app,
                            const ui::EntradaUsuario& entrada) {
  const ui::ControlesModulo3 ctrl = ui::obtenerControlesModulo3(app);

  ui::manejarEventoCajaTexto(ctrl.cajaRepId, entrada);
  ui::manejarEventoCajaTexto(ctrl.cajaRepNombre, entrada);
  ui::manejarEventoCajaTexto(ctrl.cajaRepZona, entrada);
  ui::manejarEventoCajaTexto(ctrl.cajaEntregaPaqId, entrada);
  ui::manejarEventoCajaTexto(ctrl.cajaEntregaDestino, entrada);

  if (ui::clicEnBoton(ctrl.btnInsertarInicio, entrada)) {
    app.insertarRepartidorM3(true);
    return;
  }
  if (ui::clicEnBoton(ctrl.btnInsertarFinal, entrada)) {
    app.insertarRepartidorM3(false);
    return;
  }
  if (ui::clicEnBoton(ctrl.btnEliminarRep, entrada)) {
    app.eliminarRepartidorPorIdM3();
    return;
  }

  if (ui::clicEnBoton(ctrl.btnPushEntrega, entrada)) {
    app.registrarEntregaPushM3();
    return;
  }
  if (ui::clicEnBoton(ctrl.btnPopEntrega, entrada)) {
    app.deshacerUltimaEntregaPopM3();
    return;
  }
  if (ui::clicEnBoton(ctrl.btnAuditarPila, entrada)) {
    app.auditarHistorialM3();
    return;
  }
}

} // namespace

void procesarEventos(estado::EstadoApp& app,
                     const ui::EntradaUsuario& entrada) {
  for (const auto& ev : entrada.teclas) {
    if (ev.esEscape) {
      app.ejecutando = false;
      return;
    }
  }

  const auto pestanas = ui::obtenerBotonesPestanas(app.moduloActivo);
  for (std::size_t i = 0; i < pestanas.size(); ++i) {
    if (ui::clicEnBoton(pestanas[i], entrada)) {
      app.cambiarModulo(static_cast<dominio::ModuloActivo>(i));
      return;
    }
  }

  if (!app.algunCampoEnfocado()) {
    for (const auto& ev : entrada.teclas) {
      if (ev.caracter >= '1' && ev.caracter <= '4') {
        app.cambiarModulo(
            static_cast<dominio::ModuloActivo>(ev.caracter - '1'));
        return;
      }
    }
  }

  switch (app.moduloActivo) {
    case dominio::ModuloActivo::ColaYPrioridad:
      procesarEventosModulo1(app, entrada);
      break;
    case dominio::ModuloActivo::Bodega:
      procesarEventosModulo2(app, entrada);
      break;
    case dominio::ModuloActivo::FlotaYAudi:
      procesarEventosModulo3(app, entrada);
      break;
    default:
      break;
  }
}

} // namespace eventos
