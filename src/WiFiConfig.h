#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H

#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>

Preferences preferences;
WebServer webServer(80);
bool isAPMode = false;

bool isWiFiInStationMode()
{
    return !isAPMode;
}

void handleWebServer()
{
    webServer.handleClient();
}

void startAccessPoint()
{
    WiFi.softAP("CAMERA_SETUP", "12345678");
    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP iniciado com IP: ");
    Serial.println(IP);
    isAPMode = true;

    webServer.on("/", HTTP_GET, []()
                 {
  preferences.begin("wifi", true);
  String ssid = preferences.getString("ssid", "");
  String pass = preferences.getString("password", "");
  String ip = preferences.getString("ip", "");
  String gateway = preferences.getString("gateway", "");
  String subnet = preferences.getString("subnet", "");
  uint8_t camAddr = preferences.getUChar("cam_address", 2);
  preferences.end();

  String html = "<h3>Configurar Wi-Fi</h3>";
  html += "<form action=\"/save\" method=\"POST\">";
  html += "SSID: <input name=\"ssid\" value=\"" + ssid + "\"><br><br>";
  html += "Senha: <input name=\"pass\" type=\"password\" value=\"" + pass + "\"><br><br>";
  html += "IP Fixo: <input name=\"ip\" value=\"" + ip + "\"><br><br>";
  html += "Gateway: <input name=\"gateway\" value=\"" + gateway + "\"><br><br>";
  html += "Subnet: <input name=\"subnet\" value=\"" + subnet + "\"><br><br>";
  
  html += "Endereço da câmera: <input name=\"cam_address\" type=\"number\" min=\"1\" max=\"255\" value=\"" + String(camAddr) + "\"><br><br>";
  html += "<input type=\"submit\" value=\"Salvar\">";
  html += "</form>";

  webServer.send(200, "text/html", html); });

    webServer.on("/save", HTTP_POST, []()
                 {
    String ssid = webServer.arg("ssid");
    String pass = webServer.arg("pass");
    String ip = webServer.arg("ip");
    String gateway = webServer.arg("gateway");
    String subnet = webServer.arg("subnet");
    String camAddrStr = webServer.arg("cam_address");
    uint8_t camAddr = camAddrStr.toInt();

    preferences.begin("wifi", false);
    preferences.putString("ssid", ssid);
    preferences.putString("password", pass);
    preferences.putString("ip", ip);
    preferences.putString("gateway", gateway);
    preferences.putString("subnet", subnet);
    preferences.putUChar("cam_address", camAddr);
    preferences.end();

    webServer.send(200, "text/html", "<h2>Configurações salvas. Reiniciando...</h2>");
    delay(2000);
    ESP.restart(); });

    webServer.begin();
}

bool tryConnectWiFi()
{
    preferences.begin("wifi", true);
    String ssid = preferences.getString("ssid", "");
    String pass = preferences.getString("password", "");
    String ipStr = preferences.getString("ip", "");
    String gatewayStr = preferences.getString("gateway", "");
    String subnetStr = preferences.getString("subnet", "");
    preferences.end();

    if (ssid == "")
    {
        Serial.println("Nenhuma configuração salva. Iniciando AP.");
        startAccessPoint();
        return false;
    }

    WiFi.mode(WIFI_STA);

    if (ipStr.length() > 0 && gatewayStr.length() > 0 && subnetStr.length() > 0)
    {
        IPAddress ip, gateway, subnet;
        if (ip.fromString(ipStr) && gateway.fromString(gatewayStr) && subnet.fromString(subnetStr))
        {
            WiFi.config(ip, gateway, subnet);
            Serial.println("IP fixo configurado:");
            Serial.println(ip);
        }
        else
        {
            Serial.println("Erro ao converter IPs salvos. Usando DHCP.");
        }
    }

    Serial.print("Conectando a: ");
    Serial.println(ssid);
    WiFi.begin(ssid.c_str(), pass.c_str());

    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 10000)
    {
        delay(500);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("\nConectado com sucesso!");
        Serial.print("IP: ");
        Serial.println(WiFi.localIP());
        return true;
    }
    else
    {
        Serial.println("\nFalha na conexão. Iniciando modo AP.");
        startAccessPoint();
        return false;
    }
}

uint8_t getCameraAddress()
{
    preferences.begin("wifi", true);
    uint8_t addr = preferences.getUChar("cam_address", 2); // valor padrão: 2
    preferences.end();
    return addr;
}

#endif