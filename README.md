# 📷 Controlador de Câmera ESP32 - Gateway VISCA para Pelco-D

## 📋 Descrição Geral do Projeto

Este projeto transforma o **ESP32** em um **controlador inteligente para câmeras PTZ (Pan-Tilt-Zoom)** que atua como gateway de comunicação entre diferentes protocolos. O sistema recebe comandos **VISCA** via TCP/IP através da rede Wi-Fi e os converte automaticamente para comandos **Pelco-D** transmitidos via interface **RS485**.

### 🎯 Propósito
- **Gateway de Protocolos**: Converter comandos VISCA (TCP/IP) para Pelco-D (RS485)
- **Controle Remoto**: Permitir controle de câmeras PTZ através da rede
- **Flexibilidade**: Configuração via interface web intuitiva
- **Modernização**: Integrar câmeras antigas (Pelco-D) com sistemas modernos (VISCA)

### 🏢 Problemas que Resolve
- **Incompatibilidade de Protocolos**: Muitas câmeras antigas usam Pelco-D, mas sistemas modernos usam VISCA
- **Limitações de Cabeamento**: Permite controle via Wi-Fi ao invés de cabos RS485 longos
- **Centralização**: Um único ponto de controle para múltiplas câmeras
- **Configuração Complexa**: Interface web simplifica a configuração de rede e parâmetros

### 👥 Público-Alvo
- **Estúdios de TV e Rádio**: Controle automatizado de câmeras
- **Sistemas de Videomonitoramento**: Integração com softwares de CCTV
- **Salas de Controle**: Operação remota de câmeras PTZ
- **Integradores de AV**: Profissionais que trabalham com automação audiovisual
- **Desenvolvedores DIY**: Projetos de automação residencial ou educacional

---

## 🔧 Pré-requisitos e Instalação

### 📦 Dependências de Hardware
- **ESP32** (qualquer variante compatível com Arduino)
- **Módulo RS485** (ex: MAX485, SN75176)
- **Câmera PTZ compatível com Pelco-D**
- **Fonte de alimentação** 5V ou 3.3V para o ESP32
- **Cabos** para conexões RS485 e alimentação

### 🛠️ Dependências de Software

#### PlatformIO (Recomendado)
```bash
# Instalar PlatformIO CLI
pip install platformio

# Clonar o repositório
git clone [URL_DO_REPOSITORIO]
cd esp32-camera-controller

# Instalar dependências automaticamente
pio lib install
```

#### Arduino IDE (Alternativo)
Instalar as seguintes bibliotecas através do Library Manager:
- `LinkedList` (ivanseidel)
- `EspSoftwareSerial` (plerup)
- `PubSubClient` (knolleary)
- `ArduinoJson` (bblanchon)
- `Adafruit GFX Library`
- `Adafruit SSD1306`
- `Blynk` (opcional)

### ⚡ Instalação e Configuração

#### 1. Configuração do Hardware
```
ESP32 GPIOs         ↔       Módulo RS485 (MAX485)
----------------------------------------------------------
GPIO16 (RX2)        →       RO (Receiver Output)
GPIO17 (TX2)        ←       DI (Driver Input)  
GPIO5  (D5)         →       RE/DE (Control)
GPIO2               →       LED Status (interno)
GPIO0               →       Botão Reset/Boot (interno)
VCC                 →       3.3V ou 5V
GND                 →       GND
```

#### 2. Configuração do Software
```bash
# Compilar e fazer upload via USB
pio run --target upload

# Ou compilar para upload OTA (após primeira configuração)
pio run --target upload --upload-port camera-controller-cam002.local
```

#### 3. Primeira Configuração
1. **Após o primeiro upload**, o ESP32 criará um Access Point chamado `CAMERA_SETUP`
2. **Conectar ao AP** com a senha: `12345678`
3. **Acessar** `http://192.168.4.1` no navegador
4. **Configurar**:
   - SSID e senha do Wi-Fi
   - IP fixo (opcional)
   - Porta TCP (padrão: 2000)
   - Endereço da câmera (1-255)
