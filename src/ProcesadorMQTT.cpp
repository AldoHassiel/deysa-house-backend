#include "ProcesadorMQTT.h"

#include "GestorArduino.h"
#include "GestorMQTT.h"
#include "PersistenciaEstado.h"

#include <ArduinoJson.h>

/* ==========================================================
   TÓPICOS MQTT
   ========================================================== */

const String TOPICO_LUCES =
  "casa/luces";

const String TOPICO_PORTON =
  "casa/porton";

const String TOPICO_PARED_LLOROSA =
  "casa/pared-llorosa";

const String TOPICO_OBTENER_ESTADO =
  "casa/estado/obtener";

const String TOPICO_ESTADO =
  "casa/estado";

/* ==========================================================
   ESTADO GLOBAL
   ========================================================== */

EstadoCasa estadoCasa;


/* ==========================================================
   CONTROL DE BOMBA (ANTI-RUIDO)
   ========================================================== */
unsigned long tiempoArranqueBomba = 0;
bool pendienteRestaurarLuces = false;
const unsigned long TIEMPO_ESPERA_BOMBA_MS = 1000; // 1 segundo de espera

void restaurarSoloLuces() {
  Serial.println("[ESTADO] Restaurando luces tras arranque de bomba...");

  auto aplicarLuz = [](const String &habitacion, const EstadoLuz &luz) {
    int pwm = 0;
    if (luz.encendida) {
      pwm = map(constrain(luz.brillo, 0, 100), 0, 100, 0, 255);
    }
    GestorArduino::actualizarLuz(habitacion, pwm);
  };

  aplicarLuz("ENTRADA", estadoCasa.entrada);
  aplicarLuz("CALLE", estadoCasa.calle);
  aplicarLuz("PORTON", estadoCasa.porton);
  aplicarLuz("COCINA", estadoCasa.cocina);
  aplicarLuz("SALA", estadoCasa.sala);
  aplicarLuz("JARDIN", estadoCasa.jardin);
  aplicarLuz("PASILLO", estadoCasa.pasillo);
  aplicarLuz("CUARTO", estadoCasa.cuarto);
  aplicarLuz("BANO", estadoCasa.bano);
}

/* ==========================================================
   DECLARACIONES ADELANTADAS
   ========================================================== */


namespace ProcesadorMQTT {
  void publicarEstadoCasa();
}

/* ==========================================================
   FUNCIONES AUXILIARES
   ========================================================== */

int convertirBrilloMQTTaPWM(
  int brillo
) {
  brillo = constrain(
    brillo,
    0,
    100
  );

  return map(
    brillo,
    0,
    100,
    0,
    255
  );
}

EstadoLuz *obtenerLuzPorHabitacion(
  const String &habitacion
) {
  if (habitacion == "entrada") {
    return &estadoCasa.entrada;
  }

  if (habitacion == "calle") {
    return &estadoCasa.calle;
  }

  if (habitacion == "porton") {
    return &estadoCasa.porton;
  }

  if (habitacion == "cocina") {
    return &estadoCasa.cocina;
  }

  if (habitacion == "sala") {
    return &estadoCasa.sala;
  }

  if (habitacion == "jardin") {
    return &estadoCasa.jardin;
  }

  if (habitacion == "pasillo") {
    return &estadoCasa.pasillo;
  }

  if (habitacion == "cuarto") {
    return &estadoCasa.cuarto;
  }

  if (habitacion == "bano") {
    return &estadoCasa.bano;
  }

  return nullptr;
}

String convertirHabitacionArduino(
  const String &habitacion
) {
  if (habitacion == "entrada") {
    return "ENTRADA";
  }

  if (habitacion == "calle") {
    return "CALLE";
  }

  if (habitacion == "porton") {
    return "PORTON";
  }

  if (habitacion == "cocina") {
    return "COCINA";
  }

  if (habitacion == "sala") {
    return "SALA";
  }

  if (habitacion == "jardin") {
    return "JARDIN";
  }

  if (habitacion == "pasillo") {
    return "PASILLO";
  }

  if (habitacion == "cuarto") {
    return "CUARTO";
  }

  if (habitacion == "bano") {
    return "BANO";
  }

  return "";
}

