// Implementacion Win32 compatible con la API estandar de MINIWIN (Pau
// Fernandez). Doble buffer real sin parpadeo, renderizado UTF-8 y
// sincronizacion por hilos.

#define MINIWIN_SOURCE
#include "miniwin.h"

#ifdef _WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include <atomic>
#include <cmath>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

namespace {

constexpr UINT WM_MINIWIN_RESIZE = WM_APP + 1;
constexpr UINT WM_MINIWIN_CLOSE = WM_APP + 2;

HWND g_hWnd = nullptr;
HDC g_hDCDraw = nullptr;
HBITMAP g_hBmpDraw = nullptr;
HBITMAP g_hOldBmpDraw = nullptr;

HDC g_hDCFront = nullptr;
HBITMAP g_hBmpFront = nullptr;
HBITMAP g_hOldBmpFront = nullptr;

HFONT g_hFont = nullptr;
HPEN g_hPen = nullptr;
HBRUSH g_hBrush = nullptr;

int g_ancho = 400;
int g_alto = 300;
COLORREF g_colorActual = RGB(255, 255, 255);

std::mutex g_mutexGraficos;
std::mutex g_mutexEntrada;

std::queue<int> g_colaTeclas;
float g_ratonX = 0.0f;
float g_ratonY = 0.0f;
bool g_ratonDentro = false;
bool g_ratonIzq = false;
bool g_ratonDer = false;
bool g_rastreandoSalida = false;

std::atomic<bool> g_ventanaCerrada{false};

std::wstring utf8AUtf16(const std::string &textoUtf8) {
  if (textoUtf8.empty()) {
    return L"";
  }
  int largo = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                  textoUtf8.c_str(), -1, nullptr, 0);
  UINT paginaCodigo = CP_UTF8;
  if (largo <= 0) {
    paginaCodigo = CP_ACP;
    largo = MultiByteToWideChar(CP_ACP, 0, textoUtf8.c_str(), -1, nullptr, 0);
  }
  if (largo <= 0) {
    return L"";
  }
  std::wstring resultado(static_cast<std::size_t>(largo - 1), L'\0');
  MultiByteToWideChar(paginaCodigo, 0, textoUtf8.c_str(), -1, &resultado[0],
                      largo);
  return resultado;
}

void aplicarEstiloActualSinLock() {
  if (g_hDCDraw == nullptr) {
    return;
  }

  HPEN nuevoPen = CreatePen(PS_SOLID, 1, g_colorActual);
  HBRUSH nuevoBrush = CreateSolidBrush(g_colorActual);

  HGDIOBJ penViejo = SelectObject(g_hDCDraw, nuevoPen);
  HGDIOBJ brushViejo = SelectObject(g_hDCDraw, nuevoBrush);

  if (g_hPen != nullptr && penViejo == g_hPen) {
    DeleteObject(g_hPen);
  }
  if (g_hBrush != nullptr && brushViejo == g_hBrush) {
    DeleteObject(g_hBrush);
  }

  g_hPen = nuevoPen;
  g_hBrush = nuevoBrush;
  SetTextColor(g_hDCDraw, g_colorActual);
  SetBkMode(g_hDCDraw, TRANSPARENT);
}

void crearBuffersSinLock(int ancho, int alto) {
  if (g_hWnd == nullptr) {
    return;
  }
  HDC hdcVentana = GetDC(g_hWnd);

  if (g_hDCDraw == nullptr) {
    g_hDCDraw = CreateCompatibleDC(hdcVentana);
  }
  if (g_hDCFront == nullptr) {
    g_hDCFront = CreateCompatibleDC(hdcVentana);
  }

  if (g_hBmpDraw != nullptr) {
    SelectObject(g_hDCDraw, g_hOldBmpDraw);
    DeleteObject(g_hBmpDraw);
    g_hBmpDraw = nullptr;
  }
  if (g_hBmpFront != nullptr) {
    SelectObject(g_hDCFront, g_hOldBmpFront);
    DeleteObject(g_hBmpFront);
    g_hBmpFront = nullptr;
  }

  g_ancho = ancho > 0 ? ancho : 1;
  g_alto = alto > 0 ? alto : 1;

  g_hBmpDraw = CreateCompatibleBitmap(hdcVentana, g_ancho, g_alto);
  g_hOldBmpDraw = static_cast<HBITMAP>(SelectObject(g_hDCDraw, g_hBmpDraw));

  g_hBmpFront = CreateCompatibleBitmap(hdcVentana, g_ancho, g_alto);
  g_hOldBmpFront = static_cast<HBITMAP>(SelectObject(g_hDCFront, g_hBmpFront));

  if (g_hFont == nullptr) {
    g_hFont =
        CreateFontW(-14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                    DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                    CLEARTYPE_QUALITY, FIXED_PITCH | FF_MODERN, L"Consolas");
  }
  SelectObject(g_hDCDraw, g_hFont);

  RECT rc = {0, 0, g_ancho, g_alto};
  HBRUSH fondoNegro = CreateSolidBrush(RGB(0, 0, 0));
  FillRect(g_hDCDraw, &rc, fondoNegro);
  FillRect(g_hDCFront, &rc, fondoNegro);
  DeleteObject(fondoNegro);

  aplicarEstiloActualSinLock();
  ReleaseDC(g_hWnd, hdcVentana);
}

