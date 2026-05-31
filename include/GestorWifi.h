#pragma once

#include <Arduino.h>

namespace GestorWifi {
    bool conectar(const String &ssid, const String &contrasena);
    bool estaConectado();
    String obtenerDireccionIP();
    void desconectar();
    void actualizar();
}