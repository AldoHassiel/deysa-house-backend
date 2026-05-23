#include "PersistenciaEstado.h"

#include <ArduinoJson.h>
#include <LittleFS.h>

const char *RUTA_ESTADO =
  "/estado_casa.json";

namespace PersistenciaEstado {

bool guardarEstado(
  const EstadoCasa &estado
) {

  File archivo =
    LittleFS.open(
      RUTA_ESTADO,
      "w"
    );

  if (!archivo) {
    Serial.println(
      "[PERSISTENCIA] Error creando archivo"
    );

    return false;
  }

  JsonDocument documento;

  JsonObject luces =
    documento["luces"]
      .to<JsonObject>();

  luces["entrada"]["estado"] =
    estado.entrada.encendida;

  luces["entrada"]["brillo"] =
    estado.entrada.brillo;

  luces["calle"]["estado"] =
    estado.calle.encendida;

  luces["calle"]["brillo"] =
    estado.calle.brillo;

  luces["porton"]["estado"] =
    estado.porton.encendida;

  luces["porton"]["brillo"] =
    estado.porton.brillo;

  luces["cocina"]["estado"] =
    estado.cocina.encendida;

  luces["cocina"]["brillo"] =
    estado.cocina.brillo;

  luces["sala"]["estado"] =
    estado.sala.encendida;

  luces["sala"]["brillo"] =
    estado.sala.brillo;

  luces["jardin"]["estado"] =
    estado.jardin.encendida;

  luces["jardin"]["brillo"] =
    estado.jardin.brillo;

  luces["pasillo"]["estado"] =
    estado.pasillo.encendida;

  luces["pasillo"]["brillo"] =
    estado.pasillo.brillo;

  luces["cuarto"]["estado"] =
    estado.cuarto.encendida;

  luces["cuarto"]["brillo"] =
    estado.cuarto.brillo;

  luces["bano"]["estado"] =
    estado.bano.encendida;

  luces["bano"]["brillo"] =
    estado.bano.brillo;

  documento["portonAbierto"] =
    estado.portonAbierto;

  JsonObject pared =
    documento["paredLlorosa"]
      .to<JsonObject>();

  pared["estado"] =
    estado.paredLlorosa.encendida;

  pared["horaEncendido"] =
    estado.paredLlorosa.horaEncendido;

  pared["horaApagado"] =
    estado.paredLlorosa.horaApagado;

  bool serializado =
    serializeJson(
      documento,
      archivo
    );

  archivo.close();

  if (!serializado) {
    Serial.println(
      "[PERSISTENCIA] Error serializando estado"
    );

    return false;
  }

  Serial.println(
    "[PERSISTENCIA] Estado guardado correctamente"
  );

  return true;
}

bool cargarEstado(
  EstadoCasa &estado
) {

  if (!LittleFS.exists(RUTA_ESTADO)) {
    Serial.println(
      "[PERSISTENCIA] No existe archivo de estado"
    );

    return false;
  }

  File archivo =
    LittleFS.open(
      RUTA_ESTADO,
      "r"
    );

  if (!archivo) {
    Serial.println(
      "[PERSISTENCIA] Error abriendo archivo"
    );

    return false;
  }

  JsonDocument documento;

  DeserializationError error =
    deserializeJson(
      documento,
      archivo
    );

  archivo.close();

  if (error) {
    Serial.println(
      "[PERSISTENCIA] Error leyendo JSON"
    );

    return false;
  }

  estado.entrada.encendida =
    documento["luces"]["entrada"]["estado"] | false;

  estado.entrada.brillo =
    documento["luces"]["entrada"]["brillo"] | 0;

  estado.calle.encendida =
    documento["luces"]["calle"]["estado"] | false;

  estado.calle.brillo =
    documento["luces"]["calle"]["brillo"] | 0;

  estado.porton.encendida =
    documento["luces"]["porton"]["estado"] | false;

  estado.porton.brillo =
    documento["luces"]["porton"]["brillo"] | 0;

  estado.cocina.encendida =
    documento["luces"]["cocina"]["estado"] | false;

  estado.cocina.brillo =
    documento["luces"]["cocina"]["brillo"] | 0;

  estado.sala.encendida =
    documento["luces"]["sala"]["estado"] | false;

  estado.sala.brillo =
    documento["luces"]["sala"]["brillo"] | 0;

  estado.jardin.encendida =
    documento["luces"]["jardin"]["estado"] | false;

  estado.jardin.brillo =
    documento["luces"]["jardin"]["brillo"] | 0;

  estado.pasillo.encendida =
    documento["luces"]["pasillo"]["estado"] | false;

  estado.pasillo.brillo =
    documento["luces"]["pasillo"]["brillo"] | 0;

  estado.cuarto.encendida =
    documento["luces"]["cuarto"]["estado"] | false;

  estado.cuarto.brillo =
    documento["luces"]["cuarto"]["brillo"] | 0;

  estado.bano.encendida =
    documento["luces"]["bano"]["estado"] | false;

  estado.bano.brillo =
    documento["luces"]["bano"]["brillo"] | 0;

  estado.portonAbierto =
    documento["portonAbierto"] | false;

  estado.paredLlorosa.encendida =
    documento["paredLlorosa"]["estado"] | false;

  estado.paredLlorosa.horaEncendido =
    documento["paredLlorosa"]["horaEncendido"] | "";

  estado.paredLlorosa.horaApagado =
    documento["paredLlorosa"]["horaApagado"] | "";

  Serial.println(
    "[PERSISTENCIA] Estado cargado correctamente"
  );

  return true;
}

}