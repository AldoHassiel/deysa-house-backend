#include "GestorWiFi.h"

#include <ESP8266WiFi.h>

const unsigned long TIEMPO_MAXIMO_ESPERA_CONEXION_MS = 15000;
const unsigned long INTERVALO_VERIFICACION_MS = 500;

namespace GestorWifi{
    bool conectar(const String &ssid, const String &contrasena){
        Serial.println();
        Serial.println("[Wifi]: Inciando conexion...");

        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid.c_str(), contrasena.c_str());

        unsigned long tiempoInicio = millis();

        while(
            WiFi.status() != WL_CONNECTED && 
            millis() - tiempoInicio < TIEMPO_MAXIMO_ESPERA_CONEXION_MS
        ){
            delay(INTERVALO_VERIFICACION_MS);
            Serial.print(".");
        }

        Serial.println();

        if(WiFi.status() != WL_CONNECTED){
            Serial.println("[Wifi]: No se pudo conectar a la red WiFi.");
            return false;
        }

        Serial.println("[WIFI] Conectado correctamente");
        Serial.print("[WIFI] Dirección IP: ");
        Serial.println(WiFi.localIP());

        return true;
    }

    bool estaConectado(){
        return WiFi.status() == WL_CONNECTED;
    }

    String obtenerDireccionIP(){
        return WiFi.localIP().toString();
    }

    void desconectar(){
        WiFi.disconnect();
        WiFi.mode(WIFI_OFF);

        Serial.println("[Wifi]: Desconectado");
    }
}
