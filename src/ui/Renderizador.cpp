#include "Renderizador.hpp"

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

namespace ui {

std::array<Boton, 4> obtenerBotonesPestanas(dominio::ModuloActivo activo) {
  const float y = 10.0f;
  const float h = 36.0f;
  return {{
      {{275.0f, y, 174.0f, h},
       "1. Cola y Prioridad",
       EstiloBoton::Pestana,
       true,
       activo == dominio::ModuloActivo::ColaYPrioridad},
      {{455.0f, y, 168.0f, h},
       "2. Bodega (Sort/Busq)",
       EstiloBoton::Pestana,
       true,
       activo == dominio::ModuloActivo::Bodega},
      {{629.0f, y, 174.0f, h},
       "3. Flota y Auditoria",
       EstiloBoton::Pestana,
       true,
       activo == dominio::ModuloActivo::FlotaYAudi},
      {{809.0f, y, 177.0f, h},
       "4. Rutas Urbanas",
       EstiloBoton::Pestana,
       true,
       activo == dominio::ModuloActivo::RutasUrbanas},
  }};
}

ControlesModulo1 obtenerControlesModulo1(estado::EstadoApp& app) {
  const float px = 706.0f;
  const float ancho = 264.0f;

  ControlesModulo1 c;
  c.cajaId = {{px, 122.0f, ancho, 28.0f},
              "ID del paquete (unico):",
              &app.formM1.idTexto,
              &app.formM1.focoId,
              true,
              6};

  c.cajaDestino = {{px, 176.0f, ancho, 28.0f},
                   "Destino / destinatario:",
                   &app.formM1.destinoTexto,
                   &app.formM1.focoDestino,
                   false,
                   20};

  c.btnPesoMenos = {{px, 224.0f, 44.0f, 28.0f},
                    "-0.5",
                    EstiloBoton::Secundario,
                    app.formM1.pesoKg > 0.51,
                    false};

  c.btnPesoMas = {{px + ancho - 44.0f, 224.0f, 44.0f, 28.0f},
                  "+0.5",
                  EstiloBoton::Secundario,
                  app.formM1.pesoKg < 49.99,
                  false};

  const float yPrio = 276.0f;
  c.btnPrioMedico = {{px, yPrio, 84.0f, 30.0f},
                     "Medico(1)",
                     EstiloBoton::Peligro,
                     true,
                     app.formM1.prioridad == dominio::Prioridad::Medico};

  c.btnPrioUrgente = {{px + 90.0f, yPrio, 84.0f, 30.0f},
                      "Urgent(2)",
                      EstiloBoton::Alerta,
                      true,
                      app.formM1.prioridad == dominio::Prioridad::Urgente};

  c.btnPrioRegular = {{px + 180.0f, yPrio, 84.0f, 30.0f},
                      "Regular",
                      EstiloBoton::Secundario,
                      true,
                      app.formM1.prioridad == dominio::Prioridad::Regular};

  c.btnIngresar = {{px, 318.0f, ancho, 34.0f},
                   "+ Ingresar pedido",
                   EstiloBoton::Primario,
                   true,
                   false};

  c.btnDespachar = {{px, 392.0f, ancho, 38.0f},
                    ">> Despachar siguiente",
                    EstiloBoton::Exito,
                    true,
                    false};

  return c;
}

ControlesModulo2 obtenerControlesModulo2(estado::EstadoApp& app) {
  const float px = 668.0f;
  const float ancho = 302.0f;

  ControlesModulo2 c;
  c.btnOrdenarMerge = {
      {px, 108.0f, ancho, 34.0f},
      app.m2.animacionActiva ? "Finalizar Merge Sort ahora"
                             : "Ordenar con Merge Sort [O(n log n)]",
      app.m2.animacionActiva ? EstiloBoton::Alerta : EstiloBoton::Exito,
      true,
      false};

  c.btnToggleAnim = {{px, 150.0f, 146.0f, 28.0f},
                     app.m2.animarPasos ? "Animar pasos: ON"
                                        : "Animar pasos: OFF",
                     EstiloBoton::Secundario,
                     true,
                     app.m2.animarPasos};

  c.btnRestaurarDesorden = {{px + 156.0f, 150.0f, 146.0f, 28.0f},
                            "Desordenar inicial",
                            EstiloBoton::Secundario,
                            true,
                            false};

  c.cajaConsulta = {{px, 218.0f, ancho, 28.0f},
                    "Buscar por ID (ej. 109) o Destino:",
                    &app.m2.consultaTexto,
                    &app.m2.focoConsulta,
                    false,
                    18};

  c.btnBuscarBinaria = {{px, 256.0f, ancho, 32.0f},
                        "Busqueda Binaria por ID [O(log n)]",
                        app.bodegaOrdenada ? EstiloBoton::Primario
                                           : EstiloBoton::Alerta,
                        true,
                        false};

  c.btnBuscarLinealId = {{px, 294.0f, ancho, 32.0f},
                         "Busqueda Lineal por ID [O(n)]",
                         EstiloBoton::Secundario,
                         true,
                         false};

  c.btnBuscarLinealDest = {{px, 332.0f, ancho, 32.0f},
                           "Busqueda Lineal por Destino [O(n)]",
                           EstiloBoton::Secundario,
                           true,
                           false};

  return c;
}

ControlesModulo3 obtenerControlesModulo3(estado::EstadoApp& app) {
  ControlesModulo3 c;

  const float lx = 28.0f;
  c.cajaRepId = {{lx, 444.0f, 114.0f, 28.0f},
                 "ID Repartidor:",
                 &app.m3.repIdTexto,
                 &app.m3.focoRepId,
                 true,
                 5};
  c.cajaRepNombre = {{lx + 124.0f, 444.0f, 232.0f, 28.0f},
                     "Nombre del repartidor:",
                     &app.m3.repNombreTexto,
                     &app.m3.focoRepNombre,
                     false,
                     18};
  c.cajaRepZona = {{lx + 366.0f, 444.0f, 198.0f, 28.0f},
                   "Zona asignada:",
                   &app.m3.repZonaTexto,
                   &app.m3.focoRepZona,
                   false,
                   14};

  c.btnInsertarInicio = {{lx, 492.0f, 180.0f, 34.0f},
                         "+ Insertar inicio [O(1)]",
                         EstiloBoton::Primario,
                         true,
                         false};
  c.btnInsertarFinal = {{lx + 190.0f, 492.0f, 180.0f, 34.0f},
                        "+ Insertar final [O(1)]",
                        EstiloBoton::Exito,
                        true,
                        false};
  c.btnEliminarRep = {{lx + 380.0f, 492.0f, 184.0f, 34.0f},
                      "- Eliminar por ID [O(n)]",
                      EstiloBoton::Peligro,
                      true,
                      false};

  const float rx = 634.0f;
  const float rw = 338.0f;
  c.cajaEntregaPaqId = {{rx, 386.0f, 128.0f, 28.0f},
                        "ID Paquete:",
                        &app.m3.entregaPaqIdTexto,
                        &app.m3.focoEntregaPaqId,
                        true,
                        6};
  c.cajaEntregaDestino = {{rx + 138.0f, 386.0f, 200.0f, 28.0f},
                          "Destino (si es nuevo):",
                          &app.m3.entregaDestinoTexto,
                          &app.m3.focoEntregaDestino,
                          false,
                          16};

  c.btnPushEntrega = {{rx, 424.0f, rw, 32.0f},
                      "+ Registrar entrega (push) [O(1)]",
                      EstiloBoton::Exito,
                      true,
                      false};
  c.btnPopEntrega = {{rx, 462.0f, rw, 32.0f},
                     "<< Deshacer ultimo despacho (pop)",
                     EstiloBoton::Alerta,
                     true,
                     false};
  c.btnAuditarPila = {{rx, 500.0f, rw, 32.0f},
                      "Auditar historial (orden inverso)",
                      EstiloBoton::Primario,
                      true,
                      app.m3.auditoriaActiva};

  return c;
}

ControlesModulo4 obtenerControlesModulo4(const estado::EstadoApp& app) {
  const float px = 696.0f;
  const float ancho = 276.0f;

  ControlesModulo4 c;
  c.btnFijarOrigen = {{px, 152.0f, 94.0f, 26.0f},
                      "Fijar Origen",
                      EstiloBoton::Secundario,
                      true,
                      app.m4.seleccionandoOrigen};
  c.btnFijarDestino = {{px + 99.0f, 152.0f, 100.0f, 26.0f},
                       "Fijar Dest.",
                       EstiloBoton::Secundario,
                       true,
                       !app.m4.seleccionandoOrigen};
  c.btnIntercambiar = {{px + 204.0f, 152.0f, 72.0f, 26.0f},
                       "O <-> D",
                       EstiloBoton::Secundario,
                       true,
                       false};

  c.btnModoBonif = {{px, 212.0f, ancho, 26.0f},
                    "Red con bonificaciones (-)",
                    EstiloBoton::Secundario,
                    true,
                    app.modoRed == dominio::ModoRed::ConBonificaciones};
  c.btnModoPositivo = {{px, 242.0f, ancho, 26.0f},
                       "Red solo positiva (pesos >= 0)",
                       EstiloBoton::Secundario,
                       true,
                       app.modoRed == dominio::ModoRed::SoloPositivos};
  c.btnModoCicloNeg = {{px, 272.0f, ancho, 26.0f},
                       "Activar ciclo negativo DEMO",
                       EstiloBoton::Peligro,
                       true,
                       app.modoRed == dominio::ModoRed::CicloNegativoDemo};

  const bool hayNeg = app.grafoTienePesosNegativos();
  c.btnDijkstra = {{px, 322.0f, ancho, 32.0f},
                   "Calcular con Dijkstra",
                   hayNeg ? EstiloBoton::Alerta : EstiloBoton::Primario,
                   true,
                   false};
  c.btnBellmanFord = {{px, 360.0f, ancho, 32.0f},
                      "Calcular con Bellman-Ford",
                      EstiloBoton::Exito,
                      true,
                      false};
  c.radioNodo = 24.0f;
  return c;
}

namespace {

ColorRGB colorPrioridad(dominio::Prioridad p) {
  switch (p) {
    case dominio::Prioridad::Medico:  return paleta::PELIGRO;
    case dominio::Prioridad::Urgente: return paleta::ALERTA;
    case dominio::Prioridad::Regular: return paleta::PRIMARIO;
  }
  return paleta::PRIMARIO;
}

void dibujarBarraSuperior(dominio::ModuloActivo activo, float ratonX,
                          float ratonY) {
  establecerColor(paleta::BARRA_SUPERIOR);
  dibujarRectanguloLleno(0.0f, 0.0f, static_cast<float>(layout::ANCHO_VENTANA),
                         layout::ALTO_BARRA_SUPERIOR);

  establecerColor(paleta::CYAN);
  dibujarTexto(16.0f, 12.0f, "CENTRAL EXPRES - UD");
  establecerColor(paleta::TEXTO_SECUNDARIO);
  dibujarTexto(16.0f, 31.0f, "Logistica y Enrutamiento");

  const auto pestanas = obtenerBotonesPestanas(activo);
  for (const auto& btn : pestanas) {
    dibujarBoton(btn, ratonX, ratonY);
  }

  establecerColor(paleta::BORDE);
  dibujarLinea(0.0f, layout::ALTO_BARRA_SUPERIOR,
               static_cast<float>(layout::ANCHO_VENTANA),
               layout::ALTO_BARRA_SUPERIOR);
}

void dibujarTarjetaPaquete(float x, float y, float w, float h,
                           const dominio::Paquete& paq,
                           const std::string& pesoTxt,
                           bool esSiguienteSalida) {
  const ColorRGB acento = colorPrioridad(paq.prioridad);

  establecerColor(paleta::PANEL_OSCURO);
  dibujarRectanguloLleno(x, y, x + w, y + h);

  establecerColor(acento);
  dibujarRectanguloLleno(x, y, x + w, y + 20.0f);

  const ColorRGB colTit =
      (paq.prioridad == dominio::Prioridad::Urgente) ? paleta::TEXTO_OSCURO
                                                     : paleta::TEXTO_PRINCIPAL;
  establecerColor(colTit);
  std::string cab = "#" + std::to_string(paq.id) + " " +
                    dominio::textoPrioridad(paq.prioridad);
  if (paq.prioridad != dominio::Prioridad::Regular) {
    cab += "(P" + std::to_string(static_cast<int>(paq.prioridad)) + ")";
  }
  dibujarTexto(x + 6.0f, y + 3.0f, recortarTexto(cab, 18));

  establecerColor(paleta::TEXTO_PRINCIPAL);
  dibujarTexto(x + 6.0f, y + 27.0f, recortarTexto(paq.destino, 18));

  establecerColor(paleta::TEXTO_SECUNDARIO);
  dibujarTexto(x + 6.0f, y + 46.0f, "Peso: " + pesoTxt);

  if (esSiguienteSalida) {
    dibujarInsignia(x + w - 66.0f, y + h - 20.0f, 62.0f, 16.0f, "NEXT",
                    paleta::EXITO, paleta::TEXTO_OSCURO);
  }

  establecerColor(esSiguienteSalida ? paleta::EXITO : paleta::BORDE_CLARO);
  dibujarRectangulo(x, y, x + w, y + h);
}

void dibujarBloqueColaTarjetas(const RectanguloUI& panel,
                               const std::vector<int>& ids,
                               const estado::EstadoApp& app,
                               bool resaltarPrimeroComoSiguiente,
                               const std::string& mensajeVacio) {
  if (ids.empty()) {
    const RectanguloUI cajaVacia{panel.x + 18.0f, panel.y + 72.0f,
                                 panel.w - 36.0f, 86.0f};
    establecerColor(paleta::PANEL_OSCURO);
    dibujarRectanguloLleno(cajaVacia.x, cajaVacia.y, cajaVacia.x + cajaVacia.w,
                           cajaVacia.y + cajaVacia.h);
    establecerColor(paleta::BORDE);
    dibujarRectangulo(cajaVacia.x, cajaVacia.y, cajaVacia.x + cajaVacia.w,
                      cajaVacia.y + cajaVacia.h);
    establecerColor(paleta::TEXTO_SECUNDARIO);
    dibujarTexto(cajaVacia.x + 16.0f, cajaVacia.y + 35.0f, mensajeVacio);
    return;
  }

  constexpr std::size_t POR_FILA = 4;
  constexpr std::size_t MAX_VISIBLES = 8;
  const float cardW = 144.0f;
  const float cardH = 74.0f;
  const float gapX = 20.0f;
  const float gapY = 16.0f;
  const float startX = panel.x + 16.0f;
  const float startY = panel.y + 52.0f;

  establecerColor(paleta::CYAN);
  dibujarTexto(startX, panel.y + 34.0f, "FRENTE (siguiente en salir)");

  const std::size_t limite =
      ids.size() < MAX_VISIBLES ? ids.size() : MAX_VISIBLES;
  for (std::size_t i = 0; i < limite; ++i) {
    const std::size_t fila = i / POR_FILA;
    const std::size_t col = i % POR_FILA;
    const float cx = startX + static_cast<float>(col) * (cardW + gapX);
    const float cy = startY + static_cast<float>(fila) * (cardH + gapY);

    const dominio::Paquete* paq = app.obtenerPaquete(ids[i]);
    dominio::Paquete info =
        paq != nullptr ? *paq
                       : dominio::Paquete{ids[i], "Paquete", 1.0,
                                          dominio::Prioridad::Regular};
    dibujarTarjetaPaquete(cx, cy, cardW, cardH, info,
                          app.formatearPeso(info.pesoKg),
                          resaltarPrimeroComoSiguiente && i == 0);

    if (col > 0) {
      dibujarFlecha(cx - 2.0f, cy + cardH * 0.5f, cx - gapX + 2.0f,
                    cy + cardH * 0.5f, paleta::TEXTO_SECUNDARIO, 6.0f, false);
    }
  }

  if (ids.size() > MAX_VISIBLES) {
    establecerColor(paleta::ALERTA);
    dibujarTexto(panel.x + panel.w - 130.0f, panel.y + 34.0f,
                 "+" + std::to_string(ids.size() - MAX_VISIBLES) +
                     " en espera");
  }
}

void dibujarModulo1(estado::EstadoApp& app, float ratonX, float ratonY) {
  const float panelIzqX = layout::MARGEN;
  const float panelIzqW = 668.0f;

  const RectanguloUI panelPrio{panelIzqX, layout::AREA_Y_INICIO, panelIzqW,
                               236.0f};
  dibujarPanel(panelPrio,
               "COLA DE PRIORIDAD (Urgentes / Medicos - Min-Heap O(log n)) | "
               "Pendientes: " +
                   std::to_string(app.colaPrioridad.tamano()));
  dibujarInsignia(panelPrio.x + panelPrio.w - 118.0f, panelPrio.y + 4.0f,
                  110.0f, 20.0f, "SALEN PRIMERO", paleta::PELIGRO);

  const auto ordenPrio = app.colaPrioridad.aVectorOrdenado();
  std::vector<int> idsPrio;
  idsPrio.reserve(ordenPrio.size());
  for (const auto& par : ordenPrio) {
    idsPrio.push_back(par.first);
  }
  dibujarBloqueColaTarjetas(
      panelPrio, idsPrio, app, !idsPrio.empty(),
      "Cola de prioridad vacia -> el despacho atendera la Cola Regular FIFO.");

  const RectanguloUI panelFifo{panelIzqX, panelPrio.y + panelPrio.h + 10.0f,
                               panelIzqW, 244.0f};
  dibujarPanel(panelFifo,
               "COLA REGULAR FIFO (Orden de llegada O(1)) | Pendientes: " +
                   std::to_string(app.colaRegular.tamano()));
  dibujarInsignia(panelFifo.x + panelFifo.w - 118.0f, panelFifo.y + 4.0f,
                  110.0f, 20.0f, "FILA GENERAL", paleta::PRIMARIO);

  const auto idsFifo = app.colaRegular.aVector();
  dibujarBloqueColaTarjetas(
      panelFifo, idsFifo, app, idsPrio.empty() && !idsFifo.empty(),
      "Cola FIFO regular vacia. Ingrese pedidos desde el panel derecho.");

  const RectanguloUI panelDer{panelIzqX + panelIzqW + 12.0f,
                              layout::AREA_Y_INICIO, 292.0f, 490.0f};
  dibujarPanel(panelDer, "RECEPCION Y DESPACHO");

  const ControlesModulo1 ctrl = obtenerControlesModulo1(app);
  dibujarCajaTexto(ctrl.cajaId, ratonX, ratonY);
  dibujarCajaTexto(ctrl.cajaDestino, ratonX, ratonY);

  establecerColor(paleta::TEXTO_SECUNDARIO);
  dibujarTexto(706.0f, 208.0f, "Peso estimado (kg):");
  dibujarBoton(ctrl.btnPesoMenos, ratonX, ratonY);
  dibujarBoton(ctrl.btnPesoMas, ratonX, ratonY);
  dibujarInsignia(756.0f, 224.0f, 164.0f, 28.0f,
                  app.formatearPeso(app.formM1.pesoKg), paleta::PANEL_OSCURO);

  establecerColor(paleta::TEXTO_SECUNDARIO);
  dibujarTexto(706.0f, 260.0f, "Nivel de prioridad:");
  dibujarBoton(ctrl.btnPrioMedico, ratonX, ratonY);
  dibujarBoton(ctrl.btnPrioUrgente, ratonX, ratonY);
  dibujarBoton(ctrl.btnPrioRegular, ratonX, ratonY);

  dibujarBoton(ctrl.btnIngresar, ratonX, ratonY);

  establecerColor(paleta::BORDE);
  dibujarLinea(panelDer.x + 12.0f, 366.0f, panelDer.x + panelDer.w - 12.0f,
               366.0f);
  establecerColor(paleta::TEXTO_SECUNDARIO);
  dibujarTexto(706.0f, 374.0f, "Despacho (prioridad antes que FIFO):");

  dibujarBoton(ctrl.btnDespachar, ratonX, ratonY);

  const RectanguloUI cajaUltimo{706.0f, 442.0f, 264.0f, 102.0f};
  dibujarPanel(cajaUltimo, "ULTIMO DESPACHO (enviado a Pila)",
               paleta::PANEL_OSCURO);
  if (!app.tieneUltimoDespachado) {
    establecerColor(paleta::TEXTO_SECUNDARIO);
    dibujarTexto(716.0f, 482.0f, "Aun no se han despachado");
    dibujarTexto(716.0f, 502.0f, "pedidos en esta sesion.");
  } else {
    establecerColor(paleta::EXITO);
    dibujarTexto(
        716.0f, 476.0f,
        "#" + std::to_string(app.ultimoDespachado.id) + " | " +
            dominio::textoPrioridad(app.ultimoDespachado.prioridad) + " (" +
            app.formatearPeso(app.ultimoDespachado.pesoKg) + ")");
    establecerColor(paleta::TEXTO_PRINCIPAL);
    dibujarTexto(716.0f, 496.0f,
                 "Dest: " + recortarTexto(app.ultimoDespachado.destino, 24));
    establecerColor(paleta::TEXTO_SECUNDARIO);
    dibujarTexto(716.0f, 516.0f,
                 "Via:  " + recortarTexto(app.origenUltimoDespacho, 24));
  }
}

void dibujarBarraComparacion(float x, float y, const std::string& etiqueta,
                             int comparaciones, int maxRef,
                             const ColorRGB& col) {
  establecerColor(paleta::TEXTO_SECUNDARIO);
  dibujarTexto(x, y, etiqueta);

  const float bx = x + 132.0f;
  const float bw = 112.0f;
  const float bh = 14.0f;
  establecerColor(paleta::PANEL_FONDO);
  dibujarRectanguloLleno(bx, y, bx + bw, y + bh);
  establecerColor(paleta::BORDE);
  dibujarRectangulo(bx, y, bx + bw, y + bh);

  if (comparaciones >= 0) {
    const float proporcion =
        maxRef > 0 ? static_cast<float>(comparaciones) / static_cast<float>(maxRef)
                   : 0.0f;
    const float anchoLleno =
        (proporcion > 1.0f ? 1.0f : proporcion) * (bw - 2.0f);
    if (anchoLleno > 0.5f) {
      establecerColor(col);
      dibujarRectanguloLleno(bx + 1.0f, y + 1.0f, bx + 1.0f + anchoLleno,
                             y + bh - 1.0f);
    }
    establecerColor(paleta::TEXTO_PRINCIPAL);
    dibujarTexto(bx + bw + 6.0f, y, std::to_string(comparaciones));
  } else {
    establecerColor(paleta::TEXTO_SECUNDARIO);
    dibujarTexto(bx + bw + 6.0f, y, "--");
  }
}

void dibujarModulo2(estado::EstadoApp& app, float ratonX, float ratonY) {
  const RectanguloUI panelIzq{layout::MARGEN, layout::AREA_Y_INICIO, 626.0f,
                              490.0f};
  dibujarPanel(panelIzq, "CATALOGO EN BODEGA (" +
                             std::to_string(app.bodegaIds.size()) +
                             " Paquetes)");

  if (app.m2.animacionActiva) {
    const std::string txtPaso =
        "MEZCLA " + std::to_string(app.m2.indicePasoActual + 1) + "/" +
        std::to_string(app.m2.pasosMerge.size());
    dibujarInsignia(panelIzq.x + panelIzq.w - 134.0f, panelIzq.y + 4.0f,
                    126.0f, 20.0f, txtPaso, paleta::CYAN, paleta::TEXTO_OSCURO);
  } else if (app.bodegaOrdenada) {
    dibujarInsignia(panelIzq.x + panelIzq.w - 134.0f, panelIzq.y + 4.0f,
                    126.0f, 20.0f, "ORDENADO", paleta::EXITO,
                    paleta::TEXTO_OSCURO);
  } else {
    dibujarInsignia(panelIzq.x + panelIzq.w - 134.0f, panelIzq.y + 4.0f,
                    126.0f, 20.0f, "DESORDENADO", paleta::ALERTA,
                    paleta::TEXTO_OSCURO);
  }

  TablaDatos tabla;
  tabla.rect = {panelIzq.x + 14.0f, panelIzq.y + 38.0f, panelIzq.w - 28.0f,
                340.0f};
  tabla.altoFila = 26.0f;
  tabla.columnas = {{"Pos", 46.0f},
                    {"ID", 64.0f},
                    {"Destino / Destinatario", 254.0f},
                    {"Peso", 96.0f},
                    {"Prioridad", 138.0f}};
  tabla.filaResaltada = app.m2.filaEncontrada;

  if (app.m2.animacionActiva &&
      app.m2.indicePasoActual < app.m2.pasosMerge.size()) {
    const auto& paso = app.m2.pasosMerge[app.m2.indicePasoActual];
    tabla.rangoMergeInicio = static_cast<int>(paso.inicio);
    tabla.rangoMergeMitad = static_cast<int>(paso.mitad);
    tabla.rangoMergeFin = static_cast<int>(paso.fin);
  }

  for (std::size_t i = 0; i < app.bodegaIds.size(); ++i) {
    const int id = app.bodegaIds[i];
    const dominio::Paquete* paq = app.obtenerPaquete(id);
    if (paq != nullptr) {
      std::string prioTxt = dominio::textoPrioridad(paq->prioridad);
      if (paq->prioridad != dominio::Prioridad::Regular) {
        prioTxt += " (P" + std::to_string(static_cast<int>(paq->prioridad)) +
                   ")";
      }
      tabla.filas.push_back({std::to_string(i + 1), "#" + std::to_string(id),
                             paq->destino, app.formatearPeso(paq->pesoKg),
                             prioTxt});
    }
  }
  dibujarTabla(tabla);

  const RectanguloUI cajaSnap{panelIzq.x + 14.0f, panelIzq.y + 388.0f,
                              panelIzq.w - 28.0f, 90.0f};
  dibujarPanel(cajaSnap, "COMPARATIVA ANTES / DESPUES (MERGE SORT)",
               paleta::PANEL_OSCURO);
  if (app.m2.animacionActiva &&
      app.m2.indicePasoActual < app.m2.pasosMerge.size()) {
    const auto& paso = app.m2.pasosMerge[app.m2.indicePasoActual];
    establecerColor(paleta::CYAN);
    dibujarTexto(cajaSnap.x + 10.0f, cajaSnap.y + 36.0f,
                 "Paso " + std::to_string(app.m2.indicePasoActual + 1) + "/" +
                     std::to_string(app.m2.pasosMerge.size()) +
                     ": combinando [" + std::to_string(paso.inicio + 1) +
                     ".." + std::to_string(paso.mitad) + "] (azul) + [" +
                     std::to_string(paso.mitad + 1) + ".." +
                     std::to_string(paso.fin) + "] (ambar)");
    establecerColor(paleta::TEXTO_PRINCIPAL);
    dibujarTexto(cajaSnap.x + 10.0f, cajaSnap.y + 58.0f,
                 "Actual: " +
                     recortarTexto(estado::EstadoApp::formatearListaIds(
                                       app.bodegaIds),
                                   68));
  } else if (app.m2.tieneSnapshots) {
    establecerColor(paleta::TEXTO_SECUNDARIO);
    dibujarTexto(cajaSnap.x + 10.0f, cajaSnap.y + 36.0f,
                 "Antes:   " +
                     recortarTexto(estado::EstadoApp::formatearListaIds(
                                       app.m2.snapshotAntes),
                                   66));
    establecerColor(paleta::EXITO);
    dibujarTexto(cajaSnap.x + 10.0f, cajaSnap.y + 58.0f,
                 "Despues: " +
                     recortarTexto(estado::EstadoApp::formatearListaIds(
                                       app.m2.snapshotDespues),
                                   66));
  } else {
    establecerColor(paleta::TEXTO_SECUNDARIO);
    dibujarTexto(cajaSnap.x + 10.0f, cajaSnap.y + 38.0f,
                 "Pulse 'Ordenar con Merge Sort' para ver la animacion de");
    dibujarTexto(cajaSnap.x + 10.0f, cajaSnap.y + 58.0f,
                 "division/mezcla y el contraste exacto Antes vs Despues.");
  }

  const RectanguloUI panelDer{panelIzq.x + panelIzq.w + 12.0f,
                              layout::AREA_Y_INICIO, 334.0f, 490.0f};
  dibujarPanel(panelDer, "ORDENAMIENTO Y BUSQUEDA");

  const ControlesModulo2 ctrl = obtenerControlesModulo2(app);
  dibujarBoton(ctrl.btnOrdenarMerge, ratonX, ratonY);
  dibujarBoton(ctrl.btnToggleAnim, ratonX, ratonY);
  dibujarBoton(ctrl.btnRestaurarDesorden, ratonX, ratonY);

  establecerColor(paleta::BORDE);
  dibujarLinea(panelDer.x + 12.0f, 188.0f, panelDer.x + panelDer.w - 12.0f,
               188.0f);

  dibujarCajaTexto(ctrl.cajaConsulta, ratonX, ratonY);
  dibujarBoton(ctrl.btnBuscarBinaria, ratonX, ratonY);
  dibujarBoton(ctrl.btnBuscarLinealId, ratonX, ratonY);
  dibujarBoton(ctrl.btnBuscarLinealDest, ratonX, ratonY);

  const RectanguloUI cajaComp{panelDer.x + 16.0f, 376.0f, panelDer.w - 32.0f,
                              168.0f};
  dibujarPanel(cajaComp, "CONTRASTE: O(log n) vs O(n)", paleta::PANEL_OSCURO);

  establecerColor(paleta::TEXTO_SECUNDARIO);
  dibujarTexto(cajaComp.x + 10.0f, cajaComp.y + 35.0f,
               "Ultima: " + recortarTexto(app.m2.ultimoMetodo, 26));

  if (app.m2.ultimoMetodo != "Ninguno") {
    establecerColor(app.m2.ultimoEncontrado ? paleta::EXITO : paleta::ALERTA);
    std::string resTxt = app.m2.ultimoEncontrado
                             ? ("OK " + app.m2.ultimoObjetivo + " en fila " +
                                std::to_string(app.m2.filaEncontrada + 1))
                             : ("No hallado: " + app.m2.ultimoObjetivo);
    dibujarTexto(cajaComp.x + 10.0f, cajaComp.y + 55.0f,
                 recortarTexto(resTxt, 34));
  } else {
    establecerColor(paleta::TEXTO_SECUNDARIO);
    dibujarTexto(cajaComp.x + 10.0f, cajaComp.y + 55.0f,
                 "Pruebe ambas busquedas con el mismo ID.");
  }

  const int n = static_cast<int>(app.bodegaIds.size());
  dibujarBarraComparacion(cajaComp.x + 10.0f, cajaComp.y + 84.0f,
                          "Binaria (ID):", app.m2.compBinaria, n,
                          paleta::EXITO);
  dibujarBarraComparacion(cajaComp.x + 10.0f, cajaComp.y + 108.0f,
                          "Lineal  (ID):", app.m2.compLinealId, n,
                          paleta::ALERTA);
  dibujarBarraComparacion(cajaComp.x + 10.0f, cajaComp.y + 132.0f,
                          "Lineal Dest.:", app.m2.compLinealDestino, n,
                          paleta::CYAN);
}

void dibujarNodoLista(float x, float y, float w, float h, int id,
                      const std::string& nombre, const std::string& zona,
                      bool esCabeza, bool resaltado) {
  const float anchoPtr = 22.0f;
  const float anchoDato = w - anchoPtr;

  establecerColor(paleta::PANEL_OSCURO);
  dibujarRectanguloLleno(x, y, x + anchoDato, y + h);

  establecerColor(paleta::PANEL_HEADER);
  dibujarRectanguloLleno(x + anchoDato, y, x + w, y + h);

  establecerColor(paleta::CYAN);
  dibujarCirculoLleno(x + anchoDato + anchoPtr * 0.5f, y + h * 0.5f, 3.5f);

  establecerColor(resaltado ? paleta::EXITO : paleta::CYAN);
  dibujarTexto(x + 6.0f, y + 6.0f, "#" + std::to_string(id));

  establecerColor(paleta::TEXTO_PRINCIPAL);
  dibujarTexto(x + 6.0f, y + 23.0f, recortarTexto(nombre, 16));

  establecerColor(paleta::TEXTO_SECUNDARIO);
  dibujarTexto(x + 6.0f, y + 40.0f, recortarTexto(zona, 16));

  establecerColor(resaltado ? paleta::EXITO
                            : (esCabeza ? paleta::CYAN : paleta::BORDE_CLARO));
  dibujarRectangulo(x, y, x + w, y + h);
  dibujarLinea(x + anchoDato, y, x + anchoDato, y + h);

  if (esCabeza) {
    dibujarInsignia(x, y - 18.0f, 52.0f, 15.0f, "HEAD", paleta::CYAN,
                    paleta::TEXTO_OSCURO);
  }
}

void dibujarModulo3(estado::EstadoApp& app, float ratonX, float ratonY) {
  const RectanguloUI panelLista{layout::MARGEN, layout::AREA_Y_INICIO, 594.0f,
                                312.0f};
  dibujarPanel(panelLista,
               "FLOTA ACTIVA - LISTA ENLAZADA (" +
                   std::to_string(app.listaRepartidores.tamano()) +
                   " repartidores)");
  dibujarInsignia(panelLista.x + panelLista.w - 118.0f, panelLista.y + 4.0f,
                  110.0f, 20.0f, "NODOS Y NEXT", paleta::PRIMARIO);

  const auto idsRep = app.listaRepartidores.aVector();
  if (idsRep.empty()) {
    dibujarInsignia(panelLista.x + 40.0f, panelLista.y + 132.0f, 64.0f, 28.0f,
                    "HEAD", paleta::CYAN, paleta::TEXTO_OSCURO);
    dibujarFlecha(panelLista.x + 106.0f, panelLista.y + 146.0f,
                  panelLista.x + 178.0f, panelLista.y + 146.0f, paleta::ALERTA,
                  9.0f, false);
    dibujarInsignia(panelLista.x + 182.0f, panelLista.y + 132.0f, 76.0f, 28.0f,
                    "NULL", paleta::PELIGRO);
    establecerColor(paleta::TEXTO_SECUNDARIO);
    dibujarTexto(panelLista.x + 276.0f, panelLista.y + 140.0f,
                 "Sin repartidores activos en memoria.");
  } else {
    constexpr std::size_t POR_FILA = 3;
    constexpr std::size_t MAX_NODOS = 9;
    const float nodeW = 156.0f;
    const float nodeH = 58.0f;
    const float gapX = 32.0f;
    const float gapY = 26.0f;
    const float startX = panelLista.x + 24.0f;
    const float startY = panelLista.y + 52.0f;

    const std::size_t limite =
        idsRep.size() < MAX_NODOS ? idsRep.size() : MAX_NODOS;

    for (std::size_t i = 0; i < limite; ++i) {
      const std::size_t fila = i / POR_FILA;
      const std::size_t col = i % POR_FILA;
      const float nx = startX + static_cast<float>(col) * (nodeW + gapX);
      const float ny = startY + static_cast<float>(fila) * (nodeH + gapY);

      const dominio::Repartidor* rep = app.obtenerRepartidor(idsRep[i]);
      const std::string nom = rep != nullptr ? rep->nombre : "Repartidor";
      const std::string zon = rep != nullptr ? rep->zona : "-";

      dibujarNodoLista(nx, ny, nodeW, nodeH, idsRep[i], nom, zon, i == 0,
                       idsRep[i] == app.m3.ultimoRepResaltado);

      const float ptrX = nx + nodeW - 11.0f;
      const float ptrY = ny + nodeH * 0.5f;
      if (i + 1 < limite) {
        const std::size_t sigFila = (i + 1) / POR_FILA;
        if (sigFila == fila) {
          dibujarFlecha(ptrX + 4.0f, ptrY, nx + nodeW + gapX - 2.0f, ptrY,
                        paleta::CYAN, 7.0f, false);
        } else {
          const float bajaY = ny + nodeH + gapY * 0.5f;
          const float destX = startX + nodeW * 0.5f;
          const float destY =
              startY + static_cast<float>(sigFila) * (nodeH + gapY);
          establecerColor(paleta::CYAN);
          dibujarLinea(ptrX + 4.0f, ptrY, nx + nodeW + 14.0f, ptrY);
          dibujarLinea(nx + nodeW + 14.0f, ptrY, nx + nodeW + 14.0f, bajaY);
          dibujarLinea(nx + nodeW + 14.0f, bajaY, destX, bajaY);
          dibujarFlecha(destX, bajaY, destX, destY - 2.0f, paleta::CYAN, 7.0f,
                        false);
        }
      } else if (idsRep.size() <= MAX_NODOS) {
        const float nullX = nx + nodeW + 22.0f;
        if (col + 1 < POR_FILA) {
          dibujarFlecha(ptrX + 4.0f, ptrY, nullX - 2.0f, ptrY, paleta::PELIGRO,
                        7.0f, false);
          dibujarInsignia(nullX, ny + 17.0f, 52.0f, 24.0f, "NULL",
                          paleta::PELIGRO_FONDO);
        } else {
          dibujarFlecha(ptrX, ny + nodeH, ptrX, ny + nodeH + 14.0f,
                        paleta::PELIGRO, 6.0f, false);
          dibujarInsignia(nx + nodeW - 54.0f, ny + nodeH + 15.0f, 54.0f, 18.0f,
                          "NULL", paleta::PELIGRO_FONDO);
        }
      }
    }

    if (idsRep.size() > MAX_NODOS) {
      establecerColor(paleta::ALERTA);
      dibujarTexto(panelLista.x + 24.0f, panelLista.y + panelLista.h - 22.0f,
                   "+" + std::to_string(idsRep.size() - MAX_NODOS) +
                       " repartidores adicionales en la lista -> NULL");
    }
  }

  const RectanguloUI panelFormRep{layout::MARGEN,
                                  panelLista.y + panelLista.h + 10.0f, 594.0f,
                                  168.0f};
  dibujarPanel(panelFormRep, "GESTION DE REPARTIDORES (Insertar / Eliminar)");

  const ControlesModulo3 ctrl = obtenerControlesModulo3(app);
  dibujarCajaTexto(ctrl.cajaRepId, ratonX, ratonY);
  dibujarCajaTexto(ctrl.cajaRepNombre, ratonX, ratonY);
  dibujarCajaTexto(ctrl.cajaRepZona, ratonX, ratonY);
  dibujarBoton(ctrl.btnInsertarInicio, ratonX, ratonY);
  dibujarBoton(ctrl.btnInsertarFinal, ratonX, ratonY);
  dibujarBoton(ctrl.btnEliminarRep, ratonX, ratonY);

  establecerColor(paleta::TEXTO_SECUNDARIO);
  dibujarTexto(28.0f, 534.0f,
               "Tip: para eliminar, ingrese el ID (ej. 30) y pulse 'Eliminar "
               "por ID'.");

  const RectanguloUI panelPila{panelLista.x + panelLista.w + 12.0f,
                               layout::AREA_Y_INICIO, 366.0f, 490.0f};
  dibujarPanel(panelPila,
               "PILA LIFO DE ENTREGAS (" +
                   std::to_string(app.pilaEntregas.tamano()) + " registros)");
  dibujarInsignia(panelPila.x + panelPila.w - 64.0f, panelPila.y + 4.0f, 56.0f,
                  20.0f, "LIFO", paleta::ALERTA, paleta::TEXTO_OSCURO);

  const float tuboX = panelPila.x + 56.0f;
  const float tuboY = panelPila.y + 42.0f;
  const float tuboW = 294.0f;
  const float tuboH = 238.0f;

  establecerColor(paleta::PANEL_OSCURO);
  dibujarRectanguloLleno(tuboX, tuboY, tuboX + tuboW, tuboY + tuboH);
  establecerColor(paleta::BORDE_CLARO);
  dibujarLinea(tuboX, tuboY, tuboX, tuboY + tuboH);
  dibujarLinea(tuboX + tuboW, tuboY, tuboX + tuboW, tuboY + tuboH);
  dibujarLinea(tuboX, tuboY + tuboH, tuboX + tuboW, tuboY + tuboH);
  dibujarLinea(tuboX, tuboY + tuboH + 1.0f, tuboX + tuboW, tuboY + tuboH + 1.0f);

  establecerColor(paleta::TEXTO_SECUNDARIO);
  dibujarTexto(tuboX + tuboW * 0.5f - 22.0f, tuboY + tuboH + 4.0f, "BASE");

  const auto idsEntregas = app.pilaEntregas.aVector();
  if (idsEntregas.empty()) {
    establecerColor(paleta::TEXTO_SECUNDARIO);
    dibujarTexto(tuboX + 66.0f, tuboY + 104.0f, "PILA VACIA (TOPE = NULL)");
  } else {
    constexpr std::size_t MAX_BLOQUES = 6;
    const float bloqueH = 34.0f;
    const float gapBloque = 4.0f;
    const std::size_t visibles =
        idsEntregas.size() < MAX_BLOQUES ? idsEntregas.size() : MAX_BLOQUES;

    establecerColor(paleta::CYAN);
    dibujarTexto(panelPila.x + 8.0f, tuboY + 12.0f, "TOPE");
    dibujarFlecha(panelPila.x + 40.0f, tuboY + 20.0f, tuboX + 4.0f,
                  tuboY + 20.0f, paleta::CYAN, 6.0f, false);

    for (std::size_t i = 0; i < visibles; ++i) {
      const float by =
          tuboY + 6.0f + static_cast<float>(i) * (bloqueH + gapBloque);
      const float bx = tuboX + 6.0f;
      const float bw = tuboW - 12.0f;

      const dominio::RegistroEntrega* reg = app.obtenerEntrega(idsEntregas[i]);
      const ColorRGB acento =
          reg != nullptr ? colorPrioridad(reg->prioridad) : paleta::PRIMARIO;

      establecerColor(app.m3.auditoriaActiva ? ColorRGB{25, 48, 72}
                                             : paleta::PANEL_FONDO);
      dibujarRectanguloLleno(bx, by, bx + bw, by + bloqueH);

      establecerColor(acento);
      dibujarRectanguloLleno(bx, by, bx + 6.0f, by + bloqueH);

      establecerColor(i == 0 ? paleta::CYAN : paleta::BORDE_CLARO);
      dibujarRectangulo(bx, by, bx + bw, by + bloqueH);

      if (reg != nullptr) {
        std::string prefijo =
            app.m3.auditoriaActiva ? ("[" + std::to_string(i + 1) + "] ") : "";
        establecerColor(paleta::TEXTO_PRINCIPAL);
        dibujarTexto(bx + 12.0f, by + 3.0f,
                     recortarTexto(prefijo + "#" +
                                       std::to_string(reg->idPaquete) + " " +
                                       reg->destino,
                                   33));
        establecerColor(paleta::TEXTO_SECUNDARIO);
        dibujarTexto(bx + 12.0f, by + 18.0f,
                     recortarTexto("Rep: " + reg->repartidor + " | " +
                                       reg->origenAccion,
                                   33));
      }
    }

    if (idsEntregas.size() > MAX_BLOQUES) {
      establecerColor(paleta::ALERTA);
      dibujarTexto(tuboX + 6.0f, tuboY + tuboH + 4.0f,
                   "+" + std::to_string(idsEntregas.size() - MAX_BLOQUES) +
                       " mas");
    }
  }

  dibujarCajaTexto(ctrl.cajaEntregaPaqId, ratonX, ratonY);
  dibujarCajaTexto(ctrl.cajaEntregaDestino, ratonX, ratonY);
  dibujarBoton(ctrl.btnPushEntrega, ratonX, ratonY);
  dibujarBoton(ctrl.btnPopEntrega, ratonX, ratonY);
  dibujarBoton(ctrl.btnAuditarPila, ratonX, ratonY);

  if (app.m3.auditoriaActiva) {
    establecerColor(paleta::CYAN);
    dibujarTexto(634.0f, 536.0f, recortarTexto(app.m3.resumenAuditoria, 44));
  } else if (!app.m3.ultimoDeshecho.empty()) {
    establecerColor(paleta::ALERTA);
    dibujarTexto(634.0f, 536.0f,
                 recortarTexto("Ultimo pop: " + app.m3.ultimoDeshecho, 44));
  }
}

bool aristaEnCamino(const std::vector<int>& camino, int u, int v) {
  for (std::size_t i = 0; i + 1 < camino.size(); ++i) {
    if (camino[i] == u && camino[i + 1] == v) {
      return true;
    }
  }
  return false;
}

bool contieneId(const std::vector<int>& lista, int id) {
  return std::find(lista.begin(), lista.end(), id) != lista.end();
}

bool existeAristaDirecta(const algoritmos::Grafo& grafo, int origen,
                         int destino) {
  const ::Nodo* n = grafo.buscarNodo(origen);
  if (n == nullptr) {
    return false;
  }
  for (const auto& par : n->getAdyacentes()) {
    if (par.first->getValor() == destino) {
      return true;
    }
  }
  return false;
}

void dibujarModulo4(estado::EstadoApp& app, float ratonX, float ratonY) {
  const ControlesModulo4 ctrl = obtenerControlesModulo4(app);
  const float radio = ctrl.radioNodo;

  // 1) Panel izquierdo: Lienzo interactivo del grafo de 8 sectores.
  const RectanguloUI panelGrafo{layout::MARGEN, layout::AREA_Y_INICIO, 658.0f,
                                490.0f};
  dibujarPanel(panelGrafo,
               "GRAFO URBANO DIRIGIDO Y PONDERADO (8 Sectores - clic en nodos)");

  // Leyenda compacta en la parte superior del lienzo.
  establecerColor(paleta::PANEL_OSCURO);
  dibujarRectanguloLleno(panelGrafo.x + 8.0f, panelGrafo.y + 34.0f,
                         panelGrafo.x + panelGrafo.w - 8.0f,
                         panelGrafo.y + 58.0f);
  establecerColor(paleta::BORDE);
  dibujarRectangulo(panelGrafo.x + 8.0f, panelGrafo.y + 34.0f,
                    panelGrafo.x + panelGrafo.w - 8.0f, panelGrafo.y + 58.0f);

  dibujarInsignia(panelGrafo.x + 14.0f, panelGrafo.y + 38.0f, 58.0f, 16.0f,
                  "Origen", paleta::CYAN, paleta::TEXTO_OSCURO);
  dibujarInsignia(panelGrafo.x + 78.0f, panelGrafo.y + 38.0f, 64.0f, 16.0f,
                  "Destino", paleta::ALERTA, paleta::TEXTO_OSCURO);
  dibujarInsignia(panelGrafo.x + 148.0f, panelGrafo.y + 38.0f, 84.0f, 16.0f,
                  "Ruta optima", paleta::EXITO, paleta::TEXTO_OSCURO);
  dibujarInsignia(panelGrafo.x + 238.0f, panelGrafo.y + 38.0f, 92.0f, 16.0f,
                  "Arista (-)", paleta::MAGENTA_NEG, paleta::TEXTO_PRINCIPAL);
  dibujarInsignia(panelGrafo.x + 336.0f, panelGrafo.y + 38.0f, 92.0f, 16.0f,
                  "Ciclo (-inf)", paleta::PELIGRO, paleta::TEXTO_PRINCIPAL);

  establecerColor(paleta::TEXTO_SECUNDARIO);
  dibujarTexto(panelGrafo.x + 436.0f, panelGrafo.y + 39.0f,
               "Izq: O/D | Der: Destino");

  const bool rutaValida = app.m4.tieneResultado && app.m4.resultado.exito;
  const bool hayCiclo =
      app.m4.tieneResultado && app.m4.resultado.hayCicloNegativo;

  // Dibujar primero las aristas (primero normales, luego resaltadas encima).
  for (int pasada = 0; pasada < 2; ++pasada) {
    for (const ::Nodo* nodo : app.grafoUrbano.getNodos()) {
      const int idU = nodo->getValor();
      const dominio::Sector* secU = app.obtenerSector(idU);
      if (secU == nullptr) {
        continue;
      }

      for (const auto& arista : nodo->getAdyacentes()) {
        const int idV = arista.first->getValor();
        const int peso = arista.second;
        const dominio::Sector* secV = app.obtenerSector(idV);
        if (secV == nullptr) {
          continue;
        }

        const bool enRuta =
            rutaValida && aristaEnCamino(app.m4.resultado.camino, idU, idV);
        const bool enCiclo =
            hayCiclo && peso < 0 &&
            contieneId(app.m4.resultado.nodosAfectadosCiclo, idU) &&
            contieneId(app.m4.resultado.nodosAfectadosCiclo, idV);
        const bool destacada = enRuta || enCiclo;

        if ((pasada == 0 && destacada) || (pasada == 1 && !destacada)) {
          continue;
        }

        const float dx = secV->x - secU->x;
        const float dy = secV->y - secU->y;
        const float dist = std::hypot(dx, dy);
        if (dist < 1.0f) {
          continue;
        }
        const float ux = dx / dist;
        const float uy = dy / dist;
        const float nx = -uy;
        const float ny = ux;

        // Separar aristas paralelas cuando existe la inversa (ej. 4<->7, 2<->4).
        const bool bidireccional = existeAristaDirecta(app.grafoUrbano, idV, idU);
        const float offset = bidireccional ? 8.5f : 0.0f;

        const float x1 = secU->x + ux * (radio + 2.0f) + nx * offset;
        const float y1 = secU->y + uy * (radio + 2.0f) + ny * offset;
        const float x2 = secV->x - ux * (radio + 3.0f) + nx * offset;
        const float y2 = secV->y - uy * (radio + 3.0f) + ny * offset;

        ColorRGB colArista = ColorRGB{120, 146, 180};
        if (peso < 0) {
          colArista = paleta::MAGENTA_NEG;
        }
        if (enRuta) {
          colArista = paleta::EXITO;
        }
        if (enCiclo) {
          colArista = paleta::PELIGRO;
        }

        dibujarFlecha(x1, y1, x2, y2, colArista, destacada ? 10.0f : 8.0f,
                      destacada);

        // Etiqueta de peso en capsula legible cerca del punto medio.
        const float tLabel = bidireccional ? 0.44f : 0.48f;
        const float offLabel = bidireccional ? 12.0f : 9.0f;
        const float lx = x1 + (x2 - x1) * tLabel + nx * offLabel;
        const float ly = y1 + (y2 - y1) * tLabel + ny * offLabel;

        const std::string pesoTxt =
            (peso > 0 ? ("+" + std::to_string(peso)) : std::to_string(peso));
        const float boxW = 28.0f;
        const float boxH = 16.0f;

        ColorRGB fondoPill = paleta::PANEL_OSCURO;
        if (enCiclo) {
          fondoPill = paleta::PELIGRO;
        } else if (enRuta) {
          fondoPill = paleta::EXITO_FONDO;
        } else if (peso < 0) {
          fondoPill = ColorRGB{86, 24, 92};
        }

        establecerColor(fondoPill);
        dibujarRectanguloLleno(lx - boxW * 0.5f, ly - boxH * 0.5f,
                               lx + boxW * 0.5f, ly + boxH * 0.5f);
        establecerColor(colArista);
        dibujarRectangulo(lx - boxW * 0.5f, ly - boxH * 0.5f, lx + boxW * 0.5f,
                          ly + boxH * 0.5f);

        establecerColor(paleta::TEXTO_PRINCIPAL);
        dibujarTexto(lx - anchoTextoAprox(pesoTxt) * 0.5f, ly - 7.0f, pesoTxt);
      }
    }
  }

  // Dibujar los 8 nodos como circulos interactivos con nombre.
  for (const auto& sec : app.sectores) {
    const bool esOrigen = (sec.id == app.m4.nodoOrigen);
    const bool esDestino = (sec.id == app.m4.nodoDestino);
    const bool enRuta =
        rutaValida && contieneId(app.m4.resultado.camino, sec.id);
    const bool afectadoCiclo =
        hayCiclo && contieneId(app.m4.resultado.nodosAfectadosCiclo, sec.id);
    const bool hover =
        std::hypot(ratonX - sec.x, ratonY - sec.y) <= (radio + 3.0f);

    ColorRGB relleno = paleta::PANEL_HEADER;
    ColorRGB borde = hover ? paleta::TEXTO_PRINCIPAL : paleta::BORDE_CLARO;
    if (enRuta) {
      relleno = paleta::EXITO_FONDO;
      borde = paleta::EXITO;
    }
    if (afectadoCiclo) {
      relleno = paleta::PELIGRO_FONDO;
      borde = paleta::PELIGRO;
    }
    if (esOrigen) {
      borde = paleta::CYAN;
    }
    if (esDestino) {
      borde = paleta::ALERTA;
    }

    establecerColor(relleno);
    dibujarCirculoLleno(sec.x, sec.y, radio);
    establecerColor(borde);
    dibujarCirculo(sec.x, sec.y, radio);
    if (esOrigen || esDestino || enRuta || afectadoCiclo || hover) {
      dibujarCirculo(sec.x, sec.y, radio - 1.5f);
      dibujarCirculo(sec.x, sec.y, radio + 1.2f);
    }

    // ID centrado dentro del circulo.
    const std::string idTxt = std::to_string(sec.id);
    establecerColor(paleta::TEXTO_PRINCIPAL);
    dibujarTexto(sec.x - anchoTextoAprox(idTxt) * 0.5f, sec.y - 7.0f, idTxt);

    // Etiqueta del nombre del sector (arriba para nodos superiores, abajo para el resto).
    const float nombreY = (sec.y < 220.0f) ? (sec.y - radio - 18.0f)
                                           : (sec.y + radio + 5.0f);
    const float nombreW = anchoTextoAprox(sec.codigoCorto) + 8.0f;
    establecerColor(paleta::PANEL_OSCURO);
    dibujarRectanguloLleno(sec.x - nombreW * 0.5f, nombreY - 1.0f,
                           sec.x + nombreW * 0.5f, nombreY + 15.0f);
    establecerColor(paleta::TEXTO_PRINCIPAL);
    dibujarTexto(sec.x - anchoTextoAprox(sec.codigoCorto) * 0.5f, nombreY,
                 sec.codigoCorto);

    // Insignia O / D / -INF en esquina superior derecha del circulo.
    if (esOrigen && esDestino) {
      dibujarInsignia(sec.x + 12.0f, sec.y - radio - 6.0f, 30.0f, 15.0f, "O=D",
                      paleta::CYAN, paleta::TEXTO_OSCURO);
    } else if (esOrigen) {
      dibujarInsignia(sec.x + 14.0f, sec.y - radio - 6.0f, 20.0f, 15.0f, "O",
                      paleta::CYAN, paleta::TEXTO_OSCURO);
    } else if (esDestino) {
      dibujarInsignia(sec.x + 14.0f, sec.y - radio - 6.0f, 20.0f, 15.0f, "D",
                      paleta::ALERTA, paleta::TEXTO_OSCURO);
    }

    if (afectadoCiclo) {
      dibujarInsignia(sec.x - 22.0f, sec.y - radio - 16.0f, 44.0f, 15.0f,
                      "-INF", paleta::PELIGRO);
    }
  }

  // 2) Panel derecho: Controles de seleccion, modo de red, algoritmos y resultado.
  const RectanguloUI panelDer{panelGrafo.x + panelGrafo.w + 12.0f,
                              layout::AREA_Y_INICIO, 302.0f, 490.0f};
  dibujarPanel(panelDer, "ENRUTAMIENTO URBANO");

  const dominio::Sector* secO = app.obtenerSector(app.m4.nodoOrigen);
  const dominio::Sector* secD = app.obtenerSector(app.m4.nodoDestino);

  establecerColor(paleta::CYAN);
  dibujarTexto(696.0f, 104.0f,
               "Origen:  (" + std::to_string(app.m4.nodoOrigen) + ") " +
                   recortarTexto(secO != nullptr ? secO->nombre : "-", 18));
  establecerColor(paleta::ALERTA);
  dibujarTexto(696.0f, 126.0f,
               "Destino: (" + std::to_string(app.m4.nodoDestino) + ") " +
                   recortarTexto(secD != nullptr ? secD->nombre : "-", 18));

  dibujarBoton(ctrl.btnFijarOrigen, ratonX, ratonY);
  dibujarBoton(ctrl.btnFijarDestino, ratonX, ratonY);
  dibujarBoton(ctrl.btnIntercambiar, ratonX, ratonY);

  establecerColor(paleta::BORDE);
  dibujarLinea(panelDer.x + 10.0f, 188.0f, panelDer.x + panelDer.w - 10.0f,
               188.0f);
  establecerColor(paleta::TEXTO_SECUNDARIO);
  dibujarTexto(696.0f, 194.0f, "Escenario de la red vial:");

  dibujarBoton(ctrl.btnModoBonif, ratonX, ratonY);
  dibujarBoton(ctrl.btnModoPositivo, ratonX, ratonY);
  dibujarBoton(ctrl.btnModoCicloNeg, ratonX, ratonY);

  establecerColor(paleta::BORDE);
  dibujarLinea(panelDer.x + 10.0f, 306.0f, panelDer.x + panelDer.w - 10.0f,
               306.0f);
  dibujarBoton(ctrl.btnDijkstra, ratonX, ratonY);
  dibujarBoton(ctrl.btnBellmanFord, ratonX, ratonY);

  // Tarjeta de resultado del calculo.
  const RectanguloUI cajaRes{696.0f, 402.0f, 276.0f, 144.0f};
  dibujarPanel(cajaRes, "RESULTADO DE RUTA", paleta::PANEL_OSCURO);

  if (!app.m4.tieneResultado) {
    establecerColor(paleta::TEXTO_SECUNDARIO);
    dibujarTexto(706.0f, 444.0f, "Haga clic en los nodos del");
    dibujarTexto(706.0f, 464.0f, "grafo para elegir Origen y");
    dibujarTexto(706.0f, 484.0f, "Destino, y ejecute Dijkstra");
    dibujarTexto(706.0f, 504.0f, "o Bellman-Ford.");
  } else if (app.m4.resultado.tienePesosNegativos) {
    establecerColor(paleta::ALERTA);
    dibujarTexto(706.0f, 438.0f, "ALERTA: DIJKSTRA NO VALIDO");
    establecerColor(paleta::TEXTO_PRINCIPAL);
    dibujarTexto(706.0f, 460.0f, "El grafo tiene aristas con");
    dibujarTexto(706.0f, 478.0f, "peso negativo (-2, -3).");
    establecerColor(paleta::EXITO);
    dibujarTexto(706.0f, 502.0f, "Sugerencia: use Bellman-Ford");
    dibujarTexto(706.0f, 520.0f, "o cambie a 'Red solo positiva'.");
  } else if (app.m4.resultado.hayCicloNegativo) {
    establecerColor(paleta::PELIGRO);
    dibujarTexto(706.0f, 438.0f, "CICLO NEGATIVO DETECTADO!");
    establecerColor(paleta::TEXTO_PRINCIPAL);
    dibujarTexto(706.0f, 460.0f, "Algoritmo: Bellman-Ford");
    dibujarTexto(706.0f, 478.0f, "No existe minimo finito (-inf).");
    establecerColor(paleta::ALERTA);
    dibujarTexto(706.0f, 500.0f, "Nodos afectados:");
    dibujarTexto(
        706.0f, 518.0f,
        recortarTexto(estado::EstadoApp::formatearListaIds(
                          app.m4.resultado.nodosAfectadosCiclo),
                      34));
  } else if (app.m4.resultado.inalcanzable) {
    establecerColor(paleta::ALERTA);
    dibujarTexto(706.0f, 440.0f, "DESTINO INALCANZABLE");
    establecerColor(paleta::TEXTO_PRINCIPAL);
    dibujarTexto(706.0f, 464.0f, "Algoritmo: " + app.m4.algoritmoUsado);
    dibujarTexto(706.0f, 484.0f,
                 "No hay camino dirigido de (" +
                     std::to_string(app.m4.nodoOrigen) + ") a (" +
                     std::to_string(app.m4.nodoDestino) + ").");
    establecerColor(paleta::TEXTO_SECUNDARIO);
    dibujarTexto(706.0f, 510.0f, "Distancia = +infinito");
  } else if (app.m4.resultado.exito) {
    establecerColor(paleta::EXITO);
    dibujarTexto(706.0f, 438.0f,
                 app.m4.algoritmoUsado + " | Costo: " +
                     std::to_string(app.m4.resultado.costoTotal));

    // Mostrar secuencia de IDs y nombres en dos lineas legibles.
    std::string seqIds;
    for (std::size_t i = 0; i < app.m4.resultado.camino.size(); ++i) {
      if (i > 0) seqIds += " -> ";
      seqIds += std::to_string(app.m4.resultado.camino[i]);
    }
    establecerColor(paleta::CYAN);
    dibujarTexto(706.0f, 460.0f, "Secuencia: " + recortarTexto(seqIds, 23));

    const std::string nombres =
        app.formatearSecuenciaSectores(app.m4.resultado.camino);
    establecerColor(paleta::TEXTO_PRINCIPAL);
    const std::size_t corte = 33;
    dibujarTexto(706.0f, 482.0f, recortarTexto(nombres, corte));
    if (nombres.size() > corte) {
      dibujarTexto(706.0f, 500.0f,
                   recortarTexto(nombres.substr(corte), corte));
    }
    establecerColor(paleta::TEXTO_SECUNDARIO);
    dibujarTexto(706.0f, 522.0f,
                 "Tramos: " +
                     std::to_string(app.m4.resultado.camino.empty()
                                        ? 0
                                        : app.m4.resultado.camino.size() - 1) +
                     " | Red: " +
                     recortarTexto(estado::EstadoApp::nombreModoRed(app.modoRed),
                                   16));
  }
}

} // namespace

void renderizarAplicacion(estado::EstadoApp& app, float ratonX, float ratonY) {
  limpiarPantalla();

  establecerColor(paleta::FONDO_VENTANA);
  dibujarRectanguloLleno(0.0f, 0.0f, static_cast<float>(layout::ANCHO_VENTANA),
                         static_cast<float>(layout::ALTO_VENTANA));

  dibujarBarraSuperior(app.moduloActivo, ratonX, ratonY);

  switch (app.moduloActivo) {
    case dominio::ModuloActivo::ColaYPrioridad:
      dibujarModulo1(app, ratonX, ratonY);
      break;
    case dominio::ModuloActivo::Bodega:
      dibujarModulo2(app, ratonX, ratonY);
      break;
    case dominio::ModuloActivo::FlotaYAudi:
      dibujarModulo3(app, ratonX, ratonY);
      break;
    case dominio::ModuloActivo::RutasUrbanas:
      dibujarModulo4(app, ratonX, ratonY);
      break;
  }

  dibujarBarraEstado(app.log);
  refrescarPantalla();
}

} // namespace ui
