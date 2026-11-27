# Guion Completo de Sustentación — Central Expres UD (`CentralLogistica`)

**Proyecto:** Diseño e Implementación de un Sistema de Gestión Logística y Enrutamiento Urbano Basado en Estructuras de Datos y Algoritmos Clásicos en C++  
**Asignatura:** Ciencias de la Computación I — Facultad de Ingeniería, Universidad Distrital Francisco José de Caldas  
**Autores:** Santiago Andrés Benavides Coral (20232020036) · Juan David Bejarano Cristancho (20232020056)  
**Duración sugerida:** 12 a 15 minutos (7 min explicación técnica + código / 6 min demo en vivo / 2 min cierre y preguntas)

---

## Cronograma Sugerido de Exposición

| Bloque | Tiempo | Tema principal | Qué mostrar |
| :--- | :---: | :--- | :--- |
| **1. Apertura y Arquitectura** | 2 min | Contexto, capas A-B-C-D y cómo se conecta MINIWIN sin mezclar lógica | Árbol de archivos + `src/main.cpp` |
| **2. Módulo 1 (Colas)** | 2.5 min | `ColaFIFO` y `ColaPrioridad` (Min-Heap estable) + demo en vivo | `algoritmos.cpp` (L26–L197) + Pestaña `[1]` |
| **3. Módulo 2 (Bodega)** | 2.5 min | `mergeSort`, `busquedaBinaria` y `busquedaLineal` + demo en vivo | `algoritmos.cpp` (L207–L322) + Pestaña `[2]` |
| **4. Módulo 3 (Flota y Pila)** | 2.5 min | `ListaEnlazada` dinámica y `Pila` LIFO (continuidad con M1) + demo en vivo | `algoritmos.cpp` (L328–L485) + Pestaña `[3]` |
| **5. Módulo 4 (Rutas Urbanas)** | 3.5 min | `Grafo`, `Dijkstra` vs `BellmanFord` y ciclo negativo + demo en vivo | `algoritmos.cpp` (L491–L825) + Pestaña `[4]` |
| **6. Cierre y Preguntas** | 2 min | Tabla Big-O, gestión de memoria y respuestas al jurado | Tabla resumen + `GUIA_SUSTENTACION.md` |

> **Consejo para 2 expositores:** Expositor 1 presenta **Bloques 1, 2 y 3** (arquitectura + colas + ordenamiento/búsqueda) y Expositor 2 presenta **Bloques 4, 5 y 6** (lista/pila + grafos + conclusiones).

---

## Bloque 1 — Introducción y Arquitectura en 4 Capas (~2 min)

### Qué decir (guion hablado)
> *"Buenos días/tardes. Presentamos **Central Expres - UD**, un sistema de gestión logística y enrutamiento urbano desarrollado en C++17 con interfaz gráfica usando la librería **MINIWIN**. El sistema simula el ciclo operativo completo de una central de mensajería exprés: recepción de pedidos con prioridad, organización y búsqueda de paquetes en bodega, administración dinámica de repartidores con auditoría de entregas, y cálculo de rutas óptimas sobre una red vial urbana.*
>
> *Una decisión clave de ingeniería fue **desacoplar completamente la lógica algorítmica de la interfaz gráfica** mediante una arquitectura de **4 capas**."*

### Qué señalar en el proyecto
1. **Capa A — Algoritmos y Estructuras Puras (`src/algoritmos/`):**
   - `NodoLista.hpp / .cpp`: clase `nodo` (`int dato`, `nodo* next`) usada por `ColaFIFO`, `ListaEnlazada` y `Pila`. *(Renombrado desde `nodo.hpp` para evitar colisión de mayúsculas/minúsculas con `Nodo.hpp` en Windows).*
   - `Nodo.hpp / .cpp`: clase `Nodo` (`int valor`, lista de adyacencia `std::vector<std::pair<Nodo*, int>>`) para los vértices del grafo.
   - `algoritmos.hpp / .cpp`: todas las estructuras y algoritmos clásicos, **cero dependencias de MINIWIN**. Trabajan sobre identificadores enteros (`int`).
2. **Capa B — Dominio, Estado y Bootstrapper (`src/estado/`):**
   - `Modelos.hpp`: entidades `Paquete`, `Repartidor`, `Sector`, `RegistroEntrega`, `LogTrazabilidad` y `ModoRed`.
   - `Bootstrapper.cpp` (`L71`): precarga en RAM 12 paquetes desordenados en bodega, 6 pedidos iniciales en colas, 6 repartidores activos, 2 entregas en pila y 8 sectores urbanos.
   - `EstadoApp.hpp / .cpp`: puente entre la GUI y Capa A; asocia los `int` con diccionarios de metadatos (`paquetesPorId`, `repartidoresPorId`, `entregasPorId`), valida entradas y actualiza la barra de trazabilidad `Antes -> Después`.
