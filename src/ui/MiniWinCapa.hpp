// ============================================================================
// Archivo: src/ui/MiniWinCapa.hpp
// Capa: Capa C — Adaptador grafico (unico modulo acoplado a MINIWIN).
// Proposito: Expone primitivas de dibujo, control de ventana y captura de
//            entrada desacopladas de miniwin.h para que el resto de capas no
//            dependa directamente de la libreria grafica.
// ============================================================================

#pragma once

#include "Paleta.hpp"

#include <string>
#include <vector>

namespace ui {

// Evento individual de teclado traducido a banderas semanticas independientes
// de las constantes internas de MINIWIN.
struct EventoTecla {
  char caracter = '\0';   // '0'-'9', 'A'-'Z' o ' '
  bool esEscape = false;  // Cierra la aplicacion
  bool esEnter = false;   // Confirma formulario / ejecuta busqueda
  bool esBorrar = false;  // Borra 1 caracter (IZQUIERDA / Backspace)
  bool esLimpiar = false; // Vacia la caja de texto completa (SUPRIMIR)
  bool esTab = false;     // Rota el foco entre cajas del modulo (TABULADOR)
  bool esF5 = false;      // Reinicia la simulacion al estado inicial
  bool esEspacio = false;
  bool esArriba = false;
  bool esAbajo = false;
  bool esIzquierda = false;
  bool esDerecha = false;
};

// Instantanea de entrada del cuadro actual.
// clicIzq y clicDer solo son true en el flanco de subida (transicion soltado -> pulsado).
struct EntradaUsuario {
  float ratonX = 0.0f;
  float ratonY = 0.0f;
  bool ratonDentro = false;
  bool clicIzq = false;
  bool clicDer = false;
  std::vector<EventoTecla> teclas; // Todas las teclas encoladas desde el cuadro previo
};

// --- Ciclo de vida de ventana y refresco ---
void iniciarVentana(int ancho, int alto);
void cerrarVentana();
void limpiarPantalla();
void refrescarPantalla();
void esperarMs(int milisegundos);

// --- Primitivas graficas encapsuladas ---
void establecerColor(const ColorRGB& col);
void dibujarLinea(float x1, float y1, float x2, float y2);
void dibujarRectangulo(float izq, float arr, float der, float aba);
void dibujarRectanguloLleno(float izq, float arr, float der, float aba);
void dibujarCirculo(float cx, float cy, float radio);
void dibujarCirculoLleno(float cx, float cy, float radio);
void dibujarTexto(float x, float y, const std::string& texto);

// Captura posicion del raton, flancos de clic y cola de teclas del cuadro actual.
EntradaUsuario leerEntrada();

} // namespace ui
