#include "EstadoApp.hpp"

#include <utility>
#include <vector>

namespace estado {

void EstadoApp::construirAristasGrafoSegunModo(dominio::ModoRed modo) {
  // Limpiar todas las aristas existentes preservando los 8 nodos.
  for (::Nodo* nodo : grafoUrbano.getNodos()) {
    std::vector<::Nodo*> vecinos;
    for (const auto& par : nodo->getAdyacentes()) {
      vecinos.push_back(par.first);
    }
    for (::Nodo* vecino : vecinos) {
      nodo->eliminarAdyacente(vecino);
    }
  }

  modoRed = modo;

  // Aristas base positivas de la red urbana (8 sectores).
  grafoUrbano.agregarAristaPonderada(1, 2, 4);
  grafoUrbano.agregarAristaPonderada(1, 4, 6);
  grafoUrbano.agregarAristaPonderada(1, 3, 7);
  grafoUrbano.agregarAristaPonderada(2, 7, 6);
  grafoUrbano.agregarAristaPonderada(3, 4, 4);
  grafoUrbano.agregarAristaPonderada(4, 7, 3);
  grafoUrbano.agregarAristaPonderada(4, 8, 5);
  grafoUrbano.agregarAristaPonderada(4, 6, 7);
  grafoUrbano.agregarAristaPonderada(5, 4, 2);
  grafoUrbano.agregarAristaPonderada(5, 6, 4);
  grafoUrbano.agregarAristaPonderada(7, 8, 5);
  grafoUrbano.agregarAristaPonderada(6, 8, 4);
  grafoUrbano.agregarAristaPonderada(7, 4, 4); // Ciclo positivo 4->7->4 (+7)

  if (modo == dominio::ModoRed::SoloPositivos) {
    grafoUrbano.agregarAristaPonderada(2, 4, 3);
    grafoUrbano.agregarAristaPonderada(3, 5, 2);
  } else {
    // Corredores con bonificacion de tiempo / descuento de peaje (sin ciclo negativo).
    grafoUrbano.agregarAristaPonderada(2, 4, -2);
    grafoUrbano.agregarAristaPonderada(3, 5, -3);
  }

  if (modo == dominio::ModoRed::CicloNegativoDemo) {
    // Cierra el ciclo negativo 2 -> 4 (-2) -> 2 (-2), costo total -4.
    grafoUrbano.agregarAristaPonderada(4, 2, -2);
  }
}

void EstadoApp::inicializarDatosPrecargados() {
  // 1. 12 paquetes precargados en bodega (en orden inicial DESORDENADO por ID).
  const std::vector<dominio::Paquete> loteBodega = {
      {108, "Zona Industrial",   5.5, dominio::Prioridad::Regular},
      {102, "Centro Historico",  4.0, dominio::Prioridad::Urgente},
      {111, "Parque Logistico",  1.8, dominio::Prioridad::Medico},
      {105, "Terminal Sur",      3.0, dominio::Prioridad::Regular},
      {112, "Bodega Norte",      7.5, dominio::Prioridad::Regular},
      {101, "Sector Residencial",6.2, dominio::Prioridad::Regular},
      {109, "Clinica Sur",       1.2, dominio::Prioridad::Medico},
      {104, "Clinica Centro",    1.5, dominio::Prioridad::Medico},
      {107, "Zona Franca",       2.8, dominio::Prioridad::Urgente},
      {103, "Central de Acopio", 4.4, dominio::Prioridad::Regular},
      {110, "Hospital Norte",    0.9, dominio::Prioridad::Medico},
      {106, "Terminal Sur",      3.6, dominio::Prioridad::Urgente},
  };

  bodegaIds.clear();
  for (const auto& paq : loteBodega) {
    paquetesPorId[paq.id] = paq;
    bodegaIds.push_back(paq.id);
  }
  bodegaIdsIniciales = bodegaIds;
  bodegaOrdenada = false;
  siguienteIdPaquete = 113;
  formM1.idTexto = std::to_string(siguienteIdPaquete);

  // 2. Pedidos iniciales en las colas del Modulo 1 para demostracion inmediata.
  // Urgentes / Medicos -> ColaPrioridad (1=Medico, 2=Urgente).
  colaPrioridad.encolar(104, static_cast<int>(dominio::Prioridad::Medico));
  colaPrioridad.encolar(111, static_cast<int>(dominio::Prioridad::Medico));
  colaPrioridad.encolar(102, static_cast<int>(dominio::Prioridad::Urgente));
  // Regulares -> ColaFIFO.
  colaRegular.encolar(108);
  colaRegular.encolar(105);
  colaRegular.encolar(101);

  // 3. Lista enlazada de 6 repartidores activos.
  const std::vector<dominio::Repartidor> plantilla = {
      {10, "Ana Gomez",    "Norte"},
      {20, "Luis Perez",   "Centro"},
      {30, "Sofia Ruiz",   "Industrial"},
      {40, "Diego Torres", "Sur"},
      {50, "Laura Diaz",   "Zona Franca"},
      {60, "Mateo Silva",  "Residencial"},
  };
  for (const auto& rep : plantilla) {
    repartidoresPorId[rep.id] = rep;
    listaRepartidores.insertarAlFinal(rep.id);
  }

  // 4. Historial inicial en Pila LIFO (2 entregas completadas).
  const std::vector<dominio::RegistroEntrega> entregasIniciales = {
      {siguienteIdEntrega++, 103, "Central de Acopio",
       dominio::Prioridad::Regular, "Ana Gomez", "Precarga turno"},
      {siguienteIdEntrega++, 106, "Terminal Sur",
       dominio::Prioridad::Urgente, "Luis Perez", "Precarga turno"},
  };
  for (const auto& reg : entregasIniciales) {
    entregasPorId[reg.idRegistro] = reg;
    pilaEntregas.push(reg.idRegistro);
  }

  // 5. Grafo dirigido y ponderado de 8 sectores urbanos (ajustado al lienzo 756x532).
  sectores = {
      {1, "Central de Acopio",  "Central",     96.0f, 360.0f},
      {2, "Bodega Norte",       "Bod. Norte", 254.0f, 214.0f},
      {3, "Zona Industrial",    "Z. Indust.", 254.0f, 506.0f},
      {4, "Centro Historico",   "Centro H.",  426.0f, 360.0f},
      {5, "Sector Residencial", "Residenc.",  450.0f, 520.0f},
      {6, "Terminal Sur",       "Term. Sur",  632.0f, 506.0f},
      {7, "Zona Franca",        "Z. Franca",  538.0f, 214.0f},
      {8, "Parque Logistico",   "P. Logist.", 686.0f, 348.0f},
  };
  for (const auto& sec : sectores) {
    grafoUrbano.agregarNodo(sec.id);
  }
  construirAristasGrafoSegunModo(dominio::ModoRed::ConBonificaciones);

  registrarLog(
      dominio::NivelLog::Info, "Bootstrapper cargado en RAM",
      "Memoria vacia al iniciar la aplicacion.",
      "Cargados: 12 paquetes bodega, 6 pedidos en colas (3 Prio / 3 FIFO), 6 "
      "repartidores, 8 sectores.");
}

} // namespace estado
