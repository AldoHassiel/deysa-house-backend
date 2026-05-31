#pragma once

#include <Arduino.h>


struct ConfiguracionWiFiRecibida {
  String ssid;
  String contrasena;
};

namespace GestorBluetooth{
    void iniciar();
    void actualizar();
    bool hayConfiguracionWifiDisponible();
    ConfiguracionWiFiRecibida obtenerConfiguracionWifiRecibida();
    void limpiarConfiguracionWifi();
    bool haySolicitudReinicioFabrica();
    void limpiarSolicitudReinicioFabrica();
    void enviarEstadoConexion(bool wifiConectado,const String &ssid,bool mqttConectado);
    void enviarEstadoSistema();
    void enviarRespuestaPruebaWiFi(bool exito, const String &ssid);
}