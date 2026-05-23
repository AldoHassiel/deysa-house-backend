#pragma once

#include <Arduino.h>

namespace GestorWifi {
    bool conectar(
        const String &ssid,
        const String &contrasena
    );

    bool estaConectado();
    void actualizar();
    String obtenerDireccionIP();

    void desconectar();
}