void liberarRecursosGraficos() {
  std::lock_guard<std::mutex> lock(g_mutexGraficos);
  if (g_hDCDraw != nullptr) {
    if (g_hOldBmpDraw != nullptr) {
      SelectObject(g_hDCDraw, g_hOldBmpDraw);
    }
    if (g_hBmpDraw != nullptr) {
      DeleteObject(g_hBmpDraw);
      g_hBmpDraw = nullptr;
    }
    DeleteDC(g_hDCDraw);
    g_hDCDraw = nullptr;
  }
  if (g_hDCFront != nullptr) {
    if (g_hOldBmpFront != nullptr) {
      SelectObject(g_hDCFront, g_hOldBmpFront);
    }
    if (g_hBmpFront != nullptr) {
      DeleteObject(g_hBmpFront);
      g_hBmpFront = nullptr;
    }
    DeleteDC(g_hDCFront);
    g_hDCFront = nullptr;
  }
  if (g_hPen != nullptr) {
    DeleteObject(g_hPen);
    g_hPen = nullptr;
  }
  if (g_hBrush != nullptr) {
    DeleteObject(g_hBrush);
    g_hBrush = nullptr;
  }
  if (g_hFont != nullptr) {
    DeleteObject(g_hFont);
    g_hFont = nullptr;
  }
}

void ajustarTamanoVentanaWin32(int ancho, int alto) {
  if (g_hWnd == nullptr) {
    return;
  }
  DWORD estilo = static_cast<DWORD>(GetWindowLongPtrW(g_hWnd, GWL_STYLE));
  RECT rc = {0, 0, ancho, alto};
  AdjustWindowRect(&rc, estilo, FALSE);
  int wTotal = rc.right - rc.left;
  int hTotal = rc.bottom - rc.top;

  int pantallaW = GetSystemMetrics(SM_CXSCREEN);
  int pantallaH = GetSystemMetrics(SM_CYSCREEN);
  int posX = (pantallaW - wTotal) / 2;
  int posY = (pantallaH - hTotal) / 2;
  if (posX < 0)
    posX = 0;
  if (posY < 0)
    posY = 0;

  SetWindowPos(g_hWnd, nullptr, posX, posY, wTotal, hTotal,
               SWP_NOZORDER | SWP_SHOWWINDOW);
}

int traducirTeclaVirtual(WPARAM wParam) {
  switch (wParam) {
  case VK_ESCAPE:
    return miniwin::ESCAPE;
  case VK_LEFT:
    return miniwin::IZQUIERDA;
  case VK_BACK:
    return miniwin::IZQUIERDA; // Comodidad adicional para borrar
  case VK_RIGHT:
    return miniwin::DERECHA;
  case VK_UP:
    return miniwin::ARRIBA;
  case VK_DOWN:
    return miniwin::ABAJO;
  case VK_RETURN:
    return miniwin::RETURN;
  case VK_SPACE:
    return miniwin::ESPACIO;
  case VK_F1:
    return miniwin::F1;
  case VK_F2:
    return miniwin::F2;
  case VK_F3:
    return miniwin::F3;
  case VK_F4:
    return miniwin::F4;
  case VK_F5:
    return miniwin::F5;
  case VK_F6:
    return miniwin::F6;
  case VK_F7:
    return miniwin::F7;
  case VK_F8:
    return miniwin::F8;
  case VK_F9:
    return miniwin::F9;
  case VK_F10:
    return miniwin::F10;
  default:
    break;
  }
  if (wParam >= '0' && wParam <= '9') {
    return static_cast<int>(wParam);
  }
  if (wParam >= VK_NUMPAD0 && wParam <= VK_NUMPAD9) {
    return '0' + static_cast<int>(wParam - VK_NUMPAD0);
  }
  if (wParam >= 'A' && wParam <= 'Z') {
    return static_cast<int>(wParam);
  }
  return miniwin::NINGUNA;
}

