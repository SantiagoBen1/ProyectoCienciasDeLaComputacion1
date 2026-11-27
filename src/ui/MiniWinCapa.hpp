#pragma once

#include "Paleta.hpp"

#include <string>
#include <vector>

namespace ui {

// Evento de teclado desacoplado de las constantes internas de MINIWIN.
struct EventoTecla {
  char caracter = '\0';
  bool esEscape = false;
  bool esEnter = false;
  bool esBorrar = false;
  bool esLimpiar = false;
  bool esTab = false;
  bool esF5 = false;
  bool esEspacio = false;
  bool esArriba = false;
  bool esAbajo = false;
  bool esIzquierda = false;
  bool esDerecha = false;
};

// Instantanea de entrada del cuadro actual (flanco de subida en clics).
struct EntradaUsuario {
  float ratonX = 0.0f;
  float ratonY = 0.0f;
  bool ratonDentro = false;
  bool clicIzq = false;
  bool clicDer = false;
  std::vector<EventoTecla> teclas;
};

// Capa C (unico modulo con llamadas a MINIWIN).
void iniciarVentana(int ancho, int alto);
void cerrarVentana();
void limpiarPantalla();
void refrescarPantalla();
void esperarMs(int milisegundos);

void establecerColor(const ColorRGB& col);
void dibujarLinea(float x1, float y1, float x2, float y2);
void dibujarRectangulo(float izq, float arr, float der, float aba);
void dibujarRectanguloLleno(float izq, float arr, float der, float aba);
void dibujarCirculo(float cx, float cy, float radio);
void dibujarCirculoLleno(float cx, float cy, float radio);
void dibujarTexto(float x, float y, const std::string& texto);

EntradaUsuario leerEntrada();

} // namespace ui
