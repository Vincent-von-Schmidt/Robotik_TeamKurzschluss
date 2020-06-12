#include <Arduino.h>
#include "fahren.h"
#include "meinePins.h"
#include "messen.h"
#include <SPI.h>
#define testeSchwarz (WerteW[RECHTS] && WerteW[LINKS] >= SCHWARZ)
#define WEISS 100
#define SCHWARZ 60
byte PEData;

bool fahreZeitBisSchwarz(long zeit, int geschwindigkeit) {
  boolean istSchwarz = 0;
  OnFwd(OUT_AB, geschwindigkeit);
  messeLicht();
  LichtUmrechnen();
  long t1 = millis();
  while ((t1 == zeit) || istSchwarz == 1) {
    messeLicht();
    LichtUmrechnen();
    t1 = millis();
    if (testeSchwarz) {
      istSchwarz = 1; 
    }
  }
  Off(OUT_AB);
  return istSchwarz;
}

bool dreheZeitBisSchwarz(long zeit, int geschwindigkeit, byte richtung) {
  boolean istSchwarz = 0;
  drehe (geschwindigkeit, zeit, richtung);
  messeLicht();
  LichtUmrechnen();
  long t1 = millis();
  while ((t1 == zeit) || istSchwarz == 1) {
    messeLicht();
    LichtUmrechnen();
    t1 = millis();
    if (testeSchwarz) {
      istSchwarz = 1; 
    }
  }
  Off(OUT_AB);
  return istSchwarz;
}



void schreibePEData(byte data) {
  digitalWrite(LICHT_CS, LOW);
  SPI.transfer(data);
  digitalWrite(LICHT_CS, HIGH);
}
byte Taster = 0;
byte leseTaster() {
  digitalWrite(LICHT_CS, LOW);
  delayMicroseconds(10);
  digitalWrite(LICHT_CS, HIGH);
  delayMicroseconds(10);
  Taster = SPI.transfer(PEData);
  return Taster;
}
/*int leseTaster() {
  digitalWrite(LICHT_CS, LOW);
  int Taster = SPI.transfer(PEData);
  digitalWrite(LICHT_CS, HIGH);
  return Taster;
  }*/


void setLED(int led) {
  PEData &= 0xF0;
  PEData |= (led & 0x0F);
  schreibePEData(PEData);
}

#define MOT_B_KORREKTUR 12

void OnFwd(byte mot, int v)
{
  // prüfe auf erlaubten Wertebereich

  byte richtung = 0;
  if (v >= 0) { // vorwärts fahren
    if (mot & OUT_A) {
      v = (v*MOT_B_KORREKTUR)/10;
      if (v > 255)
        v = 255;
      analogWrite(PWMA, v);
      PEData = PEData & ~32;
      PEData = PEData | 16;
      schreibePEData(PEData);
    }
    if (mot & OUT_B) {
      if (v > 255)
          v = 255;
      analogWrite(PWMB,  v);
      PEData = PEData & ~128;
      PEData = PEData | 64;
      schreibePEData(PEData);
    }
  }
  else {
    v = -v;
    if (mot & OUT_A) {
      v = (v*MOT_B_KORREKTUR)/10;
      if (v > 255)
        v = 255;
      analogWrite(PWMA, v);
      PEData = PEData | 32;
      PEData = PEData & ~16;
      schreibePEData(PEData);
    }
    if (mot & OUT_B) {
      if (v > 255)
          v = 255;
      analogWrite(PWMB, v);
      PEData = PEData | 128;
      PEData = PEData & ~64;
      schreibePEData(PEData);
    }
  }

}

void OnRev(byte mot, int v) {
  OnFwd(mot, -v);
}

void Off(byte mot) {
  byte richtung = 0;
  if (mot & OUT_A) {
    analogWrite(PWMA, 0);
    PEData = PEData & ~48;
    schreibePEData(PEData);
  }
  if (mot & OUT_B) {
    analogWrite(PWMB, 0);
    PEData = PEData & ~192;
    schreibePEData(PEData);
  }
}