3. **Capa C — Interfaz Gráfica (`src/ui/` y `lib/miniwin/`):**
   - `MiniWinCapa.hpp / .cpp`: **único archivo en `src/` que llama a MINIWIN**; convierte el estado continuo del ratón en flancos de subida (`clicIzq`, `clicDer`) para que un clic no se dispare varias veces.
   - `ComponentesUI.hpp / .cpp`: widgets propios hechos con primitivas 2D (paneles con sombra, botones con hover, cajas de texto con `[<]` y `[X]`, flechas vectoriales, tablas con ventana deslizante y barra de estado).
   - `Renderizador.hpp / .cpp`: dibuja la ventana de `1160 × 720` a ~60 FPS con doble buffer Win32 (`BitBlt` en `lib/miniwin/miniwin.cpp`) sin parpadeo y con soporte UTF-8.
4. **Capa D — Manejo de Eventos (`src/eventos/` y `src/main.cpp`):**
   - `ControladorEventos.cpp` (`L378`): procesa atajos globales (`1-4`, `Tab`, `F5`, `Esc`) y delega clics y teclado al módulo activo.
   - `src/main.cpp` (`L20–L39`): bucle principal limpio `leerEntrada -> procesarEventos -> actualizarAnimaciones -> renderizarAplicacion -> esperarMs(16)`.

---

## Bloque 2 — Módulo 1: Recepción y Despacho (`ColaFIFO` y `ColaPrioridad`) (~2.5 min)

### 2.1 Algoritmos usados, ubicación y cómo se implementaron

#### A. Cola Regular FIFO (`algoritmos::ColaFIFO`)
- **Ubicación en código:**
  - Declaración: `src/algoritmos/algoritmos.hpp` (`L27–L48`)
  - Implementación: `src/algoritmos/algoritmos.cpp` (`L26–L90`)
  - Nodo base: `src/algoritmos/NodoLista.hpp` (`L12–L27`) y `NodoLista.cpp` (`L10–L33`)
  - Conexión con negocio: `src/estado/EstadoApp.cpp` (`ingresarPedidoM1` `L317`, `despacharSiguienteM1` `L391`)
- **Para qué se usa:** Recibir y despachar pedidos **Regulares (P3)** respetando estrictamente su orden de llegada (*First-In, First-Out*).
- **Cómo se implementó:** Lista simplemente enlazada dinámica con dos punteros: `frente_` (por donde se extrae) y `final_` (por donde se inserta).
  - `encolar` (`L36`): reserva un `new ::nodo(valor)`, lo enlaza después de `final_` y actualiza `final_` en **$O(1)$**.
  - `desencolar` (`L48`): lee `frente_->getDato()`, avanza `frente_`, ejecuta `delete eliminado` y libera memoria en **$O(1)$**.
  - Destructor `~ColaFIFO` (`L29`): vacía la cola liberando todos los nodos al cerrar.
  - `aVector()` (`L82`): recorrido no destructivo $O(n)$ usado por el `Renderizador` para pintar las tarjetas sin extraer elementos.

#### B. Cola de Prioridad (`algoritmos::ColaPrioridad`)
- **Ubicación en código:**
  - Declaración: `src/algoritmos/algoritmos.hpp` (`L55–L80`)
  - Implementación: `src/algoritmos/algoritmos.cpp` (`L96–L197`)
  - Conexión con negocio: `src/estado/EstadoApp.cpp` (`L365`, `L401`)
- **Para qué se usa:** Garantizar atención preferencial a paquetes **Médicos (prioridad 1)** y **Urgentes (prioridad 2)** por encima de la fila regular.
- **Cómo se implementó:** **Min-Heap binario** almacenado en un `std::vector<Elemento>` contiguo en memoria.
  - Cada `Elemento` guarda `{valor, prioridad, orden}`. El comparador `precede(a, b)` (`L99`) ordena por menor `prioridad` primero y, si empatan (ej. dos envíos Médicos), desempata por `orden` de llegada (estabilidad FIFO).
  - `encolar` (`L143`): inserta al final y llama a `subir(indice)` (`L107`) comparando con el padre `(i - 1) / 2` en **$O(\log n)$**.
  - `desencolar` (`L150`): extrae la raíz (`front()`), sube el último elemento a la raíz y llama a `bajar(0)` (`L119`) comparando hijos `2i+1` y `2i+2` en **$O(\log n)$**.
  - `aVectorOrdenado()` (`L188`): ordena una copia temporal para mostrar en pantalla el orden real de despacho sin alterar el montículo.