/* ==========================================================
   PROCESAMIENTO DE LUCES
   ========================================================== */

void procesarLuces(
  const String &mensaje
) {
  JsonDocument documento;

  DeserializationError error =
    deserializeJson(
      documento,
      mensaje
    );

  if (error) {
    Serial.println(
      "[MQTT] JSON inválido para luces"
    );

    return;
  }

  String habitacion =
    documento["habitacion"] | "";

  bool estado =
    documento["estado"] | false;

  int brillo =
    documento["brillo"] | 100;

  if (habitacion.length() == 0) {
    Serial.println(
      "[MQTT] Habitación inválida"
    );

    return;
  }

  EstadoLuz *luz =
    obtenerLuzPorHabitacion(
      habitacion
    );

  if (luz == nullptr) {
    Serial.println(
      "[MQTT] Habitación no encontrada"
    );

    return;
  }

  if (!estado) {
    luz->encendida = false;
    luz->brillo = 0;

    GestorArduino::actualizarLuz(
      convertirHabitacionArduino(
        habitacion
      ),
      0
    );

    ProcesadorMQTT
      ::publicarEstadoCasa();

    return;
  }

  if (brillo == 0) {
    brillo = 100;
  }

  brillo = constrain(
    brillo,
    0,
    100
  );

  luz->encendida = true;
  luz->brillo = brillo;

  int pwm =
    convertirBrilloMQTTaPWM(
      brillo
    );

  GestorArduino::actualizarLuz(
    convertirHabitacionArduino(
      habitacion
    ),
    pwm
  );

  ProcesadorMQTT
    ::publicarEstadoCasa();
}

/* ==========================================================
   PROCESAMIENTO DE PORTÓN
   ========================================================== */

void procesarPorton(
  const String &mensaje
) {
  JsonDocument documento;

  DeserializationError error =
    deserializeJson(
      documento,
      mensaje
    );

  if (error) {
    Serial.println(
      "[MQTT] JSON inválido para portón"
    );

    return;
  }

  String accion =
    documento["accion"] | "";

  if (accion == "abrir") {
    estadoCasa.portonAbierto = true;

    GestorArduino::abrirPorton();

    ProcesadorMQTT
      ::publicarEstadoCasa();

    return;
  }

  if (accion == "cerrar") {
    estadoCasa.portonAbierto = false;

    GestorArduino::cerrarPorton();

    ProcesadorMQTT
      ::publicarEstadoCasa();

    return;
  }

  Serial.println(
    "[MQTT] Acción inválida para portón"
  );
}

/* ==========================================================
   PROCESAMIENTO PARED LLOROSA
   ========================================================== */

void procesarParedLlorosa(
  const String &mensaje
) {

  JsonDocument documento;

  DeserializationError error =
    deserializeJson(
      documento,
      mensaje
    );

  if (error) {

    Serial.println(
      "[MQTT] JSON inválido para pared llorosa"
    );

    return;
  }

  String accion =
    documento["accion"] | "";

  /* ======================================================
     ENCENDER MANUALMENTE
     ====================================================== */

  if (accion == "encender") {

    ProcesadorMQTT
      ::encenderParedLlorosa();

    return;
  }

  /* ======================================================
     APAGAR MANUALMENTE
     ====================================================== */

  if (accion == "apagar") {

    ProcesadorMQTT
      ::apagarParedLlorosa();

    return;
  }

  /* ======================================================
     PROGRAMAR HORARIO
     ====================================================== */

  if (accion == "programar") {

    String horaEncendido =
      documento["horaEncendido"] | "";

    String horaApagado =
      documento["horaApagado"] | "";

    estadoCasa
      .paredLlorosa
      .horaEncendido =
        horaEncendido;

    estadoCasa
      .paredLlorosa
      .horaApagado =
        horaApagado;

    ProcesadorMQTT::publicarEstadoCasa();

    Serial.println(
      "[HORARIO] Horario actualizado"
    );

    return;
  }

  Serial.println(
    "[MQTT] Accion invalida para pared llorosa"
  );
}