LRESULT CALLBACK procedimientoVentana(HWND hWnd, UINT msg, WPARAM wParam,
                                      LPARAM lParam) {
  switch (msg) {
  case WM_PAINT: {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);
    {
      std::lock_guard<std::mutex> lock(g_mutexGraficos);
      if (g_hDCFront != nullptr) {
        BitBlt(hdc, 0, 0, g_ancho, g_alto, g_hDCFront, 0, 0, SRCCOPY);
      }
    }
    EndPaint(hWnd, &ps);
    return 0;
  }

  case WM_ERASEBKGND:
    return 1;

  case WM_MINIWIN_RESIZE: {
    int nuevoAncho = static_cast<int>(wParam);
    int nuevoAlto = static_cast<int>(lParam);
    ajustarTamanoVentanaWin32(nuevoAncho, nuevoAlto);
    InvalidateRect(hWnd, nullptr, FALSE);
    return 0;
  }

  case WM_MINIWIN_CLOSE:
    DestroyWindow(hWnd);
    return 0;

  case WM_KEYDOWN: {
    int codigo = traducirTeclaVirtual(wParam);
    if (codigo != miniwin::NINGUNA) {
      std::lock_guard<std::mutex> lock(g_mutexEntrada);
      g_colaTeclas.push(codigo);
    }
    return 0;
  }

  case WM_MOUSEMOVE: {
    std::lock_guard<std::mutex> lock(g_mutexEntrada);
    g_ratonX = static_cast<float>(static_cast<short>(LOWORD(lParam)));
    g_ratonY = static_cast<float>(static_cast<short>(HIWORD(lParam)));
    g_ratonDentro = true;
    if (!g_rastreandoSalida) {
      TRACKMOUSEEVENT tme;
      tme.cbSize = sizeof(TRACKMOUSEEVENT);
      tme.dwFlags = TME_LEAVE;
      tme.hwndTrack = hWnd;
      tme.dwHoverTime = 0;
      TrackMouseEvent(&tme);
      g_rastreandoSalida = true;
    }
    return 0;
  }

  case WM_MOUSELEAVE: {
    std::lock_guard<std::mutex> lock(g_mutexEntrada);
    g_ratonDentro = false;
    g_rastreandoSalida = false;
    return 0;
  }

  case WM_LBUTTONDOWN: {
    std::lock_guard<std::mutex> lock(g_mutexEntrada);
    g_ratonX = static_cast<float>(static_cast<short>(LOWORD(lParam)));
    g_ratonY = static_cast<float>(static_cast<short>(HIWORD(lParam)));
    g_ratonDentro = true;
    g_ratonIzq = true;
    return 0;
  }

  case WM_LBUTTONUP: {
    std::lock_guard<std::mutex> lock(g_mutexEntrada);
    g_ratonIzq = false;
    return 0;
  }

  case WM_RBUTTONDOWN: {
    std::lock_guard<std::mutex> lock(g_mutexEntrada);
    g_ratonX = static_cast<float>(static_cast<short>(LOWORD(lParam)));
    g_ratonY = static_cast<float>(static_cast<short>(HIWORD(lParam)));
    g_ratonDentro = true;
    g_ratonDer = true;
    return 0;
  }

  case WM_RBUTTONUP: {
    std::lock_guard<std::mutex> lock(g_mutexEntrada);
    g_ratonDer = false;
    return 0;
  }

  case WM_CLOSE: {
    g_ventanaCerrada = true;
    {
      std::lock_guard<std::mutex> lock(g_mutexEntrada);
      g_colaTeclas.push(miniwin::ESCAPE);
    }
    DestroyWindow(hWnd);
    return 0;
  }

  case WM_DESTROY:
    g_ventanaCerrada = true;
    PostQuitMessage(0);
    return 0;

  default:
    return DefWindowProcW(hWnd, msg, wParam, lParam);
  }
}

} // namespace

