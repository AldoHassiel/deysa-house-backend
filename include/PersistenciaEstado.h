#pragma once

#include <Arduino.h>

#include "ProcesadorMQTT.h"

namespace PersistenciaEstado {

bool guardarEstado(
  const EstadoCasa &estado
);

bool cargarEstado(
  EstadoCasa &estado
);

bool borrarEstado();
}