/* ==========================================================
   NAMESPACE PRINCIPAL
   ========================================================== */

namespace ProcesadorMQTT {

void iniciar() {
  estadoCasa.entrada = { false, 0 };
  estadoCasa.calle = { false, 0 };
  estadoCasa.porton = { false, 0 };
  estadoCasa.cocina = { false, 0 };
  estadoCasa.sala = { false, 0 };
  estadoCasa.jardin = { false, 0 };
  estadoCasa.pasillo = { false, 0 };
  estadoCasa.cuarto = { false, 0 };
  estadoCasa.bano = { false, 0 };

  estadoCasa.portonAbierto = false;

  estadoCasa.paredLlorosa.encendida =
    false;

  estadoCasa.paredLlorosa.horaEncendido =
    "";

  estadoCasa.paredLlorosa.horaApagado =
    "";

  Serial.println(
    "[MQTT] Procesador MQTT inicializado"
  );
}

void procesarMensaje(
  const String &topico,
  const String &mensaje
) {
  Serial.println(
    "[MQTT] Procesando mensaje..."
  );

  if (topico == TOPICO_LUCES) {
    procesarLuces(mensaje);
    return;
  }

  if (topico == TOPICO_PORTON) {
    procesarPorton(mensaje);
    return;
  }

  if (
    topico ==
    TOPICO_PARED_LLOROSA
  ) {
    procesarParedLlorosa(mensaje);
    return;
  }

  if (
    topico ==
    TOPICO_OBTENER_ESTADO
  ) {
    publicarEstadoCasa();
    return;
  }

  Serial.println(
    "[MQTT] Tópico no reconocido"
  );
}

void publicarEstadoCasa() {
  PersistenciaEstado
  ::guardarEstado(
    estadoCasa
  );

  JsonDocument documento;

  JsonObject luces =
    documento["luces"]
      .to<JsonObject>();

  luces["entrada"]["estado"] =
    estadoCasa.entrada.encendida;

  luces["entrada"]["brillo"] =
    estadoCasa.entrada.brillo;

  luces["calle"]["estado"] =
    estadoCasa.calle.encendida;

  luces["calle"]["brillo"] =
    estadoCasa.calle.brillo;

  luces["porton"]["estado"] =
    estadoCasa.porton.encendida;

  luces["porton"]["brillo"] =
    estadoCasa.porton.brillo;

  luces["cocina"]["estado"] =
    estadoCasa.cocina.encendida;

  luces["cocina"]["brillo"] =
    estadoCasa.cocina.brillo;

  luces["sala"]["estado"] =
    estadoCasa.sala.encendida;

  luces["sala"]["brillo"] =
    estadoCasa.sala.brillo;

  luces["jardin"]["estado"] =
    estadoCasa.jardin.encendida;

  luces["jardin"]["brillo"] =
    estadoCasa.jardin.brillo;

  luces["pasillo"]["estado"] =
    estadoCasa.pasillo.encendida;

  luces["pasillo"]["brillo"] =
    estadoCasa.pasillo.brillo;

  luces["cuarto"]["estado"] =
    estadoCasa.cuarto.encendida;

  luces["cuarto"]["brillo"] =
    estadoCasa.cuarto.brillo;

  luces["bano"]["estado"] =
    estadoCasa.bano.encendida;

  luces["bano"]["brillo"] =
    estadoCasa.bano.brillo;

  documento["portonAbierto"] =
    estadoCasa.portonAbierto;

  JsonObject pared =
    documento["paredLlorosa"]
      .to<JsonObject>();

  pared["estado"] =
    estadoCasa.paredLlorosa.encendida;

  pared["horaEncendido"] =
    estadoCasa.paredLlorosa.horaEncendido;

  pared["horaApagado"] =
    estadoCasa.paredLlorosa.horaApagado;

  String payload;

  serializeJson(
    documento,
    payload
  );

  bool publicado =
    GestorMQTT::publicar(
      TOPICO_ESTADO,
      payload,
      true
    );

  if (!publicado) {
    Serial.println(
      "[MQTT] Error al publicar estado"
    );

    return;
  }

  Serial.println(
    "[MQTT] Estado publicado correctamente"
  );
}

EstadoCasa obtenerEstadoCasa() {
  return estadoCasa;
}

bool restaurarEstadoGuardado() {

  EstadoCasa estadoGuardado;

  bool cargado =
    PersistenciaEstado
      ::cargarEstado(
        estadoGuardado
      );

  if (!cargado) {
    Serial.println(
      "[ESTADO] No existe estado guardado"
    );

    return false;
  }

  estadoCasa = estadoGuardado;

  Serial.println(
    "[ESTADO] Restaurando estado..."
  );

  auto aplicarLuz = [](
    const String &habitacion,
    const EstadoLuz &luz
  ) {

    int pwm = 0;

    if (luz.encendida) {
      pwm = map(
        constrain(
          luz.brillo,
          0,
          100
        ),
        0,
        100,
        0,
        255
      );
    }

    GestorArduino::actualizarLuz(
      habitacion,
      pwm
    );
  };

  aplicarLuz(
    "ENTRADA",
    estadoCasa.entrada
  );

  aplicarLuz(
    "CALLE",
    estadoCasa.calle
  );

  aplicarLuz(
    "PORTON",
    estadoCasa.porton
  );

  aplicarLuz(
    "COCINA",
    estadoCasa.cocina
  );

  aplicarLuz(
    "SALA",
    estadoCasa.sala
  );

  aplicarLuz(
    "JARDIN",
    estadoCasa.jardin
  );

  aplicarLuz(
    "PASILLO",
    estadoCasa.pasillo
  );

  aplicarLuz(
    "CUARTO",
    estadoCasa.cuarto
  );

  aplicarLuz(
    "BANO",
    estadoCasa.bano
  );

  if (
    estadoCasa
      .paredLlorosa
      .encendida
  ) {

    GestorArduino
      ::encenderParedLlorosa();

  } else {

    GestorArduino
      ::apagarParedLlorosa();
  }

  // if (
  //   estadoCasa
  //     .portonAbierto
  // ) {

  //   GestorArduino
  //     ::abrirPorton();

  // } else {

  //   GestorArduino
  //     ::cerrarPorton();
  // }

  Serial.println(
    "[ESTADO] Estado restaurado correctamente"
  );

  return true;
}
void encenderParedLlorosa() {

  if (
    estadoCasa
      .paredLlorosa
      .encendida
  ) {
    return;
  }

  estadoCasa
    .paredLlorosa
    .encendida = true;

  GestorArduino
    ::encenderParedLlorosa();

  publicarEstadoCasa();

  tiempoArranqueBomba = millis();
  pendienteRestaurarLuces = true;

  Serial.println(
    "[HORARIO] Pared llorosa encendida automaticamente"
  );
}

void apagarParedLlorosa() {

  if (
    !estadoCasa
      .paredLlorosa
      .encendida
  ) {
    return;
  }

  estadoCasa
    .paredLlorosa
    .encendida = false;

  GestorArduino
    ::apagarParedLlorosa();

  publicarEstadoCasa();

  Serial.println(
    "[HORARIO] Pared llorosa apagada automaticamente"
  );
}

void actualizar() {
  // Si tenemos luces pendientes por restaurar y ya pasó 1 segundo
  if (pendienteRestaurarLuces && (millis() - tiempoArranqueBomba >= TIEMPO_ESPERA_BOMBA_MS)) {
    restaurarSoloLuces();
    pendienteRestaurarLuces = false; // Apagar la bandera
  }
}

}