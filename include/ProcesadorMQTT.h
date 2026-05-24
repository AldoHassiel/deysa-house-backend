#pragma once

#include <Arduino.h>

struct EstadoLuz {
  bool encendida;
  int brillo;
};

struct EstadoParedLlorosa {
  bool encendida;
  String horaEncendido;
  String horaApagado;
};

struct EstadoCasa {
  EstadoLuz entrada;
  EstadoLuz calle;
  EstadoLuz porton;
  EstadoLuz cocina;
  EstadoLuz sala;
  EstadoLuz jardin;
  EstadoLuz pasillo;
  EstadoLuz cuarto;
  EstadoLuz bano;

  bool portonAbierto;

  EstadoParedLlorosa paredLlorosa;
};

namespace ProcesadorMQTT {
  void iniciar();
  void actualizar();
  void procesarMensaje(
    const String &topico,
    const String &mensaje
  );
  void publicarEstadoCasa();
  EstadoCasa obtenerEstadoCasa();
  bool restaurarEstadoGuardado();
  void encenderParedLlorosa();

  void apagarParedLlorosa();
}