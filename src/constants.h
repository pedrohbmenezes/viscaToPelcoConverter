#ifndef CONSTANTS_H
#define CONSTANTS_H

// Pin Definitions
const byte rxPin = 3;
const byte txPin = 4;
const byte rePin = 5;

const int LED = 13;

const int P_LEFT = 12;    // esquerda
const int P_UP = 11;      // cima
const int P_RIGHT = 10;   // direita
const int P_DOWN = 9;     // baixo
const int P_ZOOM_IN = 7;  // zoom in
const int P_ZOOM_OUT = 8; // zoom out

// Camera Configuration
const byte address = 1; // endereço da câmera
byte speed = 100;       // pode ser alterada pressionando esquerda+cima ou esquerda+baixo

// Pelco D Commands
const byte C_STOP = 0x00;
const byte C_UP = 0x08;
const byte C_DOWN = 0x10;
const byte C_LEFT = 0x04;
const byte C_RIGHT = 0x02;

const byte C_ZOOMIN = 0x20;
const byte C_ZOOMOUT = 0x40;

const byte C_SET_PAN_POSITION = 0x4B;  // posição pan em 1/100 graus
const byte C_SET_TILT_POSITION = 0x4D; // posição tilt em 1/100 graus

#endif