### 2.2 Qué decir en el código y en la demostración (Pestaña `[1]`)
> *"En el Módulo 1 tenemos dos colas visuales. Arriba la **Cola de Prioridad** (Min-Heap) para envíos Médicos y Urgentes, y abajo la **Cola Regular FIFO**. Como ven en `EstadoApp::despacharSiguienteM1` (`L401`), la regla de despacho siempre consulta primero `!colaPrioridad.estaVacia()` y solo atiende la cola FIFO cuando no quedan urgentes."*

**Pasos en vivo en la aplicación (Pestaña `[1] Cola y Prioridad`):**
1. Señala las **6 tarjetas precargadas** (3 en Prioridad con `SALE YA` en `#104 Médico`, y 3 en FIFO).
2. Haz **clic directo en una tarjeta** (p. ej. `#102 Urgente`): muestra que en la barra inferior `TRAZABILIDAD` aparece su turno exacto en el Min-Heap y su ID queda precargado en el buscador del Módulo 2.
3. **Ingresa un pedido nuevo:** deja el ID `113`, escribe destino `HOSPITAL CENTRAL`, ajusta peso con `+0.5`, elige **`Medico (1)`** y pulsa **`+ Ingresar pedido a cola`**.
   - Señala que entró en el Min-Heap **delante** de `#102 Urgente` (y después de `#104` y `#111` por desempate FIFO).
   - Señala el KPI superior **`Bodega:13`**: el nuevo pedido también se sincronizó con el inventario de bodega (`EstadoApp.cpp` `L354–L357`).
4. **Prueba caso borde (ID duplicado):** escribe `104` en ID y pulsa ingresar → muestra la **`ALERTA`** de ID duplicado.
5. **Despacha 2 veces** con **`>> Despachar siguiente pedido`**: muestra cómo salen primero los Médicos, se actualiza la tarjeta **`ULTIMO DESPACHO`**, se incrementa el KPI **`Pila`** (continuidad hacia Módulo 3) y se registra el `Antes` y `Después` en la barra inferior.
6. *(Opcional caso borde cola vacía)*: si despachas todos los pedidos, al volver a pulsar despachar aparece `ALERTA: Despacho omitido (colas vacias)`. Luego puedes presionar **`F5`** si quieres restaurar el estado inicial limpio antes de seguir (o conservar el despacho para verlo en el Módulo 3).

---

## Bloque 3 — Módulo 2: Bodega (`Merge Sort`, `Búsqueda Binaria` y `Búsqueda Lineal`) (~2.5 min)

### 3.1 Algoritmos usados, ubicación y cómo se implementaron

#### A. Ordenamiento con Merge Sort (`algoritmos::mergeSort`)
- **Ubicación en código:**
  - Declaración: `src/algoritmos/algoritmos.hpp` (`L87–L97`)
  - Implementación: `src/algoritmos/algoritmos.cpp` (`combinar` `L207`, `mergeSortRecursivo` `L242`, `mergeSort` `L260`)
  - Conexión y animación: `src/estado/EstadoApp.cpp` (`ordenarBodegaMergeSortM2` `L468`, `actualizarAnimaciones` `L224`)
- **Para qué se usa:** Ordenar ascendentemente por `ID` el catálogo de paquetes en bodega como requisito previo para habilitar la búsqueda binaria.
- **Cómo se implementó:** Paradigma **Divide y Vencerás** recursivo:
  - Reserva **un único vector auxiliar** de tamaño $n$ en `mergeSort` (`L266`) para evitar reservas repetitivas dentro de la recursión.
  - `mergeSortRecursivo` (`L242`) parte `[inicio, fin)` por `mitad = inicio + (fin - inicio) / 2` (evitando overflow) hasta subarreglos de longitud $\le 1$.
  - `combinar` (`L207`) mezcla ordenadamente `[inicio, mitad)` y `[mitad, fin)` de forma estable (`<=`) y guarda cada instantánea en `PasoMerge` para animar la tabla en pantalla.
  - Complejidad garantizada **$\Theta(n \log n)$** en mejor, promedio y peor caso; espacio **$O(n)$**.

#### B. Búsqueda Binaria (`algoritmos::busquedaBinaria`) y Búsqueda Lineal (`algoritmos::busquedaLineal`)
- **Ubicación en código:**
  - Declaración: `src/algoritmos/algoritmos.hpp` (`L101–L107`)
  - Implementación: `src/algoritmos/algoritmos.cpp` (`busquedaBinaria` `L274–L304`, `busquedaLineal` `L308–L322`)
  - Conexión y búsqueda por destino: `src/estado/EstadoApp.cpp` (`buscarBinariaIdM2` `L541`, `buscarLinealIdM2` `L609`, `buscarLinealDestinoM2` `L667`)
