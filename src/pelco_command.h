#include <Wire.h>
#include <Arduino.h>
#include <SoftwareSerial.h>
#define RS485Transmit HIGH
#define RS485Receive LOW

const byte rePin = 5;
const byte LEDi = 13;

const byte address = 1; // Endereço da câmera
byte speed = 100;       // Pode ser alterada pressionando esquerda+cima ou esquerda+baixo

const byte C_STOP = 0x00;
const byte C_UP = 0x08;
const byte C_DOWN = 0x10;
const byte C_LEFT = 0x04;
const byte C_RIGHT = 0x02;
const byte C_ZOOMIN = 0x20;
const byte C_ZOOMOUT = 0x40;
const byte C_SET_PRESET = 0x03;
const byte C_CLEAR_PRESET = 0x05;
const byte C_CALL_PRESET = 0x07;

bool stopped = false;

SoftwareSerial SerialRS(3, 4); // Pinos RX e TX para a comunicação RS485

void sendPelcoDFrame(byte command, byte data1, byte data2)
{
    byte bytes[7] = {0xFF, address, 0x00, command, data1, data2, 0x00};
    byte crc = (bytes[1] + bytes[2] + bytes[3] + bytes[4] + bytes[5]) % 0x100;
    bytes[6] = crc;

    for (int i = 0; i < 7; i++)
    {
        SerialRS.write(bytes[i]);
    }
}

void blinkLED()
{
    for (int i = 0; i < 4; i++)
    {
        digitalWrite(LEDi, HIGH);
        delay(100);
        digitalWrite(LEDi, LOW);
        delay(100);
    }
}

void stopCamera()
{
    sendPelcoDFrame(C_STOP, 0x00, 0x00);
    Serial.println("Pare");
}

void moveCamera(byte command, const char *message)
{
    sendPelcoDFrame(command, speed, speed);
    Serial.println(message);
}

void setPreset(byte presetID)
{
    byte data1 = 0x00;
    byte data2 = presetID;
    sendPelcoDFrame(C_SET_PRESET, data1, data2);
    Serial.println("Set Preset");
    delay(500);
    stopCamera();
}

void clearPreset(byte presetID)
{
    byte data1 = 0x00;
    byte data2 = presetID;
    sendPelcoDFrame(C_CLEAR_PRESET, data1, data2);
    Serial.println("Clear Preset");
    delay(500);
    stopCamera();
}

void callPreset(byte presetID)
{
    byte data1 = 0x00;
    byte data2 = presetID;
    sendPelcoDFrame(C_CALL_PRESET, data1, data2);
    Serial.println("Call Preset");
    delay(500);
    stopCamera();
}

void handleDirectionalZoomCommand(uint8_t command)
{
    switch (command)
    {
    case 0x01: // Direita
        moveCamera(C_RIGHT, "Direita");
        break;
    case 0x02: // Pare
        stopCamera();
        break;
    case 0x03: // Esquerda
        moveCamera(C_LEFT, "Esquerda");
        break;
    case 0x04: // Cima
        moveCamera(C_UP, "Cima");
        break;
    case 0x05: // Baixo
        moveCamera(C_DOWN, "Baixo");
        break;
    case 0x06: // Zoom In
        moveCamera(C_ZOOMIN, "Zoom In");
        break;
    case 0x07: // Zoom Out
        moveCamera(C_ZOOMOUT, "Zoom Out");
        break;
    default:
        // Comando inválido
        Serial.println("Comando direcional/zoom inválido!");
        break;
    }
}

void receiveEvent(int byteCount)
{
    Serial.println("Recebeu comando");

    while (Wire.available())
    {
        uint8_t command[3];
        for (int i = 0; i < 3; i++)
        {
            command[i] = Wire.read();
        }

        Serial.print("Comando recebido: ");
        for (int i = 0; i < 3; i++)
        {
            Serial.print("0x");
            Serial.print(command[i], HEX);
            Serial.print(" ");
        }
        Serial.println();

        if (command[0] != 0xFF)
        {
            Serial.println("Comando inválido!");
            continue;
        }

        switch (command[1])
        {
        case 0x02: // Comandos direcionais e de zoom
            handleDirectionalZoomCommand(command[2]);
            break;
        case 0x03:                  // Call Preset
            callPreset(command[2]); // Exemplo de Preset ID = command[1]
            break;
        default:
            break;
        }
    }
}

void startRS485()
{
    pinMode(rePin, OUTPUT);
    digitalWrite(rePin, RS485Transmit);
    SerialRS.begin(9600);
}

void setup()
{
    pinMode(LEDi, OUTPUT);
    startRS485();

    Serial.begin(115200);
    Wire.begin(8); // Inicia a comunicação I2C com o endereço da câmera
    Wire.onReceive(receiveEvent);

    delay(1000);
    Serial.println("Controlador Pelco D");
}

void loop()
{
    // Continua o loop principal
}
