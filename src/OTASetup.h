#ifndef OTA_SETUP_H
#define OTA_SETUP_H

#include <ArduinoOTA.h>
#include "./envs/config.h" // ✅ onde está o DEVICE_ID definido

void setupOTA(const char *baseHostname = "camera-controller", const char *password = "123456")
{
    // 🧠 Hostname final com o ID do dispositivo: ex: camera-controller-cam001
    String fullHostname = String(baseHostname) + "-" + String(DEVICE_ID);
    ArduinoOTA.setHostname(fullHostname.c_str());
    ArduinoOTA.setPassword(password);

    ArduinoOTA.onStart([]()
                       { Serial.println("Iniciando atualização OTA..."); });
    ArduinoOTA.onEnd([]()
                     { Serial.println("\nAtualização finalizada!"); });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
                          { Serial.printf("Progresso: %u%%\r", (progress * 100) / total); });
    ArduinoOTA.onError([](ota_error_t error)
                       {
    Serial.printf("Erro [%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Falha na autenticação");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Erro ao iniciar");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Erro de conexão");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Erro ao receber");
    else if (error == OTA_END_ERROR) Serial.println("Erro ao finalizar"); });

    ArduinoOTA.begin();
    Serial.println("OTA pronta.");
}

void handleOTA()
{
    ArduinoOTA.handle();
}

#endif