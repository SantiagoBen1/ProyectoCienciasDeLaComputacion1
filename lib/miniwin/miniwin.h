// ============================================================================
// Archivo: lib/miniwin/miniwin.h
// Proposito: Cabecera publica de la libreria grafica MINIWIN (compatible con
//            la API estandar de Pau Fernandez) extendida con TABULADOR y
//            SUPRIMIR para mejorar la navegacion en formularios.
// Nota: Solo debe incluirse desde la capa de adaptacion grafica
//       (src/ui/MiniWinCapa.cpp) y src/main.cpp (por la macro main -> _main_).
// ============================================================================

#ifndef _MINIWIN_H_
#define _MINIWIN_H_

#include <string>

// En Windows, MINIWIN controla el punto de entrada WinMain y ejecuta el
// codigo del usuario dentro de _main_() en un hilo dedicado.
#ifndef MINIWIN_SOURCE
#define main _main_
#endif

// Punto de entrada logico del programa que MINIWIN invoca desde WinMain.
int _main_();

namespace miniwin {

// --- Control de ventana, refresco, dialogos y temporizacion ---
void borra();                       // Limpia el lienzo de dibujo a negro
void refresca();                    // Copia el buffer de dibujo al frente y repinta
void mensaje(std::string msj);      // Muestra un cuadro de dialogo informativo
bool pregunta(std::string msj);     // Muestra un dialogo Si/No y devuelve la eleccion
void espera(int miliseg);           // Pausa el hilo actual los milisegundos indicados

int vancho();                       // Devuelve el ancho actual del area cliente (px)
int valto();                        // Devuelve el alto actual del area cliente (px)
void vredimensiona(int ancho, int alto); // Redimensiona la ventana y recrea buffers
void vcierra();                     // Solicita el cierre ordenado de la ventana

// --- Configuracion de color activo para primitivas y texto ---
void color(int c);                  // Selecciona un color de la paleta basica (enum)
void color_rgb(int r, int g, int b);// Selecciona un color arbitrario RGB (0..255)

// --- Primitivas graficas 2D ---
void punto(float x, float y);
void linea(float x_ini, float y_ini, float x_fin, float y_fin);
void rectangulo(float izq, float arr, float der, float aba);       // Solo contorno
void rectangulo_lleno(float izq, float arr, float der, float aba); // Relleno solido
void circulo(float x_centro, float y_centro, float radio);         // Solo contorno
void circulo_lleno(float x_centro, float y_centro, float radio);   // Relleno solido
void texto(float x, float y, const std::string &texto);            // Dibuja texto UTF-8

// --- Lectura de teclado (cola FIFO de pulsaciones, no bloqueante) ---
int tecla();

// --- Consulta de estado del raton ---
bool raton(float &x, float &y);     // Obtiene coordenadas y retorna si esta dentro
bool raton_dentro();                // True si el cursor esta dentro de la ventana
float raton_x();                    // Coordenada X actual del cursor
float raton_y();                    // Coordenada Y actual del cursor
void raton_botones(bool &izq, bool &der); // Estado simultaneo de ambos botones
bool raton_boton_izq();             // True mientras el boton izquierdo este pulsado
bool raton_boton_der();             // True mientras el boton derecho este pulsado

// Codigos de teclas especiales devueltos por tecla() (ademas de '0'-'9' y 'A'-'Z').
enum {
  ESCAPE,
  IZQUIERDA,
  DERECHA,
  ARRIBA,
  ABAJO,
  F1,
  F2,
  F3,
  F4,
  F5,
  F6,
  F7,
  F8,
  F9,
  F10,
  ESPACIO,
  RETURN,
  TABULADOR, // Extension usada para rotar el foco entre cajas de texto
  SUPRIMIR,  // Extension usada para vaciar la caja de texto activa
  NINGUNA
};

// Paleta de colores predefinida para la funcion color(int).
enum { NEGRO, ROJO, VERDE, AZUL, AMARILLO, MAGENTA, CYAN, BLANCO };

} // namespace miniwin

#endif // _MINIWIN_H_
