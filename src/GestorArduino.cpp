#include "GestorArduino.h"
#include "ProcesadorMQTT.h"
#include <SoftwareSerial.h>

const int PIN_TX_ARDUINO = D5;
const int PIN_RX_ARDUINO = D6;

SoftwareSerial serialArduino(
  PIN_RX_ARDUINO,
  PIN_TX_ARDUINO
);

String bufferRecepcionArduino = "";

void enviarComando(
  const String &comando
) {
  serialArduino.println(comando);

  Serial.print(
    "[ARDUINO] Comando enviado: "
  );

  Serial.println(comando);
}

namespace GestorArduino {
    void iniciar() {
    serialArduino.begin(9600);

    Serial.println(
        "[ARDUINO] Comunicación serial iniciada"
    );
    }

    void actualizar() {
    while (serialArduino.available()) {
      char caracter = serialArduino.read();

      if (caracter != '\n') {
        bufferRecepcionArduino += caracter;
        continue;
      }

      bufferRecepcionArduino.trim();

      if (bufferRecepcionArduino.length() > 0) {
        Serial.print("[ARDUINO] Mensaje recibido: ");
        Serial.println(bufferRecepcionArduino);

        // Verificación de sincronización
        if (bufferRecepcionArduino == "SYNC:READY") {
          Serial.println("[SISTEMA] Arduino reiniciado. Resincronizando estado físico...");
          ProcesadorMQTT::restaurarEstadoGuardado();
        } 
        else if (bufferRecepcionArduino.startsWith("ACK:")) {
          // Aquí puedes agregar lógica futura si necesitas confirmar 
          // a la app que la acción realmente sucedió físicamente.
          Serial.println("[SISTEMA] Acción física confirmada por Arduino.");
        }
      }

      bufferRecepcionArduino = "";
    }
  }

    void actualizarLuz(
    const String &habitacion,
    int brillo
    ) {
    brillo = constrain(
        brillo,
        0,
        255
    );

    String comando =
        "LUZ:" +
        habitacion +
        ":" +
        String(brillo);

    enviarComando(comando);
    }

    void abrirPorton() {
    enviarComando(
        "PORTON:ABRIR"
    );
    }

    void cerrarPorton() {
    enviarComando(
        "PORTON:CERRAR"
    );
    }

    void encenderParedLlorosa() {
    enviarComando(
        "PARED:ENCENDER"
    );
    }

    void apagarParedLlorosa() {
    enviarComando(
        "PARED:APAGAR"
    );
    }

  void detenerPorton() {
    enviarComando("PORTON:DETENER");
  }
}