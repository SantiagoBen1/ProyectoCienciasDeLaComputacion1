// ============================================================================
// Archivo: src/ui/ComponentesUI.hpp
// Capa: Capa C — Biblioteca de Componentes Reutilizables de Interfaz.
// Proposito: Como MINIWIN carece de widgets nativos, declara componentes
//            reutilizables construidos sobre primitivas 2D: paneles, botones,
//            insignias, cajas de texto con botones [<] y [X], flechas
//            direccionales, tablas con ventana deslizante y barra de estado.
// ============================================================================

#pragma once

#include "MiniWinCapa.hpp"
#include "Paleta.hpp"
#include "../estado/Modelos.hpp"

#include <cstddef>
#include <string>
#include <vector>

namespace ui {

// Caja delimitadora rectangular reutilizada para dibujo y deteccion de clics (hit-test).
struct RectanguloUI {
  float x = 0.0f;
  float y = 0.0f;
  float w = 0.0f;
  float h = 0.0f;

  // Retorna true si el punto (px, py) cae dentro del rectangulo (bordes incluidos).
  bool contiene(float px, float py) const {
    return px >= x && px <= (x + w) && py >= y && py <= (y + h);
  }
};

// Variantes visuales semanticas de un boton.
enum class EstiloBoton {
  Primario,
  Secundario,
  Exito,
  Alerta,
  Peligro,
  Pestana
};

// Descriptor de un boton clicable con estado de habilitacion y seleccion activa.
struct Boton {
  RectanguloUI rect;
  std::string etiqueta;
  EstiloBoton estilo = EstiloBoton::Primario;
  bool habilitado = true;
  bool activo = false;
};

// Descriptor de un campo de entrada editable enlazado por puntero a EstadoApp.
struct CajaTexto {
  RectanguloUI rect;
  std::string etiqueta;
  std::string* valor = nullptr;   // Cadena editada en memoria
  bool* enfocada = nullptr;       // Bandera de foco activo
  bool soloNumeros = false;       // Si es true, restringe entrada a digitos '0'-'9'
  std::size_t maxLongitud = 20;
};

// Definicion de una columna dentro de TablaDatos.
struct ColumnaTabla {
  std::string titulo;
  float ancho = 80.0f;
};

// Modelo de datos y resaltados para el componente tabla (usado en Modulo 2).
struct TablaDatos {
  RectanguloUI rect;
  std::vector<ColumnaTabla> columnas;
  std::vector<std::vector<std::string>> filas;
  int filaResaltada = -1;     // Fila encontrada/seleccionada (verde)
  int filaSecundaria = -1;    // Resaltado secundario opcional (ambar)
  int rangoMergeInicio = -1;  // Inicio del subarreglo en animacion Merge Sort
  int rangoMergeMitad = -1;   // Punto medio (azul izquierda / ambar derecha)
  int rangoMergeFin = -1;     // Fin exclusivo del subarreglo en mezcla
  float altoFila = 24.0f;
};

// --- Utilidades de medicion y recorte de texto (fuente Consolas 14px = 8px/char) ---
std::string recortarTexto(const std::string& texto, std::size_t maxChars);
std::string recortarTextoPorAncho(const std::string& texto, float maxAnchoPx);
float anchoTextoAprox(const std::string& texto);
void dibujarTextoAcotado(float x, float y, float maxAnchoPx,
                         const std::string& texto);

// --- Componentes visuales e interactivos reutilizables ---
void dibujarPanel(const RectanguloUI& rect, const std::string& titulo = "",
                  const ColorRGB& fondo = paleta::PANEL_FONDO);
void dibujarInsignia(float x, float y, float w, float h,
                     const std::string& texto, const ColorRGB& fondo,
                     const ColorRGB& colorTexto = paleta::TEXTO_PRINCIPAL);

void dibujarBoton(const Boton& btn, float ratonX, float ratonY);
bool clicEnBoton(const Boton& btn, const EntradaUsuario& entrada);

RectanguloUI rectBotonBorrarCaja(const CajaTexto& caja);
RectanguloUI rectBotonLimpiarCaja(const CajaTexto& caja);
void dibujarCajaTexto(const CajaTexto& caja, float ratonX, float ratonY);
bool manejarEventoCajaTexto(const CajaTexto& caja,
                            const EntradaUsuario& entrada);

void dibujarFlecha(float x1, float y1, float x2, float y2, const ColorRGB& col,
                   float tamPunta = 9.0f, bool gruesa = false);
void dibujarTabla(const TablaDatos& tabla, float ratonX = -1.0f,
                  float ratonY = -1.0f);
void dibujarBarraEstado(const dominio::LogTrazabilidad& log);

} // namespace ui
