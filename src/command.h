#ifndef COMMANDVISCA_H
#define COMMANDVISCA_H

class CommandVisca
{
public:
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
        if (isCommandCima(comand))
        {
            return "Cima";
        }
        else if (isCommandBaixo(comand))
        {
            return "Baixo";
        }
        else if (isCommandEsquerda(comand))
        {
            return "Esquerda";
        }
        else if (isCommandDireita(comand))
        {
            return "Direita";
        }
        else if (init(comand))
        {
            return "Inicializacao";
        }
        else if (zoomIn(comand))
        {
            return "Zoom In";
        }
        else if (zoomOut(comand))
        {
            return "Zoom Out";
        }
        else if (directionalStop(comand))
        {
            return "Parar";
        }
        else if (zoomStop(comand))
        {
            return "Parar Zoom";
        }
        else
        {
            return "Desconhecido";
        }
    }
};
#endif