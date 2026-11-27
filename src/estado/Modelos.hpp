// ============================================================================
// Archivo: src/estado/Modelos.hpp
// Capa: Capa B — Dominio y Estado de la Aplicacion (sin dependencias graficas).
// Proposito: Define las entidades del dominio logistico (Paquete, Repartidor,
//            Sector, RegistroEntrega) y los tipos enumerados compartidos por
//            la aplicacion (Prioridad, ModuloActivo, NivelLog, ModoRed).
// ============================================================================

#pragma once

#include <string>

namespace dominio {

// Nivel de criticidad de un paquete.
// Convencion compatible con algoritmos::ColaPrioridad: menor valor = mayor urgencia.
enum class Prioridad : int {
  Medico = 1,
  Urgente = 2,
  Regular = 3
};

// Devuelve el nombre legible de una prioridad para tablas, tarjetas y logs.
inline std::string textoPrioridad(Prioridad p) {
  switch (p) {
    case Prioridad::Medico:  return "Medico";
    case Prioridad::Urgente: return "Urgente";
    case Prioridad::Regular: return "Regular";
  }
  return "Regular";
}

// Entidad base de envio gestionada en colas (Modulo 1) y bodega (Modulo 2).
struct Paquete {
  int id = 0;                               // Identificador unico del envio
  std::string destino;                      // Sector o destinatario final
  double pesoKg = 0.0;                      // Peso en kilogramos
  Prioridad prioridad = Prioridad::Regular; // Nivel de urgencia
};

// Entidad almacenada en la nomina activa (ListaEnlazada del Modulo 3).
struct Repartidor {
  int id = 0;         // Identificador unico del mensajero
  std::string nombre; // Nombre completo
  std::string zona;   // Sector urbano de cobertura asignado
};

// Vertice urbano del grafo ponderado (Modulo 4), incluyendo su posicion (x, y)
// en el lienzo grafico para la seleccion interactiva con el raton.
struct Sector {
  int id = 0;              // ID del nodo dentro de algoritmos::Grafo
  std::string nombre;      // Nombre completo del sector
  std::string codigoCorto; // Etiqueta compacta dibujada junto al circulo
  float x = 0.0f;          // Coordenada X del centro en el lienzo del grafo
  float y = 0.0f;          // Coordenada Y del centro en el lienzo del grafo
};

// Registro historico de una entrega apilada en la Pila LIFO (Modulo 3).
struct RegistroEntrega {
  int idRegistro = 0;                       // Clave unica del evento apilado
  int idPaquete = 0;                        // Paquete despachado/entregado
  std::string destino;                      // Destino del paquete
  Prioridad prioridad = Prioridad::Regular; // Prioridad original (util al revertir)
  std::string repartidor;                   // Mensajero asignado al despacho
  std::string origenAccion;                 // Indica si vino de Cola Prioridad, FIFO o manual
};

// Pestanas principales navegables desde la barra superior o teclas 1-4.
enum class ModuloActivo {
  ColaYPrioridad = 0,
  Bodega = 1,
  FlotaYAudi = 2,
  RutasUrbanas = 3
};

// Severidad visual de la barra de trazabilidad inferior.
enum class NivelLog {
  Info,
  Exito,
  Alerta,
  Error
};

// Contenido mostrado en la barra fija inferior con trazabilidad Antes -> Despues.
struct LogTrazabilidad {
  NivelLog nivel = NivelLog::Info;
  std::string titulo = "Sistema iniciado";
  std::string antes = "Sin operaciones ejecutadas.";
  std::string despues = "Esqueleto grafico listo. Seleccione un modulo arriba.";
};

// Escenarios de la red vial urbana disponibles para contrastar Dijkstra y Bellman-Ford.
enum class ModoRed {
  ConBonificaciones, // Pesos positivos + negativos sin ciclos negativos (por defecto)
  SoloPositivos,     // Solo pesos >= 0 para demostrar Dijkstra exitoso
  CicloNegativoDemo  // Agrega cierre negativo (4->2 con -2) para demostracion de Bellman-Ford
};

} // namespace dominio
