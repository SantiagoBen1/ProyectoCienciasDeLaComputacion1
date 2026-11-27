#include "MiniWinCapa.hpp"

#include "miniwin.h"

namespace ui {

namespace {
bool g_izqAnterior = false;
bool g_derAnterior = false;
} // namespace

void iniciarVentana(int ancho, int alto) {
  miniwin::vredimensiona(ancho, alto);
}

void cerrarVentana() {
  miniwin::vcierra();
}

void limpiarPantalla() {
  miniwin::borra();
}

void refrescarPantalla() {
  miniwin::refresca();
}

void esperarMs(int milisegundos) {
  miniwin::espera(milisegundos);
}

void establecerColor(const ColorRGB& col) {
  miniwin::color_rgb(col.r, col.g, col.b);
}

void dibujarLinea(float x1, float y1, float x2, float y2) {
  miniwin::linea(x1, y1, x2, y2);
}

void dibujarRectangulo(float izq, float arr, float der, float aba) {
  miniwin::rectangulo(izq, arr, der, aba);
}

void dibujarRectanguloLleno(float izq, float arr, float der, float aba) {
  miniwin::rectangulo_lleno(izq, arr, der, aba);
}

void dibujarCirculo(float cx, float cy, float radio) {
  miniwin::circulo(cx, cy, radio);
}

void dibujarCirculoLleno(float cx, float cy, float radio) {
  miniwin::circulo_lleno(cx, cy, radio);
}

void dibujarTexto(float x, float y, const std::string& texto) {
  miniwin::texto(x, y, texto);
}

EntradaUsuario leerEntrada() {
  EntradaUsuario entrada;
  entrada.ratonDentro = miniwin::raton(entrada.ratonX, entrada.ratonY);

  const bool izqActual = miniwin::raton_boton_izq();
  const bool derActual = miniwin::raton_boton_der();
  entrada.clicIzq = izqActual && !g_izqAnterior;
  entrada.clicDer = derActual && !g_derAnterior;
  g_izqAnterior = izqActual;
  g_derAnterior = derActual;

  // Drenar todas las teclas acumuladas desde el cuadro anterior.
  for (int guard = 0; guard < 16; ++guard) {
    const int codigo = miniwin::tecla();
    if (codigo == miniwin::NINGUNA) {
      break;
    }
    EventoTecla ev;
    if (codigo == miniwin::ESCAPE) {
      ev.esEscape = true;
    } else if (codigo == miniwin::RETURN) {
      ev.esEnter = true;
    } else if (codigo == miniwin::TABULADOR) {
      ev.esTab = true;
    } else if (codigo == miniwin::SUPRIMIR) {
      ev.esLimpiar = true;
    } else if (codigo == miniwin::F5) {
      ev.esF5 = true;
    } else if (codigo == miniwin::ESPACIO) {
      ev.esEspacio = true;
      ev.caracter = ' ';
    } else if (codigo == miniwin::IZQUIERDA) {
      ev.esIzquierda = true;
      ev.esBorrar = true; // Compatible con MINIWIN estandar (sin VK_BACK)
    } else if (codigo == miniwin::DERECHA) {
      ev.esDerecha = true;
    } else if (codigo == miniwin::ARRIBA) {
      ev.esArriba = true;
    } else if (codigo == miniwin::ABAJO) {
      ev.esAbajo = true;
    } else if ((codigo >= '0' && codigo <= '9') ||
               (codigo >= 'A' && codigo <= 'Z')) {
      ev.caracter = static_cast<char>(codigo);
    }
    entrada.teclas.push_back(ev);
  }

  return entrada;
}

} // namespace ui
