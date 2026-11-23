#pragma once

#include <string>

namespace dominio {

// Menor numero = mayor prioridad en algoritmos::ColaPrioridad.
enum class Prioridad : int {
  Medico = 1,
  Urgente = 2,
  Regular = 3
};

inline std::string textoPrioridad(Prioridad p) {
  switch (p) {
    case Prioridad::Medico:  return "Medico";
    case Prioridad::Urgente: return "Urgente";
    case Prioridad::Regular: return "Regular";
  }
  return "Regular";
}

struct Paquete {
  int id = 0;
  std::string destino;
  double pesoKg = 0.0;
  Prioridad prioridad = Prioridad::Regular;
};

struct Repartidor {
  int id = 0;
  std::string nombre;
  std::string zona;
};

struct Sector {
  int id = 0;
  std::string nombre;
  std::string codigoCorto;
  float x = 0.0f;
  float y = 0.0f;
};

struct RegistroEntrega {
  int idRegistro = 0;
  int idPaquete = 0;
  std::string destino;
  Prioridad prioridad = Prioridad::Regular;
  std::string repartidor;
  std::string origenAccion;
};

enum class ModuloActivo {
  ColaYPrioridad = 0,
  Bodega = 1,
  FlotaYAudi = 2,
  RutasUrbanas = 3
};

enum class NivelLog {
  Info,
  Exito,
  Alerta,
  Error
};

struct LogTrazabilidad {
  NivelLog nivel = NivelLog::Info;
  std::string titulo = "Sistema iniciado";
  std::string antes = "Sin operaciones ejecutadas.";
  std::string despues = "Esqueleto grafico listo. Seleccione un modulo arriba.";
};

enum class ModoRed {
  ConBonificaciones, // Pesos positivos + negativos sin ciclos negativos (por defecto)
  SoloPositivos,     // Solo pesos >= 0 para demostrar Dijkstra exitoso
  CicloNegativoDemo  // Agrega cierre negativo para demostracion de Bellman-Ford
};

} // namespace dominio
