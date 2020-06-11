#ifndef MEINEPINS_H
#define MEINEPINS_H

// Chip select für den 74HC595
#define LICHT_CS A0
#define SE_LI A3
#define SE_RE A7

#define LED_W2    8   //P4
#define LED_G     4   //P3
#define LED_R     2   //P2
#define LED_W     1   //P1
#define ANZ_LEDs  2
#define ANZ_LS    2


//Taster
#define Taster_L  1   //Taster Links
#define Taster_R  4   //Taster Rechts

//Motoren
#define OUT_A  1
#define OUT_B  2
#define OUT_AB 3
#define PWMA 3     // Motor links
#define PWMB 5     // Motor rechts

extern void setLED(int led);

#endif
