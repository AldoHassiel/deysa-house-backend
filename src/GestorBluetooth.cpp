#include "GestorBluetooth.h"
#include "GestorWiFi.h"
#include "GestorMQTT.h"
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

String bufferRecepcion;
bool configuracionDisponible = false;
ConfiguracionWiFiRecibida configuracionWiFi;
bool reinicioFabricaSolicitado = false;

namespace GestorBluetooth {
    void iniciar() {
        bufferRecepcion = "";
        configuracionDisponible = false;
    }

    void actualizar() {
        while (Serial.available()) {
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
            DeserializationError error = deserializeJson(documento, bufferRecepcion);

            if (error) {
                Serial.println("[Bluetooth]: Error al deserializar el JSON");
                bufferRecepcion = "";
                continue;
            }

            String tipo = documento["type"] | "";

            if (tipo == "factory_reset") {
                reinicioFabricaSolicitado = true;
                Serial.println("[Bluetooth] Reinicio de fabrica solicitado");
                bufferRecepcion = "";
                continue;
            }

            if (tipo == "estado") {
                enviarEstadoSistema();
                bufferRecepcion = "";
                continue;
            }

            if (tipo == "wifi_config") {
                String ssid = documento["ssid"] | "";
                String contrasena = documento["password"] | "";

                if (ssid.length() == 0 || contrasena.length() == 0) {
                    Serial.println("[Bluetooth]: Credenciales invalidas");
                    bufferRecepcion = "";
                    continue;
                }

                // Guardamos en memoria temporal y levantamos la bandera
                configuracionWiFi.ssid = ssid;
                configuracionWiFi.contrasena = contrasena;
                configuracionDisponible = true;

                Serial.println("[Bluetooth]: Configuracion wifi en cola para prueba.");
                bufferRecepcion = "";
                continue;
            }

            bufferRecepcion = "";
        }
    }

    bool hayConfiguracionWifiDisponible() {
        return configuracionDisponible;
    }

    ConfiguracionWiFiRecibida obtenerConfiguracionWifiRecibida() {
        return configuracionWiFi;
    }

    void limpiarConfiguracionWifi() {
        configuracionWiFi.ssid = "";
        configuracionWiFi.contrasena = "";
        configuracionDisponible = false;
    }

    bool haySolicitudReinicioFabrica() {
        return reinicioFabricaSolicitado;
    }

    void limpiarSolicitudReinicioFabrica() {
        reinicioFabricaSolicitado = false;
    }

    void enviarRespuestaPruebaWiFi(bool exito, const String &ssid) {
        JsonDocument documento;
        documento["wifiConectado"] = exito;
        if (exito) {
            documento["ssid"] = ssid;
        } else {
            documento["errorWifi"] = true;
        }

        String payload;
        serializeJson(documento, payload);
        Serial.println(payload); 
    }

    void enviarEstadoSistema() {
        JsonDocument documento;
        documento["wifiConectado"] = GestorWifi::estaConectado();
        documento["mqttConectado"] = GestorMQTT::estaConectado();
        documento["ssid"] = WiFi.SSID();
        documento["ip"] = WiFi.localIP().toString();
        documento["disponible"] = GestorWifi::estaConectado() && GestorMQTT::estaConectado();

        String respuesta;
        serializeJson(documento, respuesta);
        Serial.println(respuesta);
    }
}