5. **Salvar** configurações e aguardar reinicialização

#### 4. Verificação de Funcionamento
```bash
# Testar conexão TCP (Linux/Mac)
telnet [IP_DO_ESP32] 2000

# Exemplo de comando VISCA (mover câmera para cima)
echo -e "\x81\x01\x06\x01\x00\x00\x03\x01\xFF" | nc [IP_DO_ESP32] 2000
```

---

## 💡 Exemplos de Uso

### 🎮 Comandos Básicos via TCP

#### Conectar via Telnet/Netcat
```bash
# Conectar ao ESP32
telnet 192.168.1.100 2000

# Ou usando netcat
nc 192.168.1.100 2000
```

#### Comandos de Movimento VISCA (em hexadecimal)
```bash
# Mover para cima
81 01 06 01 00 00 03 01 FF

# Mover para baixo  
81 01 06 01 00 00 03 02 FF

# Mover para esquerda
81 01 06 01 00 00 01 03 FF

# Mover para direita
81 01 06 01 00 00 02 03 FF

# Parar movimento
81 01 06 01 00 00 03 03 FF
```

#### Comandos de Zoom
```bash
# Zoom In (velocidade 2)
81 01 04 07 22 FF

# Zoom Out (velocidade 2)  
81 01 04 07 32 FF

# Parar Zoom
81 01 04 07 00 FF
```

#### Comandos de Preset
```bash
# Salvar preset 1
81 01 04 3F 01 01 FF

# Chamar preset 1
81 01 04 3F 02 02 FF
```

### 🐍 Exemplo em Python
```python
import socket
import time

class CameraController:
    def __init__(self, esp32_ip, port=2000):
        self.ip = esp32_ip
        self.port = port
        
    def send_command(self, command_bytes):
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.connect((self.ip, self.port))
            sock.send(bytes(command_bytes))
            sock.close()
            print(f"Comando enviado: {[hex(b) for b in command_bytes]}")
        except Exception as e:
            print(f"Erro: {e}")
    
    def move_up(self):
        self.send_command([0x81, 0x01, 0x06, 0x01, 0x00, 0x00, 0x03, 0x01, 0xFF])
    
    def move_down(self):
        self.send_command([0x81, 0x01, 0x06, 0x01, 0x00, 0x00, 0x03, 0x02, 0xFF])
    
    def stop(self):
        self.send_command([0x81, 0x01, 0x06, 0x01, 0x00, 0x00, 0x03, 0x03, 0xFF])
    
    def zoom_in(self):
        self.send_command([0x81, 0x01, 0x04, 0x07, 0x22, 0xFF])
    
    def call_preset(self, preset_number):
        self.send_command([0x81, 0x01, 0x04, 0x3F, 0x02, preset_number + 1, 0xFF])

# Uso
camera = CameraController("192.168.1.100")
camera.move_up()
time.sleep(2)
camera.stop()
camera.call_preset(1)
```

### 🌐 Controle via Interface Web (modo AP)
Quando em modo de configuração, acesse `http://192.168.4.1`:
- Configure rede Wi-Fi
- Defina IP fixo (opcional)
- Configure porta TCP personalizada
- Ajuste endereço da câmera Pelco-D

---

## 📁 Estrutura do Projeto

```
esp32-camera-controller/
├── 📄 README.md                    # Documentação principal
├── 📄 platformio.ini               # Configurações do PlatformIO
├── 📄 ota_upload.py                # Script para upload OTA
├── 📄 .gitignore                   # Arquivos ignorados pelo Git
├── 📂 src/                         # Código fonte principal
│   ├── 📄 main.cpp                 # Loop principal e configuração inicial
│   ├── 📄 WiFiConfig.h             # Gerenciamento de Wi-Fi e interface web
│   ├── 📄 command.h                # Interpretação de comandos VISCA
│   ├── 📄 pelco_command.h          # Geração e envio de comandos Pelco-D
│   ├── 📄 OTASetup.h               # Configuração de atualizações OTA
│   ├── 📄 mqttAws.h                # Integração com AWS IoT Core
│   ├── 📂 envs/                    # Configurações do ambiente
│   │   ├── 📄 config.h             # Configurações principais (ID, portas)
│   │   └── 📄 topics.h             # Tópicos MQTT
│   └── 📂 utils/                   # Utilitários
│       └── 📄 logUtil.h            # Sistema de logs (Serial + MQTT)
├── 📂 include/                     # Headers globais (padrão PlatformIO)
│   └── 📄 README                   # Documentação padrão do PlatformIO
├── 📂 lib/                         # Bibliotecas customizadas (se houver)
│   └── 📄 README                   # Documentação de bibliotecas
└── 📂 test/                        # Testes unitários (futuro)
```

