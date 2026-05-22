#pragma once

#include <Arduino.h>

struct CredencialesWiFi {
  char marca[3];
  char ssid[32];
  char contrasena[64];
};

namespace Almacenamiento {
  void iniciar();
  bool existenCredencialesWifi();
  bool cargarCredencialesWifi(
    String &ssid,
    String &contrasena
  );
  bool guardarCredencialesWifi(
    const String &ssid,
    const String &contrasena
  );
  void borrarCredencialesWifi();
}