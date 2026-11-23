#pragma once

namespace ui {

struct ColorRGB {
  int r = 255;
  int g = 255;
  int b = 255;
};

namespace layout {
constexpr int ANCHO_VENTANA = 1000;
constexpr int ALTO_VENTANA = 650;

constexpr float ALTO_BARRA_SUPERIOR = 56.0f;
constexpr float Y_BARRA_ESTADO = 566.0f;
constexpr float ALTO_BARRA_ESTADO = ALTO_VENTANA - Y_BARRA_ESTADO;

constexpr float MARGEN = 14.0f;
constexpr float AREA_Y_INICIO = ALTO_BARRA_SUPERIOR + 10.0f;
constexpr float AREA_Y_FIN = Y_BARRA_ESTADO - 10.0f;
} // namespace layout

namespace paleta {
constexpr ColorRGB FONDO_VENTANA{15, 23, 42};
constexpr ColorRGB BARRA_SUPERIOR{20, 30, 50};
constexpr ColorRGB PANEL_FONDO{30, 41, 59};
constexpr ColorRGB PANEL_HEADER{38, 52, 74};
constexpr ColorRGB PANEL_OSCURO{22, 31, 48};
constexpr ColorRGB BORDE{60, 76, 102};
constexpr ColorRGB BORDE_CLARO{100, 120, 150};

constexpr ColorRGB TEXTO_PRINCIPAL{241, 245, 249};
constexpr ColorRGB TEXTO_SECUNDARIO{155, 172, 196};
constexpr ColorRGB TEXTO_OSCURO{15, 23, 42};

constexpr ColorRGB PRIMARIO{37, 99, 235};
constexpr ColorRGB PRIMARIO_HOVER{59, 130, 246};
constexpr ColorRGB SECUNDARIO{51, 65, 85};
constexpr ColorRGB SECUNDARIO_HOVER{71, 85, 105};

constexpr ColorRGB EXITO{16, 185, 129};
constexpr ColorRGB EXITO_HOVER{34, 197, 145};
constexpr ColorRGB EXITO_FONDO{18, 64, 54};

constexpr ColorRGB ALERTA{245, 158, 11};
constexpr ColorRGB ALERTA_HOVER{251, 180, 45};
constexpr ColorRGB ALERTA_FONDO{78, 52, 16};

constexpr ColorRGB PELIGRO{220, 38, 38};
constexpr ColorRGB PELIGRO_HOVER{239, 68, 68};
constexpr ColorRGB PELIGRO_FONDO{76, 26, 34};

constexpr ColorRGB CYAN{6, 182, 212};
constexpr ColorRGB MAGENTA_NEG{232, 80, 232};
constexpr ColorRGB FILA_PAR{26, 36, 54};
constexpr ColorRGB FILA_IMPAR{33, 45, 66};
constexpr ColorRGB FILA_RESALTADA{24, 88, 72};
constexpr ColorRGB DESHABILITADO{42, 52, 68};
} // namespace paleta

} // namespace ui
