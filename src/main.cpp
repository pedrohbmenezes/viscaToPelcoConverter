#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <command.h>
#include <pelco_command.h>

const char *ssid = "cameraPNSR1";
const char *password = "05011920PNSR";

IPAddress ip(10, 0, 0, 20);
IPAddress gateway(10, 0, 0, 1);
IPAddress subnet(255, 255, 255, 0);

WiFiServer server(2000);
const int ledPin = 2;

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
    int numBytes = 3;

    receiveEvent(commandBytes);

    delete[] commandBytes;
    resetArray(received, sizeof(received));

    // Atualiza o tempo do último dado recebido
    lastDataReceivedTime = millis();
  }
}
void verifyConnection()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
      blinkLed(5, 100, ledPin);
      Serial.println("Conectando...");
      delay(1000);
    }
    Serial.println("Conectado");
  }
};

void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.print("Configurando Camera");
  Serial.print(address);
  Serial.println();
  pinMode(ledPin, OUTPUT);
  WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    blinkLed(5, 200, ledPin);
    Serial.println("Conectando...");
    delay(1000);
  }

  server.begin();
  startRS485();
  Serial.println("Servidor iniciado!");
  blinkLed(4, 500, ledPin);
}

void loop()
{
  WiFiClient client = server.available();
  verifyConnection();
  if (client)
  {
    digitalWrite(ledPin, LOW);
    Serial.println("Cliente conectado");
    // Atualiza o tempo do último dado recebido
    lastDataReceivedTime = millis();

    while (client.connected())
    {
      processClientData(client);

      // verifica o estado do wifi
      verifyConnection();
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
