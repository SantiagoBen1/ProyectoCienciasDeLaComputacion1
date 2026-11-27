// ============================================================================
// Archivo: src/ui/Renderizador.cpp
// Capa: Capa C — Renderizador de Pantallas de los 4 Modulos.
// Proposito: Define la disposicion exacta de los controles y dibuja la barra
//            superior con KPIs globales, la guia contextual y las 4 vistas:
//              - Modulo 1: Tarjetas de Cola de Prioridad y Cola FIFO
//              - Modulo 2: Tabla de bodega, animacion Merge Sort y barras O(log n) vs O(n)
//              - Modulo 3: Nodos/flechas de Lista Enlazada y tubo vertical de Pila LIFO
//              - Modulo 4: Grafo dirigido de 8 sectores, aristas paralelas e inspector
// ============================================================================

#include "Renderizador.hpp"

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

namespace ui {

// ============================================================================
// Constructores de geometria compartida entre Renderizador y ControladorEventos
// ============================================================================

// Retorna los 4 botones de pestana superiores marcando como activo el modulo actual.
std::array<Boton, 4> obtenerBotonesPestanas(dominio::ModuloActivo activo) {
  const float y = 9.0f;
  const float h = 34.0f;
  return {{
      {{356.0f, y, 192.0f, h},
       "[1] Cola y Prioridad",
       EstiloBoton::Pestana,
       true,
       activo == dominio::ModuloActivo::ColaYPrioridad},
      {{554.0f, y, 192.0f, h},
       "[2] Bodega y Busqueda",
       EstiloBoton::Pestana,
       true,
       activo == dominio::ModuloActivo::Bodega},
      {{752.0f, y, 192.0f, h},
       "[3] Flota y Auditoria",
       EstiloBoton::Pestana,
       true,
       activo == dominio::ModuloActivo::FlotaYAudi},
      {{950.0f, y, 196.0f, h},
       "[4] Rutas Urbanas",
       EstiloBoton::Pestana,
       true,
       activo == dominio::ModuloActivo::RutasUrbanas},
  }};
}

// Boton global en la barra superior para reiniciar el estado al Bootstrapper.
Boton obtenerBotonReiniciar() {
  return {{244.0f, 11.0f, 98.0f, 30.0f},
          "Reset [F5]",
          EstiloBoton::Secundario,
          true,
          false};
}

// Construye la geometria del panel derecho de recepcion y despacho del Modulo 1.
ControlesModulo1 obtenerControlesModulo1(estado::EstadoApp& app) {
  const float px = 812.0f;
  const float ancho = 318.0f;

  ControlesModulo1 c;
  c.cajaId = {{px, 174.0f, ancho, 28.0f},
              "ID del paquete (unico):",
              &app.formM1.idTexto,
              &app.formM1.focoId,
              true,
              6};

  c.cajaDestino = {{px, 230.0f, ancho, 28.0f},
                   "Destino / destinatario:",
                   &app.formM1.destinoTexto,
                   &app.formM1.focoDestino,
                   false,
                   22};

  c.btnPesoMenos = {{px, 282.0f, 54.0f, 28.0f},
                    "-0.5",
                    EstiloBoton::Secundario,
                    app.formM1.pesoKg > 0.51,
                    false};

  c.btnPesoMas = {{px + ancho - 54.0f, 282.0f, 54.0f, 28.0f},
                  "+0.5",
                  EstiloBoton::Secundario,
                  app.formM1.pesoKg < 49.99,
                  false};

  const float yPrio = 336.0f;
  c.btnPrioMedico = {{px, yPrio, 102.0f, 30.0f},
                     "Medico (1)",
                     EstiloBoton::Peligro,
                     true,
                     app.formM1.prioridad == dominio::Prioridad::Medico};

  c.btnPrioUrgente = {{px + 108.0f, yPrio, 102.0f, 30.0f},
                      "Urgente (2)",
                      EstiloBoton::Alerta,
                      true,
                      app.formM1.prioridad == dominio::Prioridad::Urgente};

  c.btnPrioRegular = {{px + 216.0f, yPrio, 102.0f, 30.0f},
                      "Regular (3)",
                      EstiloBoton::Secundario,
                      true,
                      app.formM1.prioridad == dominio::Prioridad::Regular};

  c.btnIngresar = {{px, 376.0f, ancho, 34.0f},
                   "+ Ingresar pedido a cola",
                   EstiloBoton::Primario,
                   true,
                   false};

  c.btnDespachar = {{px, 446.0f, ancho, 36.0f},
                    ">> Despachar siguiente pedido",
                    EstiloBoton::Exito,
                    true,
                    false};

  return c;
}

// Construye la geometria del panel derecho de ordenamiento y busqueda del Modulo 2.
ControlesModulo2 obtenerControlesModulo2(estado::EstadoApp& app) {
  const float px = 788.0f;
  const float ancho = 342.0f;

  ControlesModulo2 c;
  c.btnOrdenarMerge = {
      {px, 154.0f, ancho, 34.0f},
      app.m2.animacionActiva ? "Finalizar Merge Sort ahora"
                             : "Ordenar con Merge Sort [O(n log n)]",
      app.m2.animacionActiva ? EstiloBoton::Alerta : EstiloBoton::Exito,
      true,
      false};

  c.btnToggleAnim = {{px, 196.0f, 166.0f, 30.0f},
                     app.m2.animarPasos ? "Animar pasos: ON"
                                        : "Animar pasos: OFF",
                     EstiloBoton::Secundario,
                     true,
                     app.m2.animarPasos};

  c.btnRestaurarDesorden = {{px + 176.0f, 196.0f, 166.0f, 30.0f},
                            "Desordenar inicial",
                            EstiloBoton::Secundario,
                            true,
                            false};

  c.cajaConsulta = {{px, 288.0f, ancho, 28.0f},
                    "Buscar por ID (ej. 109) o Destino:",
                    &app.m2.consultaTexto,
                    &app.m2.focoConsulta,
                    false,
                    20};

  // El estilo de Busqueda Binaria cambia a Alerta mientras el catalogo siga desordenado.
  c.btnBuscarBinaria = {{px, 326.0f, ancho, 32.0f},
                        "Busqueda Binaria por ID [O(log n)]",
                        app.bodegaOrdenada ? EstiloBoton::Primario
                                           : EstiloBoton::Alerta,
                        true,
                        false};

  c.btnBuscarLinealId = {{px, 364.0f, ancho, 32.0f},
                         "Busqueda Lineal por ID [O(n)]",
                         EstiloBoton::Secundario,
                         true,
                         false};

  c.btnBuscarLinealDest = {{px, 402.0f, ancho, 32.0f},
                           "Busqueda Lineal por Destino [O(n)]",
                           EstiloBoton::Secundario,
                           true,
                           false};

  return c;
}

// Construye la geometria de los formularios de Lista Enlazada (izq) y Pila LIFO (der).
ControlesModulo3 obtenerControlesModulo3(estado::EstadoApp& app) {
  ControlesModulo3 c;

  const float lx = 28.0f;
  c.cajaRepId = {{lx, 498.0f, 132.0f, 28.0f},
                 "ID Repartidor:",
                 &app.m3.repIdTexto,
                 &app.m3.focoRepId,
                 true,
                 5};
  c.cajaRepNombre = {{lx + 144.0f, 498.0f, 288.0f, 28.0f},
                     "Nombre del repartidor:",
                     &app.m3.repNombreTexto,
                     &app.m3.focoRepNombre,
                     false,
                     20};
  c.cajaRepZona = {{lx + 444.0f, 498.0f, 228.0f, 28.0f},
                   "Zona asignada:",
                   &app.m3.repZonaTexto,
                   &app.m3.focoRepZona,
                   false,
                   16};

  c.btnInsertarInicio = {{lx, 540.0f, 216.0f, 36.0f},
                         "+ Insertar inicio [O(1)]",
                         EstiloBoton::Primario,
                         true,
                         false};
  c.btnInsertarFinal = {{lx + 228.0f, 540.0f, 216.0f, 36.0f},
                        "+ Insertar final [O(1)]",
                        EstiloBoton::Exito,
                        true,
                        false};
  c.btnEliminarRep = {{lx + 456.0f, 540.0f, 216.0f, 36.0f},
                      "- Eliminar por ID [O(n)]",
                      EstiloBoton::Peligro,
                      true,
                      false};

  const float rx = 742.0f;
  const float rw = 388.0f;
  c.cajaEntregaPaqId = {{rx, 432.0f, 144.0f, 28.0f},
                        "ID Paquete:",
                        &app.m3.entregaPaqIdTexto,
                        &app.m3.focoEntregaPaqId,
                        true,
                        6};
  c.cajaEntregaDestino = {{rx + 156.0f, 432.0f, 232.0f, 28.0f},
                          "Destino (si es nuevo):",
                          &app.m3.entregaDestinoTexto,
                          &app.m3.focoEntregaDestino,
                          false,
                          18};

  c.btnPushEntrega = {{rx, 470.0f, rw, 32.0f},
                      "+ Registrar entrega (push) [O(1)]",
                      EstiloBoton::Exito,
                      true,
                      false};
  c.btnPopEntrega = {{rx, 508.0f, rw, 32.0f},
                     "<< Deshacer ultimo despacho (pop)",
                     EstiloBoton::Alerta,
                     true,
                     false};
  c.btnAuditarPila = {{rx, 546.0f, rw, 32.0f},
                      "Auditar historial (orden inverso)",
                      EstiloBoton::Primario,
                      true,
                      app.m3.auditoriaActiva};

  return c;
}

// Construye los controles del panel derecho de enrutamiento del Modulo 4.
ControlesModulo4 obtenerControlesModulo4(const estado::EstadoApp& app) {
  const float px = 798.0f;
  const float ancho = 332.0f;

  ControlesModulo4 c;
  c.btnFijarOrigen = {{px, 184.0f, 114.0f, 28.0f},
                      "Fijar Origen",
                      EstiloBoton::Secundario,
                      true,
                      app.m4.seleccionandoOrigen};
  c.btnFijarDestino = {{px + 120.0f, 184.0f, 118.0f, 28.0f},
                       "Fijar Destino",
                       EstiloBoton::Secundario,
                       true,
                       !app.m4.seleccionandoOrigen};
  c.btnIntercambiar = {{px + 244.0f, 184.0f, 88.0f, 28.0f},
                       "O <-> D",
                       EstiloBoton::Secundario,
                       true,
                       false};

  c.btnModoBonif = {{px, 246.0f, ancho, 28.0f},
                    "Red con bonificaciones (-)",
                    EstiloBoton::Secundario,
                    true,
                    app.modoRed == dominio::ModoRed::ConBonificaciones};
  c.btnModoPositivo = {{px, 278.0f, ancho, 28.0f},
                       "Red solo positiva (pesos >= 0)",
                       EstiloBoton::Secundario,
                       true,
                       app.modoRed == dominio::ModoRed::SoloPositivos};
  c.btnModoCicloNeg = {{px, 310.0f, ancho, 28.0f},
                       "Activar ciclo negativo DEMO",
                       EstiloBoton::Peligro,
                       true,
                       app.modoRed == dominio::ModoRed::CicloNegativoDemo};

  // Resalta el boton de Dijkstra en ambar cuando hay aristas negativas activas.
  const bool hayNeg = app.grafoTienePesosNegativos();
  c.btnDijkstra = {{px, 362.0f, ancho, 32.0f},
                   "Calcular ruta con Dijkstra",
                   hayNeg ? EstiloBoton::Alerta : EstiloBoton::Primario,
                   true,
                   false};
  c.btnBellmanFord = {{px, 398.0f, ancho, 32.0f},
                      "Calcular ruta con Bellman-Ford",
                      EstiloBoton::Exito,
                      true,
                      false};
  c.radioNodo = 25.0f;
  return c;
}

namespace {

// Asocia cada prioridad con su color semantico (Rojo=Medico, Ambar=Urgente, Azul=Regular).
ColorRGB colorPrioridad(dominio::Prioridad p) {
  switch (p) {
    case dominio::Prioridad::Medico:  return paleta::PELIGRO;
    case dominio::Prioridad::Urgente: return paleta::ALERTA;
    case dominio::Prioridad::Regular: return paleta::PRIMARIO;
  }
  return paleta::PRIMARIO;
}

// ============================================================================
// Marco Superior: Navegacion, Reset [F5], Sub-barra Guia y KPIs en Vivo
// ============================================================================

void dibujarBarraSuperior(const estado::EstadoApp& app, float ratonX,
                          float ratonY) {
  const float w = static_cast<float>(layout::ANCHO_VENTANA);
  establecerColor(paleta::BARRA_SUPERIOR);
  dibujarRectanguloLleno(0.0f, 0.0f, w, layout::ALTO_BARRA_SUPERIOR);

  establecerColor(paleta::CYAN);
  dibujarTexto(16.0f, 10.0f, "CENTRAL EXPRES - UD");
  establecerColor(paleta::TEXTO_SECUNDARIO);
  dibujarTexto(16.0f, 28.0f, "Logistica y Enrutamiento C++");

  dibujarBoton(obtenerBotonReiniciar(), ratonX, ratonY);

  const auto pestanas = obtenerBotonesPestanas(app.moduloActivo);
  for (const auto& btn : pestanas) {
    dibujarBoton(btn, ratonX, ratonY);
  }

  establecerColor(paleta::BORDE);
  dibujarLinea(0.0f, layout::ALTO_BARRA_SUPERIOR, w,
               layout::ALTO_BARRA_SUPERIOR);

  // Sub-barra contextual de guia paso a paso + indicadores globales en vivo.
  const float gy = layout::ALTO_BARRA_SUPERIOR;
  const float gh = layout::ALTO_BARRA_GUIA;
  establecerColor(paleta::PANEL_OSCURO);
  dibujarRectanguloLleno(0.0f, gy, w, gy + gh);
  establecerColor(paleta::BORDE);
  dibujarLinea(0.0f, gy + gh, w, gy + gh);

  dibujarInsignia(14.0f, gy + 6.0f, 54.0f, 20.0f, "GUIA", paleta::CYAN,
                  paleta::TEXTO_OSCURO);

  std::string guiaModulo;
  switch (app.moduloActivo) {
    case dominio::ModuloActivo::ColaYPrioridad:
      guiaModulo =
          "1) Complete ID, Destino y Prioridad -> 2) Ingrese a Cola -> 3) "
          "Despache o haga clic en una tarjeta";
      break;
    case dominio::ModuloActivo::Bodega:
      guiaModulo =
          "1) Ordene con Merge Sort -> 2) Compare Binaria O(log n) vs Lineal "
          "O(n) (clic en fila carga ID)";
      break;
    case dominio::ModuloActivo::FlotaYAudi:
      guiaModulo =
          "Clic en un nodo de la Lista Enlazada carga su ID | Pila LIFO: "
          "registre (push) o revierta (pop)";
      break;
    case dominio::ModuloActivo::RutasUrbanas:
      guiaModulo =
          "Clic Izq en nodo = Origen, Clic Der = Destino | Elija escenario "
          "vial y compare Dijkstra vs Bellman-Ford";
      break;
  }
  establecerColor(paleta::TEXTO_PRINCIPAL);
  dibujarTextoAcotado(76.0f, gy + 9.0f, 740.0f, guiaModulo);

  // Pildoras de estado global en tiempo real (evidencian continuidad entre modulos).
  const std::string kpiPrio =
      "Prio:" + std::to_string(app.colaPrioridad.tamano());
  const std::string kpiFifo =
      "FIFO:" + std::to_string(app.colaRegular.tamano());
  const std::string kpiBod =
      "Bodega:" + std::to_string(app.bodegaIds.size());
  const std::string kpiFlota =
      "Flota:" + std::to_string(app.listaRepartidores.tamano());
  const std::string kpiPila =
      "Pila:" + std::to_string(app.pilaEntregas.tamano());

  dibujarInsignia(828.0f, gy + 6.0f, 58.0f, 20.0f, kpiPrio,
                  paleta::PELIGRO_FONDO);
  dibujarInsignia(890.0f, gy + 6.0f, 58.0f, 20.0f, kpiFifo,
                  paleta::PANEL_HEADER);
  dibujarInsignia(952.0f, gy + 6.0f, 74.0f, 20.0f, kpiBod,
                  paleta::PANEL_HEADER);
  dibujarInsignia(1030.0f, gy + 6.0f, 62.0f, 20.0f, kpiFlota,
                  paleta::PANEL_HEADER);
  dibujarInsignia(1096.0f, gy + 6.0f, 50.0f, 20.0f, kpiPila,
                  paleta::EXITO_FONDO);
}

// ============================================================================
// Renderizado del Modulo 1: Cola de Prioridad y Cola Regular FIFO
// ============================================================================

// Dibuja una tarjeta individual de paquete con franja de prioridad, hover e insignia SALE YA.
void dibujarTarjetaPaquete(float x, float y, float w, float h,
                           const dominio::Paquete& paq,
                           const std::string& pesoTxt,
                           bool esSiguienteSalida, float ratonX, float ratonY) {
  const ColorRGB acento = colorPrioridad(paq.prioridad);
  const bool hover = RectanguloUI{x, y, w, h}.contiene(ratonX, ratonY);

  establecerColor(hover ? paleta::FILA_HOVER : paleta::PANEL_OSCURO);
  dibujarRectanguloLleno(x, y, x + w, y + h);

  establecerColor(acento);
  dibujarRectanguloLleno(x, y, x + w, y + 22.0f);

  const ColorRGB colTit =
      (paq.prioridad == dominio::Prioridad::Urgente) ? paleta::TEXTO_OSCURO
                                                     : paleta::TEXTO_PRINCIPAL;
  establecerColor(colTit);
  std::string cab = "#" + std::to_string(paq.id) + " " +
                    dominio::textoPrioridad(paq.prioridad);
  if (paq.prioridad != dominio::Prioridad::Regular) {
    cab += " (P" + std::to_string(static_cast<int>(paq.prioridad)) + ")";
  }
  dibujarTextoAcotado(x + 8.0f, y + 4.0f, w - 16.0f, cab);

  establecerColor(paleta::TEXTO_PRINCIPAL);
  dibujarTextoAcotado(x + 8.0f, y + 30.0f, w - 16.0f, paq.destino);

  establecerColor(paleta::TEXTO_SECUNDARIO);
  const float anchoMaxPeso = esSiguienteSalida ? (w - 78.0f) : (w - 16.0f);
  dibujarTextoAcotado(x + 8.0f, y + 54.0f, anchoMaxPeso, "Peso: " + pesoTxt);

  if (esSiguienteSalida) {
    dibujarInsignia(x + w - 66.0f, y + h - 24.0f, 60.0f, 18.0f, "SALE YA",
                    paleta::EXITO, paleta::TEXTO_OSCURO);
  }

  establecerColor(hover ? paleta::TEXTO_PRINCIPAL
                        : (esSiguienteSalida ? paleta::EXITO
                                             : paleta::BORDE_CLARO));
  dibujarRectangulo(x, y, x + w, y + h);
}

// Renderiza hasta 8 tarjetas (2 filas de 4) con flechas apuntando hacia el frente de cola.
void dibujarBloqueColaTarjetas(const RectanguloUI& panel,
                               const std::vector<int>& ids,
                               const estado::EstadoApp& app,
                               bool resaltarPrimeroComoSiguiente,
                               const std::string& mensajeVacio, float ratonX,
                               float ratonY) {
  if (ids.empty()) {
    const RectanguloUI cajaVacia{panel.x + 18.0f, panel.y + 80.0f,
                                 panel.w - 36.0f, 96.0f};
    establecerColor(paleta::PANEL_OSCURO);
    dibujarRectanguloLleno(cajaVacia.x, cajaVacia.y, cajaVacia.x + cajaVacia.w,
                           cajaVacia.y + cajaVacia.h);
    establecerColor(paleta::BORDE);
    dibujarRectangulo(cajaVacia.x, cajaVacia.y, cajaVacia.x + cajaVacia.w,
                      cajaVacia.y + cajaVacia.h);
    establecerColor(paleta::TEXTO_SECUNDARIO);
    dibujarTextoAcotado(cajaVacia.x + 16.0f, cajaVacia.y + 40.0f,
                        cajaVacia.w - 32.0f, mensajeVacio);
    return;
  }

  constexpr std::size_t POR_FILA = 4;
  constexpr std::size_t MAX_VISIBLES = 8;
  const float cardW = 168.0f;
  const float cardH = 82.0f;
  const float gapX = 20.0f;
  const float gapY = 16.0f;
  const float startX = panel.x + 18.0f;
  const float startY = panel.y + 56.0f;

  establecerColor(paleta::CYAN);
  dibujarTextoAcotado(
      startX, panel.y + 34.0f, panel.w - 160.0f,
      "FRENTE DE COLA (orden real de despacho) - Clic en tarjeta para ver detalle");

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
                          resaltarPrimeroComoSiguiente && i == 0, ratonX,
                          ratonY);

