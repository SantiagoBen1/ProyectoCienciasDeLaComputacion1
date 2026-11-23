#pragma once

#include "MiniWinCapa.hpp"
#include "Paleta.hpp"
#include "../estado/Modelos.hpp"

#include <cstddef>
#include <string>
#include <vector>

namespace ui {

struct RectanguloUI {
  float x = 0.0f;
  float y = 0.0f;
  float w = 0.0f;
  float h = 0.0f;

  bool contiene(float px, float py) const {
    return px >= x && px <= (x + w) && py >= y && py <= (y + h);
  }
};

enum class EstiloBoton {
  Primario,
  Secundario,
  Exito,
  Alerta,
  Peligro,
  Pestana
};

struct Boton {
  RectanguloUI rect;
  std::string etiqueta;
  EstiloBoton estilo = EstiloBoton::Primario;
  bool habilitado = true;
  bool activo = false;
};

struct CajaTexto {
  RectanguloUI rect;
  std::string etiqueta;
  std::string* valor = nullptr;
  bool* enfocada = nullptr;
  bool soloNumeros = false;
  std::size_t maxLongitud = 20;
};

struct ColumnaTabla {
  std::string titulo;
  float ancho = 80.0f;
};

struct TablaDatos {
  RectanguloUI rect;
  std::vector<ColumnaTabla> columnas;
  std::vector<std::vector<std::string>> filas;
  int filaResaltada = -1;
  int filaSecundaria = -1;
  int rangoMergeInicio = -1;
  int rangoMergeMitad = -1;
  int rangoMergeFin = -1;
  float altoFila = 24.0f;
};

// Utilidades de texto (ancho aproximado para fuente monoespaciada Consolas 14px).
std::string recortarTexto(const std::string& texto, std::size_t maxChars);
float anchoTextoAprox(const std::string& texto);

// Componentes reutilizables.
void dibujarPanel(const RectanguloUI& rect, const std::string& titulo = "",
                  const ColorRGB& fondo = paleta::PANEL_FONDO);
void dibujarInsignia(float x, float y, float w, float h,
                     const std::string& texto, const ColorRGB& fondo,
                     const ColorRGB& colorTexto = paleta::TEXTO_PRINCIPAL);

void dibujarBoton(const Boton& btn, float ratonX, float ratonY);
bool clicEnBoton(const Boton& btn, const EntradaUsuario& entrada);

RectanguloUI rectBotonBorrarCaja(const CajaTexto& caja);
void dibujarCajaTexto(const CajaTexto& caja, float ratonX, float ratonY);
bool manejarEventoCajaTexto(const CajaTexto& caja,
                            const EntradaUsuario& entrada);

void dibujarFlecha(float x1, float y1, float x2, float y2, const ColorRGB& col,
                   float tamPunta = 9.0f, bool gruesa = false);
void dibujarTabla(const TablaDatos& tabla);
void dibujarBarraEstado(const dominio::LogTrazabilidad& log);

} // namespace ui
