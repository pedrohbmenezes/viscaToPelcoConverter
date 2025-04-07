#ifndef TOPICS_H
#define TOPICS_H

#include "./config.h"
// Prefixo base
#define MQTT_PREFIX "esp32/" DEVICE_ID "/"

// Tópicos com prefixo do dispositivo
#define MQTT_TOPIC_LOG MQTT_PREFIX "log"
#define MQTT_TOPIC_CONFIG MQTT_PREFIX "config"
#define MQTT_TOPIC_PELCO_COMMAND MQTT_PREFIX "pelco_cmd"
#define MQTT_TOPIC_VISCA_COMMAND MQTT_PREFIX "visca_cmd"
#define MQTT_TOPIC_RESET_WIFI MQTT_PREFIX "cmd/reset_wifi"
#define MQTT_TOPIC_RESTART_DEVICE MQTT_PREFIX "cmd/restart"
#define MQTT_TOPIC_STATUS MQTT_PREFIX "status"

// Tópicos globais (broadcast para todos os dispositivos)
#define MQTT_TOPIC_RESET_ALL "esp32/cmd/reset_all"
#define MQTT_TOPIC_RESTART_ALL "esp32/cmd/restart_all"

// Tópico para publicar identificação
#define MQTT_TOPIC_DEVICE_INFO "esp32/devices/info"

#endif