- **Para qué se usan:** Localizar paquetes por ID en $O(\log n)$ cuando el catálogo está ordenado, y ofrecer búsqueda secuencial $O(n)$ por ID o subcadena de destinatario incluso sin ordenar, contrastando empíricamente el número de comparaciones.
- **Cómo se implementaron:**
  - `busquedaBinaria` (`L274`): iterativa sobre rango semiabierto `[inicio, fin)`, descarta la mitad del arreglo en cada paso, incrementa `*comparaciones` y registra en `historialPivotes` cada valor medio evaluado (**$O(\log n)$** tiempo, **$O(1)$** espacio).
  - `busquedaLineal` (`L308`): recorre índice por índice contando comparaciones hasta hallar el objetivo o agotar el vector (**$O(n)$** tiempo, **$O(1)$** espacio).
  - `buscarLinealDestinoM2` (`L667`): normaliza a mayúsculas (`aMayusculas`) y busca coincidencia parcial en `Paquete::destino` en **$O(n)$**.

### 3.2 Qué decir y pasos en vivo (Pestaña `[2] Bodega y Busqueda`)
> *"Al entrar a Bodega notamos la insignia **`DESORDENADO`**, ya que el Bootstrapper cargó los 12 paquetes con IDs mezclados (`108, 102, 111...`). Esto nos permite demostrar primero la precondición de la búsqueda binaria y luego el salto de eficiencia frente a la lineal."*

**Pasos en vivo en la aplicación (Pestaña `[2]`):**
1. **Caso borde — Búsqueda binaria sin ordenar:** sin ordenar todavía, pulsa **`Busqueda Binaria por ID [O(log n)]`** → muestra **`ALERTA: Busqueda Binaria bloqueada: catalogo no ordenado`**.
2. **Búsqueda lineal sin ordenar:**
   - Escribe `110` (o haz clic en la fila del paquete `#110` para cargar su ID) y pulsa **`Busqueda Lineal por ID [O(n)]`** → encuentra el paquete en la fila 11 tras **11 comparaciones**.
   - Vacía la caja con **`[X]`** (o tecla `Supr`), escribe `FRANCA` y pulsa **`Busqueda Lineal por Destino [O(n)]`** → resalta en verde `#107 Zona Franca`.
3. **Ordenar con Merge Sort animado:** verifica `Animar pasos: ON` y pulsa **`Ordenar con Merge Sort [O(n log n)]`**.
   - Mientras corre la animación, señala cómo la tabla pinta de **azul** la mitad izquierda y de **ámbar** la mitad derecha que `combinar()` está mezclando, y cómo el cuadro inferior muestra el arreglo `Antes` y `Despues` al finalizar (insignia cambia a **`ORDENADO`**).
4. **Contraste empírico $O(\log n)$ vs $O(n)$:**
   - Escribe `110` y pulsa **`Busqueda Binaria por ID`** → la encuentra en solo **~2–3 comparaciones** y muestra los **`Pivotes Bin`** evaluados.
   - Pulsa **`Busqueda Lineal por ID`** sobre el mismo `110` → toma **10 comparaciones**. Señala las barras comparativas en el panel inferior derecho.
5. **Caso borde — ID inexistente:** escribe `999`, ejecuta **Binaria** (≤ 4 comparaciones, peor caso $\lceil \log_2 n \rceil$) y luego **Lineal** (recorre los 12/13 paquetes completos).

---

## Bloque 4 — Módulo 3: Flota y Auditoría (`ListaEnlazada` y `Pila` LIFO) (~2.5 min)

### 4.1 Algoritmos usados, ubicación y cómo se implementaron

#### A. Lista Enlazada de Repartidores (`algoritmos::ListaEnlazada`)
- **Ubicación en código:**
  - Declaración: `src/algoritmos/algoritmos.hpp` (`L113–L133`)
  - Implementación: `src/algoritmos/algoritmos.cpp` (`L328–L422`)
  - Conexión con negocio: `src/estado/EstadoApp.cpp` (`insertarRepartidorM3` `L730`, `eliminarRepartidorPorIdM3` `L794`)
- **Para qué se usa:** Administrar dinámicamente la nómina de repartidores activos en turno (altas al inicio/final y bajas por ID).
- **Cómo se implementó:** Lista simplemente enlazada con memoria dinámica (`new ::nodo` / `delete`) manteniendo punteros `cabeza_` (`HEAD`) y `cola_` (`TAIL`).
  - `insertarAlInicio` (`L343`) e `insertarAlFinal` (`L354`): ambas en **$O(1)$** gracias a mantener `cola_`.
  - `eliminarNodo` (`L367`): recorre con doble puntero `(anterior, actual)` hasta hallar el ID, re-enlaza saltando el nodo, actualiza `cabeza_` o `cola_` si corresponde y libera memoria con `delete actual` en **$O(n)$**.
  - `~ListaEnlazada` (`L332`): recorre toda la lista aplicando `delete` para garantizar cero fugas de memoria.

