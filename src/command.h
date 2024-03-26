#ifndef COMMANDVISCA_H
#define COMMANDVISCA_H

#include <Arduino.h>

class CommandVisca
{
private:
    int maxPresentNumber = 6;

public:
    bool isSetPresent(char *command, int presentNumber)
    {
        char memoryPosition = presentNumber; // Ajusta o número de chamada presente para a posição de memória correspondente
        return (checkByte(command, 0, 0x81) && checkByte(command, 1, 0x01) && checkByte(command, 2, 0x04) && checkByte(command, 3, 0x3F) && checkByte(command, 4, 0x01) && checkByte(command, 5, memoryPosition) && checkByte(command, 6, 0xFF));
    }
    bool isCallPresent(char *command, int presentNumber)
    {
        char memoryPosition = presentNumber; // Ajusta o número de chamada presente para a posição de memória correspondente
        return (checkByte(command, 0, 0x81) && checkByte(command, 1, 0x01) && checkByte(command, 2, 0x04) && checkByte(command, 3, 0x3F) && checkByte(command, 4, 0x02) && checkByte(command, 5, memoryPosition) && checkByte(command, 6, 0xFF));
    }

    bool isComandHorizontal(char *comand)
    {
        return (checkByte(comand, 0, 0x81) && checkByte(comand, 1, 0x01) && checkByte(comand, 2, 0x06) && checkByte(comand, 3, 0x01));
    }
    bool checkByte(char *data, int position, char value)
    {
        return (data[position] == value);
    }

    bool isCommandCima(char *comand)
    {
        return (checkByte(comand, 0, 0x81) && checkByte(comand, 1, 0x01) && checkByte(comand, 2, 0x06) && checkByte(comand, 3, 0x01) && checkByte(comand, 6, 0x03) && checkByte(comand, 7, 0x01));
    }

    bool isCommandBaixo(char *comand)
    {
        return (checkByte(comand, 0, 0x81) && checkByte(comand, 1, 0x01) && checkByte(comand, 2, 0x06) && checkByte(comand, 3, 0x01) && checkByte(comand, 6, 0x03) && checkByte(comand, 7, 0x02));
    }

    bool isCommandEsquerda(char *comand)
    {
        return (checkByte(comand, 0, 0x81) && checkByte(comand, 1, 0x01) && checkByte(comand, 2, 0x06) && checkByte(comand, 3, 0x01) && checkByte(comand, 6, 0x01) && checkByte(comand, 7, 0x03));
    }

    bool isCommandDireita(char *comand)
    {
        return (checkByte(comand, 0, 0x81) && checkByte(comand, 1, 0x01) && checkByte(comand, 2, 0x06) && checkByte(comand, 3, 0x01) && checkByte(comand, 6, 0x02) && checkByte(comand, 7, 0x03));
    }
    bool init(char *comand)
    {
        return (checkByte(comand, 0, 0x81) && checkByte(comand, 1, 0x09) && checkByte(comand, 2, 0x7E) && checkByte(comand, 3, 0x7E) && checkByte(comand, 4, 0x01));
    }
    bool zoomIn(char *comand)
    {
        char speed = comand[4];
        return (checkByte(comand, 0, 0x81) && checkByte(comand, 1, 0x01) && checkByte(comand, 2, 0x04) && checkByte(comand, 3, 0x07) && (speed >= 0x20 && speed <= 0x27));
    }
    bool zoomOut(char *comand)
    {
        char speed = comand[4];
        return (checkByte(comand, 0, 0x81) && checkByte(comand, 1, 0x01) && checkByte(comand, 2, 0x04) && checkByte(comand, 3, 0x07) && (speed >= 0x30 && speed <= 0x37));
    }
    bool zoomStop(char *comand)
    {
        return (checkByte(comand, 0, 0x81) && checkByte(comand, 1, 0x01) && checkByte(comand, 2, 0x04) && checkByte(comand, 3, 0x07) && checkByte(comand, 4, 0x00));
    }
    bool directionalStop(char *comand)
    {
        return (checkByte(comand, 0, 0x81) && checkByte(comand, 1, 0x01) && checkByte(comand, 2, 0x06) && checkByte(comand, 3, 0x01) && checkByte(comand, 4, 0x00) && checkByte(comand, 5, 0x00) && checkByte(comand, 6, 0x03) && checkByte(comand, 7, 0x03));
    }

    uint8_t *getCommandBytes(char *command)
    {
        Serial.print("Visca Comando recebido: ");
        for (int i = 0; i < 7; i++)
        {
            Serial.print("0x");
            Serial.print(command[i], HEX);
            Serial.print(" ");
        }
        Serial.println();
        uint8_t *commandBytes = new uint8_t[3];
        if (isSetPresent(command, command[5]))
        {
            commandBytes[0] = 0xFF;
            commandBytes[1] = 0x04;
            commandBytes[2] = command[5];
        }
        else if (isCallPresent(command, command[5]))
        {
            commandBytes[0] = 0xFF;
            commandBytes[1] = 0x03;
            commandBytes[2] = command[5] + 1;
        }
        else if (isCommandCima(command))
        {
            commandBytes[0] = 0xFF;
            commandBytes[1] = 0x02;
            commandBytes[2] = 0x04;
        }
        else if (isCommandBaixo(command))
        {
            commandBytes[0] = 0xFF;
            commandBytes[1] = 0x02;
            commandBytes[2] = 0x05;
        }
        else if (isCommandEsquerda(command))
        {
            commandBytes[0] = 0xFF;
            commandBytes[1] = 0x02;
            commandBytes[2] = 0x03;
        }
        else if (isCommandDireita(command))
        {
            commandBytes[0] = 0xFF;
            commandBytes[1] = 0x02;
            commandBytes[2] = 0x01;
        }
        else if (init(command))
        {
            commandBytes[0] = 0xFF;
            commandBytes[1] = 0x02;
            commandBytes[2] = 0x01;
        }
        else if (zoomIn(command))
        {
            commandBytes[0] = 0xFF;
            commandBytes[1] = 0x02;
            commandBytes[2] = 0x06;
        }
        else if (zoomOut(command))
        {
            commandBytes[0] = 0xFF;
            commandBytes[1] = 0x02;
            commandBytes[2] = 0x07;
        }
        else if (directionalStop(command) || zoomStop(command))
        {
            commandBytes[0] = 0xFF;
            commandBytes[1] = 0x02;
            commandBytes[2] = 0x02;
        }
        else
        {
            commandBytes[0] = 0xFF;
            commandBytes[1] = 0x00;
            commandBytes[2] = 0x00;
        }

        return commandBytes;
    }
};
#endif