    // Flecha de derecha a izquierda indicando avance hacia el frente de salida.
    if (col > 0) {
      dibujarFlecha(cx - 2.0f, cy + cardH * 0.5f, cx - gapX + 2.0f,
                    cy + cardH * 0.5f, paleta::TEXTO_SECUNDARIO, 6.0f, false);
    }
  }

  if (ids.size() > MAX_VISIBLES) {
    establecerColor(paleta::ALERTA);
    dibujarTextoAcotado(panel.x + panel.w - 134.0f, panel.y + 34.0f, 124.0f,
                        "+" + std::to_string(ids.size() - MAX_VISIBLES) +
                            " en espera");
  }
}

void dibujarModulo1(estado::EstadoApp& app, float ratonX, float ratonY) {
  const float panelIzqX = layout::MARGEN;
  const float panelIzqW = 770.0f;

  // Panel superior izquierdo: Cola de Prioridad (Min-Heap).
  const RectanguloUI panelPrio{panelIzqX, layout::AREA_Y_INICIO, panelIzqW,
                               260.0f};
  dibujarPanel(panelPrio,
               "COLA DE PRIORIDAD (Urgentes / Medicos - Min-Heap O(log n)) | "
               "Pendientes: " +
                   std::to_string(app.colaPrioridad.tamano()));
  dibujarInsignia(panelPrio.x + panelPrio.w - 122.0f, panelPrio.y + 4.0f,
                  114.0f, 20.0f, "SALEN PRIMERO", paleta::PELIGRO);

  const auto ordenPrio = app.colaPrioridad.aVectorOrdenado();
  std::vector<int> idsPrio;
  idsPrio.reserve(ordenPrio.size());
  for (const auto& par : ordenPrio) {
    idsPrio.push_back(par.first);
  }
  dibujarBloqueColaTarjetas(
      panelPrio, idsPrio, app, !idsPrio.empty(),
      "Cola de prioridad vacia -> el despacho atendera la Cola Regular FIFO.",
      ratonX, ratonY);

  // Panel inferior izquierdo: Cola Regular FIFO.
  const RectanguloUI panelFifo{panelIzqX, panelPrio.y + panelPrio.h + 12.0f,
                               panelIzqW, 260.0f};
  dibujarPanel(panelFifo,
               "COLA REGULAR FIFO (Orden de llegada O(1)) | Pendientes: " +
                   std::to_string(app.colaRegular.tamano()));
  dibujarInsignia(panelFifo.x + panelFifo.w - 122.0f, panelFifo.y + 4.0f,
                  114.0f, 20.0f, "FILA GENERAL", paleta::PRIMARIO);

  const auto idsFifo = app.colaRegular.aVector();
  dibujarBloqueColaTarjetas(
      panelFifo, idsFifo, app, idsPrio.empty() && !idsFifo.empty(),
      "Cola FIFO regular vacia. Ingrese pedidos desde el panel derecho.",
      ratonX, ratonY);

  // Panel derecho: Formulario de ingreso, boton de despacho y ultimo despachado.
  const RectanguloUI panelDer{panelIzqX + panelIzqW + 12.0f,
                              layout::AREA_Y_INICIO, 350.0f, 532.0f};
  dibujarPanel(panelDer, "RECEPCION Y DESPACHO");

  establecerColor(paleta::CYAN);
  dibujarTextoAcotado(812.0f, 132.0f, 318.0f,
                      "PASO 1: REGISTRAR NUEVO PEDIDO");

  const ControlesModulo1 ctrl = obtenerControlesModulo1(app);
  dibujarCajaTexto(ctrl.cajaId, ratonX, ratonY);
  dibujarCajaTexto(ctrl.cajaDestino, ratonX, ratonY);

  establecerColor(paleta::TEXTO_SECUNDARIO);
  dibujarTexto(812.0f, 264.0f, "Peso estimado (kg):");
  dibujarBoton(ctrl.btnPesoMenos, ratonX, ratonY);
  dibujarBoton(ctrl.btnPesoMas, ratonX, ratonY);
  dibujarInsignia(874.0f, 282.0f, 194.0f, 28.0f,
                  app.formatearPeso(app.formM1.pesoKg), paleta::PANEL_OSCURO);

  establecerColor(paleta::TEXTO_SECUNDARIO);
  dibujarTexto(812.0f, 318.0f, "Nivel de prioridad:");
  dibujarBoton(ctrl.btnPrioMedico, ratonX, ratonY);
  dibujarBoton(ctrl.btnPrioUrgente, ratonX, ratonY);
  dibujarBoton(ctrl.btnPrioRegular, ratonX, ratonY);

  dibujarBoton(ctrl.btnIngresar, ratonX, ratonY);

  // Vista previa en vivo del proximo paquete que saldra al pulsar Despachar.
  establecerColor(paleta::BORDE);
  dibujarLinea(panelDer.x + 14.0f, 418.0f, panelDer.x + panelDer.w - 14.0f,
               418.0f);
  std::string sigTxt = "Colas vacias";
  if (!idsPrio.empty()) {
    sigTxt = "Sale #" + std::to_string(idsPrio.front()) + " (Cola Prioridad)";
  } else if (!idsFifo.empty()) {
    sigTxt = "Sale #" + std::to_string(idsFifo.front()) + " (Cola FIFO)";
  }
  establecerColor(paleta::EXITO);
  dibujarTextoAcotado(812.0f, 426.0f, 318.0f, "PASO 2: " + sigTxt);

  dibujarBoton(ctrl.btnDespachar, ratonX, ratonY);

  const RectanguloUI cajaUltimo{812.0f, 494.0f, 318.0f, 118.0f};
  dibujarPanel(cajaUltimo, "ULTIMO DESPACHO (Enviado a Pila)",
               paleta::PANEL_OSCURO);
  if (!app.tieneUltimoDespachado) {
    establecerColor(paleta::TEXTO_SECUNDARIO);
    dibujarTextoAcotado(824.0f, 538.0f, 294.0f,
                        "Aun no se han despachado pedidos");
    dibujarTextoAcotado(824.0f, 558.0f, 294.0f,
                        "en esta sesion.");
  } else {
    establecerColor(paleta::EXITO);
    dibujarTextoAcotado(
        824.0f, 532.0f, 294.0f,
        "#" + std::to_string(app.ultimoDespachado.id) + " | " +
            dominio::textoPrioridad(app.ultimoDespachado.prioridad) + " (" +
            app.formatearPeso(app.ultimoDespachado.pesoKg) + ")");
    establecerColor(paleta::TEXTO_PRINCIPAL);
    dibujarTextoAcotado(824.0f, 556.0f, 294.0f,
                        "Destino: " + app.ultimoDespachado.destino);
    establecerColor(paleta::TEXTO_SECUNDARIO);
    dibujarTextoAcotado(824.0f, 580.0f, 294.0f,
                        "Origen:  " + app.origenUltimoDespacho);
  }
}

