#include <Arduino.h>
#include <LittleFS.h>

#include "Almacenamiento.h"
#include "GestorArduino.h"
#include "GestorBluetooth.h"
#include "GestorMQTT.h"
#include "GestorWiFi.h"
#include "ProcesadorMQTT.h"
#include "GestorHorario.h"
#include "PersistenciaEstado.h"
#include "GestorLED.h"

#include <ESP8266WiFi.h>

void alRecibirMensajeMQTT(
  const String &topico,
  const String &mensaje
) {

  Serial.println(
    "[MQTT]: Mensaje recibido"
  );

  Serial.println(
    "[MQTT]: Tópico: " + topico
  );

  Serial.println(
    "[MQTT]: Mensaje: " + mensaje
  );

  ProcesadorMQTT
    ::procesarMensaje(
      topico,
      mensaje
    );
}

void setup() {

  Serial.begin(9600);

  delay(1000);

  GestorLED::iniciar();

  Serial.println(
    "[DeysaHouse]: Empezando setup..."
  );

  if (!LittleFS.begin()) {

    Serial.println(
      "[DeysaHouse]: Error iniciando LittleFS"
    );

    return;
  }

  Almacenamiento::iniciar();

  GestorArduino::iniciar();

  GestorBluetooth::iniciar();

  GestorMQTT::iniciar(
    alRecibirMensajeMQTT
  );

  GestorHorario::iniciar();

  ProcesadorMQTT::iniciar();

  ProcesadorMQTT
    ::restaurarEstadoGuardado();

  String ssid;
  String contrasena;

  if (
    !Almacenamiento
      ::cargarCredencialesWifi(
        ssid,
        contrasena
      )
  ) {

    Serial.println(
      "[DeysaHouse]: No existen credenciales wifi guardadas"
    );

    return;
  }

  Serial.println(
    "[DeysaHouse]: Credenciales wifi encontradas"
  );

  if (
    !GestorWifi::conectar(
      ssid,
      contrasena
    )
  ) {

    Serial.println(
      "[DeysaHouse]: No se pudo conectar al wifi"
    );

    return;
  }

  if (!GestorMQTT::conectar()) {

    Serial.println(
      "[DeysaHouse] Error conectando MQTT"
    );

    return;
  }

  Serial.println(
    "[DeysaHouse] Sistema iniciado correctamente"
  );
}

void loop() {
    GestorLED::actualizar();
    GestorBluetooth::actualizar();
    GestorArduino::actualizar();
    
    if (GestorBluetooth::haySolicitudReinicioFabrica()) {
        Serial.println("[SISTEMA] Reiniciando de fabrica...");
        PersistenciaEstado::borrarEstado();
        Almacenamiento::borrarCredencialesWifi();
        GestorWifi::desconectar();
        delay(1000);
        ESP.restart();
        return;
    }

    if (GestorBluetooth::hayConfiguracionWifiDisponible()) {
        
        ConfiguracionWiFiRecibida conf = GestorBluetooth::obtenerConfiguracionWifiRecibida();
        Serial.println("[DeysaHouse]: Iniciando prueba de red WiFi nueva...");

        bool conexionExitosa = GestorWifi::conectar(conf.ssid, conf.contrasena);

        GestorBluetooth::enviarRespuestaPruebaWiFi(conexionExitosa, conf.ssid);

        if (conexionExitosa) {
            Serial.println("[DeysaHouse]: Guardando nuevas credenciales validas.");
            Almacenamiento::guardarCredencialesWifi(conf.ssid, conf.contrasena);
            GestorMQTT::conectar();
        } else {
            Serial.println("[DeysaHouse]: Prueba fallida. Descartando credenciales.");
        }

        GestorBluetooth::limpiarConfiguracionWifi();
    }

    GestorWifi::actualizar();
    GestorMQTT::actualizar();
    GestorHorario::actualizar();
    ProcesadorMQTT::actualizar();
}