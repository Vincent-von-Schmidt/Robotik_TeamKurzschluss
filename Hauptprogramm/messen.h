#ifndef MESSEN_H
#define MESSEN_H

#define LINKS 0
#define RECHTS 1

extern int WerteW2[ANZ_LS];
extern int WerteW[ANZ_LS];

extern void zeigeLichtSeriell();
extern void messeLicht();

void kalibrieren();
void zeigeMinMax ();
void LichtUmrechnen ();

void SpeichereMinMax();
void LeseMinMax();

void hardwareInit ();


#endif
