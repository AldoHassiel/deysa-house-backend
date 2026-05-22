#pragma once

#include <Arduino.h>

using CallbackMensajeMQTT = void (*)(const String &topico, const String &mensaje);

namespace GestorMQTT {
    void iniciar(CallbackMensajeMQTT callback);
    bool conectar();
    void actualizar();
    bool estaConectado();
    bool publicar(const String &topico, const String &mensaje, bool retenido = false);
    bool suscribirse(const String &topico);
}