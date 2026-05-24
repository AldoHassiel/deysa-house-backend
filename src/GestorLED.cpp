#include "GestorLED.h"
#include "Almacenamiento.h"
#include "GestorWifi.h"
#include "GestorMQTT.h"

// Usaremos el pin D1 para tu LED externo
const int PIN_LED = D1; 

unsigned long ultimoCambioLed = 0;
bool estadoLed = LOW; // Iniciamos con el valor "LOW" (apagado)

namespace GestorLED {

  void iniciar() {
    pinMode(PIN_LED, OUTPUT);
    digitalWrite(PIN_LED, LOW); // Nos aseguramos de que inicie apagado
  }

  void actualizar() {
    bool wifiConfigurado = Almacenamiento::existenCredencialesWifi();
    bool wifiConectado = GestorWifi::estaConectado();
    bool mqttConectado = GestorMQTT::estaConectado();

    /* ======================================================
       ESTADO: FIJO ENCENDIDO (SISTEMA LISTO)
       ====================================================== */
    if (wifiConectado && mqttConectado) {
      digitalWrite(PIN_LED, HIGH); // El LED externo se enciende con HIGH
      return;
    }

    /* ======================================================
       DETERMINAR VELOCIDAD DE PARPADEO
       ====================================================== */
    unsigned long intervaloParpadeo = 0;
    
    if (!wifiConfigurado) {
      // Parpadeo Lento (1000 ms) -> No hay credenciales
      intervaloParpadeo = 1000;
    } else {
      // Parpadeo Rápido (200 ms) -> Iniciando / Conectando
      intervaloParpadeo = 200;
    }

    /* ======================================================
       EJECUTAR PARPADEO ASÍNCRONO
       ====================================================== */
    if (millis() - ultimoCambioLed >= intervaloParpadeo) {
      ultimoCambioLed = millis();
      
      // Invertir el estado actual
      estadoLed = !estadoLed; 
      
      digitalWrite(PIN_LED, estadoLed ? HIGH : LOW);
    }
  }

}