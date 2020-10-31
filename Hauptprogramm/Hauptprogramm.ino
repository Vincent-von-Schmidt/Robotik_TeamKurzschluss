#include <SPI.h>
#include "meinePins.h"
#include "messen.h"
#include "fahren.h"
#include <EEPROM.h>

#define V 80
#define WEISS 90
#define SCHWARZ 25

#define testeWeiss ((WerteW[RECHTS] >= WEISS) && (WerteW[LINKS] >= WEISS))
#define testeSchwarz ((WerteW[RECHTS] <= SCHWARZ) && (WerteW[LINKS] <= SCHWARZ))
#define testeLinie  ((WerteW[RECHTS] <= 50) || (WerteW[LINKS] <= 50))

#define WEISS_LUECKE 90
#define testeWeiss_Lueke ((WerteW[RECHTS] >= WEISS_LUECKE) && (WerteW[LINKS] >= WEISS_LUECKE))

int trigger = 7;
int echo = 8;
int dauer = 0;
long entfernung = 0;

int StatusGruen = 0;

void LichtTest() {
  messeLicht();
  LichtUmrechnen();
  zeigeLichtSeriell();
  delay(200);
  return;
}
void setup() {
  Serial.begin(115200);
  hardwareInit();
  // Serial.println("Init ok");

  LeseMinMax();
  zeigeMinMax ();

  int taste = leseTaster();
  while (taste == 0)
    taste = leseTaster();

  if (taste == Taster_L) {
    Serial.println("Kalibrierung...");
    kalibrieren();
    zeigeMinMax ();
    SpeichereMinMax();
    Serial.println("Kalibriert");

  }
  delay(1500);
  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);
}

void Entfernung () {
  digitalWrite(trigger, LOW);
  delay(5);
  digitalWrite(trigger, HIGH);
  delay(10);
  digitalWrite(trigger, LOW);
  dauer = pulseIn(echo, HIGH);
  entfernung = (dauer / 2) / 29.1;
  Serial.print(entfernung);
  Serial.println(" cm");

}