// ============================================================================
// Renderizado del Modulo 2: Bodega (Merge Sort, Busqueda Binaria y Lineal)
// ============================================================================

// Dibuja una barra horizontal proporcional para contrastar comparaciones O(log n) vs O(n).
void dibujarBarraComparacion(float x, float y, float anchoTotal,
                             const std::string& etiqueta, int comparaciones,
                             int maxRef, const ColorRGB& col) {
  establecerColor(paleta::TEXTO_SECUNDARIO);
  dibujarTextoAcotado(x, y, 124.0f, etiqueta);

  const float bx = x + 130.0f;
  const float bw = anchoTotal - 174.0f;
  const float bh = 14.0f;
  establecerColor(paleta::PANEL_FONDO);
  dibujarRectanguloLleno(bx, y, bx + bw, y + bh);
  establecerColor(paleta::BORDE);
  dibujarRectangulo(bx, y, bx + bw, y + bh);

  if (comparaciones >= 0) {
    const float proporcion =
        maxRef > 0
            ? static_cast<float>(comparaciones) / static_cast<float>(maxRef)
            : 0.0f;
    const float anchoLleno =
        (proporcion > 1.0f ? 1.0f : proporcion) * (bw - 2.0f);
    if (anchoLleno > 0.5f) {
      establecerColor(col);
      dibujarRectanguloLleno(bx + 1.0f, y + 1.0f, bx + 1.0f + anchoLleno,
                             y + bh - 1.0f);
    }
    establecerColor(paleta::TEXTO_PRINCIPAL);
    dibujarTextoAcotado(bx + bw + 8.0f, y, 36.0f,
                        std::to_string(comparaciones));
  } else {
    establecerColor(paleta::TEXTO_SECUNDARIO);
    dibujarTexto(bx + bw + 8.0f, y, "--");
  }
}

