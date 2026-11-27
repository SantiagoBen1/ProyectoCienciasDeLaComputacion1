# Guía Completa de Sustentación — CentralLogistica (Ciencias de la Computación I)

Esta guía resume la arquitectura, estructuras de datos, complejidades algorítmicas (Big-O), guion paso a paso de demostración en vivo y preguntas frecuentes de sustentación para **Central Expres - UD (`CentralLogistica`)**.

---

## 1. Arquitectura en 4 Capas Desacopladas

1. **Capa A — Estructuras y Algoritmos Puros (`src/algoritmos/`)**:
   - `NodoLista.hpp / .cpp`: Clase `nodo` (`int dato`, `nodo* next`) para estructuras lineales dinámicas.
   - `Nodo.hpp / .cpp`: Clase `Nodo` (`int valor`, `std::vector<std::pair<Nodo*, int>> adyacentes`) para vértices del grafo.
   - `algoritmos.hpp / .cpp`: Implementación de `ColaFIFO`, `ColaPrioridad` (Min-Heap), `mergeSort`, `busquedaBinaria`, `busquedaLineal`, `ListaEnlazada`, `Pila` y `Grafo` (`Dijkstra` y `BellmanFord`). Cero dependencias de `MiniWin`.
2. **Capa B — Estado y Dominio (`src/estado/`)**:
   - `Modelos.hpp`: Entidades del negocio (`Paquete`, `Repartidor`, `RegistroEntrega`, `Sector`, `LogTrazabilidad`).
   - `Bootstrapper.cpp`: Precarga en RAM de 12 paquetes, 6 pedidos en colas, 6 repartidores, 2 entregas en pila y 8 sectores urbanos con 3 modos de red vial.
   - `EstadoApp.hpp / .cpp`: Lógica de negocio, validación de entradas y registro de trazabilidad (`Antes` vs. `Después`).
3. **Capa C — Interfaz Gráfica (`src/ui/` y `lib/miniwin/`)**:
   - `MiniWinCapa.hpp / .cpp`: Único punto de contacto con `miniwin.h`.
   - `ComponentesUI.hpp / .cpp`: Controles reutilizables (paneles, botones, cajas de texto, tablas con scroll automático, flechas, barra de estado).
   - `Renderizador.hpp / .cpp`: Renderizado de los 4 módulos a ~60 FPS con doble buffer Win32 (`BitBlt`) sin parpadeo.
4. **Capa D — Controlador de Eventos (`src/eventos/`)**:
   - `ControladorEventos.hpp / .cpp`: Enrutamiento de clics y atajos de teclado (`1`-`4`, `Tab`, `Enter`, `Supr`, `F5`) hacia `EstadoApp`.

---

## 2. Resumen de Estructuras, Algoritmos y Complejidad (Big-O)

| Módulo | Estructura / Algoritmo | Implementación en código | Complejidad Temporal | Complejidad Espacial |
| :--- | :--- | :--- | :--- | :--- |
| **Módulo 1** | `ColaFIFO` | Lista enlazada con punteros `frente_` y `final_` | `encolar`: $O(1)$<br>`desencolar`: $O(1)$ | $O(n)$ |
| **Módulo 1** | `ColaPrioridad` | Min-Heap binario sobre `std::vector<Elemento>` con desempate FIFO (`orden`) | `encolar` (`subir`): $O(\log n)$<br>`desencolar` (`bajar`): $O(\log n)$<br>`consultar`: $O(1)$ | $O(n)$ |
| **Módulo 2** | `mergeSort` | Divide y Vencerás recursivo + vector auxiliar y captura de `PasoMerge` | Mejor / Prom / Peor: $\Theta(n \log n)$ | $O(n)$ |
| **Módulo 2** | `busquedaBinaria` | Búsqueda dicotómica iterativa sobre `bodegaIds` ordenado + traza de pivotes | Peor / Prom: $O(\log n)$<br>Mejor: $O(1)$ | $O(1)$ |
| **Módulo 2** | `busquedaLineal` | Recorrido secuencial por ID o por subcadena de texto en `Destino` | Peor / Prom: $O(n)$<br>Mejor: $O(1)$ | $O(1)$ |
| **Módulo 3** | `ListaEnlazada` | Lista simple con punteros `cabeza_` (`HEAD`) y `cola_` (`TAIL`) | `insertarAlInicio`: $O(1)$<br>`insertarAlFinal`: $O(1)$<br>`eliminarNodo`: $O(n)$ | $O(n)$ |
| **Módulo 3** | `Pila` (LIFO) | Lista enlazada con puntero `tope_` + reversión de despachos | `push`: $O(1)$<br>`pop`: $O(1)$<br>Auditoría (`aVector`): $O(n)$ | $O(n)$ |
| **Módulo 4** | `Grafo::Dijkstra` | Lista de adyacencia + Min-Heap (`std::priority_queue`) + parada temprana | $O((V + E) \log V)$<br>*(Solo pesos $\ge 0$)* | $O(V + E)$ |
| **Módulo 4** | `Grafo::BellmanFord` | Relajación en hasta $V-1$ rondas + pasada $V$ + BFS de ciclos negativos | $O(V \cdot E)$<br>*(Soporta pesos $< 0$ y detecta ciclos)* | $O(V + E)$ |

---

## 3. Atajos de Teclado y Controles Interactivos para la Demostración

- **`1`, `2`, `3`, `4`**: Cambiar entre los 4 módulos (cuando no hay una caja de texto enfocada).
- **`F5`** o botón **`Reset [F5]`** (barra superior): Reiniciar toda la simulación al estado original del `Bootstrapper`.
- **`Tab`**: Alternar el foco entre las cajas de texto del módulo actual.
- **`Retroceso` (`Backspace`) / `Flecha Izquierda` / clic izquierdo en `<`**: Borrar el último carácter.
- **`Supr` (`Delete`) / clic derecho en `<`**: Vaciar por completo la caja de texto activa.
- **Clic directo en filas de la tabla (Módulo 2)**: Selecciona el paquete y carga su ID en el buscador.
- **Clic directo en nodos de la Lista Enlazada (Módulo 3)**: Selecciona el repartidor, muestra su puntero `next` y carga su ID, nombre y zona en el formulario.
- **Clic izquierdo / derecho en nodos del Grafo (Módulo 4)**: Selecciona el sector de Origen (clic izq.) o Destino (clic der.).