### 📋 Descrição dos Arquivos Principais

| Arquivo | Responsabilidade | Funcionalidades |
|---------|------------------|-----------------|
| **main.cpp** | Loop principal e coordenação | Inicialização, servidor TCP, processamento de clientes |
| **WiFiConfig.h** | Gerenciamento de conectividade | STA/AP, interface web, persistência de configurações |
| **command.h** | Processamento VISCA | Interpretação e conversão de comandos VISCA |
| **pelco_command.h** | Comunicação RS485 | Geração e envio de comandos Pelco-D |
| **OTASetup.h** | Atualizações remotas | Configuração e handling de OTA |
| **mqttAws.h** | Conectividade IoT | Comunicação com AWS IoT Core |
| **logUtil.h** | Sistema de logs | Logs locais e remotos via MQTT |

---

## 🧩 Principais Funções, Classes e Módulos

### 📊 Classe `CommandVisca` (command.h)

#### Propósito
Interpreta comandos VISCA recebidos via TCP e os converte para um formato interno.

#### Métodos Principais
```cpp
class CommandVisca {
public:
    // Verifica se é comando de salvar preset
    bool isSetPresent(char *command, int presentNumber);
    
    // Verifica se é comando de chamar preset  
    bool isCallPresent(char *command, int presentNumber);
    
    // Verifica comandos direcionais
    bool isCommandCima(char *command);
    bool isCommandBaixo(char *command);
    bool isCommandEsquerda(char *command);
    bool isCommandDireita(char *command);
    
    // Verifica comandos de zoom
    bool zoomIn(char *command);
    bool zoomOut(char *command);
    bool zoomStop(char *command);
    
    // Conversão principal: VISCA → formato interno
    uint8_t* getCommandBytes(char *command);
};
```

#### Como Usar
```cpp
CommandVisca commandVisca;
char received[350]; // Buffer com comando VISCA
uint8_t *commandBytes = commandVisca.getCommandBytes(received);
receiveEvent(commandBytes); // Processa comando convertido
delete[] commandBytes; // Limpa memória
```

### 🎛️ Funções de Controle Pelco-D (pelco_command.h)

#### Funções de Movimento
```cpp
// Parar todos os movimentos
void stopCamera();

// Movimento direcional
void moveCamera(byte command, const char *message);

// Comandos específicos (internos)
void handleDirectionalZoomCommand(uint8_t command);
```

#### Funções de Preset
```cpp
// Salvar posição atual no preset
void setPreset(byte presetID);

// Mover câmera para preset salvo
void callPreset(byte presetID); 

// Limpar preset da memória
void clearPreset(byte presetID);
```

#### Função Principal de Processamento
```cpp
// Processa comando interno e envia via RS485
void receiveEvent(uint8_t *command);
```

#### Configuração
```cpp
// Define endereço da câmera (1-255)
void setCameraAddress(byte addr);

// Inicializa comunicação RS485
void startRS485();
```

### 🌐 Sistema de Wi-Fi (WiFiConfig.h)

#### Funções de Conectividade
```cpp
// Tenta conectar ao Wi-Fi salvo
bool tryConnectWiFi();

// Inicia modo Access Point para configuração
void startAccessPoint();

// Verifica se está em modo Station (conectado)
bool isWiFiInStationMode();

// Processa requisições da interface web
void handleWebServer();
```

