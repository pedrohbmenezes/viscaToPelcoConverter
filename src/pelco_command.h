#include <SoftwareSerial.h>
#include <constants.h>

#define RS485Transmit HIGH
#define RS485Receive LOW

SoftwareSerial SerialRS = SoftwareSerial(rxPin, txPin);

bool stopped = false;

bool check(int pin)
{
    return (digitalRead(pin) == HIGH);
}

void sendPelcoDFrame(byte command, byte data1, byte data2)
{
    byte bytes[7] = {0xFF, address, 0x00, command, data1, data2, 0x00};
    byte crc = (bytes[1] + bytes[2] + bytes[3] + bytes[4] + bytes[5]) % 0x100;
    bytes[6] = crc;

    for (int i = 0; i < 7; i++)
    {
        SerialRS.write(bytes[i]);
        // Serial.print(bytes[i], HEX); //debug
    }
    // Serial.println(); //debug
}

// void blinkLED()
// {
//     for (int i = 0; i < 4; i++)
//     {
//         digitalWrite(LED, HIGH);
//         delay(100);
//         digitalWrite(LED, LOW);
//         delay(100);
//     }
// }

// void setup()
// {
//     pinMode(LED, OUTPUT);

//     Serial.begin(115200);
//     SerialRS.begin(9600);
//     delay(5000);
//     Serial.println("Controlador Pelco D");
//     digitalWrite(rePin, RS485Transmit);
//     sendPelcoDFrame(C_STOP, 0, 0);
//     delay(10);
//     sendPelcoDFrame(C_SET_PAN_POSITION, speed, speed);

//     Serial.println("Controlador Pelco D");
//     blinkLED();
// }

// void loop()
// {
//     if (Serial.available())
//     {
//         int command = Serial.read();
//         switch (command)
//         {
//         case '1': // direita
//             sendPelcoDFrame(C_RIGHT, speed, speed);
//             Serial.println("Direita");
//             break;
//         case '2': // pare
//             sendPelcoDFrame(C_STOP, speed, speed);
//             Serial.println("Pare");
//             break;
//         case '3': // esquerda
//             sendPelcoDFrame(C_LEFT, speed, speed);
//             Serial.println("Esquerda");
//             break;
//         case '4': // cima
//             sendPelcoDFrame(C_UP, speed, speed);
//             Serial.println("Cima");
//             break;
//         case '5': // baixo
//             sendPelcoDFrame(C_DOWN, speed, speed);
//             Serial.println("Baixo");
//             break;
//         case '6': // zoom in
//             sendPelcoDFrame(C_ZOOMIN, speed, speed);
//             Serial.println("Zoom In");
//             break;
//         case '7': // zoom out
//             sendPelcoDFrame(C_ZOOMOUT, speed, speed);
//             Serial.println("Zoom Out");
//             break;
//         // adicione mais casos conforme necessário para outros comandos
//         default:
//             break;
//         }
//         blinkLED();
//     }
// }