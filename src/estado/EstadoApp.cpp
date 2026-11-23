#include "EstadoApp.hpp"

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <sstream>
#include <utility>

namespace estado {

namespace {

bool parsearEnteroPositivo(const std::string& texto, int& resultado) {
  if (texto.empty()) {
    return false;
  }
  long long acumulado = 0;
  for (char c : texto) {
    if (c < '0' || c > '9') {
      return false;
    }
    acumulado = acumulado * 10 + (c - '0');
    if (acumulado > 999999) {
      return false;
    }
  }
  if (acumulado <= 0) {
    return false;
  }
  resultado = static_cast<int>(acumulado);
  return true;
}

std::string recortarEspacios(const std::string& s) {
  const std::size_t ini = s.find_first_not_of(' ');
  if (ini == std::string::npos) {
    return "";
  }
  const std::size_t fin = s.find_last_not_of(' ');
  return s.substr(ini, fin - ini + 1);
}

std::string aMayusculas(const std::string& s) {
  std::string out = s;
  for (char& c : out) {
    c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
  }
  return out;
}

std::string resumenColas(const EstadoApp& app) {
  return "Prioridad=" + std::to_string(app.colaPrioridad.tamano()) +
         ", FIFO=" + std::to_string(app.colaRegular.tamano());
}

} // namespace

std::string nombreModulo(dominio::ModuloActivo modulo) {
  switch (modulo) {
    case dominio::ModuloActivo::ColaYPrioridad: return "1. Cola y Prioridad";
    case dominio::ModuloActivo::Bodega:         return "2. Bodega (Sort/Busq)";
    case dominio::ModuloActivo::FlotaYAudi:     return "3. Flota y Auditoria";
    case dominio::ModuloActivo::RutasUrbanas:   return "4. Rutas Urbanas";
  }
  return "Desconocido";
}

std::string EstadoApp::nombreModoRed(dominio::ModoRed modo) {
  switch (modo) {
    case dominio::ModoRed::ConBonificaciones: return "Con bonificaciones (-)";
    case dominio::ModoRed::SoloPositivos:     return "Solo vias positivas (+)";
    case dominio::ModoRed::CicloNegativoDemo: return "Ciclo negativo DEMO";
  }
  return "Desconocido";
}

std::string EstadoApp::formatearListaIds(const std::vector<int>& ids) {
  std::ostringstream oss;
  oss << "[";
  for (std::size_t i = 0; i < ids.size(); ++i) {
    if (i > 0) {
      oss << ", ";
    }
    oss << ids[i];
  }
  oss << "]";
  return oss.str();
}

void EstadoApp::registrarLog(dominio::NivelLog nivel, const std::string& titulo,
                             const std::string& antes,
                             const std::string& despues) {
  log.nivel = nivel;
  log.titulo = titulo;
  log.antes = antes;
  log.despues = despues;
}

void EstadoApp::limpiarFocos() {
  formM1.focoId = false;
  formM1.focoDestino = false;
  m2.focoConsulta = false;
  m3.focoRepId = false;
  m3.focoRepNombre = false;
  m3.focoRepZona = false;
  m3.focoEntregaPaqId = false;
  m3.focoEntregaDestino = false;
}

bool EstadoApp::algunCampoEnfocado() const {
  return formM1.focoId || formM1.focoDestino || m2.focoConsulta ||
         m3.focoRepId || m3.focoRepNombre || m3.focoRepZona ||
         m3.focoEntregaPaqId || m3.focoEntregaDestino;
}

void EstadoApp::cambiarModulo(dominio::ModuloActivo nuevoModulo) {
  if (moduloActivo == nuevoModulo) {
    return;
  }
  if (m2.animacionActiva) {
    finalizarAnimacionMergeSortM2();
  }
  const std::string anterior = nombreModulo(moduloActivo);
  moduloActivo = nuevoModulo;
  limpiarFocos();
  registrarLog(dominio::NivelLog::Info, "Cambio de modulo",
               "Vista activa: " + anterior,
               "Vista activa: " + nombreModulo(moduloActivo));
}

void EstadoApp::actualizarAnimaciones() {
  if (!m2.animacionActiva) {
    return;
  }
  ++m2.contadorCuadros;
  if (m2.contadorCuadros < 22) {
    return;
  }
  m2.contadorCuadros = 0;
  ++m2.indicePasoActual;

  if (m2.indicePasoActual >= m2.pasosMerge.size()) {
    finalizarAnimacionMergeSortM2();
  } else {
    bodegaIds = m2.pasosMerge[m2.indicePasoActual].estado;
  }
}

const dominio::Paquete* EstadoApp::obtenerPaquete(int id) const {
  const auto it = paquetesPorId.find(id);
  if (it == paquetesPorId.end()) {
    return nullptr;
  }
  return &it->second;
}

const dominio::Repartidor* EstadoApp::obtenerRepartidor(int id) const {
  const auto it = repartidoresPorId.find(id);
  if (it == repartidoresPorId.end()) {
    return nullptr;
  }
  return &it->second;
}

const dominio::RegistroEntrega* EstadoApp::obtenerEntrega(int idRegistro) const {
  const auto it = entregasPorId.find(idRegistro);
  if (it == entregasPorId.end()) {
    return nullptr;
  }
  return &it->second;
}

const dominio::Sector* EstadoApp::obtenerSector(int id) const {
  for (const auto& sec : sectores) {
    if (sec.id == id) {
      return &sec;
    }
  }
  return nullptr;
}

bool EstadoApp::grafoTienePesosNegativos() const {
  for (const ::Nodo* nodo : grafoUrbano.getNodos()) {
    for (const auto& arista : nodo->getAdyacentes()) {
      if (arista.second < 0) {
        return true;
      }
    }
  }
  return false;
}

std::string EstadoApp::formatearPeso(double pesoKg) const {
  std::ostringstream oss;
  oss << std::fixed << std::setprecision(1) << pesoKg << " kg";
  return oss.str();
}

std::string EstadoApp::formatearSecuenciaSectores(
    const std::vector<int>& camino) const {
  std::ostringstream oss;
  for (std::size_t i = 0; i < camino.size(); ++i) {
    if (i > 0) {
      oss << " -> ";
    }
    const dominio::Sector* sec = obtenerSector(camino[i]);
    if (sec != nullptr) {
      oss << "(" << sec->id << ") " << sec->codigoCorto;
    } else {
      oss << camino[i];
    }
  }
  return oss.str();
}

// ======================= MODULO 1 =======================

bool EstadoApp::ingresarPedidoM1() {
  const std::string estadoAntes = resumenColas(*this);

  int id = 0;
  if (!parsearEnteroPositivo(formM1.idTexto, id)) {
    registrarLog(dominio::NivelLog::Error, "Ingreso rechazado (ID invalido)",
                 estadoAntes,
                 "Ingrese un ID numerico positivo valido (ej. 113).");
    return false;
  }

  if (paquetesPorId.count(id) > 0) {
    registrarLog(dominio::NivelLog::Alerta,
                 "Ingreso rechazado (ID duplicado #" + std::to_string(id) + ")",
                 estadoAntes,
                 "El ID #" + std::to_string(id) +
                     " ya existe en el sistema. Use otro identificador unico.");
    return false;
  }

  const std::string destinoLimpio = recortarEspacios(formM1.destinoTexto);
  if (destinoLimpio.empty()) {
    registrarLog(dominio::NivelLog::Error, "Ingreso rechazado (sin destino)",
                 estadoAntes,
                 "Escriba un destino o destinatario antes de ingresar.");
    return false;
  }

  dominio::Paquete nuevo;
  nuevo.id = id;
  nuevo.destino = destinoLimpio;
  nuevo.pesoKg = formM1.pesoKg;
  nuevo.prioridad = formM1.prioridad;
  paquetesPorId[id] = nuevo;

  std::string colaDestino;
  if (nuevo.prioridad == dominio::Prioridad::Regular) {
    colaRegular.encolar(id);
    colaDestino = "ColaFIFO (Regular)";
  } else {
    colaPrioridad.encolar(id, static_cast<int>(nuevo.prioridad));
    colaDestino = "ColaPrioridad (" + dominio::textoPrioridad(nuevo.prioridad) +
                  ", P" + std::to_string(static_cast<int>(nuevo.prioridad)) +
                  ")";
  }

  if (id >= siguienteIdPaquete) {
    siguienteIdPaquete = id + 1;
  } else {
    while (paquetesPorId.count(siguienteIdPaquete) > 0) {
      ++siguienteIdPaquete;
    }
  }
  formM1.idTexto = std::to_string(siguienteIdPaquete);

  registrarLog(dominio::NivelLog::Exito,
               "Pedido #" + std::to_string(id) + " ingresado en " + colaDestino,
               estadoAntes,
               resumenColas(*this) + " | Destino: " + nuevo.destino + " (" +
                   formatearPeso(nuevo.pesoKg) + ")");
  return true;
}

bool EstadoApp::despacharSiguienteM1() {
  const std::string estadoAntes =
      resumenColas(*this) + " | Pila auditoria=" +
      std::to_string(pilaEntregas.tamano());

  int idDespachado = 0;
  int prioExtraida = 0;
  std::string origenCola;

  if (!colaPrioridad.estaVacia()) {
    colaPrioridad.desencolar(idDespachado, prioExtraida);
    origenCola = "Cola de Prioridad (P" + std::to_string(prioExtraida) + ")";
  } else if (!colaRegular.estaVacia()) {
    colaRegular.desencolar(idDespachado);
    origenCola = "Cola Regular FIFO";
  } else {
    registrarLog(
        dominio::NivelLog::Alerta, "Despacho omitido (colas vacias)",
        estadoAntes,
        "No hay pedidos pendientes en Cola de Prioridad ni en Cola FIFO.");
    return false;
  }

  const dominio::Paquete* paq = obtenerPaquete(idDespachado);
  dominio::Paquete info =
      paq != nullptr ? *paq
                     : dominio::Paquete{idDespachado, "Desconocido", 1.0,
                                        dominio::Prioridad::Regular};

  tieneUltimoDespachado = true;
  ultimoDespachado = info;
  origenUltimoDespacho = origenCola;
  ++totalDespachados;

  std::string nombreRep = "Sin flota";
  const auto reps = listaRepartidores.aVector();
  if (!reps.empty()) {
    const int idRep = reps[static_cast<std::size_t>(totalDespachados - 1) %
                           reps.size()];
    const auto itRep = repartidoresPorId.find(idRep);
    if (itRep != repartidoresPorId.end()) {
      nombreRep = itRep->second.nombre;
    }
  }

  const int idReg = siguienteIdEntrega++;
  dominio::RegistroEntrega reg;
  reg.idRegistro = idReg;
  reg.idPaquete = info.id;
  reg.destino = info.destino;
  reg.prioridad = info.prioridad;
  reg.repartidor = nombreRep;
  reg.origenAccion = origenCola;
  entregasPorId[idReg] = reg;
  pilaEntregas.push(idReg);
  m3.auditoriaActiva = false;

  registrarLog(dominio::NivelLog::Exito,
               "Despachado #" + std::to_string(info.id) + " desde " +
                   origenCola,
               estadoAntes,
               resumenColas(*this) + " | Pila=" +
                   std::to_string(pilaEntregas.tamano()) + " | " +
                   info.destino + " (" +
                   dominio::textoPrioridad(info.prioridad) + ")");
  return true;
}

// ======================= MODULO 2 =======================

void EstadoApp::ordenarBodegaMergeSortM2() {
  if (m2.animacionActiva) {
    finalizarAnimacionMergeSortM2();
    return;
  }

  m2.snapshotAntes = bodegaIds;
  m2.filaEncontrada = -1;
  m2.pasosMerge.clear();

  std::vector<int> ordenados = algoritmos::mergeSort(bodegaIds, &m2.pasosMerge);
  m2.idsOrdenadosDestino = ordenados;
  m2.snapshotDespues = ordenados;
  m2.tieneSnapshots = true;

  if (m2.animarPasos && !m2.pasosMerge.empty()) {
    m2.animacionActiva = true;
    m2.indicePasoActual = 0;
    m2.contadorCuadros = 0;
    bodegaOrdenada = false;
    bodegaIds = m2.pasosMerge[0].estado;
    registrarLog(
        dominio::NivelLog::Info,
        "Merge Sort en curso (" + std::to_string(m2.pasosMerge.size()) +
            " pasos de mezcla)",
        "IDs antes: " + formatearListaIds(m2.snapshotAntes),
        "Animando division y mezcla O(n log n)... (pulse de nuevo para "
        "finalizar ya)");
  } else {
    bodegaIds = ordenados;
    bodegaOrdenada = true;
    registrarLog(
        dominio::NivelLog::Exito,
        "Bodega ordenada con Merge Sort O(n log n) (" +
            std::to_string(m2.pasosMerge.size()) + " mezclas)",
        "IDs antes: " + formatearListaIds(m2.snapshotAntes),
        "IDs despues: " + formatearListaIds(m2.snapshotDespues));
  }
}

void EstadoApp::finalizarAnimacionMergeSortM2() {
  if (!m2.animacionActiva) {
    return;
  }
  m2.animacionActiva = false;
  bodegaIds = m2.idsOrdenadosDestino;
  bodegaOrdenada = true;
  registrarLog(
      dominio::NivelLog::Exito,
      "Merge Sort completado (" + std::to_string(m2.pasosMerge.size()) +
          " pasos de mezcla)",
      "IDs antes: " + formatearListaIds(m2.snapshotAntes),
      "IDs despues: " + formatearListaIds(m2.snapshotDespues));
}

void EstadoApp::restaurarDesordenBodegaM2() {
  const std::string antes = formatearListaIds(bodegaIds);
  m2.animacionActiva = false;
  m2.pasosMerge.clear();
  bodegaIds = bodegaIdsIniciales;
  bodegaOrdenada = false;
  m2.filaEncontrada = -1;
  registrarLog(
      dominio::NivelLog::Info, "Orden inicial restaurado (bodega desordenada)",
      "IDs antes: " + antes,
      "IDs restaurados: " + formatearListaIds(bodegaIds) +
          " (requiere Merge Sort para Busqueda Binaria)");
}

bool EstadoApp::buscarBinariaIdM2() {
  if (m2.animacionActiva) {
    finalizarAnimacionMergeSortM2();
  }

  const std::string antes =
      std::string("Catalogo ") + (bodegaOrdenada ? "ORDENADO" : "DESORDENADO") +
      " (" + std::to_string(bodegaIds.size()) + " paquetes)";

  if (!bodegaOrdenada) {
    m2.filaEncontrada = -1;
    registrarLog(
        dominio::NivelLog::Alerta,
        "Busqueda Binaria bloqueada: catalogo no ordenado", antes,
        "Ordene primero con Merge Sort para aplicar Busqueda Binaria O(log n) "
        "(o use Busqueda Lineal).");
    return false;
  }

  int objetivo = 0;
  const std::string consultaLimpia = recortarEspacios(m2.consultaTexto);
  if (!parsearEnteroPositivo(consultaLimpia, objetivo)) {
    m2.filaEncontrada = -1;
    registrarLog(dominio::NivelLog::Error,
                 "Busqueda Binaria: ingrese un ID numerico", antes,
                 "Entrada '" + consultaLimpia +
                     "' no es un ID numerico valido (ej. 109).");
    return false;
  }

  int comparaciones = 0;
  const int indice =
      algoritmos::busquedaBinaria(bodegaIds, objetivo, &comparaciones);

  m2.ultimoMetodo = "Binaria (ID)";
  m2.ultimoObjetivo = "#" + std::to_string(objetivo);
  m2.compBinaria = comparaciones;
  m2.filaEncontrada = indice;
  m2.ultimoEncontrado = (indice >= 0);

  if (indice >= 0) {
    const dominio::Paquete* paq =
        obtenerPaquete(bodegaIds[static_cast<std::size_t>(indice)]);
    const std::string detalle =
        paq != nullptr ? (paq->destino + " | " + formatearPeso(paq->pesoKg) +
                          " | " + dominio::textoPrioridad(paq->prioridad))
                       : "";
    registrarLog(dominio::NivelLog::Exito,
                 "Busqueda Binaria O(log n): paquete #" +
                     std::to_string(objetivo) + " encontrado",
                 antes + " | Objetivo: #" + std::to_string(objetivo),
                 "Posicion " + std::to_string(indice + 1) + " en " +
                     std::to_string(comparaciones) + " comparaciones | " +
                     detalle);
    return true;
  }

  registrarLog(dominio::NivelLog::Alerta,
               "Busqueda Binaria O(log n): ID #" + std::to_string(objetivo) +
                   " inexistente",
               antes + " | Objetivo: #" + std::to_string(objetivo),
               "No encontrado tras " + std::to_string(comparaciones) +
                   " comparaciones (peor caso logaritmico).");
  return false;
}

bool EstadoApp::buscarLinealIdM2() {
  if (m2.animacionActiva) {
    finalizarAnimacionMergeSortM2();
  }

  const std::string antes =
      std::string("Catalogo ") + (bodegaOrdenada ? "ORDENADO" : "DESORDENADO") +
      " (" + std::to_string(bodegaIds.size()) + " paquetes)";

  int objetivo = 0;
  const std::string consultaLimpia = recortarEspacios(m2.consultaTexto);
  if (!parsearEnteroPositivo(consultaLimpia, objetivo)) {
    m2.filaEncontrada = -1;
    registrarLog(dominio::NivelLog::Error,
                 "Busqueda Lineal (ID): ingrese un ID numerico", antes,
                 "Entrada '" + consultaLimpia +
                     "' no es un ID numerico valido (o use Busqueda Lineal "
                     "por Destino).");
    return false;
  }

  int comparaciones = 0;
  const int indice =
      algoritmos::busquedaLineal(bodegaIds, objetivo, &comparaciones);

  m2.ultimoMetodo = "Lineal (ID)";
  m2.ultimoObjetivo = "#" + std::to_string(objetivo);
  m2.compLinealId = comparaciones;
  m2.filaEncontrada = indice;
  m2.ultimoEncontrado = (indice >= 0);

  if (indice >= 0) {
    const dominio::Paquete* paq =
        obtenerPaquete(bodegaIds[static_cast<std::size_t>(indice)]);
    const std::string detalle =
        paq != nullptr ? (paq->destino + " | " + formatearPeso(paq->pesoKg) +
                          " | " + dominio::textoPrioridad(paq->prioridad))
                       : "";
    registrarLog(dominio::NivelLog::Exito,
                 "Busqueda Lineal O(n): paquete #" + std::to_string(objetivo) +
                     " encontrado",
                 antes + " | Objetivo: #" + std::to_string(objetivo),
                 "Posicion " + std::to_string(indice + 1) + " en " +
                     std::to_string(comparaciones) + " comparaciones | " +
                     detalle);
    return true;
  }

  registrarLog(dominio::NivelLog::Alerta,
               "Busqueda Lineal O(n): ID #" + std::to_string(objetivo) +
                   " inexistente",
               antes + " | Objetivo: #" + std::to_string(objetivo),
               "No encontrado tras recorrer los " +
                   std::to_string(comparaciones) + " paquetes.");
  return false;
}

bool EstadoApp::buscarLinealDestinoM2() {
  if (m2.animacionActiva) {
    finalizarAnimacionMergeSortM2();
  }

  const std::string consultaLimpia = recortarEspacios(m2.consultaTexto);
  const std::string antes =
      "Bodega (" + std::to_string(bodegaIds.size()) +
      " paquetes) | Texto: '" + consultaLimpia + "'";

  if (consultaLimpia.empty()) {
    m2.filaEncontrada = -1;
    registrarLog(dominio::NivelLog::Error,
                 "Busqueda Lineal (Destino): texto vacio", antes,
                 "Escriba parte del nombre del destino (ej. CLINICA, FRANCA, "
                 "SUR).");
    return false;
  }

  const std::string aguja = aMayusculas(consultaLimpia);
  int comparaciones = 0;
  int encontrado = -1;

  for (std::size_t i = 0; i < bodegaIds.size(); ++i) {
    ++comparaciones;
    const dominio::Paquete* paq = obtenerPaquete(bodegaIds[i]);
    if (paq != nullptr &&
        aMayusculas(paq->destino).find(aguja) != std::string::npos) {
      encontrado = static_cast<int>(i);
      break;
    }
  }

  m2.ultimoMetodo = "Lineal (Destino)";
  m2.ultimoObjetivo = consultaLimpia;
  m2.compLinealDestino = comparaciones;
  m2.filaEncontrada = encontrado;
  m2.ultimoEncontrado = (encontrado >= 0);

  if (encontrado >= 0) {
    const dominio::Paquete* paq =
        obtenerPaquete(bodegaIds[static_cast<std::size_t>(encontrado)]);
    registrarLog(dominio::NivelLog::Exito,
                 "Busqueda Lineal por destino O(n): coincidencia encontrada",
                 antes,
                 "Paquete #" + std::to_string(paq->id) + " (" + paq->destino +
                     ") en fila " + std::to_string(encontrado + 1) + " tras " +
                     std::to_string(comparaciones) + " comparaciones.");
    return true;
  }

  registrarLog(dominio::NivelLog::Alerta,
               "Busqueda Lineal por destino O(n): sin coincidencias", antes,
               "Ningun paquete coincide con '" + consultaLimpia + "' tras " +
                   std::to_string(comparaciones) + " comparaciones.");
  return false;
}

// ======================= MODULO 3 =======================

bool EstadoApp::insertarRepartidorM3(bool alInicio) {
  const std::string antes =
      "Lista (" + std::to_string(listaRepartidores.tamano()) + "): " +
      formatearListaIds(listaRepartidores.aVector());

  int id = 0;
  if (!parsearEnteroPositivo(m3.repIdTexto, id)) {
    registrarLog(dominio::NivelLog::Error,
                 "Insercion en lista rechazada (ID invalido)", antes,
                 "Ingrese un ID numerico positivo para el repartidor.");
    return false;
  }

  if (listaRepartidores.contiene(id)) {
    registrarLog(dominio::NivelLog::Alerta,
                 "Repartidor #" + std::to_string(id) + " ya existe en la flota",
                 antes,
                 "El ID #" + std::to_string(id) +
                     " ya esta activo en la Lista Enlazada. Use un ID unico.");
    return false;
  }

  const std::string nombreLimpio = recortarEspacios(m3.repNombreTexto);
  if (nombreLimpio.empty()) {
    registrarLog(dominio::NivelLog::Error,
                 "Insercion en lista rechazada (nombre vacio)", antes,
                 "Escriba el nombre del repartidor antes de insertar.");
    return false;
  }

  std::string zonaLimpia = recortarEspacios(m3.repZonaTexto);
  if (zonaLimpia.empty()) {
    zonaLimpia = "General";
  }

  repartidoresPorId[id] = {id, nombreLimpio, zonaLimpia};
  if (alInicio) {
    listaRepartidores.insertarAlInicio(id);
  } else {
    listaRepartidores.insertarAlFinal(id);
  }
  m3.ultimoRepResaltado = id;

  if (id >= m3.siguienteIdRepartidor) {
    m3.siguienteIdRepartidor = id + 10;
  } else {
    while (listaRepartidores.contiene(m3.siguienteIdRepartidor)) {
      m3.siguienteIdRepartidor += 10;
    }
  }
  m3.repIdTexto = std::to_string(m3.siguienteIdRepartidor);

  const std::string posTxt = alInicio ? "al inicio (HEAD)" : "al final (TAIL)";
  registrarLog(dominio::NivelLog::Exito,
               "Repartidor #" + std::to_string(id) + " (" + nombreLimpio +
                   ") insertado " + posTxt + " [O(1)]",
               antes,
               "Lista (" + std::to_string(listaRepartidores.tamano()) + "): " +
                   formatearListaIds(listaRepartidores.aVector()));
  return true;
}

bool EstadoApp::eliminarRepartidorPorIdM3() {
  const std::string antes =
      "Lista (" + std::to_string(listaRepartidores.tamano()) + "): " +
      formatearListaIds(listaRepartidores.aVector());

  if (listaRepartidores.estaVacia()) {
    registrarLog(dominio::NivelLog::Alerta,
                 "Eliminacion omitida: lista de repartidores vacia", antes,
                 "No hay repartidores activos para dar de baja.");
    return false;
  }

  int id = 0;
  if (!parsearEnteroPositivo(m3.repIdTexto, id)) {
    registrarLog(dominio::NivelLog::Error,
                 "Eliminacion rechazada (ID invalido)", antes,
                 "Ingrese en 'ID' el identificador del repartidor a eliminar "
                 "(ej. 30).");
    return false;
  }

  const dominio::Repartidor* repPrev = obtenerRepartidor(id);
  const std::string nombreElim =
      repPrev != nullptr ? repPrev->nombre : ("ID #" + std::to_string(id));

  if (!listaRepartidores.eliminarNodo(id)) {
    registrarLog(dominio::NivelLog::Alerta,
                 "Repartidor #" + std::to_string(id) + " inexistente", antes,
                 "No se encontro el ID #" + std::to_string(id) +
                     " en la Lista Enlazada (recorrido O(n)).");
    return false;
  }

  if (m3.ultimoRepResaltado == id) {
    m3.ultimoRepResaltado = -1;
  }

  registrarLog(dominio::NivelLog::Exito,
               "Repartidor #" + std::to_string(id) + " (" + nombreElim +
                   ") eliminado [O(n)]",
               antes,
               "Lista (" + std::to_string(listaRepartidores.tamano()) + "): " +
                   formatearListaIds(listaRepartidores.aVector()) +
                   " (nodo liberado)");
  return true;
}

bool EstadoApp::registrarEntregaPushM3() {
  const std::string antes =
      "Pila LIFO tamano=" + std::to_string(pilaEntregas.tamano());

  int idPaq = 0;
  if (!parsearEnteroPositivo(m3.entregaPaqIdTexto, idPaq)) {
    registrarLog(dominio::NivelLog::Error,
                 "Push rechazado (ID de paquete invalido)", antes,
                 "Ingrese un ID numerico positivo de paquete (ej. 107).");
    return false;
  }

  dominio::RegistroEntrega reg;
  reg.idRegistro = siguienteIdEntrega++;
  reg.idPaquete = idPaq;
  reg.origenAccion = "Registro manual";

  const dominio::Paquete* paq = obtenerPaquete(idPaq);
  if (paq != nullptr) {
    reg.destino = paq->destino;
    reg.prioridad = paq->prioridad;
  } else {
    const std::string destLimpio = recortarEspacios(m3.entregaDestinoTexto);
    reg.destino = destLimpio.empty() ? "Entrega directa" : destLimpio;
    reg.prioridad = dominio::Prioridad::Regular;
  }

  const auto reps = listaRepartidores.aVector();
  if (!reps.empty()) {
    const int idRep = reps[static_cast<std::size_t>(reg.idRegistro - 1) %
                           reps.size()];
    const dominio::Repartidor* rep = obtenerRepartidor(idRep);
    reg.repartidor =
        rep != nullptr ? rep->nombre : ("Rep #" + std::to_string(idRep));
  } else {
    reg.repartidor = "Sin flota";
  }

  entregasPorId[reg.idRegistro] = reg;
  pilaEntregas.push(reg.idRegistro);
  m3.auditoriaActiva = false;

  registrarLog(dominio::NivelLog::Exito,
               "Entrega apilada (push O(1)): paquete #" + std::to_string(idPaq),
               antes,
               "Pila LIFO tamano=" + std::to_string(pilaEntregas.tamano()) +
                   " | Nuevo TOPE: #" + std::to_string(reg.idPaquete) + " -> " +
                   reg.destino + " (" + reg.repartidor + ")");
  return true;
}

bool EstadoApp::deshacerUltimaEntregaPopM3() {
  const std::string antes =
      "Pila LIFO tamano=" + std::to_string(pilaEntregas.tamano());

  int idReg = 0;
  if (!pilaEntregas.pop(idReg)) {
    registrarLog(dominio::NivelLog::Alerta,
                 "Deshacer omitido: Pila LIFO vacia", antes,
                 "No hay entregas registradas en la pila para revertir (pop).");
    return false;
  }

  m3.auditoriaActiva = false;
  const dominio::RegistroEntrega* reg = obtenerEntrega(idReg);
  if (reg == nullptr) {
    registrarLog(dominio::NivelLog::Exito, "Ultimo registro desapilado (pop)",
                 antes,
                 "Pila LIFO tamano=" + std::to_string(pilaEntregas.tamano()));
    return true;
  }

  std::string notaContinuidad;
  if (reg->origenAccion.find("Cola de Prioridad") != std::string::npos) {
    colaPrioridad.encolar(reg->idPaquete, static_cast<int>(reg->prioridad));
    notaContinuidad = " -> devuelto a Cola de Prioridad";
  } else if (reg->origenAccion.find("Cola Regular") != std::string::npos) {
    colaRegular.encolar(reg->idPaquete);
    notaContinuidad = " -> devuelto a Cola FIFO";
  }

  m3.ultimoDeshecho = "#" + std::to_string(reg->idPaquete) + " (" +
                      reg->destino + ")" + notaContinuidad;

  registrarLog(dominio::NivelLog::Exito,
               "Ultimo despacho deshecho (pop O(1)): #" +
                   std::to_string(reg->idPaquete),
               antes,
               "Pila LIFO tamano=" + std::to_string(pilaEntregas.tamano()) +
                   " | Revertido: " + m3.ultimoDeshecho);
  return true;
}

bool EstadoApp::auditarHistorialM3() {
  const auto idsReg = pilaEntregas.aVector();
  const std::string antes =
      "Pila LIFO con " + std::to_string(idsReg.size()) + " entregas";

  if (idsReg.empty()) {
    m3.auditoriaActiva = false;
    m3.resumenAuditoria = "Pila vacia: sin entregas para auditar.";
    registrarLog(dominio::NivelLog::Alerta,
                 "Auditoria LIFO: historial vacio", antes,
                 "Registre una entrega (push) o despache un pedido en Modulo "
                 "1.");
    return false;
  }

  std::ostringstream oss;
  for (std::size_t i = 0; i < idsReg.size(); ++i) {
    if (i > 0) {
      oss << " -> ";
    }
    const dominio::RegistroEntrega* reg = obtenerEntrega(idsReg[i]);
    if (reg != nullptr) {
      oss << "#" << reg->idPaquete;
    } else {
      oss << "R" << idsReg[i];
    }
  }

  m3.auditoriaActiva = true;
  m3.resumenAuditoria = "Orden inverso (TOPE->BASE): " + oss.str();

  registrarLog(dominio::NivelLog::Info,
               "Auditoria LIFO completada (" + std::to_string(idsReg.size()) +
                   " entregas en orden inverso)",
               antes, m3.resumenAuditoria);
  return true;
}

// ======================= MODULO 4 =======================

void EstadoApp::seleccionarNodoGrafoM4(int idSector, bool forzarDestino) {
  const std::string antes = "Origen=(" + std::to_string(m4.nodoOrigen) +
                            "), Destino=(" + std::to_string(m4.nodoDestino) +
                            ")";
  if (forzarDestino || !m4.seleccionandoOrigen) {
    m4.nodoDestino = idSector;
    if (!forzarDestino) {
      m4.seleccionandoOrigen = true;
    }
  } else {
    m4.nodoOrigen = idSector;
    m4.seleccionandoOrigen = false;
  }

  m4.tieneResultado = false;
  m4.resultado = algoritmos::ResultadoRuta{};

  const dominio::Sector* secO = obtenerSector(m4.nodoOrigen);
  const dominio::Sector* secD = obtenerSector(m4.nodoDestino);
  const std::string nomO = secO != nullptr ? secO->nombre : std::to_string(m4.nodoOrigen);
  const std::string nomD = secD != nullptr ? secD->nombre : std::to_string(m4.nodoDestino);

  registrarLog(dominio::NivelLog::Info, "Seleccion de sectores en grafo",
               antes,
               "Origen=(" + std::to_string(m4.nodoOrigen) + ") " + nomO +
                   " -> Destino=(" + std::to_string(m4.nodoDestino) + ") " +
                   nomD);
}

void EstadoApp::intercambiarOrigenDestinoM4() {
  const std::string antes = "Origen=(" + std::to_string(m4.nodoOrigen) +
                            "), Destino=(" + std::to_string(m4.nodoDestino) +
                            ")";
  std::swap(m4.nodoOrigen, m4.nodoDestino);
  m4.tieneResultado = false;
  m4.resultado = algoritmos::ResultadoRuta{};
  registrarLog(dominio::NivelLog::Info, "Origen y destino intercambiados",
               antes,
               "Origen=(" + std::to_string(m4.nodoOrigen) + "), Destino=(" +
                   std::to_string(m4.nodoDestino) + ")");
}

void EstadoApp::cambiarModoRedM4(dominio::ModoRed nuevoModo) {
  if (modoRed == nuevoModo) {
    return;
  }
  const std::string antes = "Red: " + nombreModoRed(modoRed);
  construirAristasGrafoSegunModo(nuevoModo);
  m4.tieneResultado = false;
  m4.resultado = algoritmos::ResultadoRuta{};

  std::string nota;
  if (nuevoModo == dominio::ModoRed::SoloPositivos) {
    nota = "Todas las vias tienen peso >= 0 (apto para Dijkstra y Bellman-Ford).";
  } else if (nuevoModo == dominio::ModoRed::ConBonificaciones) {
    nota = "Vias 2->4 (-2) y 3->5 (-3) activas sin ciclo negativo (use Bellman-Ford).";
  } else {
    nota = "Agregada arista 4->2 (-2): cierra ciclo negativo 2->4->2 (costo -4).";
  }

  registrarLog(nuevoModo == dominio::ModoRed::CicloNegativoDemo
                   ? dominio::NivelLog::Alerta
                   : dominio::NivelLog::Info,
               "Modo de red actualizado: " + nombreModoRed(nuevoModo), antes,
               nota);
}

bool EstadoApp::calcularRutaDijkstraM4() {
  const std::string antes =
      "Red: " + nombreModoRed(modoRed) + " | Consulta: (" +
      std::to_string(m4.nodoOrigen) + ") -> (" +
      std::to_string(m4.nodoDestino) + ")";

  // Llamada directa a tu algoritmo algoritmos::Grafo::Dijkstra.
  m4.algoritmoUsado = "Dijkstra";
  m4.resultado = grafoUrbano.Dijkstra(m4.nodoOrigen, m4.nodoDestino);
  m4.tieneResultado = true;

  if (m4.resultado.tienePesosNegativos) {
    registrarLog(dominio::NivelLog::Alerta,
                 "Dijkstra bloqueado: existen pesos negativos", antes,
                 m4.resultado.mensaje);
    return false;
  }

  if (m4.resultado.inalcanzable) {
    registrarLog(dominio::NivelLog::Alerta,
                 "Dijkstra: destino inalcanzable", antes,
                 m4.resultado.mensaje);
    return false;
  }

  if (!m4.resultado.exito) {
    registrarLog(dominio::NivelLog::Error, "Error en Dijkstra", antes,
                 m4.resultado.mensaje);
    return false;
  }

  registrarLog(dominio::NivelLog::Exito,
               "Dijkstra O((V+E) log V): ruta optima hallada", antes,
               "Costo=" + std::to_string(m4.resultado.costoTotal) + " | " +
                   formatearSecuenciaSectores(m4.resultado.camino));
  return true;
}

bool EstadoApp::calcularRutaBellmanFordM4() {
  const std::string antes =
      "Red: " + nombreModoRed(modoRed) + " | Consulta: (" +
      std::to_string(m4.nodoOrigen) + ") -> (" +
      std::to_string(m4.nodoDestino) + ")";

  // Llamada directa a tu algoritmo algoritmos::Grafo::BellmanFord.
  m4.algoritmoUsado = "Bellman-Ford";
  m4.resultado = grafoUrbano.BellmanFord(m4.nodoOrigen, m4.nodoDestino);
  m4.tieneResultado = true;

  if (m4.resultado.hayCicloNegativo) {
    registrarLog(dominio::NivelLog::Error,
                 "Bellman-Ford O(V*E): CICLO NEGATIVO DETECTADO", antes,
                 "Nodos afectados (-inf): " +
                     formatearListaIds(m4.resultado.nodosAfectadosCiclo));
    return false;
  }

  if (m4.resultado.inalcanzable) {
    registrarLog(dominio::NivelLog::Alerta,
                 "Bellman-Ford: destino inalcanzable", antes,
                 m4.resultado.mensaje);
    return false;
  }

  if (!m4.resultado.exito) {
    registrarLog(dominio::NivelLog::Error, "Error en Bellman-Ford", antes,
                 m4.resultado.mensaje);
    return false;
  }

  registrarLog(dominio::NivelLog::Exito,
               "Bellman-Ford O(V*E): ruta optima hallada", antes,
               "Costo neto=" + std::to_string(m4.resultado.costoTotal) +
                   " | " + formatearSecuenciaSectores(m4.resultado.camino));
  return true;
}

} // namespace estado
