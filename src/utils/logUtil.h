#ifndef LOG_UTIL_H
#define LOG_UTIL_H

#include <Arduino.h>
#include "../mqttAws.h"
#include "../envs/topics.h"

inline String getTimestamp()
{
    return "[" + String(millis() / 1000) + "s] ";
}

// Log genérico (usado por logMessage)
inline void logMessage(const String &message, bool sendToAWS = true)
{
    String finalMsg = getTimestamp() + message;
    Serial.println(finalMsg);
    if (sendToAWS)
    {
        publishToMQTT(MQTT_TOPIC_LOG, finalMsg);
    }
}

// Log de configuração
inline void logConfig(const String &message, bool retain = true)
{
    String finalMsg = getTimestamp() + "[CONFIG] " + message;
    Serial.println(finalMsg);
    publishToMQTT(MQTT_TOPIC_CONFIG, finalMsg, retain);
}

// Log de comando Pelco-D
inline void logPelcoCommand(const String &cmdDescription)
{
    String finalMsg = getTimestamp() + "[PELCO] " + cmdDescription;
    Serial.println(finalMsg);
    publishToMQTT(MQTT_TOPIC_PELCO_COMMAND, finalMsg);
}

// Futuro: log VISCA
inline void logViscaCommand(const String &cmdDescription)
{
    String finalMsg = getTimestamp() + "[VISCA] " + cmdDescription;
    Serial.println(finalMsg);
    publishToMQTT(MQTT_TOPIC_VISCA_COMMAND, finalMsg);
}

#endif