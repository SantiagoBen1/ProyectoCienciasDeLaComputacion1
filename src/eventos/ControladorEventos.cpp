#include "ControladorEventos.hpp"

#include "../ui/ComponentesUI.hpp"
#include "../ui/Renderizador.hpp"

#include <cmath>

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

  // Clic directo sobre una tarjeta de paquete en Cola de Prioridad o Cola FIFO
  if (entrada.clicIzq) {
    constexpr std::size_t POR_FILA = 4;
    constexpr std::size_t MAX_VISIBLES = 8;
    const float cardW = 168.0f;
    const float cardH = 82.0f;
    const float gapX = 20.0f;
    const float gapY = 16.0f;
    const float startX = 14.0f + 18.0f;

    const auto ordenPrio = app.colaPrioridad.aVectorOrdenado();
    const std::size_t limPrio =
        ordenPrio.size() < MAX_VISIBLES ? ordenPrio.size() : MAX_VISIBLES;
    for (std::size_t i = 0; i < limPrio; ++i) {
      const float cx =
          startX + static_cast<float>(i % POR_FILA) * (cardW + gapX);
      const float cy =
          (94.0f + 56.0f) + static_cast<float>(i / POR_FILA) * (cardH + gapY);
      if (ui::RectanguloUI{cx, cy, cardW, cardH}.contiene(entrada.ratonX,
                                                          entrada.ratonY)) {
        const int idSel = ordenPrio[i].first;
        app.m2.consultaTexto = std::to_string(idSel);
        const dominio::Paquete* paq = app.obtenerPaquete(idSel);
        if (paq != nullptr) {
          app.registrarLog(
              dominio::NivelLog::Info,
              "Inspeccion en Cola de Prioridad: paquete #" +
                  std::to_string(idSel),
              "Turno de salida en Min-Heap: #" + std::to_string(i + 1) +
                  " de " + std::to_string(ordenPrio.size()),
              "Destino: " + paq->destino + " | Prioridad: " +
                  dominio::textoPrioridad(paq->prioridad) + " | Peso: " +
                  app.formatearPeso(paq->pesoKg));
        }
        return;
      }
    }

    const auto idsFifo = app.colaRegular.aVector();
    const std::size_t limFifo =
        idsFifo.size() < MAX_VISIBLES ? idsFifo.size() : MAX_VISIBLES;
    for (std::size_t i = 0; i < limFifo; ++i) {
      const float cx =
          startX + static_cast<float>(i % POR_FILA) * (cardW + gapX);
      const float cy =
          (366.0f + 56.0f) + static_cast<float>(i / POR_FILA) * (cardH + gapY);
      if (ui::RectanguloUI{cx, cy, cardW, cardH}.contiene(entrada.ratonX,
                                                          entrada.ratonY)) {
        const int idSel = idsFifo[i];
        app.m2.consultaTexto = std::to_string(idSel);
        const dominio::Paquete* paq = app.obtenerPaquete(idSel);
        if (paq != nullptr) {
          app.registrarLog(
              dominio::NivelLog::Info,
              "Inspeccion en Cola Regular FIFO: paquete #" +
                  std::to_string(idSel),
              "Posicion en Cola FIFO: #" + std::to_string(i + 1) + " de " +
                  std::to_string(idsFifo.size()),
              "Destino: " + paq->destino + " | Peso: " +
                  app.formatearPeso(paq->pesoKg) +
                  " (sale tras vaciar Cola de Prioridad)");
        }
        return;
      }
    }
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

  // Clic directo en una fila de la tabla de bodega para cargar su ID.
  if (entrada.clicIzq) {
    const ui::RectanguloUI areaFilas{28.0f, 158.0f, 718.0f, 338.0f};
    if (areaFilas.contiene(entrada.ratonX, entrada.ratonY)) {
      const int filaRel = static_cast<int>((entrada.ratonY - 158.0f) / 28.0f);
      const int maxVisibles = 12;
      int filaInicio = 0;
      if (static_cast<int>(app.bodegaIds.size()) > maxVisibles &&
          app.m2.filaEncontrada >= maxVisibles) {
        filaInicio = app.m2.filaEncontrada - maxVisibles + 1;
      }
      const int idx = filaInicio + filaRel;
      if (idx >= 0 && idx < static_cast<int>(app.bodegaIds.size())) {
        const int idSel = app.bodegaIds[static_cast<std::size_t>(idx)];
        app.m2.filaEncontrada = idx;
        app.m2.consultaTexto = std::to_string(idSel);
        const dominio::Paquete* paq = app.obtenerPaquete(idSel);
        if (paq != nullptr) {
          app.registrarLog(
              dominio::NivelLog::Info,
              "Paquete #" + std::to_string(idSel) + " seleccionado en tabla",
              "Fila seleccionada: " + std::to_string(idx + 1),
              "ID #" + std::to_string(idSel) + " cargado en buscador | " +
                  paq->destino + " (" + app.formatearPeso(paq->pesoKg) + ")");
        }
        return;
      }
    }
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

  // Clic directo sobre un nodo de la Lista Enlazada para seleccionarlo.
  if (entrada.clicIzq) {
    const auto idsRep = app.listaRepartidores.aVector();
    constexpr std::size_t POR_FILA = 3;
    constexpr std::size_t MAX_NODOS = 9;
    const float nodeW = 190.0f;
    const float nodeH = 62.0f;
    const float gapX = 34.0f;
    const float gapY = 28.0f;
    const float startX = 14.0f + 24.0f;
    const float startY = 94.0f + 46.0f;
    const std::size_t limite =
        idsRep.size() < MAX_NODOS ? idsRep.size() : MAX_NODOS;
    for (std::size_t i = 0; i < limite; ++i) {
      const std::size_t fila = i / POR_FILA;
      const std::size_t col = i % POR_FILA;
      const float nx = startX + static_cast<float>(col) * (nodeW + gapX);
      const float ny = startY + static_cast<float>(fila) * (nodeH + gapY);
      const ui::RectanguloUI rectNodo{nx, ny, nodeW, nodeH};
      if (rectNodo.contiene(entrada.ratonX, entrada.ratonY)) {
        const int idSel = idsRep[i];
        app.m3.ultimoRepResaltado = idSel;
        app.m3.repIdTexto = std::to_string(idSel);
        const dominio::Repartidor* rep = app.obtenerRepartidor(idSel);
        if (rep != nullptr) {
          app.m3.repNombreTexto = rep->nombre;
          app.m3.repZonaTexto = rep->zona;
        }
        const std::string sigTxt =
            (i + 1 < idsRep.size())
                ? ("#" + std::to_string(idsRep[i + 1]))
                : "NULL";
        app.registrarLog(
            dominio::NivelLog::Info,
            "Nodo #" + std::to_string(idSel) + " seleccionado en Lista Enlazada",
            "Posicion en lista: " + std::to_string(i + 1) + " de " +
                std::to_string(idsRep.size()),
            "Puntero next -> " + sigTxt +
                " | ID cargado para edicion o eliminacion.");
        return;
      }
    }
  }
}

void procesarEventosModulo4(estado::EstadoApp& app,
                            const ui::EntradaUsuario& entrada) {
  const ui::ControlesModulo4 ctrl = ui::obtenerControlesModulo4(app);

  // Clic directo sobre los circulos de los 8 sectores en el grafo.
  if (entrada.clicIzq || entrada.clicDer) {
    for (const auto& sec : app.sectores) {
      const float d = std::hypot(entrada.ratonX - sec.x, entrada.ratonY - sec.y);
      if (d <= ctrl.radioNodo + 4.0f) {
        app.seleccionarNodoGrafoM4(sec.id, entrada.clicDer);
        return;
      }
    }
  }

  if (ui::clicEnBoton(ctrl.btnFijarOrigen, entrada)) {
    app.m4.seleccionandoOrigen = true;
    return;
  }
  if (ui::clicEnBoton(ctrl.btnFijarDestino, entrada)) {
    app.m4.seleccionandoOrigen = false;
    return;
  }
  if (ui::clicEnBoton(ctrl.btnIntercambiar, entrada)) {
    app.intercambiarOrigenDestinoM4();
    return;
  }

  if (ui::clicEnBoton(ctrl.btnModoBonif, entrada)) {
    app.cambiarModoRedM4(dominio::ModoRed::ConBonificaciones);
    return;
  }
  if (ui::clicEnBoton(ctrl.btnModoPositivo, entrada)) {
    app.cambiarModoRedM4(dominio::ModoRed::SoloPositivos);
    return;
  }
  if (ui::clicEnBoton(ctrl.btnModoCicloNeg, entrada)) {
    app.cambiarModoRedM4(dominio::ModoRed::CicloNegativoDemo);
    return;
  }

  if (ui::clicEnBoton(ctrl.btnDijkstra, entrada)) {
    app.calcularRutaDijkstraM4();
    return;
  }
  if (ui::clicEnBoton(ctrl.btnBellmanFord, entrada)) {
    app.calcularRutaBellmanFordM4();
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
    if (ev.esF5) {
      app.reiniciarSimulacion();
      return;
    }
    if (ev.esTab) {
      app.rotarFocoModulo();
      return;
    }
  }

  if (ui::clicEnBoton(ui::obtenerBotonReiniciar(), entrada)) {
    app.reiniciarSimulacion();
    return;
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
    case dominio::ModuloActivo::RutasUrbanas:
      procesarEventosModulo4(app, entrada);
      break;
  }
}

} // namespace eventos
