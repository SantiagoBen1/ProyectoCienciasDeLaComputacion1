@echo off
setlocal
echo Compilando Sistema de Gestion Logistica y Enrutamiento Urbano (C++17 + MINIWIN)...

where g++ >nul 2>nul
if %errorlevel% equ 0 (
  echo [INFO] Compilando con g++ ^(MinGW^)...
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
    -o CentralLogistica.exe -lgdi32 -mwindows -static -static-libgcc -static-libstdc++
  goto :check
)

set "VCVARS=C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
if exist "%VCVARS%" (
  echo [INFO] Compilando con MSVC ^(cl.exe^)...
  call "%VCVARS%" >nul
  cl /nologo /std:c++17 /EHsc /O2 /utf-8 /MT ^
    /Ilib\miniwin /Isrc ^
    lib\miniwin\miniwin.cpp ^
    src\algoritmos\NodoLista.cpp ^
    src\algoritmos\Nodo.cpp ^
    src\algoritmos\algoritmos.cpp ^
    src\estado\EstadoApp.cpp ^
    src\estado\Bootstrapper.cpp ^
    src\ui\MiniWinCapa.cpp ^
    src\ui\ComponentesUI.cpp ^
    src\ui\Renderizador.cpp ^
    src\eventos\ControladorEventos.cpp ^
    src\main.cpp ^
    /Fe:CentralLogistica.exe user32.lib gdi32.lib /link /SUBSYSTEM:WINDOWS
  del /q *.obj >nul 2>nul
  goto :check
)

echo [ERROR] No se encontro g++ ni Visual Studio Build Tools ^(cl.exe^).
exit /b 1

:check
if %errorlevel% neq 0 (
  echo [ERROR] La compilacion fallo.
  exit /b %errorlevel%
)
echo [OK] Compilacion exitosa: CentralLogistica.exe