void dibujarModulo2(estado::EstadoApp& app, float ratonX, float ratonY) {
  const RectanguloUI panelIzq{layout::MARGEN, layout::AREA_Y_INICIO, 746.0f,
                              532.0f};
  dibujarPanel(panelIzq, "CATALOGO EN BODEGA (" +
                             std::to_string(app.bodegaIds.size()) +
                             " Paquetes - clic en fila para seleccionar)");

  // Insignia dinamica del estado del catalogo (MEZCLA k/N, ORDENADO o DESORDENADO).
  if (app.m2.animacionActiva) {
    const std::string txtPaso =
        "MEZCLA " + std::to_string(app.m2.indicePasoActual + 1) + "/" +
        std::to_string(app.m2.pasosMerge.size());
    dibujarInsignia(panelIzq.x + panelIzq.w - 138.0f, panelIzq.y + 4.0f,
                    130.0f, 20.0f, txtPaso, paleta::CYAN, paleta::TEXTO_OSCURO);
  } else if (app.bodegaOrdenada) {
    dibujarInsignia(panelIzq.x + panelIzq.w - 138.0f, panelIzq.y + 4.0f,
                    130.0f, 20.0f, "ORDENADO", paleta::EXITO,
                    paleta::TEXTO_OSCURO);
  } else {
    dibujarInsignia(panelIzq.x + panelIzq.w - 138.0f, panelIzq.y + 4.0f,
                    130.0f, 20.0f, "DESORDENADO", paleta::ALERTA,
                    paleta::TEXTO_OSCURO);
  }

  // Construye y renderiza la tabla de paquetes de bodega.
  TablaDatos tabla;
  tabla.rect = {panelIzq.x + 14.0f, panelIzq.y + 38.0f, panelIzq.w - 28.0f,
                364.0f};
  tabla.altoFila = 28.0f;
  tabla.columnas = {{"Pos", 54.0f},
                    {"ID", 80.0f},
                    {"Destino / Destinatario", 310.0f},
                    {"Peso", 114.0f},
                    {"Prioridad", 160.0f}};
  tabla.filaResaltada = app.m2.filaEncontrada;

  // Durante la animacion de Merge Sort resalta las mitades izquierda y derecha.
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
  dibujarTabla(tabla, ratonX, ratonY);

  // Panel inferior izquierdo: Comparativa Antes / Despues o paso activo de mezcla.
  const RectanguloUI cajaSnap{panelIzq.x + 14.0f, panelIzq.y + 412.0f,
                              panelIzq.w - 28.0f, 106.0f};
  dibujarPanel(cajaSnap, "COMPARATIVA ANTES / DESPUES (MERGE SORT)",
               paleta::PANEL_OSCURO);
  const float maxSnapTexto = cajaSnap.w - 24.0f;
  if (app.m2.animacionActiva &&
      app.m2.indicePasoActual < app.m2.pasosMerge.size()) {
    const auto& paso = app.m2.pasosMerge[app.m2.indicePasoActual];
    establecerColor(paleta::CYAN);
    dibujarTextoAcotado(
        cajaSnap.x + 12.0f, cajaSnap.y + 40.0f, maxSnapTexto,
        "Paso " + std::to_string(app.m2.indicePasoActual + 1) + "/" +
            std::to_string(app.m2.pasosMerge.size()) + ": combinando [" +
            std::to_string(paso.inicio + 1) + ".." +
            std::to_string(paso.mitad) + "] (azul) + [" +
            std::to_string(paso.mitad + 1) + ".." + std::to_string(paso.fin) +
            "] (ambar)");
    establecerColor(paleta::TEXTO_PRINCIPAL);
    dibujarTextoAcotado(
        cajaSnap.x + 12.0f, cajaSnap.y + 68.0f, maxSnapTexto,
        "Actual:  " + estado::EstadoApp::formatearListaIds(app.bodegaIds));
  } else if (app.m2.tieneSnapshots) {
    establecerColor(paleta::TEXTO_SECUNDARIO);
    dibujarTextoAcotado(
        cajaSnap.x + 12.0f, cajaSnap.y + 40.0f, maxSnapTexto,
        "Antes:   " +
            estado::EstadoApp::formatearListaIds(app.m2.snapshotAntes));
    establecerColor(paleta::EXITO);
    dibujarTextoAcotado(
        cajaSnap.x + 12.0f, cajaSnap.y + 68.0f, maxSnapTexto,
        "Despues: " +
            estado::EstadoApp::formatearListaIds(app.m2.snapshotDespues));
  } else {
    establecerColor(paleta::TEXTO_SECUNDARIO);
    dibujarTextoAcotado(
        cajaSnap.x + 12.0f, cajaSnap.y + 42.0f, maxSnapTexto,
        "Pulse 'Ordenar con Merge Sort' para visualizar la division/mezcla");
    dibujarTextoAcotado(
        cajaSnap.x + 12.0f, cajaSnap.y + 66.0f, maxSnapTexto,
        "recursiva y comparar el arreglo de IDs Antes vs Despues.");
  }

  // Panel derecho: controles de Merge Sort, buscador y contraste de comparaciones.
  const RectanguloUI panelDer{panelIzq.x + panelIzq.w + 12.0f,
                              layout::AREA_Y_INICIO, 374.0f, 532.0f};
  dibujarPanel(panelDer, "ORDENAMIENTO Y BUSQUEDA");

  establecerColor(paleta::CYAN);
  dibujarTextoAcotado(788.0f, 132.0f, 342.0f,
                      "PASO 1: ORDENAR CATALOGO");

  const ControlesModulo2 ctrl = obtenerControlesModulo2(app);
  dibujarBoton(ctrl.btnOrdenarMerge, ratonX, ratonY);
  dibujarBoton(ctrl.btnToggleAnim, ratonX, ratonY);
  dibujarBoton(ctrl.btnRestaurarDesorden, ratonX, ratonY);

  establecerColor(paleta::BORDE);
  dibujarLinea(panelDer.x + 14.0f, 238.0f, panelDer.x + panelDer.w - 14.0f,
               238.0f);
  establecerColor(paleta::CYAN);
  dibujarTextoAcotado(788.0f, 248.0f, 342.0f,
                      "PASO 2: BUSCAR PAQUETE");

  dibujarCajaTexto(ctrl.cajaConsulta, ratonX, ratonY);
  dibujarBoton(ctrl.btnBuscarBinaria, ratonX, ratonY);
  dibujarBoton(ctrl.btnBuscarLinealId, ratonX, ratonY);
  dibujarBoton(ctrl.btnBuscarLinealDest, ratonX, ratonY);

  // Tarjeta de contraste empirico O(log n) vs O(n) e historial de pivotes binarios.
  const RectanguloUI cajaComp{788.0f, 444.0f, 342.0f, 170.0f};
  dibujarPanel(cajaComp, "CONTRASTE: O(log n) vs O(n)", paleta::PANEL_OSCURO);
  const float maxCompW = cajaComp.w - 24.0f;

  establecerColor(paleta::TEXTO_SECUNDARIO);
  dibujarTextoAcotado(cajaComp.x + 12.0f, cajaComp.y + 34.0f, maxCompW,
                      "Ultima: " + app.m2.ultimoMetodo);

  if (app.m2.ultimoMetodo != "Ninguno") {
    establecerColor(app.m2.ultimoEncontrado ? paleta::EXITO : paleta::ALERTA);
    std::string resTxt = app.m2.ultimoEncontrado
                             ? ("OK " + app.m2.ultimoObjetivo + " en fila " +
                                std::to_string(app.m2.filaEncontrada + 1))
                             : ("No hallado: " + app.m2.ultimoObjetivo);
    dibujarTextoAcotado(cajaComp.x + 12.0f, cajaComp.y + 52.0f, maxCompW,
                        resTxt);
  } else {
    establecerColor(paleta::TEXTO_SECUNDARIO);
    dibujarTextoAcotado(cajaComp.x + 12.0f, cajaComp.y + 52.0f, maxCompW,
                        "Haga clic en una fila o busque por ID.");
  }

  if (!app.m2.pivotesBinaria.empty()) {
    establecerColor(paleta::CYAN);
    dibujarTextoAcotado(
        cajaComp.x + 12.0f, cajaComp.y + 70.0f, maxCompW,
        "Pivotes Bin: " +
            estado::EstadoApp::formatearListaIds(app.m2.pivotesBinaria));
  }

  const int n = static_cast<int>(app.bodegaIds.size());
  dibujarBarraComparacion(cajaComp.x + 12.0f, cajaComp.y + 96.0f, maxCompW,
                          "Binaria (ID):", app.m2.compBinaria, n,
                          paleta::EXITO);
  dibujarBarraComparacion(cajaComp.x + 12.0f, cajaComp.y + 118.0f, maxCompW,
                          "Lineal  (ID):", app.m2.compLinealId, n,
                          paleta::ALERTA);
  dibujarBarraComparacion(cajaComp.x + 12.0f, cajaComp.y + 140.0f, maxCompW,
                          "Lineal Dest.:", app.m2.compLinealDestino, n,
                          paleta::CYAN);
}