namespace miniwin {

void borra() {
  std::lock_guard<std::mutex> lock(g_mutexGraficos);
  if (g_hDCDraw == nullptr) {
    return;
  }
  RECT rc = {0, 0, g_ancho, g_alto};
  HBRUSH fondo = CreateSolidBrush(RGB(0, 0, 0));
  FillRect(g_hDCDraw, &rc, fondo);
  DeleteObject(fondo);
}

void refresca() {
  {
    std::lock_guard<std::mutex> lock(g_mutexGraficos);
    if (g_hDCDraw != nullptr && g_hDCFront != nullptr) {
      BitBlt(g_hDCFront, 0, 0, g_ancho, g_alto, g_hDCDraw, 0, 0, SRCCOPY);
    }
  }
  if (g_hWnd != nullptr && !g_ventanaCerrada) {
    InvalidateRect(g_hWnd, nullptr, FALSE);
  }
}

void mensaje(std::string msj) {
  std::wstring wMsj = utf8AUtf16(msj);
  MessageBoxW(g_hWnd, wMsj.c_str(), L"Central de Mensajeria Expres",
              MB_OK | MB_ICONINFORMATION);
}

bool pregunta(std::string msj) {
  std::wstring wMsj = utf8AUtf16(msj);
  return MessageBoxW(g_hWnd, wMsj.c_str(), L"Confirmacion",
                     MB_YESNO | MB_ICONQUESTION) == IDYES;
}

void espera(int miliseg) {
  if (miliseg > 0) {
    Sleep(static_cast<DWORD>(miliseg));
  }
}

int vancho() {
  std::lock_guard<std::mutex> lock(g_mutexGraficos);
  return g_ancho;
}

int valto() {
  std::lock_guard<std::mutex> lock(g_mutexGraficos);
  return g_alto;
}

void vredimensiona(int ancho, int alto) {
  {
    std::lock_guard<std::mutex> lock(g_mutexGraficos);
    crearBuffersSinLock(ancho, alto);
  }
  if (g_hWnd != nullptr && !g_ventanaCerrada) {
    PostMessageW(g_hWnd, WM_MINIWIN_RESIZE, static_cast<WPARAM>(ancho),
                 static_cast<LPARAM>(alto));
  }
}

void vcierra() {
  g_ventanaCerrada = true;
  if (g_hWnd != nullptr) {
    PostMessageW(g_hWnd, WM_MINIWIN_CLOSE, 0, 0);
  }
}

void color(int c) {
  switch (c) {
  case NEGRO:
    color_rgb(0, 0, 0);
    break;
  case ROJO:
    color_rgb(255, 0, 0);
    break;
  case VERDE:
    color_rgb(0, 255, 0);
    break;
  case AZUL:
    color_rgb(0, 0, 255);
    break;
  case AMARILLO:
    color_rgb(255, 255, 0);
    break;
  case MAGENTA:
    color_rgb(255, 0, 255);
    break;
  case CYAN:
    color_rgb(0, 255, 255);
    break;
  case BLANCO:
  default:
    color_rgb(255, 255, 255);
    break;
  }
}

void color_rgb(int r, int g, int b) {
  std::lock_guard<std::mutex> lock(g_mutexGraficos);
  g_colorActual = RGB(r, g, b);
  aplicarEstiloActualSinLock();
}

void punto(float x, float y) {
  std::lock_guard<std::mutex> lock(g_mutexGraficos);
  if (g_hDCDraw == nullptr) {
    return;
  }
  SetPixel(g_hDCDraw, static_cast<int>(std::lround(x)),
           static_cast<int>(std::lround(y)), g_colorActual);
}

void linea(float x_ini, float y_ini, float x_fin, float y_fin) {
  std::lock_guard<std::mutex> lock(g_mutexGraficos);
  if (g_hDCDraw == nullptr) {
    return;
  }
  MoveToEx(g_hDCDraw, static_cast<int>(std::lround(x_ini)),
           static_cast<int>(std::lround(y_ini)), nullptr);
  LineTo(g_hDCDraw, static_cast<int>(std::lround(x_fin)),
         static_cast<int>(std::lround(y_fin)));
}

void rectangulo(float izq, float arr, float der, float aba) {
  std::lock_guard<std::mutex> lock(g_mutexGraficos);
  if (g_hDCDraw == nullptr) {
    return;
  }
  HGDIOBJ brushAnterior = SelectObject(g_hDCDraw, GetStockObject(HOLLOW_BRUSH));
  Rectangle(g_hDCDraw, static_cast<int>(std::lround(izq)),
            static_cast<int>(std::lround(arr)),
            static_cast<int>(std::lround(der)),
            static_cast<int>(std::lround(aba)));
  SelectObject(g_hDCDraw, brushAnterior);
}

void rectangulo_lleno(float izq, float arr, float der, float aba) {
  std::lock_guard<std::mutex> lock(g_mutexGraficos);
  if (g_hDCDraw == nullptr) {
    return;
  }
  Rectangle(g_hDCDraw, static_cast<int>(std::lround(izq)),
            static_cast<int>(std::lround(arr)),
            static_cast<int>(std::lround(der)),
            static_cast<int>(std::lround(aba)));
}

void circulo(float x_centro, float y_centro, float radio) {
  std::lock_guard<std::mutex> lock(g_mutexGraficos);
  if (g_hDCDraw == nullptr) {
    return;
  }
  HGDIOBJ brushAnterior = SelectObject(g_hDCDraw, GetStockObject(HOLLOW_BRUSH));
  Ellipse(g_hDCDraw, static_cast<int>(std::lround(x_centro - radio)),
          static_cast<int>(std::lround(y_centro - radio)),
          static_cast<int>(std::lround(x_centro + radio)),
          static_cast<int>(std::lround(y_centro + radio)));
  SelectObject(g_hDCDraw, brushAnterior);
}

void circulo_lleno(float x_centro, float y_centro, float radio) {
  std::lock_guard<std::mutex> lock(g_mutexGraficos);
  if (g_hDCDraw == nullptr) {
    return;
  }
  Ellipse(g_hDCDraw, static_cast<int>(std::lround(x_centro - radio)),
          static_cast<int>(std::lround(y_centro - radio)),
          static_cast<int>(std::lround(x_centro + radio)),
          static_cast<int>(std::lround(y_centro + radio)));
}

void texto(float x, float y, const std::string &t) {
  std::lock_guard<std::mutex> lock(g_mutexGraficos);
  if (g_hDCDraw == nullptr || t.empty()) {
    return;
  }
  std::wstring wt = utf8AUtf16(t);
  TextOutW(g_hDCDraw, static_cast<int>(std::lround(x)),
           static_cast<int>(std::lround(y)), wt.c_str(),
           static_cast<int>(wt.size()));
}

int tecla() {
  std::lock_guard<std::mutex> lock(g_mutexEntrada);
  if (g_colaTeclas.empty()) {
    return NINGUNA;
  }
  int t = g_colaTeclas.front();
  g_colaTeclas.pop();
  return t;
}

bool raton(float &x, float &y) {
  std::lock_guard<std::mutex> lock(g_mutexEntrada);
  x = g_ratonX;
  y = g_ratonY;
  return g_ratonDentro;
}

bool raton_dentro() {
  std::lock_guard<std::mutex> lock(g_mutexEntrada);
  return g_ratonDentro;
}

float raton_x() {
  std::lock_guard<std::mutex> lock(g_mutexEntrada);
  return g_ratonX;
}

float raton_y() {
  std::lock_guard<std::mutex> lock(g_mutexEntrada);
  return g_ratonY;
}

void raton_botones(bool &izq, bool &der) {
  std::lock_guard<std::mutex> lock(g_mutexEntrada);
  izq = g_ratonIzq;
  der = g_ratonDer;
}

bool raton_boton_izq() {
  std::lock_guard<std::mutex> lock(g_mutexEntrada);
  return g_ratonIzq;
}

bool raton_boton_der() {
  std::lock_guard<std::mutex> lock(g_mutexEntrada);
  return g_ratonDer;
}

} // namespace miniwin

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
  const wchar_t *nombreClase = L"MiniWinLogisticsClass";

  WNDCLASSW wc = {};
  wc.lpfnWndProc = procedimientoVentana;
  wc.hInstance = hInstance;
  wc.lpszClassName = nombreClase;
  wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
  RegisterClassW(&wc);

  DWORD estilo = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
  RECT rc = {0, 0, g_ancho, g_alto};
  AdjustWindowRect(&rc, estilo, FALSE);

  g_hWnd = CreateWindowExW(
      0, nombreClase,
      L"Sistema de Gestion Logistica y Enrutamiento Urbano - UD", estilo,
      CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top,
      nullptr, nullptr, hInstance, nullptr);

  if (g_hWnd == nullptr) {
    return 1;
  }

  {
    std::lock_guard<std::mutex> lock(g_mutexGraficos);
    crearBuffersSinLock(g_ancho, g_alto);
  }

  ShowWindow(g_hWnd, nCmdShow);
  UpdateWindow(g_hWnd);

  std::thread hiloUsuario([]() {
    _main_();
    if (!g_ventanaCerrada && g_hWnd != nullptr) {
      PostMessageW(g_hWnd, WM_MINIWIN_CLOSE, 0, 0);
    }
  });

  MSG msg;
  while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
    TranslateMessage(&msg);
    DispatchMessageW(&msg);
  }

  g_ventanaCerrada = true;
  if (hiloUsuario.joinable()) {
    hiloUsuario.join();
  }
  liberarRecursosGraficos();
  return 0;
}

#endif // _WIN32
