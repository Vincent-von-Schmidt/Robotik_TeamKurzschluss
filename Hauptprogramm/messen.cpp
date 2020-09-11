#include <Arduino.h>
#include "meinePins.h"
#include "messen.h"
#include <SPI.h>
#include <EEPROM.h>

#define MAX_ADC 1023
#define GRUEN_SCHWELLE 11

byte LEDs[ANZ_LEDs] = {LED_W, LED_W2};
byte LS[ANZ_LS] = {SE_LI, SE_RE};
int WerteW[ANZ_LS];

int WerteW2[ANZ_LS];
int WerteG[ANZ_LS];
int WerteR[ANZ_LS];
int MinW[ANZ_LS];
int MinG[ANZ_LS];
int MinR[ANZ_LS];
int MaxW[ANZ_LS];
int MaxG[ANZ_LS];
int MaxR[ANZ_LS];
int MinW2[ANZ_LS];
int MaxW2[ANZ_LS];
int RGruen;
int LGruen;

void messeLicht() {
  byte pause = 2;
  setLED (LED_W);
  delay(pause);
  for (int i = 0; i < ANZ_LS; i++) {
    WerteW[i] = analogRead(LS[i]);
  }
  setLED (LED_W2);
  delay(pause);
  for (int i = 0; i < ANZ_LS; i++) {
    WerteW2[i] = analogRead(LS[i]);
  }
  setLED (LED_G);
  delay(pause);
  for (int i = 0; i < ANZ_LS; i++) {
    WerteG[i] = analogRead(LS[i]);
  }
  setLED (LED_R);
  delay(pause);
  for (int i = 0; i < ANZ_LS; i++) {
    WerteR[i] = analogRead(LS[i]);
  }
  setLED(0);
}


#define GRUEN_LINKS  1
#define GRUEN_RECHTS 2
#define GRUEN_BEIDE  3


int gruenepunkte() {
  int diff = WerteG[0] - WerteR[0];
  if (diff >= GRUEN_SCHWELLE) {
    LGruen++;
  }
  else {
    LGruen = 0;
  }
  diff = WerteG[1] - WerteR[1];
  if (diff >= GRUEN_SCHWELLE) {
    RGruen++;
  }
  else {
    RGruen = 0;
  }
  if (LGruen >= 3) {
    LGruen = 0;
    return GRUEN_LINKS;
  }
  if (RGruen >= 3) {
    RGruen = 0;
    return GRUEN_RECHTS;
  }

  return 0;
}

void zeigeLichtSeriell() {
  //Serial.print ("W_li: ");
  Serial.print(WerteW2[0]);
  Serial.print(" ");
  Serial.print(WerteW[0]);
  Serial.print(" ");
  Serial.print(WerteG[0]);
  Serial.print(" ");
  Serial.print(WerteR[0]);
  //Serial.print(" \t W_re: ");
  Serial.print(" ");
  Serial.print(WerteW[1]);
  Serial.print(" ");
  Serial.print(WerteW2[1]);
  Serial.print(" ");
  Serial.print(WerteG[1]);
  Serial.print(" ");
  Serial.print(WerteR[1]);
  Serial.println("");
}

void kalibrieren() {
  for (int j = 0; j < ANZ_LS; j++) {
    MinW[j] = MAX_ADC;
    MinW2[j] = MAX_ADC;
    MinG[j] = MAX_ADC;
    MinR[j] = MAX_ADC;
    MaxW[j] = 0;
    MaxW2[j] = 0;
    MaxG[j] = 0;
    MaxR[j] = 0;
  }
  for (int i = 0; i < 500; i++) {
    messeLicht();
    for (int j = 0; j < ANZ_LS; j++) {


      if (WerteW[j] > MaxW[j])
        MaxW[j] = WerteW[j];
      if (WerteW[j] < MinW[j])
        MinW[j] = WerteW[j];

      if (WerteW2[j] > MaxW2[j])
        MaxW2[j] = WerteW2[j];
      if (WerteW2[j] < MinW2[j])
        MinW2[j] = WerteW2[j];


      if (WerteG[j] > MaxG[j])
        MaxG[j] = WerteG[j];
      if (WerteG[j] < MinG[j])
        MinG[j] = WerteG[j];



      if (WerteR[j] > MaxR[j])
        MaxR[j] = WerteR[j];
      if (WerteR[j] < MinR[j])
        MinR[j] = WerteR[j];

    }
  }
}