#### Funções de Configuração
```cpp
// Obtém endereço da câmera salvo
uint8_t getCameraAddress();

// Obtém porta TCP configurada
uint16_t getTcpPort();

// LED indicador de status AP
void blinkAPLed();
```

### 📡 Sistema de Logs (logUtil.h)

#### Funções de Log
```cpp
// Log genérico com timestamp
void logMessage(const String &message, bool sendToAWS = true);

// Log de configurações (persistente)
void logConfig(const String &message, bool retain = true);

// Log específico de comandos Pelco-D
void logPelcoCommand(const String &cmdDescription);

// Log específico de comandos VISCA
void logViscaCommand(const String &cmdDescription);
```

### 🔄 Sistema OTA (OTASetup.h)

#### Funções OTA
```cpp
// Configura atualização Over-the-Air
void setupOTA(const char *baseHostname = "camera-controller", 
              const char *password = "123456");

// Processa requisições OTA (deve ser chamado no loop)
void handleOTA();
```

---

## 🔧 Configurações Avançadas

### 📋 Configurações em `config.h`
```cpp
#define DEVICE_ID "cam002"                    // ID único do dispositivo
#define DEVICE_NAME "Camera Nossa Senhora"    // Nome descritivo
#define DEFAULT_TCP_PORT 2000                 // Porta TCP padrão
#define DEFAULT_AP_SSID "CAMERA_SETUP"       // SSID do Access Point
#define DEFAULT_AP_PASS "12345678"           // Senha do Access Point
#define WIFI_CONNECT_TIMEOUT 10000           // Timeout de conexão Wi-Fi (ms)
```

### 🏷️ Tópicos MQTT (topics.h)
```cpp
// Logs e configurações
#define MQTT_TOPIC_LOG "esp32/cam002/log"
#define MQTT_TOPIC_CONFIG "esp32/cam002/config"

// Comandos
#define MQTT_TOPIC_PELCO_COMMAND "esp32/cam002/pelco_cmd"
#define MQTT_TOPIC_VISCA_COMMAND "esp32/cam002/visca_cmd"

// Controle remoto
#define MQTT_TOPIC_RESET_WIFI "esp32/cam002/cmd/reset_wifi"
#define MQTT_TOPIC_RESTART_DEVICE "esp32/cam002/cmd/restart"

// Status
#define MQTT_TOPIC_STATUS "esp32/cam002/status"
```

### 🔌 Mapeamento de Pinos
```cpp
// Comunicação RS485
#define RS485_RX_PIN 16      // Recepção RS485
#define RS485_TX_PIN 17      // Transmissão RS485  
#define RS485_RE_DE_PIN 5    // Controle direção RS485

// Interface
#define LED_STATUS_PIN 2     // LED de status
#define BOOT_BUTTON_PIN 0    // Botão de reset (interno)
```

---

## 🚀 Atualizações OTA (Over-the-Air)

### 📡 Configuração Automática
O sistema configura automaticamente o hostname como: `camera-controller-[DEVICE_ID].local`

**Exemplo**: `camera-controller-cam002.local`

### 🔐 Credenciais OTA
- **Porta**: 3232
- **Senha**: 123456

### 💻 Upload via PlatformIO
```bash
# Upload OTA básico
pio run --target upload --upload-port camera-controller-cam002.local

# Upload com logs detalhados
pio run --target upload --upload-port camera-controller-cam002.local --verbose
```

### ⚙️ Configuração `platformio.ini` para OTA
```ini
[env:esp32-ota]
platform = espressif32
board = alksesp32
framework = arduino
upload_protocol = espota
upload_port = camera-controller-cam002.local
upload_flags =
  --auth=123456
  --port=3232
extra_scripts = ota_upload.py
```

---

## 🔄 Reset e Troubleshooting

### 🔧 Reset de Configurações
**Método 1 - Botão Físico:**
1. Manter pressionado o botão **BOOT (GPIO0)** durante a inicialização
2. Manter por **3 segundos** após ligar
3. O sistema apagará todas as configurações e retornará ao modo AP