#### B. Pila LIFO de Entregas (`algoritmos::Pila`)
- **Ubicación en código:**
  - Declaración: `src/algoritmos/algoritmos.hpp` (`L139–L158`)
  - Implementación: `src/algoritmos/algoritmos.cpp` (`L428–L485`)
  - Conexión con negocio: `src/estado/EstadoApp.cpp` (`registrarEntregaPushM3` `L843`, `deshacerUltimaEntregaPopM3` `L896`, `auditarHistorialM3` `L940`)
- **Para qué se usa:** Registrar cada entrega completada en orden *Last-In, First-Out* para auditar los despachos más recientes primero o deshacer (`pop`) el último despacho ante una novedad operativa.
- **Cómo se implementó:** Lista enlazada por puntero único `tope_`:
  - `push` (`L438`): inserta nuevo nodo sobre `tope_` en **$O(1)$**.
  - `pop` (`L446`): extrae `tope_->getDato()`, mueve `tope_` al siguiente y ejecuta `delete eliminado` en **$O(1)$**.
  - `aVector()` (`L477`): recorre de `tope_` hacia `nullptr` en **$O(n)$** produciendo naturalmente el orden cronológico inverso para auditoría.
  - **Continuidad con Módulo 1 (`EstadoApp.cpp` `L919–L925`):** si el registro desapilado provino de `Cola de Prioridad` o `Cola Regular`, al deshacerlo se re-encola automáticamente en su cola original.

### 4.2 Qué decir y pasos en vivo (Pestaña `[3] Flota y Auditoria`)
> *"A la izquierda visualizamos la **Lista Enlazada** real en memoria: cada caja muestra sus datos y su compartimento de puntero `*next` conectado con flechas desde `HEAD` hasta `TAIL` y `NULL`. A la derecha tenemos la **Pila LIFO** vertical conectada directamente con los despachos del Módulo 1."*

**Pasos en vivo en la aplicación (Pestaña `[3]`):**
1. **Inserciones $O(1)$:**
   - Con ID `70` (`CARLOS ROJAS`, `NORTE`), pulsa **`+ Insertar final [O(1)]`** → señala que la insignia `TAIL` se mueve al nuevo nodo `#70`.
   - Escribe ID `5` (nombre `ELENA`), pulsa **`+ Insertar inicio [O(1)]`** → señala que `HEAD` ahora es `#5`.
2. **Inspección y eliminación $O(n)$:**
   - Haz **clic directo sobre el nodo `#30` (`Sofia Ruiz`)**: muestra cómo carga sus datos en el formulario y reporta `Puntero next -> #40` en la barra de trazabilidad.
   - Pulsa **`- Eliminar por ID [O(n)]`** → `#30` desaparece, `#20` apunta directamente a `#40` y la memoria del nodo fue liberada con `delete`.
   - **Caso borde — ID inexistente:** escribe `999` y pulsa eliminar → muestra `ALERTA: Repartidor #999 inexistente`.
3. **Auditoría y reversión con continuidad (Pila LIFO):**
   - Señala que en el **`TOPE`** de la pila ya aparece el último paquete despachado desde el Módulo 1.
   - Pulsa **`Auditar historial (orden inverso)`** → los bloques se numeran `[1], [2], [3]...` desde el tope hasta la base y se imprime la secuencia inversa.
   - Pulsa **`<< Deshacer ultimo despacho (pop)`** → muestra el mensaje `devuelto a Cola de Prioridad / FIFO` y cambia un segundo a la pestaña **`[1]`** para evidenciar que el paquete volvió a su cola.
   - Regresa a **`[3]`**, prueba **`+ Registrar entrega (push)`** con ID `107`, y luego pulsa **`<< Deshacer`** repetidamente hasta vaciar la pila (`PILA VACIA (TOPE = NULL)`) y pulsa una vez más para demostrar el **caso borde de pila vacía**.

---

## Bloque 5 — Módulo 4: Rutas Urbanas (`Grafo`, `Dijkstra` y `Bellman-Ford`) (~3.5 min)

### 5.1 Algoritmos usados, ubicación y cómo se implementaron

#### A. Grafo Dirigido y Ponderado (`algoritmos::Grafo` + `::Nodo`)
- **Ubicación en código:**
  - Vertice: `src/algoritmos/Nodo.hpp` (`L13–L29`) y `Nodo.cpp` (`L13–L53`)
  - Grafo: `src/algoritmos/algoritmos.hpp` (`L166–L203`) y `algoritmos.cpp` (`L491–L570`)
  - Carga de 8 sectores y 3 modos de red: `src/estado/Bootstrapper.cpp` (`L21–L68`, `L120–L135`)