void zeigeMinMax () {
  Serial.print ("Min_li: ");
  Serial.print(MinW[0]);
  Serial.print(" ");
  Serial.print(MinW2[0]);

  Serial.print(" ");
  Serial.print(MinG[0]);
  Serial.print(" ");
  Serial.print(MinR[0]);

  Serial.print(" \t Min_re: ");
  Serial.print(MinW[1]);
  Serial.print(" ");
  Serial.print(MinW2[1]);

  Serial.print(" ");
  Serial.print(MinG[1]);
  Serial.print(" ");
  Serial.print(MinR[1]);

  Serial.println();

  Serial.print ("Max_li: ");
  Serial.print(MaxW[0]);
  Serial.print(" ");
  Serial.print(MaxW2[0]);


  Serial.print(" ");
  Serial.print(MaxG[0]);
  Serial.print(" ");
  Serial.print(MaxR[0]);

  Serial.print(" \t Max_re: ");
  Serial.print(MaxW[1]);
  Serial.print(" ");
  Serial.print(MaxW2[1]);

  Serial.print(MaxG[1]);
  Serial.print(" ");
  Serial.print(MaxR[1]);

  Serial.println();
}
void LichtUmrechnen () {
  for (int j = 0; j < ANZ_LS; j++) {
    WerteW[j] =   map (WerteW[j], MinW[j], MaxW[j], 0, 100);
    WerteW2[j] =   map (WerteW2[j], MinW2[j], MaxW2[j], 0, 100);
    WerteG[j] =   map (WerteG[j], MinG[j], MaxG[j], 0, 100);
    WerteR[j] =   map (WerteR[j], MinR[j], MaxR[j], 0, 100);
  }

}

void SpeichereMinMax() {
  EEPROM.put( 0, MinW[0]);
  EEPROM.put( 2, MinG[0]);
  EEPROM.put( 4, MinR[0]);
  EEPROM.put( 6, MaxW[0]);
  EEPROM.put( 8, MaxG[0]);
  EEPROM.put(10, MaxR[0]);
  EEPROM.put(12, MinW[1]);
  EEPROM.put(14, MinG[1]);
  EEPROM.put(16, MinR[1]);
  EEPROM.put(18, MaxW[1]);
  EEPROM.put(20, MaxG[1]);
  EEPROM.put(22, MaxR[1]);
  EEPROM.put( 24, MinW2[0]);
  EEPROM.put( 26, MinW2[1]);
  EEPROM.put( 28, MaxW2[0]);
  EEPROM.put( 30, MaxW2[1]);
}


void LeseMinMax() {
  EEPROM.get( 0, MinW[0]);
  EEPROM.get( 2, MinG[0]);
  EEPROM.get( 4, MinR[0]);
  EEPROM.get( 6, MaxW[0]);
  EEPROM.get( 8, MaxG[0]);
  EEPROM.get(10, MaxR[0]);
  EEPROM.get(12, MinW[1]);
  EEPROM.get(14, MinG[1]);
  EEPROM.get(16, MinR[1]);
  EEPROM.get(18, MaxW[1]);
  EEPROM.get(20, MaxG[1]);
  EEPROM.get(22, MaxR[1]);
  EEPROM.get( 24, MinW2[0]);
  EEPROM.get( 26, MinW2[1]);
  EEPROM.get( 28, MaxW2[0]);
  EEPROM.get( 30, MaxW2[1]);

}


void hardwareInit () {
  pinMode(LICHT_CS, OUTPUT);
  digitalWrite(LICHT_CS, HIGH);
  SPI.begin();

  /*Serial.println("Hallo Herr Krause.");
    Serial.println("Hardware Init");*/
}



/*void erkenneWeiss() {
  if (WerteW[RECHTS] && WerteW[LINKS] > WEISS) {
    Off(OUT_AB);
  }
  }*/