**Método 2 - Via MQTT:**
```bash
# Publicar no tópico de reset (se conectado)
mosquitto_pub -h [BROKER_MQTT] -t "esp32/cam002/cmd/reset_wifi" -m "reset"
```

### 🚨 Indicadores de Status

#### LED Integrado (GPIO2)
- **Piscando (500ms)**: Modo Access Point ativo
- **4 piscadas rápidas**: Conexão Wi-Fi bem-sucedida
- **Aceso durante uso**: Cliente TCP conectado

#### Logs Serial (115200 baud)
```
[123s] Iniciando ESP32...
[125s] [CONFIG] WiFi conectado com sucesso. IP: 192.168.1.100
[126s] [CONFIG] Endereço da câmera configurado: 2
[127s] Servidor iniciado e ouvindo na porta 2000.
[130s] Cliente conectado
[131s] [PELCO] Cima
```

### 🐛 Problemas Comuns

#### Não Conecta ao Wi-Fi
1. Verificar SSID e senha
2. Verificar se rede está em 2.4GHz (ESP32 não suporta 5GHz)
3. Resetar configurações e reconfigurar

#### Não Recebe Comandos TCP
1. Verificar porta configurada (padrão: 2000)
2. Testar conectividade: `telnet [IP] [PORTA]`
3. Verificar firewall

#### Câmera Não Responde
1. Verificar conexões RS485
2. Confirmar endereço da câmera (1-255)
3. Verificar alimentação da câmera
4. Testar com baudrate 9600 (padrão Pelco-D)

---

## 🌐 Integração MQTT/AWS IoT

### ☁️ Configuração AWS IoT Core
O sistema possui integração completa com AWS IoT Core para monitoramento e controle remoto.

#### Funcionalidades MQTT
- **Logs Remotos**: Todos os logs são enviados para AWS
- **Status Periódico**: Heartbeat a cada 10 segundos
- **Controle Remoto**: Reset e restart via MQTT
- **Monitoramento**: Comandos Pelco-D e VISCA logados

#### Tópicos Utilizados
```
esp32/cam002/log            # Logs gerais
esp32/cam002/config         # Logs de configuração  
esp32/cam002/pelco_cmd      # Comandos Pelco-D executados
esp32/cam002/status         # Status do dispositivo
esp32/cmd/reset_all         # Reset broadcast (todos os dispositivos)
```

---

## 🤝 Como Contribuir

### 🛠️ Configuração do Ambiente de Desenvolvimento

#### 1. Fork e Clone
```bash
# Fork o repositório no GitHub
# Clone seu fork
git clone https://github.com/SEU_USUARIO/esp32-camera-controller.git
cd esp32-camera-controller

# Adicionar repositório upstream
git remote add upstream https://github.com/USUARIO_ORIGINAL/esp32-camera-controller.git
```

#### 2. Instalar Dependências
```bash
# Instalar PlatformIO
pip install platformio

# Instalar dependências do projeto
pio lib install
```

#### 3. Configurar Ambiente
```bash
# Copiar configurações de exemplo
cp src/envs/config.h.example src/envs/config.h

# Editar com suas configurações
nano src/envs/config.h
```

### 📋 Diretrizes de Contribuição

#### Padrões de Código
- **Indentação**: 4 espaços
- **Nomenclatura**: camelCase para funções, UPPER_CASE para constantes
- **Comentários**: Em português, descritivos
- **Headers**: Sempre incluir guards (`#ifndef/#define/#endif`)

#### Estrutura de Commits
```
tipo(escopo): descrição concisa

Descrição mais detalhada se necessário.

- Mudança específica 1
- Mudança específica 2

Closes #123
```

**Tipos de commit:**
- `feat`: Nova funcionalidade
- `fix`: Correção de bug
- `docs`: Documentação
- `style`: Formatação de código
- `refactor`: Refatoração
- `test`: Testes
- `chore`: Tarefas de manutenção

#### Exemplo de Commit
```bash
git commit -m "feat(mqtt): adicionar suporte a SSL/TLS

Implementa conexão segura com AWS IoT Core usando certificados.

- Adicionar configuração de certificados
- Implementar validação SSL
- Atualizar documentação de setup

Closes #42"
```

