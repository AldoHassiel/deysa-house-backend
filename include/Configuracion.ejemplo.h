#pragma once

// Almacenamiento interno
const int TAMANO_EEPROM = 128;
const char MARCA_WIFI_CONFIGURADA[] = "OK";

// MQTT
const char MQTT_HOST[] = "";
const int MQTT_PUERTO = 8883;
const char MQTT_USUARIO[] = "";
const char MQTT_CONTRASENA[] = "";
const char MQTT_ID_CLIENTE[] = "DEYSAHOUSE-ESP8266";

// Topicos
const char TOPICO_ESTADO_CASA[] = "casa/estado";
const char TOPICO_LUCES[] = "casa/luces";
const char TOPICO_PORTON[] = "casa/porton";
const char TOPICO_PARED_LLOROSA[] = "casa/pared-llorosa";