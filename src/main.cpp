#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <command.h>
#include <Wire.h>

const char *ssid = "CLARO_648DFE-IOT";
const char *password = "TGtgnCCaGm";

IPAddress ip(192, 168, 1, 158);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

WiFiServer server(80);
const int ledPin = LED_BUILTIN;
const int rele = 2;

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
  pinMode(rele, OUTPUT);

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

void loop()
{
  WiFiClient client = server.available();
  if (client)
  {
    // Cliente conectado
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("Cliente conectado");
    while (client.connected())
    {
      char recived[350];
      int positions = 0;
      while (client.available() > 0)
      {
        // Dados recebidos do cliente
        recived[positions] = client.read();
        if (0xFF == recived[positions])
        {
          break;
        }
        positions++;
      }

      if (recived[0] != '\0')
      {
        CommandVisca commandVisca;
        const String commandName = commandVisca.getCommandName(recived);
        Serial.println(commandName);
        Wire.beginTransmission(8);
        Wire.write(commandName.c_str());
        Wire.endTransmission();
        resetArray(recived, sizeof(recived));
      }
    }
    // Cliente desconectado
    client.stop();
    Serial.println("Cliente desconectado");
    digitalWrite(LED_BUILTIN, LOW);
  }
}