- **Para qué se usa:** Modelar la red vial de 8 sectores (*Central de Acopio, Bodega Norte, Zona Industrial, Centro Histórico, Sector Residencial, Terminal Sur, Zona Franca y Parque Logístico*) con sentidos de vía y costos positivos o bonificaciones negativas.
- **Cómo se implementó:** Representación por **listas de adyacencia** (`std::vector<std::pair<Nodo*, int>>` por vértice). Soporta agregar/eliminar aristas dinámicamente para alternar entre los 3 escenarios viales y libera todos los vértices en `~Grafo()` (`L494`).

#### B. Algoritmo de Dijkstra (`algoritmos::Grafo::Dijkstra`)
- **Ubicación en código:** `src/algoritmos/algoritmos.cpp` (`L575–L689`), invocado desde `EstadoApp::calcularRutaDijkstraM4` (`L1054`).
- **Para qué se usa:** Hallar la ruta de menor costo cuando todas las vías tienen pesos **no negativos ($\ge 0$)**.
- **Cómo se implementó:**
  1. **Validación previa (`L584–L594`):** recorre todas las aristas; si existe algún peso `< 0`, retorna `tienePesosNegativos = true` impidiendo un resultado incorrecto y sugiriendo Bellman-Ford.
  2. **Búsqueda voraz con min-heap (`L596–L640`):** usa `std::priority_queue` mínima `(distancia, idNodo)`, descarta entradas obsoletas (`distanciaActual != distancias[valorActual]`), aplica **parada temprana** al extraer `destino`, relaja aristas registrando `predecesores`, `iteraciones` y `relajaciones`.
  3. **Reconstrucción (`L650–L681`):** reconstruye el camino desde `destino` hasta `origen`, invierte el vector y recolecta `pesosTramos`.
  - Complejidad: **$O((V + E) \log V)$** en tiempo, **$O(V + E)$** en espacio.

#### C. Algoritmo de Bellman-Ford (`algoritmos::Grafo::BellmanFord`)
- **Ubicación en código:** `src/algoritmos/algoritmos.cpp` (`L694–L825`), invocado desde `EstadoApp::calcularRutaBellmanFordM4` (`L1092`).
- **Para qué se usa:** Calcular la ruta de menor costo neto cuando existen **pesos negativos** (descuentos de peaje / bonificaciones de tiempo) y detectar **ciclos negativos** que invalidan la red.
- **Cómo se implementó:**
  1. **Fase 1 — Relajación (`L711–L736`):** relaja todas las aristas hasta $V - 1$ rondas (con corte temprano si una ronda completa no genera cambios), guardando `predecesores`, `iteraciones` y `relajaciones`.
  2. **Fase 2 — Detección de ciclo negativo (`L739–L753`):** realiza la **pasada $V$**; si alguna arista aún reduce una distancia, existe un ciclo negativo alcanzable.
  3. **Fase 3 — Propagación BFS (`L756–L776`):** a partir de los nodos detectados en el ciclo, ejecuta un recorrido en anchura (`std::queue`) para marcar todos los vértices alcanzables desde el ciclo con distancia $-\infty$ (`nodosAfectadosCiclo`).
  4. **Fase 4 — Reconstrucción (`L787–L818`):** si no hay ciclo negativo y el destino es alcanzable, reconstruye `camino` y `pesosTramos`.
  - Complejidad: **$O(V \cdot E)$** en tiempo, **$O(V + E)$** en espacio.

### 5.2 Qué decir y pasos en vivo (Pestaña `[4] Rutas Urbanas`)
> *"Finalmente, en el Módulo 4 representamos los 8 sectores como un grafo dirigido y ponderado. En la base del mapa tenemos un **Inspector de Adyacencia en vivo** que muestra las aristas salientes del nodo bajo el cursor, y separamos con desplazamiento perpendicular las aristas bidireccionales como `4 <-> 7` para que nunca se tapen."*

**Pasos en vivo en la aplicación (Pestaña `[4]`):**
1. **Pasa el ratón por los nodos `(1)` y `(4)`** señalando el **Inspector Nodo** abajo (demuestra la lista de adyacencia viva).
2. **Escenario 1 — Red con bonificaciones (`-`) [por defecto, Origen `(1)` → Destino `(8)`]:**
   - Señala las aristas negativas en **magenta**: `2 -> 4 (-2)` y `3 -> 5 (-3)` (sin ciclos negativos).
   - Pulsa **`Calcular ruta con Dijkstra`** → aparece **`ALERTA: DIJKSTRA NO VALIDO`** explicando que hay pesos negativos y sugiriendo Bellman-Ford.
   - Pulsa **`Calcular ruta con Bellman-Ford`** → se resalta en verde grueso la ruta **`1(+4)->2(-2)->4(+5)->8`** con **Costo Total = 7**, mostrando iteraciones y relajaciones.
