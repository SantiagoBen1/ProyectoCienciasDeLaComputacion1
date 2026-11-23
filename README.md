# CentralLogistica

Aplicación de escritorio desarrollada en **C++** para el curso **Ciencias de la Computación I** en la **Universidad Distrital Francisco José de Caldas**, orientada a la simulación y gestión de operaciones de **logística urbana y planificación de rutas**.

El sistema combina estructuras de datos dinámicas y algoritmos sobre grafos y listas con una interfaz gráfica interactiva construida sobre la biblioteca **MiniWin**, permitiendo visualizar redes de nodos, gestionar pedidos o puntos de entrega, calcular recorridos óptimos y analizar el estado operativo en tiempo real.

## ¿De qué trata la aplicación?

**CentralLogistica** modela una red logística urbana compuesta por nodos (centros de distribución, estaciones o puntos de entrega) y conexiones ponderadas entre ellos (rutas con distancias o costos asociados). A través de su entorno visual, el usuario puede:

* **Visualizar la red logística:** Representación gráfica de nodos, conexiones, estados y elementos activos sobre un lienzo interactivo.

* **Calcular y optimizar rutas:** Aplicación de algoritmos de búsqueda y caminos mínimos sobre grafos para determinar recorridos eficientes entre puntos de origen y destino.

* **Gestionar entidades y ordenamientos:** Organización y consulta de registros, nodos y listas enlazadas mediante algoritmos de ordenamiento y estructuras personalizadas.

* **Interactuar en tiempo real:** Control mediante eventos de teclado y ratón para inspeccionar elementos, ejecutar simulaciones y actualizar el estado del sistema paso a paso.

## Estructura del proyecto

```
ProyectoCienciasDeLaComputacion1/
├── CentralLogistica.exe
├── Makefile
├── build.bat
├── lib/
│   └── miniwin/
│       ├── miniwin.h
│       └── miniwin.cpp
└── src/
    ├── main.cpp
    ├── algoritmos/
    │   ├── Nodo.hpp
    │   ├── Nodo.cpp
    │   ├── NodoLista.hpp
    │   ├── NodoLista.cpp
    │   ├── algoritmos.hpp
    │   └── algoritmos.cpp
    ├── estado/
    │   ├── Modelos.hpp
    │   ├── EstadoApp.hpp
    │   ├── EstadoApp.cpp
    │   └── Bootstrapper.cpp
    ├── eventos/
    │   ├── ControladorEventos.hpp
    │   └── ControladorEventos.cpp
    └── ui/
        ├── Paleta.hpp
        ├── ComponentesUI.hpp
        ├── ComponentesUI.cpp
        ├── MiniWinCapa.hpp
        ├── MiniWinCapa.cpp
        ├── Renderizador.hpp
        └── Renderizador.cpp

```

## Arquitectura y módulos

* **`src/main.cpp`**: Punto de entrada de la aplicación; inicializa el estado base y ejecuta el bucle principal de eventos y renderizado.

* **`src/algoritmos/`**: Implementación desde cero de estructuras enlazadas (`Nodo`, `NodoLista`) y algoritmos de grafos, recorridos y ordenamiento (`algoritmos.hpp`, `algoritmos.cpp`).

* **`src/estado/`**: Definición de los modelos de dominio (`Modelos.hpp`), carga de escenarios iniciales (`Bootstrapper.cpp`) y administración centralizada del estado de la aplicación (`EstadoApp`).

* **`src/eventos/`**: Procesamiento de las interacciones del usuario (`ControladorEventos`) y traducción de acciones hacia cambios en el estado del sistema.

* **`src/ui/`**: Capa de presentación visual que incluye la paleta de colores (`Paleta.hpp`), controles reutilizables (`ComponentesUI`), envoltorio sobre MiniWin (`MiniWinCapa`) y motor de dibujo (`Renderizador`).

* **`lib/miniwin/`**: Biblioteca gráfica `miniwin` encargada de la ventana y primitivas de dibujo 2D.

## Compilación y ejecución

### En Windows (`build.bat`)

Ejecuta el script de compilación desde la terminal de Windows (requiere `g++` / MinGW en el `PATH`):

```
build.bat

```

También puedes ejecutar directamente el binario precompilado incluido en el repositorio:

```
CentralLogistica.exe

```

### Mediante `Makefile`

Desde la raíz del proyecto en una terminal con herramientas GNU Make:

```
make

```

> **Nota:** Verifica en `Makefile` las banderas de compilación y enlazado correspondientes a tu entorno (`g++` nativo en Windows o compilación cruzada con MinGW en Linux).

## Autores

* **Santiago Andrés Benavides Coral**

* **Juan David Bejarano Cristancho**

**Programa:** Ingeniería de Sistemas — Universidad Distrital Francisco José de Caldas

**Asignatura:** Ciencias de la Computación I