### 🧪 Testes

#### Testes Manuais
```bash
# Compilar sem upload
pio run

# Verificar sintaxe
pio check

# Monitor serial
pio device monitor --baud 115200
```

#### Testes de Integração
```bash
# Testar comandos VISCA
echo -e "\x81\x01\x06\x01\x00\x00\x03\x01\xFF" | nc [IP_ESP32] 2000

# Verificar logs MQTT
mosquitto_sub -h [BROKER] -t "esp32/+/+"
```

### 📝 Reportar Issues

#### Template de Bug Report
```markdown
**Descrição do Bug**
Descrição clara e concisa do problema.

**Passos para Reproduzir**
1. Configurar ESP32 com...
2. Enviar comando...
3. Verificar comportamento...

**Comportamento Esperado**
O que deveria acontecer.

**Comportamento Atual**
O que realmente acontece.

**Ambiente**
- Placa: ESP32-WROOM-32
- PlatformIO: v6.1.0
- Firmware: v1.2.0

**Logs**
```
[timestamp] log relevante
```

**Screenshots/Vídeos**
Se aplicável.
```

### 💡 Sugestões de Melhorias

#### Roadmap Atual
- [ ] Interface web mais moderna (React/Vue.js)
- [ ] Suporte a HTTPS na interface web
- [ ] Backup/restore de configurações
- [ ] Suporte a múltiplas câmeras
- [ ] Dashboard de monitoramento
- [ ] API RESTful
- [ ] Suporte a outros protocolos (ONVIF, etc.)
- [ ] Configuração via Bluetooth
- [ ] Logs estruturados (JSON)
- [ ] Métricas de performance

#### Como Sugerir Funcionalidades
1. **Verificar Issues existentes** para evitar duplicatas
2. **Abrir novo Issue** com label "enhancement"
3. **Descrever caso de uso** específico
4. **Propor implementação** se possível
5. **Discutir** com mantenedores antes de implementar

### 🏅 Reconhecimentos

#### Contribuidores Atuais
- **Pedro Henrique** - Autor original e mantenedor principal

#### Como Ser Reconhecido
- Contribuições significativas são adicionadas ao README
- Commits seguem padrão de co-autoria quando aplicável
- Issues e PRs recebem labels de reconhecimento

---

## 📞 Suporte e Contato

### 🐛 Reportar Problemas
- **GitHub Issues**: Para bugs e sugestões
- **Discussões**: Para dúvidas e discussões gerais

### 📚 Documentação Adicional
- **Protocolo VISCA**: [Sony VISCA Documentation](https://pro.sony/s3/cms-static-content/file/06/1237494271006.pdf)
- **Protocolo Pelco-D**: [Pelco-D Specification](https://www.pelco.com/sites/default/files/legacy/manuals/C681M-D.pdf)
- **ESP32 Reference**: [Espressif Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/)

---

## 📄 Licença

Este projeto está licenciado sob a **MIT License** - veja o arquivo [LICENSE](LICENSE) para detalhes.

### Resumo da Licença
- ✅ Uso comercial permitido
- ✅ Modificação permitida
- ✅ Distribuição permitida
- ✅ Uso privado permitido
- ❌ Garantia não fornecida
- ❌ Responsabilidade não assumida

---

## 🎉 Agradecimentos

Desenvolvido com foco em **conectividade**, **flexibilidade** e **facilidade de uso**. 

**Obrigado por usar o Controlador de Câmera ESP32!** 

Se este projeto foi útil para você, considere:
- ⭐ Dar uma **estrela** no GitHub
- 🐛 **Reportar bugs** encontrados  
- 💡 **Sugerir melhorias**
- 🤝 **Contribuir** com código
- 📢 **Compartilhar** com outros desenvolvedores

---

**Versão da Documentação**: 2.0  
**Última Atualização**: Dezembro 2024  
**Compatibilidade**: ESP32 (todas as variantes), PlatformIO 6.0+

