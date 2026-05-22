#include "GestorArduino.h"

#include <SoftwareSerial.h>

const int PIN_TX_ARDUINO = D5;

SoftwareSerial serialArduino(
  -1,
  PIN_TX_ARDUINO
);

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

}