/*
    fährt max. t ms mit gegebenem V,
    bricht ab, wenn schwarz gefunden wird
   return 0, wenn keien schwarze Linie gefunden wurde
          1 wenn swarze Linie gefunden wurde
*/
#define SCHWARZ 60

int drehe (int V, int t, int Richtung) {
  long t1 = millis();
  long t2 = t1;
  int Mot1 = OUT_A;
  int Mot2 = OUT_B;
  if (Richtung == RECHTS) {
    Mot1 = OUT_B;
    Mot2 = OUT_A;
  }
  OnFwd(Mot2, V); //drehen
  OnRev(Mot1, V);

  while ((t2 - t1) < t) {
    delay (1);
    messeLicht();
    LichtUmrechnen();
    t2 = millis();
    if (WerteW[Richtung] < SCHWARZ) {
      Off (OUT_AB);
      return 1; // schwarz gefunden -> stop und return
    }
  }
  Off (OUT_AB); // ?
  return 0;
}

/*
    fährt max. t ms mit gegebenem V,
    bricht ab, wenn schwarz gefunden wird
   return 0, wenn keien schwarze Linie gefunden wurde
          1 wenn swarze Linie gefunden wurde
*/



int fahreVor (int V, int t, int Sensor) {

  long t1 = millis();
  long t2 = t1;

  OnFwd (OUT_AB, V);


  while ((t2 - t1) < t) {
    delay (1);
    messeLicht();
    LichtUmrechnen();
    t2 = millis();
    Serial.println(WerteW[Sensor]);
    if (WerteW[Sensor] < SCHWARZ) {
      Off (OUT_AB);
      return 1; // schwarz gefunden -> stop und return
    }
  }
  Off (OUT_AB); // ?
  return 0;
}


void umfahreDose (int Richtung) {
  int Mot1 = OUT_A;
  int Mot2 = OUT_B;
  if (Richtung == RECHTS) {
    Mot1 = OUT_B;
    Mot2 = OUT_A;
  }
  OnRev(OUT_AB, 60);
  delay(1000);
  Off(OUT_AB);
  OnFwd(Mot1, 80); //drehen
  OnRev(Mot2, 80);
  delay(1000);
  Off(OUT_AB);
  OnFwd(OUT_AB, 80);
  delay(1500);

  boolean istSchwarz = 1; // =1 wenn Linie gefunden
  do {
    Serial.println(istSchwarz);
    istSchwarz = drehe (120, 400, Richtung);

    if (istSchwarz == 0) {
      istSchwarz = fahreVor (80, 700, Richtung);
    }
  }
  while (istSchwarz == 0);

  OnFwd(Mot1, 120); //drehen
  OnRev(Mot2, 50);
  delay(500);
  OnFwd(OUT_AB, 80);
  delay(500);

  if (Richtung == RECHTS) {
    drehe (100, 2000, LINKS);
  }
  else {
    drehe (100, 2000, RECHTS);
  }



  /* messeLicht();
    LichtUmrechnen();
    while (WerteW[Richtung] > 60) {
     //Serial.println(WerteW[LINKS]);
     messeLicht();
     LichtUmrechnen();
     OnFwd(Mot1, 150);
     OnFwd(Mot2, 150);
     delay(300);
     Off(OUT_AB);
     OnFwd(Mot1, 150);
     OnRev(Mot1, 150);
     delay(300);
     Off(OUT_AB);
    } */
  Off(OUT_AB);
}

/*void luecke() {
  long t1 = millis
    long t2 = t2
       messeLicht();
  LichtUmrechnen();
  OnFwd(OUT_AB, V);
  while (WerteW[RECHTS] < SCHWARZ) {
    Serial.println("Lücke!");
  }
  Off(OUT_AB);
  Serial.println("Lücke fertig!");
  OnFwd(OUT_AB, V);
  }*/
