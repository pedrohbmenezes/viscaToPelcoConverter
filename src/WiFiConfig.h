#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H

#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include "./utils/logUtil.h"
#include "envs/config.h"

// Declaração de variáveis globais
Preferences preferences;     // Gerencia as configurações salvas em memória não volátil
WebServer webServer(80);     // Servidor Web na porta 80
bool isAPMode = false;       // Indica se o dispositivo está no modo Access Point (AP)
const int ledPinn = 2;       // Pino do LED
unsigned long lastBlink = 0; // Controle de tempo para piscar o LED no modo AP
bool ledState = false;       // Estado atual do LED

// Função para verificar se o dispositivo está no modo Station (Wi-Fi conectado)
bool isWiFiInStationMode()
{
    return !isAPMode;
}

// Função para lidar com as requisições do servidor web
void handleWebServer()
{
    webServer.handleClient();
}

// Função para iniciar o modo Access Point (AP)
void startAccessPoint()
{
    // Configura o AP com SSID e senha padrão
    WiFi.softAP("CAMERA_SETUP", "12345678");
    IPAddress IP = WiFi.softAPIP();
    logConfig("Modo AP iniciado com IP: " + IP.toString());
    isAPMode = true;

    // Define a rota para exibir a página de configuração
    webServer.on("/", HTTP_GET, []()
                 {
        preferences.begin("wifi", true);
        String ssid = preferences.getString("ssid", "");
        String pass = preferences.getString("password", "");
        String ip = preferences.getString("ip", "");
        String gateway = preferences.getString("gateway", "");
        String subnet = preferences.getString("subnet", "");
        uint8_t camAddr = preferences.getUChar("cam_address", 2);
        String port = preferences.getString("port", "2000");
        preferences.end();

        // Gera o HTML para a página de configuração
        String html = "<h3>Configurar Wi-Fi</h3>";
        html += "<form action='/save' method='POST'>";
        html += "SSID: <input name='ssid' value='" + ssid + "'><br><br>";
        html += "Senha: <input name='pass' type='password' value='" + pass + "'><br><br>";
        html += "IP Fixo: <input name='ip' value='" + ip + "'><br><br>";
        html += "Gateway: <input name='gateway' value='" + gateway + "'><br><br>";
        html += "Subnet: <input name='subnet' value='" + subnet + "'><br><br>";
        html += "Porta TCP: <input name=\"port\" type=\"number\" value=\"" + port + "\"><br><br>";
        html += "Endereço da câmera: <input name='cam_address' type='number' min='1' max='255' value='" + String(camAddr) + "'><br><br>";
        html += "<input type='submit' value='Salvar'>";
        html += "</form>";

        webServer.send(200, "text/html", html); });

    // Define a rota para salvar as configurações
    webServer.on("/save", HTTP_POST, []()
                 {
        String ssid = webServer.arg("ssid");
        String pass = webServer.arg("pass");
        String ip = webServer.arg("ip");
        String gateway = webServer.arg("gateway");
        String subnet = webServer.arg("subnet");
        String camAddrStr = webServer.arg("cam_address");
        String port = webServer.arg("port");
        uint8_t camAddr = camAddrStr.toInt();

        // Salva as configurações na memória não volátil
        preferences.begin("wifi", false);
        preferences.putString("ssid", ssid);
        preferences.putString("password", pass);
        preferences.putString("ip", ip);
        preferences.putString("gateway", gateway);
        preferences.putString("subnet", subnet);
        preferences.putUChar("cam_address", camAddr);
        preferences.putString("port", port);
        preferences.end();

        logConfig("Configuração salva via Web. SSID: " + ssid + " | IP: " + ip + " | CamAddr: " + String(camAddr));

        // Responde ao cliente e reinicia o dispositivo
        webServer.send(200, "text/html", "<h2>Configurações salvas. Reiniciando...</h2>");
        delay(2000);
        ESP.restart(); });

    webServer.begin(); // Inicia o servidor web
}

// Função para tentar conectar ao Wi-Fi usando as configurações salvas
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
        logConfig("Nenhuma configuração salva. Iniciando modo AP.");
        startAccessPoint();
        return false;
    }

    WiFi.mode(WIFI_STA);

    if (ipStr.length() > 0 && gatewayStr.length() > 0 && subnetStr.length() > 0)
    {
        IPAddress ip, gateway, subnet;
        IPAddress dns1(8, 8, 8, 8); // Google DNS
        IPAddress dns2(1, 1, 1, 1); // Cloudflare DNS
        if (ip.fromString(ipStr) && gateway.fromString(gatewayStr) && subnet.fromString(subnetStr))
        {
            WiFi.config(ip, gateway, subnet, dns1, dns2);
            logConfig("Config IP Fixo:");
            logConfig("IP: " + ip.toString());
            logConfig("Gateway: " + gateway.toString());
            logConfig("Subnet: " + subnet.toString());
        }
        else
        {
            logConfig("Erro ao converter IPs salvos. Usando DHCP.");
        }
    }

    logConfig("Conectando ao WiFi: " + ssid);
    WiFi.begin(ssid.c_str(), pass.c_str());

    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < WIFI_CONNECT_TIMEOUT)
    {
        delay(500);
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        // Teste de conectividade IP + Gateway
        IPAddress gateway = WiFi.gatewayIP();
        WiFiClient client;
        if (client.connect(gateway, 80))
        {
            logConfig("WiFi conectado com sucesso. IP: " + WiFi.localIP().toString());
            return true;
        }
        else
        {
            logConfig("Gateway não responde. Retornando ao modo AP.");
            startAccessPoint();
            return false;
        }
    }

    logConfig("Falha na conexão WiFi. Modo AP iniciado.");
    startAccessPoint();
    return false;
}

// Função para obter o endereço da câmera salvo
uint8_t getCameraAddress()
{
    preferences.begin("wifi", true);
    uint8_t addr = preferences.getUChar("cam_address", 2);
    preferences.end();
    return addr;
}

// Função para obter a porta TCP salva
uint16_t getTcpPort()
{
    preferences.begin("wifi", true);
    String portStr = preferences.getString("port", "2000");
    preferences.end();
    uint16_t port = portStr.toInt();
    return port > 0 ? port : DEFAULT_TCP_PORT;
}

// Função para piscar o LED no modo AP
void blinkAPLed()
{
    unsigned long currentMillis = millis();
    if (currentMillis - lastBlink >= 500)
    {
        lastBlink = currentMillis;
        ledState = !ledState;
        digitalWrite(ledPinn, ledState ? LOW : HIGH);
    }
}

#endif // WIFI_CONFIG_H