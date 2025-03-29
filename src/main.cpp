#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <command.h>
#include <pelco_command.h>
#include "WiFiConfig.h" // Importa o módulo de configuração Wi-Fi
#include "OTASetup.h"

WiFiServer server(2000);
const int ledPin = 2;

unsigned long lastDataReceivedTime = 0;
const unsigned long DATA_TIMEOUT = 600000; // 10 minutos

void blinkLed(int n, int s, int led)
{
  for (int i = 0; i < n; i++)
  {
    digitalWrite(led, LOW);
    delay(s);
    digitalWrite(led, HIGH);
    delay(s);
  }
}

void resetArray(char *array, int size)
{
  for (int i = 0; i < size; i++)
  {
    array[i] = '\0';
  }
}

void processClientData(WiFiClient &client)
{
  char received[350];
  int positions = 0;
  bool messageComplete = false;

  while (client.available() > 0 && !messageComplete && positions < sizeof(received) - 1)
  {
    received[positions] = client.read();
    if (received[positions] == 0xFF)
    {
      messageComplete = true;
    }
    positions++;
  }

  if (messageComplete)
  {
    received[positions] = '\0';
    CommandVisca commandVisca;
    uint8_t *commandBytes = commandVisca.getCommandBytes(received);

    receiveEvent(commandBytes);

    delete[] commandBytes;
    resetArray(received, sizeof(received));
    lastDataReceivedTime = millis();
  }
}

void setup()
{
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);

  bool connected = tryConnectWiFi(); // tenta conectar ou ativa o modo AP

  if (connected)
  {
    server.begin();
    setupOTA();
    startRS485();
    setCameraAddress(getCameraAddress()); // define o endereço da câmera vindo das configs
    Serial.print("Endereço da câmera configurado: ");
    Serial.println(getCameraAddress());
    Serial.println("Servidor iniciado!");
    blinkLed(4, 500, ledPin);
  }
}

void loop()
{
  if (!isWiFiInStationMode())
  {
    handleWebServer(); // mantém o web server do modo AP funcionando
    return;
  }

  WiFiClient client = server.available();
  if (client)
  {
    digitalWrite(ledPin, LOW);
    Serial.println("Cliente conectado");
    lastDataReceivedTime = millis();

    while (client.connected())
    {
      processClientData(client);

      if (millis() - lastDataReceivedTime > DATA_TIMEOUT)
      {
        Serial.println("Tempo de inatividade excedido. Desconectando o cliente.");
        client.stop();
        break;
      }
    }

    Serial.println("Cliente desconectado");
    digitalWrite(ledPin, HIGH);
  }
  handleOTA(); // Verifica atualizações OTA
  delay(10);   // Pequeno atraso para evitar sobrecarga da CPU
}