// ============================================================================
// Archivo: src/ui/ComponentesUI.cpp
// Capa: Capa C — Biblioteca de Componentes Reutilizables de Interfaz.
// Proposito: Implementa el renderizado y manejo de eventos de todos los
//            componentes graficos (paneles con sombra, botones con hover,
//            cajas de texto con botones [<] y [X], flechas vectoriales, tablas
//            con scroll automatico y barra inferior de trazabilidad).
// ============================================================================

#include "ComponentesUI.hpp"

#include <cmath>

namespace ui {

namespace {

// Metricas de la fuente monoespaciada Consolas (-14px) configurada en miniwin.cpp.
constexpr float ANCHO_CARACTER = 8.0f;
constexpr float ALTO_TEXTO = 14.0f;

// Resuelve el color de fondo del boton segun su estilo, habilitacion, foco y hover.
ColorRGB fondoBoton(const Boton& btn, bool hover) {
  if (!btn.habilitado) {
    return paleta::DESHABILITADO;
  }
  switch (btn.estilo) {
    case EstiloBoton::Primario:
      return (hover || btn.activo) ? paleta::PRIMARIO_HOVER : paleta::PRIMARIO;
    case EstiloBoton::Secundario:
      return btn.activo ? paleta::PRIMARIO
                        : (hover ? paleta::SECUNDARIO_HOVER
                                 : paleta::SECUNDARIO);
    case EstiloBoton::Exito:
      return (hover || btn.activo) ? paleta::EXITO_HOVER : paleta::EXITO;
    case EstiloBoton::Alerta:
      return (hover || btn.activo) ? paleta::ALERTA_HOVER : paleta::ALERTA;
    case EstiloBoton::Peligro:
      return (hover || btn.activo) ? paleta::PELIGRO_HOVER : paleta::PELIGRO;
    case EstiloBoton::Pestana:
      if (btn.activo) {
        return paleta::PRIMARIO;
      }
      return hover ? paleta::SECUNDARIO_HOVER : paleta::PANEL_OSCURO;
  }
  return paleta::SECUNDARIO;
}

} // namespace

// Recorta una cadena a 'maxChars' caracteres agregando ".." si excede el limite.
std::string recortarTexto(const std::string& texto, std::size_t maxChars) {
  if (texto.size() <= maxChars) {
    return texto;
  }
  if (maxChars <= 2) {
    return texto.substr(0, maxChars);
  }
  return texto.substr(0, maxChars - 2) + "..";
}

// Calcula cuantos caracteres caben en 'maxAnchoPx' y recorta el texto para evitar desbordes.
std::string recortarTextoPorAncho(const std::string& texto, float maxAnchoPx) {
  if (maxAnchoPx <= 4.0f) {
    return "";
  }
  const std::size_t maxChars =
      static_cast<std::size_t>(maxAnchoPx / ANCHO_CARACTER);
  return recortarTexto(texto, maxChars);
}

float anchoTextoAprox(const std::string& texto) {
  return static_cast<float>(texto.size()) * ANCHO_CARACTER;
}

// Dibuja texto garantizando que nunca sobresalga del ancho maximo permitido.
void dibujarTextoAcotado(float x, float y, float maxAnchoPx,
                         const std::string& texto) {
  if (maxAnchoPx <= 4.0f || texto.empty()) {
    return;
  }
  dibujarTexto(x, y, recortarTextoPorAncho(texto, maxAnchoPx));
}

// Dibuja un contenedor de panel con sombra sutil, cabecera opcional con barra
// de acento cyan a la izquierda y borde perimetral.
void dibujarPanel(const RectanguloUI& rect, const std::string& titulo,
                  const ColorRGB& fondo) {
  // Sombra sutil inferior-derecha para dar profundidad visual.
  establecerColor(paleta::PANEL_SOMBRA);
  dibujarRectanguloLleno(rect.x + 2.0f, rect.y + 2.0f, rect.x + rect.w + 2.0f,
                         rect.y + rect.h + 2.0f);

  establecerColor(fondo);
  dibujarRectanguloLleno(rect.x, rect.y, rect.x + rect.w, rect.y + rect.h);

  if (!titulo.empty()) {
    const float altoCabecera = 28.0f;
    establecerColor(paleta::PANEL_HEADER);
    dibujarRectanguloLleno(rect.x, rect.y, rect.x + rect.w,
                           rect.y + altoCabecera);
    // Franja lateral cyan de acento en la cabecera.
    establecerColor(paleta::CYAN);
    dibujarRectanguloLleno(rect.x, rect.y, rect.x + 4.0f,
                           rect.y + altoCabecera);
    establecerColor(paleta::BORDE);
    dibujarLinea(rect.x, rect.y + altoCabecera, rect.x + rect.w,
                 rect.y + altoCabecera);
    establecerColor(paleta::TEXTO_PRINCIPAL);
    dibujarTextoAcotado(rect.x + 12.0f, rect.y + 7.0f, rect.w - 20.0f, titulo);
  }

  establecerColor(paleta::BORDE);
  dibujarRectangulo(rect.x, rect.y, rect.x + rect.w, rect.y + rect.h);
}

// Dibuja una pildora/etiqueta compacta con texto centrado y recorte automatico.
void dibujarInsignia(float x, float y, float w, float h,
                     const std::string& texto, const ColorRGB& fondo,
                     const ColorRGB& colorTexto) {
  establecerColor(fondo);
  dibujarRectanguloLleno(x, y, x + w, y + h);
  establecerColor(paleta::BORDE_CLARO);
  dibujarRectangulo(x, y, x + w, y + h);

  const std::string ajustado = recortarTextoPorAncho(texto, w - 6.0f);
  const float tx = x + (w - anchoTextoAprox(ajustado)) * 0.5f;
  const float ty = y + (h - ALTO_TEXTO) * 0.5f;
  establecerColor(colorTexto);
  dibujarTexto(tx < x + 3.0f ? x + 3.0f : tx, ty, ajustado);
}

// Dibuja un boton con feedback de hover, indicador de estado activo (linea
// inferior en pestanas o barra lateral en toggles) y texto centrado.
void dibujarBoton(const Boton& btn, float ratonX, float ratonY) {
  const bool hover = btn.habilitado && btn.rect.contiene(ratonX, ratonY);
  const ColorRGB colFondo = fondoBoton(btn, hover);

  establecerColor(colFondo);
  dibujarRectanguloLleno(btn.rect.x, btn.rect.y, btn.rect.x + btn.rect.w,
                         btn.rect.y + btn.rect.h);

  if (btn.estilo == EstiloBoton::Pestana && btn.activo) {
    establecerColor(paleta::CYAN);
    dibujarRectanguloLleno(btn.rect.x, btn.rect.y + btn.rect.h - 3.0f,
                           btn.rect.x + btn.rect.w, btn.rect.y + btn.rect.h);
  } else if (btn.activo) {
    establecerColor(paleta::CYAN);
    dibujarRectanguloLleno(btn.rect.x, btn.rect.y, btn.rect.x + 4.0f,
                           btn.rect.y + btn.rect.h);
  }

  establecerColor(btn.activo ? paleta::CYAN
                             : (hover ? paleta::TEXTO_PRINCIPAL
                                      : paleta::BORDE_CLARO));
  dibujarRectangulo(btn.rect.x, btn.rect.y, btn.rect.x + btn.rect.w,
                    btn.rect.y + btn.rect.h);

  ColorRGB colTexto = btn.habilitado ? paleta::TEXTO_PRINCIPAL
                                     : paleta::TEXTO_SECUNDARIO;
  if (btn.habilitado && btn.estilo == EstiloBoton::Alerta) {
    colTexto = paleta::TEXTO_OSCURO;
  }

  const std::string ajustado =
      recortarTextoPorAncho(btn.etiqueta, btn.rect.w - 10.0f);
  const float tx =
      btn.rect.x + (btn.rect.w - anchoTextoAprox(ajustado)) * 0.5f;
  const float ty = btn.rect.y + (btn.rect.h - ALTO_TEXTO) * 0.5f;
  establecerColor(colTexto);
  dibujarTexto(tx < btn.rect.x + 5.0f ? btn.rect.x + 5.0f : tx, ty, ajustado);
}

// Detecta si se produjo un clic izquierdo valido sobre un boton habilitado.
bool clicEnBoton(const Boton& btn, const EntradaUsuario& entrada) {
  return btn.habilitado && entrada.clicIzq &&
         btn.rect.contiene(entrada.ratonX, entrada.ratonY);
}

// Geometria del mini-boton [X] ubicado en el borde derecho de una CajaTexto.
RectanguloUI rectBotonLimpiarCaja(const CajaTexto& caja) {
  const float anchoBtn = 22.0f;
  return {caja.rect.x + caja.rect.w - anchoBtn, caja.rect.y, anchoBtn,
          caja.rect.h};
}

// Geometria del mini-boton [<] ubicado inmediatamente a la izquierda de [X].
RectanguloUI rectBotonBorrarCaja(const CajaTexto& caja) {
  const float anchoBtn = 22.0f;
  return {caja.rect.x + caja.rect.w - anchoBtn * 2.0f, caja.rect.y, anchoBtn,
          caja.rect.h};
}

// Dibuja la etiqueta, el campo de texto (con placeholder o cursor '|') y los
// botones integrados [<] (borrar 1 caracter) y [X] (vaciar campo).
void dibujarCajaTexto(const CajaTexto& caja, float ratonX, float ratonY) {
  const bool enfocada = (caja.enfocada != nullptr && *caja.enfocada);
  const bool hoverCaja = caja.rect.contiene(ratonX, ratonY);

  if (!caja.etiqueta.empty()) {
    establecerColor(enfocada ? paleta::CYAN : paleta::TEXTO_SECUNDARIO);
    dibujarTexto(caja.rect.x, caja.rect.y - 16.0f, caja.etiqueta);
  }

  const RectanguloUI rBorrar = rectBotonBorrarCaja(caja);
  const RectanguloUI rLimpiar = rectBotonLimpiarCaja(caja);
  const float finCampo = rBorrar.x;

  establecerColor(enfocada ? paleta::CAMPO_ACTIVO : paleta::PANEL_OSCURO);
  dibujarRectanguloLleno(caja.rect.x, caja.rect.y, finCampo,
                         caja.rect.y + caja.rect.h);
  if (enfocada) {
    establecerColor(paleta::CYAN);
    dibujarRectanguloLleno(caja.rect.x, caja.rect.y, caja.rect.x + 3.0f,
                           caja.rect.y + caja.rect.h);
  }
  establecerColor(enfocada ? paleta::CYAN
                           : (hoverCaja ? paleta::TEXTO_PRINCIPAL
                                        : paleta::BORDE_CLARO));
  dibujarRectangulo(caja.rect.x, caja.rect.y, finCampo,
                    caja.rect.y + caja.rect.h);

  const std::string valorReal = (caja.valor != nullptr) ? *caja.valor : "";
  const std::size_t maxVisibles =
      static_cast<std::size_t>((finCampo - caja.rect.x - 10.0f) / ANCHO_CARACTER);

  if (valorReal.empty() && !enfocada) {
    const std::string placeholder =
        maxVisibles >= 14 ? "Clic para editar" : "Editar..";
    establecerColor(paleta::TEXTO_SECUNDARIO);
    dibujarTexto(caja.rect.x + 6.0f,
                 caja.rect.y + (caja.rect.h - ALTO_TEXTO) * 0.5f,
                 recortarTexto(placeholder, maxVisibles));
  } else {
    // Muestra los ultimos caracteres visibles junto con el cursor '|' si esta enfocada.
    std::string textoMostrado = valorReal;
    if (enfocada) {
      textoMostrado += "|";
    }
    if (textoMostrado.size() > maxVisibles && maxVisibles > 0) {
      textoMostrado = textoMostrado.substr(textoMostrado.size() - maxVisibles);
    }
    establecerColor(paleta::TEXTO_PRINCIPAL);
    dibujarTexto(caja.rect.x + 6.0f,
                 caja.rect.y + (caja.rect.h - ALTO_TEXTO) * 0.5f,
                 textoMostrado);
  }

  // Boton [<] para borrar 1 caracter con el raton.
  const bool hoverBorrar = rBorrar.contiene(ratonX, ratonY);
  establecerColor(hoverBorrar ? paleta::SECUNDARIO_HOVER : paleta::SECUNDARIO);
  dibujarRectanguloLleno(rBorrar.x, rBorrar.y, rBorrar.x + rBorrar.w,
                         rBorrar.y + rBorrar.h);
  establecerColor(hoverBorrar ? paleta::TEXTO_PRINCIPAL : paleta::BORDE_CLARO);
  dibujarRectangulo(rBorrar.x, rBorrar.y, rBorrar.x + rBorrar.w,
                    rBorrar.y + rBorrar.h);
  establecerColor(paleta::TEXTO_PRINCIPAL);
  dibujarTexto(rBorrar.x + 7.0f,
               rBorrar.y + (rBorrar.h - ALTO_TEXTO) * 0.5f, "<");

  // Boton [X] para vaciar toda la caja de un solo clic.
  const bool hoverLimpiar = rLimpiar.contiene(ratonX, ratonY);
  establecerColor(hoverLimpiar ? paleta::PELIGRO_HOVER : paleta::PELIGRO_FONDO);
  dibujarRectanguloLleno(rLimpiar.x, rLimpiar.y, rLimpiar.x + rLimpiar.w,
                         rLimpiar.y + rLimpiar.h);
  establecerColor(hoverLimpiar ? paleta::TEXTO_PRINCIPAL : paleta::BORDE_CLARO);
  dibujarRectangulo(rLimpiar.x, rLimpiar.y, rLimpiar.x + rLimpiar.w,
                    rLimpiar.y + rLimpiar.h);
  establecerColor(paleta::TEXTO_PRINCIPAL);
  dibujarTexto(rLimpiar.x + 7.0f,
               rLimpiar.y + (rLimpiar.h - ALTO_TEXTO) * 0.5f, "X");
}

// Procesa foco, clic en [<]/[X], clic derecho para limpiar y escritura por teclado.
bool manejarEventoCajaTexto(const CajaTexto& caja,
                            const EntradaUsuario& entrada) {
  if (caja.valor == nullptr || caja.enfocada == nullptr) {
    return false;
  }

  bool huboCambio = false;
  const RectanguloUI rBorrar = rectBotonBorrarCaja(caja);
  const RectanguloUI rLimpiar = rectBotonLimpiarCaja(caja);

  // Clic derecho sobre [<] o [X] vacia rapidamente todo el campo.
  if (entrada.clicDer &&
      (rBorrar.contiene(entrada.ratonX, entrada.ratonY) ||
       rLimpiar.contiene(entrada.ratonX, entrada.ratonY))) {
    *caja.enfocada = true;
    if (!caja.valor->empty()) {
      caja.valor->clear();
      huboCambio = true;
    }
    return huboCambio;
  }

  if (entrada.clicIzq) {
    if (rLimpiar.contiene(entrada.ratonX, entrada.ratonY)) {
      *caja.enfocada = true;
      if (!caja.valor->empty()) {
        caja.valor->clear();
        huboCambio = true;
      }
      return huboCambio;
    }
    if (rBorrar.contiene(entrada.ratonX, entrada.ratonY)) {
      *caja.enfocada = true;
      if (!caja.valor->empty()) {
        caja.valor->pop_back();
        huboCambio = true;
      }
      return huboCambio;
    }
    *caja.enfocada = caja.rect.contiene(entrada.ratonX, entrada.ratonY);
  }

  if (!(*caja.enfocada)) {
    return false;
  }

  // Aplica teclas de edicion (Supr, Borrar/Izquierda y caracteres permitidos).
  for (const EventoTecla& ev : entrada.teclas) {
    if (ev.esLimpiar) {
      if (!caja.valor->empty()) {
        caja.valor->clear();
        huboCambio = true;
      }
      continue;
    }
    if (ev.esBorrar) {
      if (!caja.valor->empty()) {
        caja.valor->pop_back();
        huboCambio = true;
      }
      continue;
    }

    if (caja.valor->size() >= caja.maxLongitud) {
      continue;
    }

    if (caja.soloNumeros) {
      if (ev.caracter >= '0' && ev.caracter <= '9') {
        caja.valor->push_back(ev.caracter);
        huboCambio = true;
      }
    } else {
      if ((ev.caracter >= 'A' && ev.caracter <= 'Z') ||
          (ev.caracter >= '0' && ev.caracter <= '9') || ev.esEspacio) {
        caja.valor->push_back(ev.esEspacio ? ' ' : ev.caracter);
        huboCambio = true;
      }
    }
  }

  return huboCambio;
}

// Dibuja una flecha dirigida de (x1, y1) a (x2, y2) con punta triangular.
// Cuando 'gruesa' es true, traza dos lineas paralelas desplazadas +-1px en la normal.
void dibujarFlecha(float x1, float y1, float x2, float y2, const ColorRGB& col,
                   float tamPunta, bool gruesa) {
  establecerColor(col);
  dibujarLinea(x1, y1, x2, y2);

  const float dx = x2 - x1;
  const float dy = y2 - y1;
  const float largo = std::hypot(dx, dy);
  if (largo < 1.0f) {
    return;
  }

  if (gruesa) {
    const float nx = -dy / largo;
    const float ny = dx / largo;
    dibujarLinea(x1 + nx, y1 + ny, x2 + nx, y2 + ny);
    dibujarLinea(x1 - nx, y1 - ny, x2 - nx, y2 - ny);
  }

  // Calcula los dos vertices traseros de la punta rotando +-0.45 rad (~26 grados).
  const float angulo = std::atan2(dy, dx);
  const float apertura = 0.45f; // ~26 grados
  const float p1x = x2 - tamPunta * std::cos(angulo - apertura);
  const float p1y = y2 - tamPunta * std::sin(angulo - apertura);
  const float p2x = x2 - tamPunta * std::cos(angulo + apertura);
  const float p2y = y2 - tamPunta * std::sin(angulo + apertura);

  dibujarLinea(x2, y2, p1x, p1y);
  dibujarLinea(x2, y2, p2x, p2y);
  dibujarLinea(p1x, p1y, p2x, p2y);
}

// Renderiza una tabla completa con cabecera, alternancia de filas, hover,
// ventana deslizante automatica, resaltado de rangos Merge Sort y fila encontrada.
void dibujarTabla(const TablaDatos& tabla, float ratonX, float ratonY) {
  const float altoCabecera = 26.0f;

  establecerColor(paleta::PANEL_OSCURO);
  dibujarRectanguloLleno(tabla.rect.x, tabla.rect.y,
                         tabla.rect.x + tabla.rect.w,
                         tabla.rect.y + tabla.rect.h);

  // Cabecera con titulos de columnas en cyan y separadores verticales.
  establecerColor(paleta::PANEL_HEADER);
  dibujarRectanguloLleno(tabla.rect.x, tabla.rect.y,
                         tabla.rect.x + tabla.rect.w,
                         tabla.rect.y + altoCabecera);

  float cursorX = tabla.rect.x;
  for (std::size_t c = 0; c < tabla.columnas.size(); ++c) {
    establecerColor(paleta::CYAN);
    dibujarTexto(cursorX + 8.0f, tabla.rect.y + 6.0f, tabla.columnas[c].titulo);
    if (c > 0) {
      establecerColor(paleta::BORDE);
      dibujarLinea(cursorX, tabla.rect.y, cursorX,
                   tabla.rect.y + tabla.rect.h);
    }
    cursorX += tabla.columnas[c].ancho;
  }

  // Ventana deslizante: si existen mas filas que el alto visible (p. ej. tras
  // ingresar nuevos pedidos en Modulo 1), desplaza la vista para mostrar la fila resaltada.
  float cursorY = tabla.rect.y + altoCabecera;
  const int maxFilasVisibles =
      tabla.altoFila > 0.0f
          ? static_cast<int>((tabla.rect.h - altoCabecera + 1.0f) / tabla.altoFila)
          : static_cast<int>(tabla.filas.size());
  std::size_t filaInicio = 0;
  if (maxFilasVisibles > 0 &&
      static_cast<int>(tabla.filas.size()) > maxFilasVisibles &&
      tabla.filaResaltada >= maxFilasVisibles) {
    filaInicio = static_cast<std::size_t>(tabla.filaResaltada - maxFilasVisibles + 1);
  }

  for (std::size_t r = filaInicio; r < tabla.filas.size(); ++r) {
    if (cursorY + tabla.altoFila > tabla.rect.y + tabla.rect.h + 1.0f) {
      break;
    }

    const RectanguloUI rectFila{tabla.rect.x + 1.0f, cursorY,
                                tabla.rect.w - 2.0f, tabla.altoFila};
    const bool hoverFila = rectFila.contiene(ratonX, ratonY);

    // Prioridad de color de fondo: zebra -> hover -> rango Merge Sort -> secundaria -> encontrada.
    ColorRGB fondoFila =
        (r % 2 == 0) ? paleta::FILA_PAR : paleta::FILA_IMPAR;
    if (hoverFila) {
      fondoFila = paleta::FILA_HOVER;
    }
    const int idx = static_cast<int>(r);
    if (tabla.rangoMergeInicio >= 0 && idx >= tabla.rangoMergeInicio &&
        idx < tabla.rangoMergeFin) {
      fondoFila = (tabla.rangoMergeMitad >= 0 && idx < tabla.rangoMergeMitad)
                      ? ColorRGB{28, 62, 108}
                      : paleta::ALERTA_FONDO;
    }
    if (idx == tabla.filaSecundaria) {
      fondoFila = paleta::ALERTA_FONDO;
    }
    if (idx == tabla.filaResaltada) {
      fondoFila = paleta::FILA_RESALTADA;
    }

    establecerColor(fondoFila);
    dibujarRectanguloLleno(tabla.rect.x + 1.0f, cursorY,
                           tabla.rect.x + tabla.rect.w - 1.0f,
                           cursorY + tabla.altoFila);
    // Franja vertical verde en el borde izquierdo de la fila encontrada/seleccionada.
    if (idx == tabla.filaResaltada) {
      establecerColor(paleta::EXITO);
      dibujarRectanguloLleno(tabla.rect.x + 1.0f, cursorY,
                             tabla.rect.x + 5.0f, cursorY + tabla.altoFila);
    }

    // Dibuja cada celda aplicando color semantico a ID (col 1) y Prioridad (col 4).
    cursorX = tabla.rect.x;
    for (std::size_t c = 0; c < tabla.columnas.size(); ++c) {
      if (c < tabla.filas[r].size()) {
        const std::size_t maxChars = static_cast<std::size_t>(
            (tabla.columnas[c].ancho - 12.0f) / ANCHO_CARACTER);
        const std::string& celda = tabla.filas[r][c];
        ColorRGB colCelda = paleta::TEXTO_PRINCIPAL;
        if (c == 1) {
          colCelda = paleta::CYAN;
        } else if (c == 4) {
          if (celda.find("Medico") != std::string::npos) {
            colCelda = ColorRGB{252, 129, 129};
          } else if (celda.find("Urgente") != std::string::npos) {
            colCelda = paleta::ALERTA_HOVER;
          }
        }
        establecerColor(colCelda);
        dibujarTexto(cursorX + 8.0f,
                     cursorY + (tabla.altoFila - ALTO_TEXTO) * 0.5f,
                     recortarTexto(celda, maxChars));
      }
      if (c > 0) {
        establecerColor(paleta::BORDE);
        dibujarLinea(cursorX, cursorY, cursorX, cursorY + tabla.altoFila);
      }
      cursorX += tabla.columnas[c].ancho;
    }

    establecerColor(paleta::BORDE);
    dibujarLinea(tabla.rect.x, cursorY, tabla.rect.x + tabla.rect.w, cursorY);
    cursorY += tabla.altoFila;
  }

  establecerColor(paleta::BORDE_CLARO);
  dibujarRectangulo(tabla.rect.x, tabla.rect.y, tabla.rect.x + tabla.rect.w,
                    tabla.rect.y + tabla.rect.h);
}

// Dibuja la barra fija inferior de trazabilidad con insignia de severidad,
// guia de atajos rapidos y las lineas "Antes" / "Despues" de la ultima accion.
void dibujarBarraEstado(const dominio::LogTrazabilidad& log) {
  const float y = layout::Y_BARRA_ESTADO;
  const float w = static_cast<float>(layout::ANCHO_VENTANA);
  const float h = layout::ALTO_BARRA_ESTADO;

  ColorRGB colBadge = paleta::PRIMARIO;
  std::string txtBadge = "INFO";
  switch (log.nivel) {
    case dominio::NivelLog::Info:
      colBadge = paleta::PRIMARIO;
      txtBadge = "INFO";
      break;
    case dominio::NivelLog::Exito:
      colBadge = paleta::EXITO;
      txtBadge = "OK";
      break;
    case dominio::NivelLog::Alerta:
      colBadge = paleta::ALERTA;
      txtBadge = "ALERTA";
      break;
    case dominio::NivelLog::Error:
      colBadge = paleta::PELIGRO;
      txtBadge = "ERROR";
      break;
  }

  establecerColor(paleta::BARRA_SUPERIOR);
  dibujarRectanguloLleno(0.0f, y, w, y + h);
  establecerColor(colBadge);
  dibujarLinea(0.0f, y, w, y);
  dibujarLinea(0.0f, y + 1.0f, w, y + 1.0f);

  dibujarInsignia(14.0f, y + 8.0f, 68.0f, 20.0f, txtBadge, colBadge,
                  log.nivel == dominio::NivelLog::Alerta
                      ? paleta::TEXTO_OSCURO
                      : paleta::TEXTO_PRINCIPAL);

  establecerColor(paleta::TEXTO_PRINCIPAL);
  dibujarTextoAcotado(92.0f, y + 11.0f, 560.0f,
                      "TRAZABILIDAD | " + log.titulo);

  // Guia rapida de atajos de teclado en la esquina superior derecha.
  establecerColor(paleta::TEXTO_SECUNDARIO);
  dibujarTextoAcotado(
      668.0f, y + 11.0f, w - 680.0f,
      "[1-4]Modulo  [Tab]Campo  [X/Supr]Vaciar  [F5]Reset  [Esc]Salir");

  establecerColor(paleta::TEXTO_SECUNDARIO);
  dibujarTextoAcotado(14.0f, y + 36.0f, w - 28.0f, "Antes:   " + log.antes);

  establecerColor(paleta::TEXTO_PRINCIPAL);
  dibujarTextoAcotado(14.0f, y + 57.0f, w - 28.0f, "Despues: " + log.despues);
}

} // namespace ui
