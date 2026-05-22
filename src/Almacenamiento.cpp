#include "Almacenamiento.h"
#include "Configuracion.h"

#include <EEPROM.h>
#include <string.h>

CredencialesWiFi credencialesWiFi;

namespace Almacenamiento {
    void iniciar(){
        EEPROM.begin(TAMANO_EEPROM);
    }

    bool existenCredencialesWifi(){
        EEPROM.get(0, credencialesWiFi);

        return strcmp(credencialesWiFi.marca, MARCA_WIFI_CONFIGURADA) == 0;
    }

    bool cargarCredencialesWifi(String &ssid, String &contrasena){
        if(!existenCredencialesWifi()){
            return false;
        }

        ssid = String(credencialesWiFi.ssid);
        contrasena = String(credencialesWiFi.contrasena);

        return true;
    }

    bool guardarCredencialesWifi(const String &ssid, const String &contrasena){
        memset(&credencialesWiFi, 0, sizeof(credencialesWiFi));

        strncpy(credencialesWiFi.ssid, ssid.c_str(), sizeof(credencialesWiFi.ssid) - 1);
        strncpy(credencialesWiFi.contrasena, contrasena.c_str(), sizeof(credencialesWiFi.contrasena) - 1);
        strncpy(credencialesWiFi.marca, MARCA_WIFI_CONFIGURADA, sizeof(credencialesWiFi.marca) - 1);

        EEPROM.put(0, credencialesWiFi);
        return EEPROM.commit();
    }

    void borrarCredencialesWifi(){
        CredencialesWiFi credencialesVacias;

        memset(&credencialesVacias, 0, sizeof(credencialesVacias));

        EEPROM.put(0, credencialesVacias);
        EEPROM.commit();
    }
}