// ============================================================================
// Renderizado del Modulo 3: Lista Enlazada de Repartidores y Pila LIFO
// ============================================================================

// Dibuja un nodo de la Lista Enlazada dividido en [datos | *next] con insignias HEAD/TAIL.
void dibujarNodoLista(float x, float y, float w, float h, int id,
                      const std::string& nombre, const std::string& zona,
                      bool esCabeza, bool esCola, bool resaltado, float ratonX,
                      float ratonY) {
  const float anchoPtr = 24.0f;
  const float anchoDato = w - anchoPtr;
  const bool hover = RectanguloUI{x, y, w, h}.contiene(ratonX, ratonY);

  establecerColor(hover ? paleta::FILA_HOVER : paleta::PANEL_OSCURO);
  dibujarRectanguloLleno(x, y, x + anchoDato, y + h);

  // Compartimento derecho representando el puntero next.
  establecerColor(paleta::PANEL_HEADER);
  dibujarRectanguloLleno(x + anchoDato, y, x + w, y + h);

  establecerColor(paleta::CYAN);
  dibujarCirculoLleno(x + anchoDato + anchoPtr * 0.5f, y + h * 0.5f, 3.5f);

  establecerColor(resaltado ? paleta::EXITO : paleta::CYAN);
  dibujarTextoAcotado(x + 8.0f, y + 6.0f, 68.0f, "#" + std::to_string(id));

  // Insignias HEAD / TAIL en la esquina superior derecha del compartimento de datos.
  if (esCabeza && esCola) {
    dibujarInsignia(x + anchoDato - 78.0f, y + 4.0f, 74.0f, 16.0f, "HEAD/TAIL",
                    paleta::CYAN, paleta::TEXTO_OSCURO);
  } else if (esCabeza) {
    dibujarInsignia(x + anchoDato - 50.0f, y + 4.0f, 46.0f, 16.0f, "HEAD",
                    paleta::CYAN, paleta::TEXTO_OSCURO);
  } else if (esCola) {
    dibujarInsignia(x + anchoDato - 50.0f, y + 4.0f, 46.0f, 16.0f, "TAIL",
                    paleta::EXITO, paleta::TEXTO_OSCURO);
  }

  establecerColor(paleta::TEXTO_PRINCIPAL);
  dibujarTextoAcotado(x + 8.0f, y + 25.0f, anchoDato - 14.0f, nombre);

  establecerColor(paleta::TEXTO_SECUNDARIO);
  dibujarTextoAcotado(x + 8.0f, y + 43.0f, anchoDato - 14.0f,
                      "Zona: " + zona);

  establecerColor(hover ? paleta::TEXTO_PRINCIPAL
                        : (resaltado ? paleta::EXITO
                                     : (esCabeza ? paleta::CYAN
                                                 : (esCola ? paleta::EXITO
                                                           : paleta::BORDE_CLARO))));
  dibujarRectangulo(x, y, x + w, y + h);
  dibujarLinea(x + anchoDato, y, x + anchoDato, y + h);
}

