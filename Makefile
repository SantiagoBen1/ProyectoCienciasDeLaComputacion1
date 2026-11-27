CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -Ilib/miniwin -Isrc
LDFLAGS = -lgdi32 -mwindows -static -static-libgcc -static-libstdc++

SRCS = \
	lib/miniwin/miniwin.cpp \
	src/algoritmos/NodoLista.cpp \
	src/algoritmos/Nodo.cpp \
	src/algoritmos/algoritmos.cpp \
	src/estado/EstadoApp.cpp \
	src/estado/Bootstrapper.cpp \
	src/ui/MiniWinCapa.cpp \
	src/ui/ComponentesUI.cpp \
	src/ui/Renderizador.cpp \
	src/eventos/ControladorEventos.cpp \
	src/main.cpp

TARGET = CentralLogistica.exe

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)
