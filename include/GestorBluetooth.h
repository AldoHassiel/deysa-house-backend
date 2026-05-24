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
}