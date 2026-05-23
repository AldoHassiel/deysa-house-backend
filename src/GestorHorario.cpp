#include "GestorHorario.h"

#include "ProcesadorMQTT.h"

#include <time.h>

namespace GestorHorario {

String obtenerHoraActual() {

  time_t ahora = time(nullptr);

  if (ahora < 100000) {
    return "";
  }

  struct tm *tiempo =
    localtime(&ahora);

  char horaActual[6];

  strftime(
    horaActual,
    sizeof(horaActual),
    "%H:%M",
    tiempo
  );

  return String(horaActual);
}

void iniciar() {

  configTime(
    -7 * 3600,
    0,
    "pool.ntp.org",
    "time.nist.gov"
  );

  Serial.println(
    "[HORARIO] Sincronizando hora NTP..."
  );
}

void actualizar() {

  static unsigned long ultimoTiempo =
    0;

  if (
    millis() - ultimoTiempo
    < 1000
  ) {
    return;
  }

  ultimoTiempo = millis();

  String horaActual =
    obtenerHoraActual();

  if (horaActual.length() == 0) {

    Serial.println(
      "[HORARIO] Esperando sincronizacion NTP..."
    );

    return;
  }

  EstadoCasa estado =
    ProcesadorMQTT
      ::obtenerEstadoCasa();

  String horaEncendido =
    estado
      .paredLlorosa
      .horaEncendido;

  String horaApagado =
    estado
      .paredLlorosa
      .horaApagado;

  static String ultimaHoraProcesada =
    "";

  if (
    horaActual ==
    ultimaHoraProcesada
  ) {
    return;
  }

  ultimaHoraProcesada =
    horaActual;

  Serial.print(
    "[HORARIO] Hora actual: "
  );

  Serial.println(
    horaActual
  );

  if (
    horaEncendido.length() > 0 &&
    horaActual == horaEncendido
  ) {

    Serial.println(
      "[HORARIO] Encendiendo pared llorosa"
    );

    ProcesadorMQTT::encenderParedLlorosa();
  }

  if (
    horaApagado.length() > 0 &&
    horaActual == horaApagado
  ) {

    Serial.println(
      "[HORARIO] Apagando pared llorosa"
    );

    ProcesadorMQTT::apagarParedLlorosa();
  }
}

}