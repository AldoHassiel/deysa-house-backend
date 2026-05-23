#include "GestorMQTT.h"
#include "Configuracion.h"
#include "ProcesadorMQTT.h"

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

WiFiClientSecure clienteSeguroWiFi;
PubSubClient clienteMQTT(clienteSeguroWiFi);
CallbackMensajeMQTT callbackUsuario = nullptr;
unsigned long ultimoIntentoConexion = 0;
const unsigned long INTERVALO_RECONEXION_MS = 5000;

void callbackInterno(char *topico, byte *payload, unsigned int longitud){
    if(callbackUsuario == nullptr){
        return;
    }

    String mensaje;
    mensaje.reserve(longitud);

    for(unsigned int i = 0; i < longitud; i++){
        mensaje += static_cast<char>(payload[i]);
    }

    callbackUsuario(String(topico), mensaje);
}

void suscribirseTopicos() {

    clienteMQTT.subscribe(
        "casa/luces"
    );

    clienteMQTT.subscribe(
        "casa/porton"
    );

    clienteMQTT.subscribe(
        "casa/pared-llorosa"
    );

    clienteMQTT.subscribe(
        "casa/estado/obtener"
    );

    Serial.println(
        "[MQTT] Topicos suscritos"
    );
}

namespace GestorMQTT {
    void iniciar(CallbackMensajeMQTT callback){
        callbackUsuario = callback;

        clienteSeguroWiFi.setInsecure();

        clienteMQTT.setServer(
            MQTT_HOST,
            MQTT_PUERTO
        );

        clienteMQTT.setCallback(callbackInterno);

        clienteMQTT.setBufferSize(1024);

        Serial.println("[MQTT]: Cliente MQTT inicializado");
    }

    bool conectar(){
        if (clienteMQTT.connected()) {
            return true;
        }

        Serial.println("[MQTT]: Conectando al broker...");

        bool conectado = clienteMQTT.connect(
            MQTT_ID_CLIENTE,
            MQTT_USUARIO,
            MQTT_CONTRASENA
        );

        if(!conectado){
            Serial.print("[MQTT]: Error de conexion. Codigo: ");
            Serial.println(clienteMQTT.state());

            return false;
        }

        Serial.println("[MQTT]: Conectado correctamente");
        suscribirseTopicos();
        ProcesadorMQTT::publicarEstadoCasa();
        return true;
    }

    void actualizar(){
        if(!WiFi.isConnected()){
            return;
        }

        if(!clienteMQTT.connected()){
            unsigned long tiempoActual = millis();

            if(tiempoActual - ultimoIntentoConexion < INTERVALO_RECONEXION_MS){
                return;
            }

            ultimoIntentoConexion = tiempoActual;
            conectar();

            return;
        }

        clienteMQTT.loop();
    }

    bool estaConectado(){
        return clienteMQTT.connected();
    }

    bool publicar(const String &topico, const String &mensaje, bool retenido){
        if(!clienteMQTT.connected()){
            return false;
        }

        return clienteMQTT.publish(
            topico.c_str(),
            mensaje.c_str(),
            retenido
        );
    }

    bool suscribirse(const String &topico){
        if(!clienteMQTT.connected()){
            return false;
        }

        return clienteMQTT.subscribe(topico.c_str());
    }
}