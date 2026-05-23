#include <Arduino.h>
#include <LittleFS.h>

#include "Almacenamiento.h"
#include "GestorArduino.h"
#include "GestorBluetooth.h"
#include "GestorMQTT.h"
#include "GestorWiFi.h"
#include "ProcesadorMQTT.h"
#include "GestorHorario.h"

void alRecibirMensajeMQTT(const String &topico, const String &mensaje) {
  Serial.println("[MQTT]: Mensaje recibido");
  Serial.println("[MQTT]: Tópico: " + topico);
  Serial.println("[MQTT]: Mensaje: " + mensaje);

  ProcesadorMQTT::procesarMensaje(topico, mensaje);
}

void setup() {
  Serial.begin(9600);
  delay(1000);
  
  Serial.println("[DeysaHouse]: Empezando setup...");

  if (!LittleFS.begin()) {
    Serial.println(
      "[DeysaHouse]: Error iniciando LittleFS"
    );

    return;
  }


  Almacenamiento::iniciar();
  GestorArduino::iniciar();
  GestorBluetooth::iniciar();
  GestorMQTT::iniciar(alRecibirMensajeMQTT);
  GestorHorario::iniciar();

  ProcesadorMQTT::iniciar();

  ProcesadorMQTT::restaurarEstadoGuardado();

  String ssid;
  String contrasena;

  if(!Almacenamiento::cargarCredencialesWifi(ssid, contrasena)){
    Serial.println("[DeysaHouse]: No existen credenciales wifi guardadas");
    return;
  }

  Serial.println("[DeysaHouse]: Credenciales wifi encontradas");

  if(!GestorWifi::conectar(ssid, contrasena)){
    Serial.println("[DeysaHouse]: No se pudo conectar al wifi");
    return;
  }

  bool conectadoMQTT = GestorMQTT::conectar();

  if (!conectadoMQTT) {
    Serial.println(
      "[DeysaHouse] Error conectando MQTT"
    );
    
    return;
  }

  GestorMQTT::suscribirse(
    "casa/luces"
  );

  GestorMQTT::suscribirse(
    "casa/porton"
  );

  GestorMQTT::suscribirse(
    "casa/pared-llorosa"
  );

  GestorMQTT::suscribirse(
    "casa/estado/obtener"
  );

  ProcesadorMQTT::publicarEstadoCasa();

  Serial.println(
    "[DeysaHouse] Sistema iniciado correctamente"
  );
}

void loop() {
  GestorBluetooth::actualizar();

  if(GestorBluetooth::hayConfiguracionWifiDisponible()){
    ConfiguracionWiFiRecibida configuracion = GestorBluetooth::obtenerConfiguracionWifiRecibida();
    Serial.println("[DeysaHouse]: Configuracion wifi recibida");

    if(!GestorWifi::conectar(configuracion.ssid, configuracion.contrasena)){
      Serial.println("[DeysaHouse]: No fue posible conectarse al wifi");

      GestorBluetooth::limpiarConfiguracionWifi();
      return;
    }

    if (Almacenamiento::guardarCredencialesWifi( configuracion.ssid, configuracion.contrasena)){
      Serial.println("[DeysaHouse]: Credenciales guardadas correctamente");
    } else {
      Serial.println("[DeysaHouse] Error al guardar credenciales");
    }

    bool conectadoMQTT = GestorMQTT::conectar();
    //GestorBluetooth::limpiarConfiguracionWifi();

    if (conectadoMQTT) {
      GestorMQTT::suscribirse(
        "casa/luces"
      );

      GestorMQTT::suscribirse(
        "casa/porton"
      );

      GestorMQTT::suscribirse(
        "casa/pared-llorosa"
      );

      GestorMQTT::suscribirse(
        "casa/estado/obtener"
      );

      ProcesadorMQTT
        ::publicarEstadoCasa();
    }

    GestorBluetooth
      ::limpiarConfiguracionWifi();
  }

  GestorMQTT::actualizar();
  GestorHorario::actualizar();
}
