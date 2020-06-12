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

void setup() {
  Serial.begin(115200);
  hardwareInit();
  LeseMinMax();
  zeigeMinMax ();
  int taste = leseTaster();
  while (taste == 0) {
    taste = leseTaster();
    if (taste == Taster_L) {
      kalibrieren();
      zeigeMinMax ();
      SpeichereMinMax();
    }
  }
  
  delay(300);
  
  void loop() {
    messeLicht();
    LichtUmrechnen();
    if (leseTaster()) {
      umfahreDose(RECHTS);
    }
    if (testeWeiss) {          //Lücke
      long t1;
      OnFwd(OUT_AB, V);
      delay(100);
      messeLicht();
      LichtUmrechnen();
      t1 = millis();
      while (((millis() - t1) < 2500) && (testeWeiss)) {
        messeLicht();
        LichtUmrechnen();
        if (!testeWeiss) {
          return;
        }// else steht immer noch auf weiss
      }
      OnRev(OUT_AB, V);
      t1 = millis();
      while (((millis() - t1) < 2700) && (testeWeiss)) {
        messeLicht();
        LichtUmrechnen();
      }
      delay(100);
      Off(OUT_AB);
      return;
    }

    else {
      // Kreuzung
      if (WerteW2[RECHTS] < 30 && WerteW2[LINKS] < 30) {
        OnFwd(OUT_A, -100);                               //+180=Links; -100=Rechts
        OnFwd(OUT_B, +180);                               //-100=Links; +180=Rechts
        delay(130);
      }

      else {
        // Linienfolger
        int diff = (13 * (WerteW2[RECHTS] - WerteW2[LINKS])) / 2;
        OnFwd(OUT_A, V + diff);
        OnFwd(OUT_B, V - diff);

      }
    }
  }
