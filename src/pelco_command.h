#ifndef PELCO_COMMAND_H
#define PELCO_COMMAND_H

#include <Wire.h>
#include <Arduino.h>
#include <SoftwareSerial.h>
#include "./utils/logUtil.h"

#define RS485Transmit HIGH
#define RS485Receive LOW

const byte rePin = 5;
const byte LEDi = 13;
const byte speed = 100;

namespace PelcoCommand
{
    const byte Stop = 0x00;
    const byte Up = 0x08;
    const byte Down = 0x10;
    const byte Left = 0x04;
    const byte Right = 0x02;
    const byte ZoomIn = 0x20;
    const byte ZoomOut = 0x40;
    const byte SetPreset = 0x03;
    const byte ClearPreset = 0x05;
    const byte CallPreset = 0x07;
}

bool stopped = false;
byte cameraAddress = 2;

void setCameraAddress(byte addr)
{
    cameraAddress = addr;
}

SoftwareSerial SerialRS(16, 17); // RX, TX

void sendPelcoDFrame(byte command, byte data1, byte data2)
{
    byte bytes[7] = {0xFF, cameraAddress, 0x00, command, data1, data2, 0x00};
    byte crc = (bytes[1] + bytes[2] + bytes[3] + bytes[4] + bytes[5]) % 0x100;
    bytes[6] = crc;

    for (int i = 0; i < 7; i++)
    {
        SerialRS.write(bytes[i]);
    }
}

void stopCamera()
{
    sendPelcoDFrame(PelcoCommand::Stop, 0x00, 0x00);
    logPelcoCommand("Parar movimento");
}

void moveCamera(byte command, const char *message)
{
    sendPelcoDFrame(command, speed, speed);
    logPelcoCommand(message);
}

void sendPresetCommand(byte commandType, byte presetID, const char *label)
{
    if (presetID < 1 || presetID > 255)
    {
        logPelcoCommand("Preset inválido!");
        return;
    }
    sendPelcoDFrame(commandType, 0x00, presetID);
    logPelcoCommand(String(label) + " - Preset " + String(presetID));
    delay(500);
}

void setPreset(byte presetID) { sendPresetCommand(PelcoCommand::SetPreset, presetID, "Set Preset"); }
void clearPreset(byte presetID) { sendPresetCommand(PelcoCommand::ClearPreset, presetID, "Clear Preset"); }
void callPreset(byte presetID) { sendPresetCommand(PelcoCommand::CallPreset, presetID, "Call Preset"); }

void handleDirectionalZoomCommand(uint8_t command)
{
    switch (command)
    {
    case 0x01:
        moveCamera(PelcoCommand::Left, "Direita");
        break;
    case 0x02:
        stopCamera();
        break;
    case 0x03:
        moveCamera(PelcoCommand::Right, "Esquerda");
        break;
    case 0x04:
        moveCamera(PelcoCommand::Up, "Cima");
        break;
    case 0x05:
        moveCamera(PelcoCommand::Down, "Baixo");
        break;
    case 0x06:
        moveCamera(PelcoCommand::ZoomIn, "Zoom In");
        break;
    case 0x07:
        moveCamera(PelcoCommand::ZoomOut, "Zoom Out");
        break;
    default:
        logPelcoCommand("Comando direcional/zoom inválido!");
        break;
    }
}

void receiveEvent(uint8_t *command)
{
    if (command[0] != 0xFF)
    {
        logPelcoCommand("Comando inválido (header diferente de 0xFF)");
        return;
    }

    switch (command[1])
    {
    case 0x01:
        stopCamera();
        logPelcoCommand("Parar câmera");
        break;
    case 0x02:
        handleDirectionalZoomCommand(command[2]);
        break;
    case 0x03:
        callPreset(command[2]);
        break;
    case 0x04:
        setPreset(command[2]);
        break;
    default:
        stopCamera();
        logPelcoCommand("Comando desconhecido. Parando câmera.");
        break;
    }
}

void startRS485()
{
    pinMode(rePin, OUTPUT);
    digitalWrite(rePin, RS485Transmit);
    SerialRS.begin(9600);
    stopCamera();
    logMessage("RS485 iniciado");
}

#endif