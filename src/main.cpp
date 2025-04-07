#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <command.h>
#include <pelco_command.h>
#include "WiFiConfig.h"
#include "OTASetup.h"
#include "mqttAws.h"
#include "./utils/logUtil.h"

WiFiServer *serverPtr = nullptr;
const int ledPin = 2;
unsigned long lastDataReceivedTime = 0;
const unsigned long DATA_TIMEOUT = 600000;
static unsigned long lastStatus = 0;

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

void handleConfigsLoop()
{
  handleOTA();
  handleAwsMQTT();
}

void statusDevice()
{
  static unsigned long lastStatusSent = 0;
  unsigned long now = millis();

  if (now - lastStatusSent >= 10000)
  {
    lastStatusSent = now;
    publishStatus(); // Envia status MQTT
  }
}

void setup()
{
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  logMessage("Iniciando ESP32...");
  bool connected = tryConnectWiFi();

  if (connected)
  {
    if (setupAwsMQTT())
    {
      logMessage("🚀 ESP32 conectado ao AWS IoT!");
    }
    IPAddress ip = WiFi.localIP();

    // Lê porta configurada e inicia servidor
    uint16_t tcpPort = getTcpPort();
    serverPtr = new WiFiServer(tcpPort);
    serverPtr->begin();

    setCameraAddress(getCameraAddress());
    startRS485();
    setupOTA();

    logConfig("Endereço da câmera configurado: " + String(getCameraAddress()));
    logMessage("Servidor iniciado e ouvindo na porta " + String(tcpPort) + ".");

    blinkLed(4, 500, ledPin);
  }
  else
  {
    logConfig("Falha ao conectar no WiFi. Modo AP ativado.");
  }
}

void loop()
{
  if (!isWiFiInStationMode())
  {
    handleWebServer();
    blinkAPLed();
    return;
  }

  if (serverPtr)
  {
    WiFiClient client = serverPtr->available();
    if (client)
    {
      digitalWrite(ledPin, LOW);
      logMessage("Cliente conectado");
      lastDataReceivedTime = millis();

      while (client.connected())
      {
        processClientData(client);

        if (millis() - lastDataReceivedTime > DATA_TIMEOUT)
        {
          logMessage("Tempo de inatividade excedido. Cliente foi desconectado.");
          client.stop();
          break;
        }
        handleConfigsLoop();
        statusDevice();
      }

      logMessage("Cliente desconectado");
      digitalWrite(ledPin, HIGH);
    }
  }

  static unsigned long lastReport = 0;
  if (millis() - lastReport > 60000)
  {
    lastReport = millis();
  }

  statusDevice();
  handleConfigsLoop();
  delay(10);
}