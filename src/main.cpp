#include <Arduino.h>
#include <ESP8266WiFi.h>

const char *ssid = "QLVD_NET";
const char *password = "qu@l1v1d@";

IPAddress ip(10, 10, 3, 158); // Endereço IP desejado
IPAddress gateway(10, 10, 1, 1);
IPAddress subnet(255, 255, 0, 0);

WiFiServer server(80);          // Cria o objeto WiFiServer
const int ledPin = LED_BUILTIN; // Pino do LED embutido (D4 no ESP8266)

void setup()
{
  // Inicializa a comunicação serial
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println("Serial funcionando no PlatformIO!");

  // Configura o pino do LED como saída
  pinMode(ledPin, OUTPUT);

  // Conecta-se à rede Wi-Fi
  WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
  }

  // Inicia o servidor TCP
  server.begin();

  // Exibe mensagem no terminal serial
  Serial.println("Servidor iniciado!");
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
  digitalWrite(LED_BUILTIN, HIGH);

  // Pisca o LED continuamente
}

void loop()
{
  WiFiClient client = server.available(); // Verifica se há conexões de entrada
  if (client)
  {
    // Cliente conectado
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("Cliente conectado");
    while (client.connected())
    {
      if (client.available())
      {
        // Dados recebidos do cliente
        char data = client.read();
        String dataformat = String(data);
        Serial.println(dataformat);
        // Faça algo com os dados recebidos
      }
    }
    // Cliente desconectado
    client.stop();
    Serial.println("Cliente desconectado");
    digitalWrite(LED_BUILTIN, HIGH);
  }
}
