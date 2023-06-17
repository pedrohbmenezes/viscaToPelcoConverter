#ifndef COMMANDVISCA_H
#define COMMANDVISCA_H

class CommandVisca
{
private:
    int maxPresentNumber = 6;

public:
    bool isCallPresent(char *command, int presentNumber)
    {
        char memoryPosition = presentNumber - 1; // Ajusta o número de chamada presente para a posição de memória correspondente
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
    String getCommandName(char *comand)
    {
        for (int presentNumber = 1; presentNumber <= maxPresentNumber; presentNumber++)
        {
            if (isCallPresent(comand, presentNumber))
            {
                return "Call Present " + String(presentNumber);
            }
        }
        if (isCommandCima(comand))
        {
            return "4";
        }
        else if (isCommandBaixo(comand))
        {
            return "5";
        }
        else if (isCommandEsquerda(comand))
        {
            return "3";
        }
        else if (isCommandDireita(comand))
        {
            return "1";
        }
        else if (init(comand))
        {
            return "Inicializacao";
        }
        else if (zoomIn(comand))
        {
            return "6";
        }
        else if (zoomOut(comand))
        {
            return "7";
        }
        else if (directionalStop(comand))
        {
            return "2";
        }
        else if (zoomStop(comand))
        {
            return "2";
        }
        else
        {
            return "Desconhecido";
        }
    }
};
#endif