void loop() {
  /*OnFwd(OUT_AB, V);
    // Entfernung ();

    if (entfernung < 11) {
    Off(OUT_AB);
      delay (2000);
    }*/
  /*
    //fahreZeitBisSchwarz(3000, 255);
    // delay(2000);
    //dreheZeitBisSchwarz(1000, V, RECHTS);
    long t1 = millis();
    long t2 = t1;
    OnFwd(OUT_AB, 80);

    messeLicht();

    while ((t2 - t1) < 3000) {
      messeLicht();
      LichtUmrechnen();
      zeigeLichtSeriell();
      t2 = millis();
      int diff = (10 * (WerteW[RECHTS] - WerteW[LINKS])) / 2;
      OnFwd(OUT_A, V + diff);
      OnFwd(OUT_B, V - diff);
    }

    Off (OUT_AB);

    while (1);

    return;*/
  messeLicht();
  LichtUmrechnen();
  if (leseTaster()) {
    umfahreDose(RECHTS);

  }


  //zeigeLichtSeriell();
  //delay(30);


  if (testeWeiss) {          //Lücke
    long t1;
    Serial.println("Weiss");
    OnFwd(OUT_AB, V);
    delay(100);
    messeLicht();
    LichtUmrechnen();
    t1 = millis();
    while (((millis() - t1) < 2500) && (testeWeiss)) {
      messeLicht();
      LichtUmrechnen();
      if (!testeWeiss) {
        Serial.println("not Weiss");
        return;
      }
      // else steht immer noch auf weiss
    }
    Serial.println("kein Linie");
    //Off(OUT_AB);
    //delay(1000);

    // Linie verloren, rückwärts
    OnRev(OUT_AB, V);

    t1 = millis();
    while (((millis() - t1) < 2700) && (testeWeiss)) {
      messeLicht();
      LichtUmrechnen();
    }
    delay(100);
    Off(OUT_AB);
    return;
    //delay(1000);

    messeLicht();
    LichtUmrechnen();
    if (testeWeiss_Lueke) {
      // Off(OUT_AB);
      Serial.println("Luecke");
      // delay(1000);
      return;
    } else {
      Serial.println("keine Luecke");
      return;
    }
    //delay(1000);
  }

  /* //Luecke oder Linie verloren
     long zeit = 2000;
     long t1 = millis();
     long t2 = t1;
     messeLicht();
     LichtUmrechnen();

     // Anhalten
     Off(OUT_AB);
     delay(1000);

     // Zurueckfahren bis Schwarz auf 1/2 (?) Sensoren
     OnRev(OUT_AB, 40);
     while ((WerteW[RECHTS] > SCHWARZ) || (WerteW[LINKS] > SCHWARZ)) {
      messeLicht();
      LichtUmrechnen();
      //zeigeLichtSeriell();
     }
     Off(OUT_AB);
     delay(1000);
     //  --> Entscheiden, ob Lücke oder Linie verloren
     // Linienende erkannt ->Lücke
     if ((WerteW2[RECHTS] < SCHWARZ) || (WerteW2[LINKS] < SCHWARZ)) {
      Serial.println("keine Lücke");
      return;
     } else {
      Serial.println("Lücke");
      // optional ausrichten
      messeLicht();
      LichtUmrechnen();
      OnFwd(OUT_A, 50);
      OnRev(OUT_B, 50);
     }
     do {
      messeLicht();
      LichtUmrechnen();
     } while (WerteW[RECHTS] && WerteW[LINKS] < SCHWARZ);
     Off(OUT_AB);
     // Vorfahren bis nächste schwarze Linie (begrenzt auf festgelegte Strecke)
     messeLicht();
     LichtUmrechnen();
     OnFwd(OUT_AB, V);
     while (WerteW[RECyHTS] && WerteW[LINKS] < SCHWARZ) {
      messeLicht();
      LichtUmrechnen();
     }
     // Linie gefunden -> return
     Off(OUT_AB);
     Serial.println("Linie gefunden");
    return;*/


  //}

  /* if (WerteW2[RECHTS] < 30 && WerteW2[LINKS] < 30) {
     OnFwd(OUT_A, -100);                               //+180=Links; -100=Rechts
     OnFwd(OUT_B, +180);                               //-100=Links; +180=Rechts
     delay(130);
    }*/




  else {
    // Kreuzung
    int GruenePunkteSpeicher = gruenepunkte(); // Fuktion muss fortlaufend den Status zurückgeben

    if (StatusGruen != 0) {
      if (GruenePunkteSpeicher == 0) {
        // schwarz oder weiss auf der richtigen Seite pruefen -> Aktion
        StatusGruen = 0;
      }
      else {
        StatusGruen = GruenePunkteSpeicher;
      }
    }
    else {
      if (GruenePunkteSpeicher == 0) {
        int diff = (13 * (WerteW2[RECHTS] - WerteW2[LINKS])) / 2;
        OnFwd(OUT_A, V + diff);
        OnFwd(OUT_B, V - diff);
      }
      else {
        OnFwd(OUT_AB, V);
        StatusGruen = GruenePunkteSpeicher;
      }
    }

    /*(if (GruenePunkteSpeicher == GRUEN_LINKS) {
      Serial.println("Links Grün");
      OnFwd(OUT_A, +180);
      OnFwd(OUT_B, -100);
      delay(700);
      }
      if (GruenePunkteSpeicher == GRUEN_RECHTS) {
      Serial.println("Rechts Grün");
      OnFwd(OUT_A, -100);
      OnFwd(OUT_B, +180);
      delay(700);
      }
      if (GruenePunkteSpeicher == GRUEN_BEIDE) {
      Serial.println("Beide Grün");
      OnFwd(OUT_A, V);
      OnFwd(OUT_B, V);
      delay(700);
      }
      else {
      // Linienfolger
      int diff = (13 * (WerteW2[RECHTS] - WerteW2[LINKS])) / 2;
      OnFwd(OUT_A, V + diff);
      OnFwd(OUT_B, V - diff);
      }*/
  }
}