void dibujarModulo3(estado::EstadoApp& app, float ratonX, float ratonY) {
  // 1) Panel superior izquierdo: Diagrama visual de la Lista Enlazada.
  const RectanguloUI panelLista{layout::MARGEN, layout::AREA_Y_INICIO, 700.0f,
                                340.0f};
  dibujarPanel(panelLista,
               "FLOTA ACTIVA - LISTA ENLAZADA (" +
                   std::to_string(app.listaRepartidores.tamano()) +
                   " repartidores - clic en nodo para seleccionar)");
  dibujarInsignia(panelLista.x + panelLista.w - 138.0f, panelLista.y + 4.0f,
                  130.0f, 20.0f, "HEAD / TAIL / NEXT", paleta::PRIMARIO);

  const auto idsRep = app.listaRepartidores.aVector();
  if (idsRep.empty()) {
    dibujarInsignia(panelLista.x + 40.0f, panelLista.y + 150.0f, 64.0f, 28.0f,
                    "HEAD", paleta::CYAN, paleta::TEXTO_OSCURO);
    dibujarFlecha(panelLista.x + 106.0f, panelLista.y + 164.0f,
                  panelLista.x + 178.0f, panelLista.y + 164.0f, paleta::ALERTA,
                  9.0f, false);
    dibujarInsignia(panelLista.x + 182.0f, panelLista.y + 150.0f, 76.0f, 28.0f,
                    "NULL", paleta::PELIGRO);
    establecerColor(paleta::TEXTO_SECUNDARIO);
    dibujarTextoAcotado(panelLista.x + 276.0f, panelLista.y + 158.0f, 380.0f,
                        "Sin repartidores activos en memoria.");
  } else {
    constexpr std::size_t POR_FILA = 3;
    constexpr std::size_t MAX_NODOS = 9;
    const float nodeW = 190.0f;
    const float nodeH = 62.0f;
    const float gapX = 34.0f;
    const float gapY = 28.0f;
    const float startX = panelLista.x + 24.0f;
    const float startY = panelLista.y + 46.0f;

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
                       i + 1 == idsRep.size(),
                       idsRep[i] == app.m3.ultimoRepResaltado, ratonX, ratonY);

      // Conector *next hacia el siguiente nodo (en la misma fila o saltando a la siguiente) o a NULL.
      const float ptrX = nx + nodeW - 12.0f;
      const float ptrY = ny + nodeH * 0.5f;
      if (i + 1 < limite) {
        const std::size_t sigFila = (i + 1) / POR_FILA;
        if (sigFila == fila) {
          dibujarFlecha(ptrX + 4.0f, ptrY, nx + nodeW + gapX - 2.0f, ptrY,
                        paleta::CYAN, 7.0f, false);
        } else {
          // Trazo ortogonal que conecta el fin de una fila con el inicio de la siguiente.
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
        // Ultimo nodo visible apuntando al centinela NULL.
        const float nullX = nx + nodeW + 18.0f;
        if (col + 1 < POR_FILA) {
          dibujarFlecha(ptrX + 4.0f, ptrY, nullX - 2.0f, ptrY, paleta::PELIGRO,
                        7.0f, false);
          dibujarInsignia(nullX, ny + 19.0f, 52.0f, 24.0f, "NULL",
                          paleta::PELIGRO_FONDO);
        } else {
          dibujarFlecha(ptrX, ny + nodeH, ptrX, ny + nodeH + 12.0f,
                        paleta::PELIGRO, 6.0f, false);
          dibujarInsignia(nx + nodeW - 54.0f, ny + nodeH + 13.0f, 54.0f, 18.0f,
                          "NULL", paleta::PELIGRO_FONDO);
        }
      }
    }

    if (idsRep.size() > MAX_NODOS) {
      establecerColor(paleta::ALERTA);
      dibujarTextoAcotado(
          panelLista.x + 24.0f, panelLista.y + panelLista.h - 22.0f,
          panelLista.w - 48.0f,
          "+" + std::to_string(idsRep.size() - MAX_NODOS) +
              " repartidores adicionales en la lista -> NULL");
    }
  }

  // 2) Panel inferior izquierdo: Formulario de altas y bajas de repartidores.
  const RectanguloUI panelFormRep{layout::MARGEN,
                                  panelLista.y + panelLista.h + 10.0f, 700.0f,
                                  182.0f};
  dibujarPanel(panelFormRep, "GESTION DE REPARTIDORES (Insertar / Eliminar)");

  const ControlesModulo3 ctrl = obtenerControlesModulo3(app);
  dibujarCajaTexto(ctrl.cajaRepId, ratonX, ratonY);
  dibujarCajaTexto(ctrl.cajaRepNombre, ratonX, ratonY);
  dibujarCajaTexto(ctrl.cajaRepZona, ratonX, ratonY);
  dibujarBoton(ctrl.btnInsertarInicio, ratonX, ratonY);
  dibujarBoton(ctrl.btnInsertarFinal, ratonX, ratonY);
  dibujarBoton(ctrl.btnEliminarRep, ratonX, ratonY);

  establecerColor(paleta::TEXTO_SECUNDARIO);
  dibujarTextoAcotado(
      28.0f, 592.0f, 672.0f,
      "Tip: haz clic sobre cualquier nodo de arriba para cargar su ID, o "
      "escribe uno nuevo.");

  // 3) Panel derecho: Contenedor vertical de la Pila LIFO y botones push/pop/auditoria.
  const RectanguloUI panelPila{panelLista.x + panelLista.w + 12.0f,
                               layout::AREA_Y_INICIO, 420.0f, 532.0f};
  dibujarPanel(panelPila,
               "PILA LIFO DE ENTREGAS (" +
                   std::to_string(app.pilaEntregas.tamano()) + " registros)");
  dibujarInsignia(panelPila.x + panelPila.w - 64.0f, panelPila.y + 4.0f, 56.0f,
                  20.0f, "LIFO", paleta::ALERTA, paleta::TEXTO_OSCURO);

  // Tubo vertical abierto arriba (TOPE) y cerrado abajo (BASE).
  const float tuboX = panelPila.x + 56.0f;
  const float tuboY = panelPila.y + 42.0f;
  const float tuboW = 344.0f;
  const float tuboH = 256.0f;

  establecerColor(paleta::PANEL_OSCURO);
  dibujarRectanguloLleno(tuboX, tuboY, tuboX + tuboW, tuboY + tuboH);
  establecerColor(paleta::BORDE_CLARO);
  dibujarLinea(tuboX, tuboY, tuboX, tuboY + tuboH);
  dibujarLinea(tuboX + tuboW, tuboY, tuboX + tuboW, tuboY + tuboH);
  dibujarLinea(tuboX, tuboY + tuboH, tuboX + tuboW, tuboY + tuboH);
  dibujarLinea(tuboX, tuboY + tuboH + 1.0f, tuboX + tuboW,
               tuboY + tuboH + 1.0f);

  establecerColor(paleta::TEXTO_SECUNDARIO);
  dibujarTexto(tuboX + tuboW * 0.5f - 20.0f, tuboY + tuboH + 5.0f, "BASE");

  const auto idsEntregas = app.pilaEntregas.aVector();
  if (idsEntregas.empty()) {
    establecerColor(paleta::TEXTO_SECUNDARIO);
    dibujarTextoAcotado(tuboX + 68.0f, tuboY + 116.0f, tuboW - 80.0f,
                        "PILA VACIA (TOPE = NULL)");
  } else {
    constexpr std::size_t MAX_BLOQUES = 6;
    const float bloqueH = 36.0f;
    const float gapBloque = 5.0f;
    const std::size_t visibles =
        idsEntregas.size() < MAX_BLOQUES ? idsEntregas.size() : MAX_BLOQUES;

    establecerColor(paleta::CYAN);
    dibujarTexto(panelPila.x + 8.0f, tuboY + 14.0f, "TOPE");
    dibujarFlecha(panelPila.x + 42.0f, tuboY + 22.0f, tuboX + 4.0f,
                  tuboY + 22.0f, paleta::CYAN, 6.0f, false);

    // Dibuja los registros apilados del mas reciente (arriba) al mas antiguo (abajo).
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
        dibujarTextoAcotado(bx + 12.0f, by + 4.0f, bw - 20.0f,
                            prefijo + "#" + std::to_string(reg->idPaquete) +
                                " -> " + reg->destino);
        establecerColor(paleta::TEXTO_SECUNDARIO);
        dibujarTextoAcotado(bx + 12.0f, by + 20.0f, bw - 20.0f,
                            "Rep: " + reg->repartidor + " | " +
                                reg->origenAccion);
      }
    }

    if (idsEntregas.size() > MAX_BLOQUES) {
      establecerColor(paleta::ALERTA);
      dibujarTextoAcotado(tuboX + 6.0f, tuboY + tuboH + 5.0f, 120.0f,
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
    dibujarTextoAcotado(742.0f, 592.0f, 388.0f, app.m3.resumenAuditoria);
  } else if (!app.m3.ultimoDeshecho.empty()) {
    establecerColor(paleta::ALERTA);
    dibujarTextoAcotado(742.0f, 592.0f, 388.0f,
                        "Ultimo pop: " + app.m3.ultimoDeshecho);
  }
}

// ============================================================================
// Renderizado del Modulo 4: Grafo Urbano, Dijkstra y Bellman-Ford
// ============================================================================

// Retorna true si el tramo dirigido u -> v pertenece a la ruta optima calculada.
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