3. **Escenario 2 — Red solo positiva (`pesos >= 0`):**
   - Pulsa **`Red solo positiva (pesos >= 0)`** (`2->4` pasa a `+3`, `3->5` a `+2`).
   - Pulsa **`Calcular ruta con Dijkstra`** → calcula la ruta **`1(+6)->4(+5)->8`** con **Costo Total = 11** (si pulsas Bellman-Ford también da 11, validando equivalencia).
4. **Caso borde — Destino inalcanzable:**
   - Pulsa **`O <-> D`** (ahora Origen = `(8) Parque Logistico`, Destino = `(1) Central de Acopio`; nota que el nodo `8` no tiene aristas salientes).
   - Pulsa **`Calcular ruta con Dijkstra`** (o Bellman-Ford) → muestra **`DESTINO INALCANZABLE`** (`Distancia = +infinito`).
5. **Caso borde — Ciclo negativo DEMO:**
   - Pulsa **`O <-> D`** (vuelve a `1 -> 8`).
   - Pulsa **`Activar ciclo negativo DEMO`** → aparece la nueva arista `4 -> 2 (-2)`, formando el ciclo negativo `2 -> 4 (-2) -> 2 (-2)` de costo `-4`.
   - Pulsa **`Calcular ruta con Bellman-Ford`** → dispara **`CICLO NEGATIVO DETECTADO!`**, marca en rojo las aristas del ciclo y coloca la insignia **`-INF`** sobre todos los nodos afectados propagados por el BFS (`[2, 4, 6, 7, 8]`).

---

## Bloque 6 — Tabla Maestra de Ubicación y Complejidades (Big-O)

| Módulo | Estructura / Algoritmo | Ubicación exacta (`src/algoritmos/`) | Uso en la Central | Complejidad Temporal | Complejidad Espacial |
| :--- | :--- | :--- | :--- | :--- | :---: |
| **M1** | `ColaFIFO` | `algoritmos.hpp` L27 · `algoritmos.cpp` L26–L90 | Envíos regulares en orden de llegada | `encolar`/`desencolar`: $O(1)$ | $O(n)$ |
| **M1** | `ColaPrioridad` | `algoritmos.hpp` L55 · `algoritmos.cpp` L96–L197 | Envíos Médicos (P1) y Urgentes (P2) | `encolar`/`desencolar`: $O(\log n)$<br>`consultar`: $O(1)$ | $O(n)$ |
| **M2** | `mergeSort` | `algoritmos.hpp` L96 · `algoritmos.cpp` L207–L270 | Ordenar catálogo de bodega por ID | Mejor / Prom / Peor: $\Theta(n \log n)$ | $O(n)$ |
| **M2** | `busquedaBinaria` | `algoritmos.hpp` L101 · `algoritmos.cpp` L274–L304 | Búsqueda por ID sobre bodega ordenada | Peor / Prom: $O(\log n)$<br>Mejor: $O(1)$ | $O(1)$ |
| **M2** | `busquedaLineal` | `algoritmos.hpp` L106 · `algoritmos.cpp` L308–L322 | Búsqueda por ID o destino sin orden | Peor / Prom: $O(n)$<br>Mejor: $O(1)$ | $O(1)$ |
| **M3** | `ListaEnlazada` | `algoritmos.hpp` L113 · `algoritmos.cpp` L328–L422 | Plantilla dinámica de repartidores | `insertarInicio`/`Final`: $O(1)$<br>`eliminarNodo`: $O(n)$ | $O(n)$ |
| **M3** | `Pila` (LIFO) | `algoritmos.hpp` L139 · `algoritmos.cpp` L428–L485 | Historial, auditoría y deshacer despacho | `push`/`pop`: $O(1)$<br>Auditoría (`aVector`): $O(n)$ | $O(n)$ |
| **M4** | `Grafo::Dijkstra` | `algoritmos.hpp` L199 · `algoritmos.cpp` L575–L689 | Ruta óptima con pesos $\ge 0$ | $O((V + E) \log V)$ | $O(V + E)$ |
| **M4** | `Grafo::BellmanFord` | `algoritmos.hpp` L202 · `algoritmos.cpp` L694–L825 | Ruta óptima con bonificaciones y ciclos `-` | $O(V \cdot E)$ | $O(V + E)$ |

---

## Referencia Rápida de Atajos y Controles para la Demo

