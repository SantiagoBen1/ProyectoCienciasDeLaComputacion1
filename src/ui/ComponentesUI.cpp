#include "ComponentesUI.hpp"

#include <cmath>

namespace ui {

namespace {

constexpr float ANCHO_CARACTER = 7.6f;
constexpr float ALTO_TEXTO = 14.0f;

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

std::string recortarTexto(const std::string& texto, std::size_t maxChars) {
  if (texto.size() <= maxChars) {
    return texto;
  }
  if (maxChars <= 2) {
    return texto.substr(0, maxChars);
  }
  return texto.substr(0, maxChars - 2) + "..";
}

float anchoTextoAprox(const std::string& texto) {
  return static_cast<float>(texto.size()) * ANCHO_CARACTER;
}

void dibujarPanel(const RectanguloUI& rect, const std::string& titulo,
                  const ColorRGB& fondo) {
  establecerColor(fondo);
  dibujarRectanguloLleno(rect.x, rect.y, rect.x + rect.w, rect.y + rect.h);

  if (!titulo.empty()) {
    const float altoCabecera = 28.0f;
    establecerColor(paleta::PANEL_HEADER);
    dibujarRectanguloLleno(rect.x, rect.y, rect.x + rect.w,
                           rect.y + altoCabecera);
    establecerColor(paleta::BORDE);
    dibujarLinea(rect.x, rect.y + altoCabecera, rect.x + rect.w,
                 rect.y + altoCabecera);
    establecerColor(paleta::TEXTO_PRINCIPAL);
    dibujarTexto(rect.x + 10.0f, rect.y + 7.0f, titulo);
  }

  establecerColor(paleta::BORDE);
  dibujarRectangulo(rect.x, rect.y, rect.x + rect.w, rect.y + rect.h);
}

void dibujarInsignia(float x, float y, float w, float h,
                     const std::string& texto, const ColorRGB& fondo,
                     const ColorRGB& colorTexto) {
  establecerColor(fondo);
  dibujarRectanguloLleno(x, y, x + w, y + h);
  establecerColor(paleta::BORDE_CLARO);
  dibujarRectangulo(x, y, x + w, y + h);

  const float tx = x + (w - anchoTextoAprox(texto)) * 0.5f;
  const float ty = y + (h - ALTO_TEXTO) * 0.5f;
  establecerColor(colorTexto);
  dibujarTexto(tx < x + 4.0f ? x + 4.0f : tx, ty, texto);
}

void dibujarBoton(const Boton& btn, float ratonX, float ratonY) {
  const bool hover = btn.habilitado && btn.rect.contiene(ratonX, ratonY);
  const ColorRGB colFondo = fondoBoton(btn, hover);

  establecerColor(colFondo);
  dibujarRectanguloLleno(btn.rect.x, btn.rect.y, btn.rect.x + btn.rect.w,
                         btn.rect.y + btn.rect.h);

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

  const float tx =
      btn.rect.x + (btn.rect.w - anchoTextoAprox(btn.etiqueta)) * 0.5f;
  const float ty = btn.rect.y + (btn.rect.h - ALTO_TEXTO) * 0.5f;
  establecerColor(colTexto);
  dibujarTexto(tx < btn.rect.x + 6.0f ? btn.rect.x + 6.0f : tx, ty,
               btn.etiqueta);
}

bool clicEnBoton(const Boton& btn, const EntradaUsuario& entrada) {
  return btn.habilitado && entrada.clicIzq &&
         btn.rect.contiene(entrada.ratonX, entrada.ratonY);
}

RectanguloUI rectBotonBorrarCaja(const CajaTexto& caja) {
  const float anchoBorrar = 26.0f;
  return {caja.rect.x + caja.rect.w - anchoBorrar, caja.rect.y, anchoBorrar,
          caja.rect.h};
}

void dibujarCajaTexto(const CajaTexto& caja, float ratonX, float ratonY) {
  if (!caja.etiqueta.empty()) {
    establecerColor(paleta::TEXTO_SECUNDARIO);
    dibujarTexto(caja.rect.x, caja.rect.y - 16.0f, caja.etiqueta);
  }

  const bool enfocada = (caja.enfocada != nullptr && *caja.enfocada);
  const RectanguloUI rBorrar = rectBotonBorrarCaja(caja);
  const float finCampo = rBorrar.x;

  establecerColor(paleta::PANEL_OSCURO);
  dibujarRectanguloLleno(caja.rect.x, caja.rect.y, finCampo,
                         caja.rect.y + caja.rect.h);
  establecerColor(enfocada ? paleta::CYAN : paleta::BORDE_CLARO);
  dibujarRectangulo(caja.rect.x, caja.rect.y, finCampo,
                    caja.rect.y + caja.rect.h);

  std::string textoMostrado = (caja.valor != nullptr) ? *caja.valor : "";
  if (enfocada) {
    textoMostrado += "_";
  }
  const std::size_t maxVisibles =
      static_cast<std::size_t>((finCampo - caja.rect.x - 10.0f) / ANCHO_CARACTER);
  if (textoMostrado.size() > maxVisibles && maxVisibles > 0) {
    textoMostrado = textoMostrado.substr(textoMostrado.size() - maxVisibles);
  }

  establecerColor(paleta::TEXTO_PRINCIPAL);
  dibujarTexto(caja.rect.x + 6.0f,
               caja.rect.y + (caja.rect.h - ALTO_TEXTO) * 0.5f, textoMostrado);

  // Boton [<] integrado para borrar el ultimo caracter con el raton.
  const bool hoverBorrar = rBorrar.contiene(ratonX, ratonY);
  establecerColor(hoverBorrar ? paleta::SECUNDARIO_HOVER : paleta::SECUNDARIO);
  dibujarRectanguloLleno(rBorrar.x, rBorrar.y, rBorrar.x + rBorrar.w,
                         rBorrar.y + rBorrar.h);
  establecerColor(paleta::BORDE_CLARO);
  dibujarRectangulo(rBorrar.x, rBorrar.y, rBorrar.x + rBorrar.w,
                    rBorrar.y + rBorrar.h);
  establecerColor(paleta::TEXTO_PRINCIPAL);
  dibujarTexto(rBorrar.x + 8.0f,
               rBorrar.y + (rBorrar.h - ALTO_TEXTO) * 0.5f, "<");
}

bool manejarEventoCajaTexto(const CajaTexto& caja,
                            const EntradaUsuario& entrada) {
  if (caja.valor == nullptr || caja.enfocada == nullptr) {
    return false;
  }

  bool huboCambio = false;
  const RectanguloUI rBorrar = rectBotonBorrarCaja(caja);

  if (entrada.clicIzq) {
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

  for (const EventoTecla& ev : entrada.teclas) {
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

void dibujarTabla(const TablaDatos& tabla) {
  const float altoCabecera = 26.0f;

  establecerColor(paleta::PANEL_OSCURO);
  dibujarRectanguloLleno(tabla.rect.x, tabla.rect.y,
                         tabla.rect.x + tabla.rect.w,
                         tabla.rect.y + tabla.rect.h);

  // Cabecera
  establecerColor(paleta::PANEL_HEADER);
  dibujarRectanguloLleno(tabla.rect.x, tabla.rect.y,
                         tabla.rect.x + tabla.rect.w,
                         tabla.rect.y + altoCabecera);

  float cursorX = tabla.rect.x;
  for (std::size_t c = 0; c < tabla.columnas.size(); ++c) {
    establecerColor(paleta::TEXTO_PRINCIPAL);
    dibujarTexto(cursorX + 8.0f, tabla.rect.y + 6.0f, tabla.columnas[c].titulo);
    if (c > 0) {
      establecerColor(paleta::BORDE);
      dibujarLinea(cursorX, tabla.rect.y, cursorX,
                   tabla.rect.y + tabla.rect.h);
    }
    cursorX += tabla.columnas[c].ancho;
  }

  // Filas
  float cursorY = tabla.rect.y + altoCabecera;
  for (std::size_t r = 0; r < tabla.filas.size(); ++r) {
    if (cursorY + tabla.altoFila > tabla.rect.y + tabla.rect.h + 1.0f) {
      break;
    }

    ColorRGB fondoFila =
        (r % 2 == 0) ? paleta::FILA_PAR : paleta::FILA_IMPAR;
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

    cursorX = tabla.rect.x;
    for (std::size_t c = 0; c < tabla.columnas.size(); ++c) {
      if (c < tabla.filas[r].size()) {
        const std::size_t maxChars = static_cast<std::size_t>(
            (tabla.columnas[c].ancho - 12.0f) / ANCHO_CARACTER);
        establecerColor(paleta::TEXTO_PRINCIPAL);
        dibujarTexto(cursorX + 8.0f,
                     cursorY + (tabla.altoFila - ALTO_TEXTO) * 0.5f,
                     recortarTexto(tabla.filas[r][c], maxChars));
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
  dibujarTexto(92.0f, y + 11.0f,
               "TRAZABILIDAD | " + recortarTexto(log.titulo, 95));

  establecerColor(paleta::TEXTO_SECUNDARIO);
  dibujarTexto(14.0f, y + 36.0f,
               "Antes:   " + recortarTexto(log.antes, 114));

  establecerColor(paleta::TEXTO_PRINCIPAL);
  dibujarTexto(14.0f, y + 57.0f,
               "Despues: " + recortarTexto(log.despues, 114));
}

} // namespace ui
