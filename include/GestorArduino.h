#pragma once

#include <Arduino.h>

namespace GestorArduino {
  void iniciar();
  void actualizarLuz(
    const String &habitacion,
    int brillo
  );
  void abrirPorton();
  void cerrarPorton();
  void encenderParedLlorosa();
  void apagarParedLlorosa();
}