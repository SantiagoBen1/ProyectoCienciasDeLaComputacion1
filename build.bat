@echo off
setlocal
echo Compilando Sistema de Gestion Logistica y Enrutamiento Urbano (C++17 + MINIWIN)...
g++ -std=c++17 -Wall -Wextra -O2 ^
  -Ilib/miniwin -Isrc ^
  lib/miniwin/miniwin.cpp ^
  src/algoritmos/NodoLista.cpp ^
  src/algoritmos/Nodo.cpp ^
  src/algoritmos/algoritmos.cpp ^
  src/estado/EstadoApp.cpp ^
  src/estado/Bootstrapper.cpp ^
  src/ui/MiniWinCapa.cpp ^
  src/ui/ComponentesUI.cpp ^
  src/ui/Renderizador.cpp ^
  src/eventos/ControladorEventos.cpp ^
  src/main.cpp ^
  -o CentralLogistica.exe -lgdi32 -mwindows

if %errorlevel% neq 0 (
  echo [ERROR] La compilacion fallo.
  exit /b %errorlevel%
)
echo [OK] Compilacion exitosa: CentralLogistica.exe