- **`1`, `2`, `3`, `4`**: Cambiar entre los 4 módulos (cuando no hay ninguna caja de texto enfocada).
- **`F5`** o botón **`Reset [F5]`**: Limpia la memoria dinámica y restaura el estado inicial del `Bootstrapper`.
- **`Tab`**: Rotar el foco entre las cajas de texto del módulo actual.
- **`Enter`**: Confirma ingreso de pedido (Módulo 1) o dispara búsqueda (Módulo 2).
- **`Backspace` / `Flecha Izquierda` / botón `[<]`**: Borrar el último carácter.
- **`Supr` (`Delete`) / botón `[X]` / clic derecho en `[<]`**: Vaciar por completo la caja de texto.
- **Clic directo en tarjetas (M1), filas (M2) y nodos (M3)**: Inspecciona el elemento y precarga su ID.
- **Clic izquierdo / derecho en nodos del Grafo (M4)**: Fija Origen (clic izq., alternando con Destino) o fuerza Destino (clic der.).
- **`ESC`** o **`X`**: Cierre ordenado de la ventana y liberación de memoria.

---

## Preguntas Frecuentes del Jurado y Respuestas Modelo

1. **¿Por qué implementaron `ColaPrioridad` con un Min-Heap sobre `std::vector` en lugar de una lista enlazada ordenada?**  
   *Respuesta:* En una lista enlazada ordenada la inserción sería $O(n)$, mientras que un Min-Heap binario garantiza $O(\log n)$ tanto en `encolar` (`subir`) como en `desencolar` (`bajar`), además de ofrecer mejor localidad de caché al almacenar los nodos contiguamente. Agregamos el campo `orden` (`uint64_t`) para preservar el orden FIFO cuando dos envíos tienen la misma prioridad.

2. **¿Por qué eligieron `Merge Sort` y no `Quick Sort` para ordenar la bodega?**  
   *Respuesta:* `Merge Sort` garantiza **$\Theta(n \log n)$ incluso en el peor caso** (por ejemplo, si el catálogo ya estuviera casi ordenado o invertido, donde Quick Sort ingenuo degrada a $O(n^2)$), es **estable** y su patrón de división y mezcla se presta directamente para animar paso a paso cada subarreglo combinado.

3. **¿Por qué `Dijkstra` no funciona con pesos negativos y cómo detecta `Bellman-Ford` un ciclo negativo?**  
   *Respuesta:* Dijkstra es voraz: asume que una vez extraído un nodo del min-heap su distancia nunca podrá disminuir, suposición que se rompe si un tramo posterior tiene peso negativo. Bellman-Ford relaja todas las aristas hasta $V - 1$ veces (longitud máxima de cualquier camino simple). Si en la **pasada $V$** alguna arista aún logra reducir una distancia (`L747–L748`), se demuestra matemáticamente la existencia de un ciclo negativo, y lanzamos un BFS (`L755`) para identificar todos los nodos cuya distancia diverge a $-\infty$.

4. **¿Cómo aseguran que no existan fugas de memoria al usar punteros crudos (`new` / `delete`)?**  
   *Respuesta:* Cada estructura dueña de nodos dinámicos (`ColaFIFO`, `ListaEnlazada`, `Pila` y `Grafo`) libera los nodos individualmente al extraer/eliminar (`desencolar`, `eliminarNodo`, `pop`) y recorre todos los elementos restantes en su **destructor** (`~ColaFIFO`, `~ListaEnlazada`, `~Pila`, `~Grafo`). Además, se eliminaron el constructor de copia y el operador de asignación (`= delete`) para prevenir dobles liberaciones (*double free*).

5. **¿Cómo evitaron que MINIWIN contaminara la lógica de los algoritmos y por qué la interfaz no parpadea?**  
   *Respuesta:* Toda llamada a `miniwin::*` está encapsulada en un único archivo (`src/ui/MiniWinCapa.cpp`), de modo que `src/algoritmos/`, `src/estado/` y `src/eventos/` compilan y pueden probarse de forma totalmente independiente sin entorno gráfico. En `lib/miniwin/miniwin.cpp` implementamos **doble buffer real** (`g_hDCDraw` → `g_hDCFront` vía `BitBlt` sincronizado con `std::mutex`) e interceptamos `WM_ERASEBKGND`, eliminando por completo el parpadeo.

6. **¿Qué ocurre si un pedido ingresado en el Módulo 1 se despacha y luego se deshace en el Módulo 3?**  
   *Respuesta:* Al despachar en Módulo 1 se registra en `Pila` con su `origenAccion` y prioridad original. Al pulsar `Deshacer (pop)` en Módulo 3 (`EstadoApp.cpp` `L919`), el sistema lee ese metadato y vuelve a encolar automáticamente el paquete en `ColaPrioridad` o `ColaFIFO`, manteniendo persistencia y coherencia durante toda la sesión.