// Verifica si existe la arista inversa para separar visualmente aristas bidireccionales.
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

  // 1) Panel izquierdo: Lienzo interactivo del grafo de 8 sectores (756x532).
  const RectanguloUI panelGrafo{layout::MARGEN, layout::AREA_Y_INICIO, 756.0f,
                                532.0f};
  dibujarPanel(panelGrafo,
               "GRAFO URBANO DIRIGIDO Y PONDERADO (8 Sectores - clic en nodos)");

  // Leyenda superior de convenciones cromaticas.
  establecerColor(paleta::PANEL_OSCURO);
  dibujarRectanguloLleno(panelGrafo.x + 10.0f, panelGrafo.y + 34.0f,
                         panelGrafo.x + panelGrafo.w - 10.0f,
                         panelGrafo.y + 60.0f);
  establecerColor(paleta::BORDE);
  dibujarRectangulo(panelGrafo.x + 10.0f, panelGrafo.y + 34.0f,
                    panelGrafo.x + panelGrafo.w - 10.0f,
                    panelGrafo.y + 60.0f);

  dibujarInsignia(panelGrafo.x + 16.0f, panelGrafo.y + 39.0f, 64.0f, 16.0f,
                  "Origen", paleta::CYAN, paleta::TEXTO_OSCURO);
  dibujarInsignia(panelGrafo.x + 86.0f, panelGrafo.y + 39.0f, 68.0f, 16.0f,
                  "Destino", paleta::ALERTA, paleta::TEXTO_OSCURO);
  dibujarInsignia(panelGrafo.x + 160.0f, panelGrafo.y + 39.0f, 92.0f, 16.0f,
                  "Ruta optima", paleta::EXITO, paleta::TEXTO_OSCURO);
  dibujarInsignia(panelGrafo.x + 258.0f, panelGrafo.y + 39.0f, 96.0f, 16.0f,
                  "Arista (-)", paleta::MAGENTA_NEG, paleta::TEXTO_PRINCIPAL);
  dibujarInsignia(panelGrafo.x + 360.0f, panelGrafo.y + 39.0f, 96.0f, 16.0f,
                  "Ciclo (-inf)", paleta::PELIGRO, paleta::TEXTO_PRINCIPAL);

  establecerColor(paleta::TEXTO_SECUNDARIO);
  dibujarTextoAcotado(panelGrafo.x + 468.0f, panelGrafo.y + 40.0f, 272.0f,
                      "Clic Izq: Origen/Dest | Der: Destino");

  const bool rutaValida = app.m4.tieneResultado && app.m4.resultado.exito;
  const bool hayCiclo =
      app.m4.tieneResultado && app.m4.resultado.hayCicloNegativo;

  // Dibujo de aristas en 2 pasadas: primero las normales y encima las resaltadas
  // (ruta optima o ciclo negativo) para que nunca queden tapadas.
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
        // Vector unitario (ux, uy) y normal perpendicular (nx, ny).
        const float ux = dx / dist;
        const float uy = dy / dist;
        const float nx = -uy;
        const float ny = ux;

        // Desplazamiento lateral cuando hay arista inversa (ej. 4<->7 o 2<->4)
        // para que ambas flechas y pesos se distingan sin solaparse.
        const bool bidireccional =
            existeAristaDirecta(app.grafoUrbano, idV, idU);
        const float offset = bidireccional ? 9.0f : 0.0f;

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

        // Capsula con el peso numerico ubicada junto al punto medio del tramo.
        const float tLabel = bidireccional ? 0.44f : 0.48f;
        const float offLabel = bidireccional ? 13.0f : 10.0f;
        const float lx = x1 + (x2 - x1) * tLabel + nx * offLabel;
        const float ly = y1 + (y2 - y1) * tLabel + ny * offLabel;

        const std::string pesoTxt =
            (peso > 0 ? ("+" + std::to_string(peso)) : std::to_string(peso));
        const float boxW = 30.0f;
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

  // Dibujo de los 8 sectores como circulos clicables con nombre e insignias O/D/-INF.
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
    // Triple anillo sutil para destacar nodos seleccionados, en ruta o bajo el cursor.
    if (esOrigen || esDestino || enRuta || afectadoCiclo || hover) {
      dibujarCirculo(sec.x, sec.y, radio - 1.5f);
      dibujarCirculo(sec.x, sec.y, radio + 1.2f);
    }

    const std::string idTxt = std::to_string(sec.id);
    establecerColor(paleta::TEXTO_PRINCIPAL);
    dibujarTexto(sec.x - anchoTextoAprox(idTxt) * 0.5f, sec.y - 7.0f, idTxt);

    // Coloca la etiqueta arriba en nodos de la fila superior y abajo en el resto.
    const float nombreY = (sec.y < 240.0f) ? (sec.y - radio - 19.0f)
                                           : (sec.y + radio + 6.0f);
    const float nombreW = anchoTextoAprox(sec.codigoCorto) + 10.0f;
    establecerColor(paleta::PANEL_OSCURO);
    dibujarRectanguloLleno(sec.x - nombreW * 0.5f, nombreY - 1.0f,
                           sec.x + nombreW * 0.5f, nombreY + 15.0f);
    establecerColor(paleta::BORDE);
    dibujarRectangulo(sec.x - nombreW * 0.5f, nombreY - 1.0f,
                      sec.x + nombreW * 0.5f, nombreY + 15.0f);
    establecerColor(paleta::TEXTO_PRINCIPAL);
    dibujarTexto(sec.x - anchoTextoAprox(sec.codigoCorto) * 0.5f, nombreY,
                 sec.codigoCorto);

    if (esOrigen && esDestino) {
      dibujarInsignia(sec.x + 12.0f, sec.y - radio - 6.0f, 34.0f, 15.0f, "O=D",
                      paleta::CYAN, paleta::TEXTO_OSCURO);
    } else if (esOrigen) {
      dibujarInsignia(sec.x + 14.0f, sec.y - radio - 6.0f, 22.0f, 15.0f, "O",
                      paleta::CYAN, paleta::TEXTO_OSCURO);
    } else if (esDestino) {
      dibujarInsignia(sec.x + 14.0f, sec.y - radio - 6.0f, 22.0f, 15.0f, "D",
                      paleta::ALERTA, paleta::TEXTO_OSCURO);
    }

    if (afectadoCiclo) {
      dibujarInsignia(sec.x - 22.0f, sec.y - radio - 16.0f, 44.0f, 15.0f,
                      "-INF", paleta::PELIGRO);
    }
  }

  // Barra inspectora de lista de adyacencia en tiempo real (al pasar el raton por un nodo).
  int idInspeccion = app.m4.nodoOrigen;
  for (const auto& sec : app.sectores) {
    if (std::hypot(ratonX - sec.x, ratonY - sec.y) <= (radio + 4.0f)) {
      idInspeccion = sec.id;
      break;
    }
  }
  const dominio::Sector* secInsp = app.obtenerSector(idInspeccion);
  const ::Nodo* nodoInsp = app.grafoUrbano.buscarNodo(idInspeccion);
  std::string txtAdy = "Salidas: ninguna";
  if (nodoInsp != nullptr && !nodoInsp->getAdyacentes().empty()) {
    txtAdy = "Salidas: ";
    const auto& ady = nodoInsp->getAdyacentes();
    for (std::size_t i = 0; i < ady.size(); ++i) {
      if (i > 0) {
        txtAdy += ", ";
      }
      const int p = ady[i].second;
      txtAdy += "->(" + std::to_string(ady[i].first->getValor()) + ")[" +
                (p >= 0 ? "+" + std::to_string(p) : std::to_string(p)) + "]";
    }
  }
  establecerColor(paleta::PANEL_OSCURO);
  dibujarRectanguloLleno(panelGrafo.x + 10.0f,
                         panelGrafo.y + panelGrafo.h - 32.0f,
                         panelGrafo.x + panelGrafo.w - 10.0f,
                         panelGrafo.y + panelGrafo.h - 6.0f);
  establecerColor(paleta::BORDE);
  dibujarRectangulo(panelGrafo.x + 10.0f, panelGrafo.y + panelGrafo.h - 32.0f,
                    panelGrafo.x + panelGrafo.w - 10.0f,
                    panelGrafo.y + panelGrafo.h - 6.0f);
  establecerColor(paleta::CYAN);
  const std::string cabInsp =
      "Inspector Nodo (" + std::to_string(idInspeccion) + ") " +
      (secInsp != nullptr ? secInsp->nombre : "") + " | ";
  dibujarTextoAcotado(panelGrafo.x + 18.0f,
                      panelGrafo.y + panelGrafo.h - 25.0f,
                      panelGrafo.w - 36.0f, cabInsp + txtAdy);

  // 2) Panel derecho: Controles de seleccion, modo de red, algoritmos y resultado.
  const RectanguloUI panelDer{panelGrafo.x + panelGrafo.w + 12.0f,
                              layout::AREA_Y_INICIO, 364.0f, 532.0f};
  dibujarPanel(panelDer, "ENRUTAMIENTO URBANO");

  const dominio::Sector* secO = app.obtenerSector(app.m4.nodoOrigen);
  const dominio::Sector* secD = app.obtenerSector(app.m4.nodoDestino);

  establecerColor(paleta::CYAN);
  dibujarTextoAcotado(
      798.0f, 134.0f, 332.0f,
      "Origen:  (" + std::to_string(app.m4.nodoOrigen) + ") " +
          (secO != nullptr ? secO->nombre : "-"));
  establecerColor(paleta::ALERTA);
  dibujarTextoAcotado(
      798.0f, 156.0f, 332.0f,
      "Destino: (" + std::to_string(app.m4.nodoDestino) + ") " +
          (secD != nullptr ? secD->nombre : "-"));

  dibujarBoton(ctrl.btnFijarOrigen, ratonX, ratonY);
  dibujarBoton(ctrl.btnFijarDestino, ratonX, ratonY);
  dibujarBoton(ctrl.btnIntercambiar, ratonX, ratonY);

  establecerColor(paleta::BORDE);
  dibujarLinea(panelDer.x + 14.0f, 222.0f, panelDer.x + panelDer.w - 14.0f,
               222.0f);
  establecerColor(paleta::TEXTO_SECUNDARIO);
  dibujarTextoAcotado(798.0f, 228.0f, 332.0f, "Escenario de la red vial:");

  dibujarBoton(ctrl.btnModoBonif, ratonX, ratonY);
  dibujarBoton(ctrl.btnModoPositivo, ratonX, ratonY);
  dibujarBoton(ctrl.btnModoCicloNeg, ratonX, ratonY);

  establecerColor(paleta::BORDE);
  dibujarLinea(panelDer.x + 14.0f, 348.0f, panelDer.x + panelDer.w - 14.0f,
               348.0f);
  dibujarBoton(ctrl.btnDijkstra, ratonX, ratonY);
  dibujarBoton(ctrl.btnBellmanFord, ratonX, ratonY);

  // Tarjeta inferior derecha con resultado, pesos tramo a tramo, iteraciones y relajaciones.
  const RectanguloUI cajaRes{798.0f, 438.0f, 332.0f, 174.0f};
  dibujarPanel(cajaRes, "RESULTADO DE RUTA", paleta::PANEL_OSCURO);
  const float maxResW = cajaRes.w - 24.0f;

  if (!app.m4.tieneResultado) {
    establecerColor(paleta::TEXTO_SECUNDARIO);
    dibujarTextoAcotado(810.0f, 482.0f, maxResW,
                        "Haga clic en los nodos del mapa");
    dibujarTextoAcotado(810.0f, 504.0f, maxResW,
                        "para elegir Origen y Destino,");
    dibujarTextoAcotado(810.0f, 526.0f, maxResW,
                        "y ejecute Dijkstra o Bellman-Ford.");
  } else if (app.m4.resultado.tienePesosNegativos) {
    establecerColor(paleta::ALERTA);
    dibujarTextoAcotado(810.0f, 476.0f, maxResW,
                        "ALERTA: DIJKSTRA NO VALIDO");
    establecerColor(paleta::TEXTO_PRINCIPAL);
    dibujarTextoAcotado(810.0f, 500.0f, maxResW,
                        "El grafo tiene aristas con peso");
    dibujarTextoAcotado(810.0f, 520.0f, maxResW,
                        "negativo: 2->4 (-2) y 3->5 (-3).");
    establecerColor(paleta::EXITO);
    dibujarTextoAcotado(810.0f, 548.0f, maxResW,
                        "Sugerencia: use Bellman-Ford o");
    dibujarTextoAcotado(810.0f, 568.0f, maxResW,
                        "cambie a 'Red solo positiva'.");
  } else if (app.m4.resultado.hayCicloNegativo) {
    establecerColor(paleta::PELIGRO);
    dibujarTextoAcotado(810.0f, 476.0f, maxResW,
                        "CICLO NEGATIVO DETECTADO!");
    establecerColor(paleta::TEXTO_PRINCIPAL);
    dibujarTextoAcotado(810.0f, 500.0f, maxResW,
                        "Algoritmo: Bellman-Ford (Pasada V)");
    dibujarTextoAcotado(810.0f, 520.0f, maxResW,
                        "No existe minimo finito (-inf).");
    establecerColor(paleta::ALERTA);
    dibujarTextoAcotado(
        810.0f, 548.0f, maxResW,
        "Nodos (-inf): " + estado::EstadoApp::formatearListaIds(
                               app.m4.resultado.nodosAfectadosCiclo));
    establecerColor(paleta::TEXTO_SECUNDARIO);
    dibujarTextoAcotado(
        810.0f, 574.0f, maxResW,
        "Iteraciones: " + std::to_string(app.m4.resultado.iteraciones) +
            " | Relajaciones: " +
            std::to_string(app.m4.resultado.relajaciones));
  } else if (app.m4.resultado.inalcanzable) {
    establecerColor(paleta::ALERTA);
    dibujarTextoAcotado(810.0f, 480.0f, maxResW, "DESTINO INALCANZABLE");
    establecerColor(paleta::TEXTO_PRINCIPAL);
    dibujarTextoAcotado(810.0f, 504.0f, maxResW,
                        "Algoritmo: " + app.m4.algoritmoUsado);
    dibujarTextoAcotado(810.0f, 526.0f, maxResW,
                        "No hay camino dirigido de (" +
                            std::to_string(app.m4.nodoOrigen) + ") a (" +
                            std::to_string(app.m4.nodoDestino) + ").");
    establecerColor(paleta::TEXTO_SECUNDARIO);
    dibujarTextoAcotado(810.0f, 554.0f, maxResW, "Distancia = +infinito");
  } else if (app.m4.resultado.exito) {
    establecerColor(paleta::EXITO);
    dibujarTextoAcotado(810.0f, 474.0f, maxResW,
                        app.m4.algoritmoUsado + " | Costo Total: " +
                            std::to_string(app.m4.resultado.costoTotal));

    // Construye el desglose con pesos de cada tramo, ej: 1(+4)->2(-2)->4(+5)->8.
    std::string seqTramos;
    for (std::size_t i = 0; i < app.m4.resultado.camino.size(); ++i) {
      if (i > 0) {
        if (i - 1 < app.m4.resultado.pesosTramos.size()) {
          const int p = app.m4.resultado.pesosTramos[i - 1];
          seqTramos +=
              "(" + (p >= 0 ? "+" + std::to_string(p) : std::to_string(p)) +
              ")->";
        } else {
          seqTramos += "->";
        }
      }
      seqTramos += std::to_string(app.m4.resultado.camino[i]);
    }
    establecerColor(paleta::CYAN);
    dibujarTextoAcotado(810.0f, 498.0f, maxResW, seqTramos);

    // Divide la secuencia de nombres de sectores en hasta 2 lineas legibles.
    const std::string nombres =
        app.formatearSecuenciaSectores(app.m4.resultado.camino);
    establecerColor(paleta::TEXTO_PRINCIPAL);
    const std::size_t maxCharsLinea =
        static_cast<std::size_t>(maxResW / 8.0f);
    if (nombres.size() <= maxCharsLinea) {
      dibujarTextoAcotado(810.0f, 522.0f, maxResW, nombres);
    } else {
      dibujarTexto(810.0f, 522.0f, nombres.substr(0, maxCharsLinea));
      dibujarTextoAcotado(810.0f, 542.0f, maxResW,
                          nombres.substr(maxCharsLinea));
    }

    establecerColor(paleta::TEXTO_SECUNDARIO);
    dibujarTextoAcotado(
        810.0f, 578.0f, maxResW,
        "Tramos: " +
            std::to_string(app.m4.resultado.camino.empty()
                               ? 0
                               : app.m4.resultado.camino.size() - 1) +
            " | Iter: " + std::to_string(app.m4.resultado.iteraciones) +
            " | Relaj: " + std::to_string(app.m4.resultado.relajaciones));
  }
}

} // namespace

// Funcion maestra de renderizado llamada en cada iteracion del bucle principal.
void renderizarAplicacion(estado::EstadoApp& app, float ratonX, float ratonY) {
  limpiarPantalla();

  establecerColor(paleta::FONDO_VENTANA);
  dibujarRectanguloLleno(0.0f, 0.0f, static_cast<float>(layout::ANCHO_VENTANA),
                         static_cast<float>(layout::ALTO_VENTANA));

  dibujarBarraSuperior(app, ratonX, ratonY);

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
