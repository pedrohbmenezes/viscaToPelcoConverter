#ifndef MQTT_AWS_H
#define MQTT_AWS_H

#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <Preferences.h>
#include "./envs/secret.h"
#include "./envs/topics.h"

WiFiClientSecure secureClient;
PubSubClient mqttClient(secureClient);

const char *MQTT_CLIENT_ID = DEVICE_ID;

unsigned long lastReconnectAttempt = 0;
const unsigned long RECONNECT_INTERVAL = 5000;
bool mqttConnecting = false;
unsigned long mqttConnectStart = 0;
const unsigned long MQTT_CONNECT_TIMEOUT = 5000;

void mqttCallback(char *topic, byte *payload, unsigned int length)
{
    String msg = "";
    for (unsigned int i = 0; i < length; i++)
        msg += (char)payload[i];

    Serial.print("📩 [");
    Serial.print(topic);
    Serial.print("]: ");
    Serial.println(msg);

    // Reset WiFi individual ou broadcast
    if (String(topic) == MQTT_TOPIC_RESET_WIFI || String(topic) == MQTT_TOPIC_RESET_ALL)
    {
        Serial.println("🧨 Reset WiFi solicitado via MQTT");
        Preferences prefs;
        prefs.begin("wifi", false);
        prefs.clear();
        prefs.end();
        delay(1000);
        ESP.restart();
    }

    // Reinício broadcast
    if (String(topic) == MQTT_TOPIC_RESTART_ALL || String(topic) == MQTT_TOPIC_RESTART_DEVICE)
    {
        Serial.println("🧨 Reinício solicitado via MQTT");
        delay(1000);
        ESP.restart();
    }
}

bool connectToMQTT()
{
    String clientId = String(MQTT_CLIENT_ID) + "_" + String(random(0xffff), HEX);
    Serial.print("🔌 Conectando como ");
    Serial.println(clientId);

    if (mqttClient.connect(clientId.c_str()))
    {
        Serial.println("✅ Conectado ao AWS IoT");

        // Subscrições
        mqttClient.subscribe(MQTT_TOPIC_RESET_WIFI);
        mqttClient.subscribe(MQTT_TOPIC_RESET_ALL);
        mqttClient.subscribe(MQTT_TOPIC_RESTART_ALL);
        mqttClient.subscribe(MQTT_TOPIC_RESTART_DEVICE);
        return true;
    }

    Serial.print("❌ Falha: ");
    Serial.println(mqttClient.state());
    return false;
}

bool setupAwsMQTT()
{
    secureClient.setCACert(AWS_ROOT_CA);
    secureClient.setCertificate(DEVICE_CERT);
    secureClient.setPrivateKey(PRIVATE_KEY);

    mqttClient.setServer(AWS_ENDPOINT, 8883);
    mqttClient.setCallback(mqttCallback);

    unsigned long start = millis();
    while (!mqttClient.connected() && millis() - start < 10000)
    {
        if (connectToMQTT())
        {
            // Enviar info do dispositivo ao se conectar
            String payload = "{\"id\":\"" + String(DEVICE_ID) + "\",\"name\":\"" + String(DEVICE_NAME) + "\"}";
            mqttClient.publish(MQTT_TOPIC_DEVICE_INFO, payload.c_str(), true); // retain=true
            return true;
        }
        delay(200);
    }

    Serial.println("❌ Timeout ao conectar ao AWS IoT");
    return false;
}

void handleAwsMQTT()
{
    mqttClient.loop();

    if (mqttClient.connected())
    {
        mqttConnecting = false;
        return;
    }

    if (!mqttConnecting && millis() - lastReconnectAttempt > RECONNECT_INTERVAL)
    {
        Serial.println("🔌 Tentando reconectar ao MQTT...");
        mqttConnectStart = millis();
        lastReconnectAttempt = millis();
        mqttConnecting = true;
    }

    // Inicia tentativa de conexão com timeout
    if (mqttConnecting && millis() - mqttConnectStart < MQTT_CONNECT_TIMEOUT)
    {
        if (connectToMQTT())
        {
            Serial.println("✅ Reconectado ao AWS IoT");
            mqttConnecting = false;
        }
    }
    else if (mqttConnecting && millis() - mqttConnectStart >= MQTT_CONNECT_TIMEOUT)
    {
        Serial.println("❌ Timeout ao reconectar ao AWS IoT");
        mqttConnecting = false;
    }
}

void publishToMQTT(const char *topic, const String &message, bool retain = false)
{
    if (mqttClient.connected())
    {
        mqttClient.publish(topic, message.c_str(), retain);
    }
    // else
    // {
    //     Serial.println("[MQTT AWS] Desconectado");
    // }
}

void logToAWS(const String &msg)
{
    publishToMQTT(MQTT_TOPIC_LOG, msg);
}

void publishStatus()
{
    IPAddress ip = WiFi.localIP();
    String statusPayload = "{";
    statusPayload += "\"id\":\"" + String(DEVICE_ID) + "\",";
    statusPayload += "\"name\":\"" + String(DEVICE_NAME) + "\",";
    statusPayload += "\"ip\":\"" + ip.toString() + "\",";
    statusPayload += "\"uptime\":" + String(millis() / 1000);
    statusPayload += "}";

    publishToMQTT(MQTT_TOPIC_STATUS, statusPayload, false);
}

#endif