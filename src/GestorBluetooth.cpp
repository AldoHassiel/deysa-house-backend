#include "GestorBluetooth.h"

#include <ArduinoJson.h>

String bufferRecepcion;
bool configuracionDisponible = false;
ConfiguracionWiFiRecibida configuracionWiFi;

namespace GestorBluetooth {
    void iniciar(){
        bufferRecepcion = "";
        configuracionDisponible = false;
    }

    void actualizar(){
        while(Serial.available()){
            char caracter = Serial.read();

            if (caracter != '\n') {
                bufferRecepcion += caracter;
                continue;
            }

            bufferRecepcion.trim();

            if (bufferRecepcion.length() == 0) {
                bufferRecepcion = "";
                continue;
            }
            
            Serial.println("[Bluetooth]: JSON recibido:");
            Serial.println(bufferRecepcion);

            JsonDocument documento;

            DeserializationError error = deserializeJson(
                documento,
                bufferRecepcion
            );

            if(error){
                Serial.println("[Bluetooth]: Error al deserializar el JSON recibidio");
                
                Serial.println(error.c_str());

                bufferRecepcion = "";

                continue;
            }

            String tipo = documento["type"] | "";

            if(tipo != "wifi_config"){
                continue;
            }

            String ssid = documento["ssid"] | "";
            String contrasena = documento["password"] | "";

            if(ssid.length() == 0){
                Serial.println("[Bluetooth]: El ssid esta vacio");
                continue;
            }

            if(contrasena.length() == 0){
                Serial.println("[Bluetooth]: La contraseña esta vacio");
                continue;
            }

            configuracionWiFi.ssid = ssid;
            configuracionWiFi.contrasena = contrasena;
            configuracionDisponible = true;

            Serial.println("[Bluetooth]: Configuracion wifi recibida correctamente");

            bufferRecepcion = "";
        }
    }

    bool hayConfiguracionWifiDisponible(){
        return configuracionDisponible;
    }

    ConfiguracionWiFiRecibida obtenerConfiguracionWifiRecibida(){
        return configuracionWiFi;
    }

    void limpiarConfiguracionWifi(){
        configuracionWiFi.ssid = "";
        configuracionWiFi.contrasena = "";
        configuracionDisponible = false;
    }
}
