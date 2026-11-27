// ============================================================================
// Archivo: src/ui/Paleta.hpp
// Capa: Capa C — Interfaz Grafica (constantes de diseño y paleta de colores).
// Proposito: Centraliza las dimensiones de la ventana (1160x720), las alturas
//            de barras fijas (superior, guia contextual y barra de estado) y
//            la paleta RGB consistente para toda la aplicacion.
// ============================================================================

#pragma once

namespace ui {

// Representacion RGB (0..255 por canal) desacoplada de tipos de MINIWIN/Win32.
struct ColorRGB {
  int r = 255;
  int g = 255;
  int b = 255;
};

// Constantes estructurales de distribucion de pantalla (ventana 1160x720).
namespace layout {
constexpr int ANCHO_VENTANA = 1160;
constexpr int ALTO_VENTANA = 720;

constexpr float ALTO_BARRA_SUPERIOR = 52.0f; // Titulo, Reset [F5] y 4 pestanas
constexpr float ALTO_BARRA_GUIA = 32.0f;     // Sub-barra guia contextual + KPIs
constexpr float Y_BARRA_ESTADO = 636.0f;     // Inicio Y de la barra de trazabilidad
constexpr float ALTO_BARRA_ESTADO = ALTO_VENTANA - Y_BARRA_ESTADO;

constexpr float MARGEN = 14.0f;
constexpr float AREA_Y_INICIO = ALTO_BARRA_SUPERIOR + ALTO_BARRA_GUIA + 10.0f;
constexpr float AREA_Y_FIN = Y_BARRA_ESTADO - 10.0f;
} // namespace layout

// Paleta de colores oscuros con alto contraste para legibilidad en proyeccion.
namespace paleta {
// Fondos y superficies de paneles
constexpr ColorRGB FONDO_VENTANA{13, 20, 36};
constexpr ColorRGB BARRA_SUPERIOR{18, 27, 46};
constexpr ColorRGB PANEL_FONDO{27, 38, 56};
constexpr ColorRGB PANEL_HEADER{35, 49, 72};
constexpr ColorRGB PANEL_OSCURO{19, 28, 44};
constexpr ColorRGB PANEL_SOMBRA{8, 13, 24};
constexpr ColorRGB CAMPO_ACTIVO{18, 42, 68};
constexpr ColorRGB BORDE{56, 73, 99};
constexpr ColorRGB BORDE_CLARO{96, 118, 148};

// Tipografia
constexpr ColorRGB TEXTO_PRINCIPAL{241, 245, 249};
constexpr ColorRGB TEXTO_SECUNDARIO{155, 172, 196};
constexpr ColorRGB TEXTO_OSCURO{15, 23, 42};

// Acciones y estados semanticos
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

// Acentos especiales (origen, aristas negativas, filas de tabla y deshabilitados)
constexpr ColorRGB CYAN{6, 182, 212};
constexpr ColorRGB MAGENTA_NEG{232, 80, 232};
constexpr ColorRGB FILA_PAR{24, 34, 52};
constexpr ColorRGB FILA_IMPAR{31, 43, 64};
constexpr ColorRGB FILA_HOVER{42, 62, 92};
constexpr ColorRGB FILA_RESALTADA{20, 92, 74};
constexpr ColorRGB DESHABILITADO{42, 52, 68};
} // namespace paleta

} // namespace ui
