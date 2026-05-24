#include "GestorWiFi.h"

#include <ESP8266WiFi.h>
#include "GestorLED.h"

String ssidGuardado = "";
String contrasenaGuardada = "";

unsigned long ultimoIntentoReconexion = 0;

const unsigned long
INTERVALO_RECONEXION_WIFI_MS = 10000;

const unsigned long TIEMPO_MAXIMO_ESPERA_CONEXION_MS = 15000;
const unsigned long INTERVALO_VERIFICACION_MS = 50;

namespace GestorWifi{
    bool conectar(const String &ssid, const String &contrasena){
        Serial.println();
        Serial.println("[Wifi]: Inciando conexion...");

        ssidGuardado = ssid;
        contrasenaGuardada = contrasena;

        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid.c_str(), contrasena.c_str());

        unsigned long tiempoInicio = millis();

        while(
            WiFi.status() != WL_CONNECTED && 
            millis() - tiempoInicio < TIEMPO_MAXIMO_ESPERA_CONEXION_MS
        ){
            GestorLED::actualizar();
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
    void actualizar(){

    if(estaConectado()){
        return;
    }

    if(ssidGuardado.length() == 0){
        return;
    }

    unsigned long tiempoActual = millis();

    if(
        tiempoActual - ultimoIntentoReconexion <
        INTERVALO_RECONEXION_WIFI_MS
    ){
        return;
    }

    ultimoIntentoReconexion =
        tiempoActual;

    Serial.println(
        "[WIFI] Intentando reconexion..."
    );

    conectar(
        ssidGuardado,
        contrasenaGuardada
    );
}
}
