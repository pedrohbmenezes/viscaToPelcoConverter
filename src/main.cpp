#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <command.h>

const char *ssid = "CLARO_648DFE-IOT";
const char *password = "TGtgnCCaGm";

IPAddress ip(192, 168, 1, 158);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

WiFiServer server(80);
const int ledPin = LED_BUILTIN;

unsigned long lastDataReceivedTime = 0;
const unsigned long DATA_TIMEOUT = 600000; // 10 minutes in milliseconds

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

void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Wire.begin(D1, D2);
  pinMode(ledPin, OUTPUT);
  WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
  }

  server.begin();

  Serial.println("Servidor iniciado!");
  blinkLed(4, 500, ledPin);
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
    const uint8_t *commandBytes = commandVisca.getCommandBytes(received);
    int numBytes = 3;

    Wire.beginTransmission(8);
    for (int i = 0; i < numBytes; i++)
    {
      Wire.write(commandBytes[i]);
    }
    Wire.endTransmission();

    delete[] commandBytes;
    resetArray(received, sizeof(received));

    // Atualiza o tempo do último dado recebido
    lastDataReceivedTime = millis();
  }
}

void loop()
{
  WiFiClient client = server.available();
  if (client)
  {
    digitalWrite(ledPin, LOW);
    Serial.println("Cliente conectado");

    // Atualiza o tempo do último dado recebido
    lastDataReceivedTime = millis();

    while (client.connected())
    {
      processClientData(client);

      // Verifica se o tempo de inatividade excedeu o limite
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
}
