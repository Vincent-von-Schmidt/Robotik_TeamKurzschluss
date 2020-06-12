#ifndef FAHREN_H
#define FAHREN_H

extern void OnFwd(byte mot, int v);
extern void OnRev(byte mot, int v);
extern void Off(byte mot);
extern byte leseTaster();
extern void umfahreDose(int Richtung);
extern void luecke();
extern int drehe (int V, int t, int Richtung);
extern bool fahreZeitBisSchwarz (long zeit, int geschwindigkeit);
extern bool dreheZeitBisSchwarz (long zeit, int geschwindigkeit, byte richtung);

#endif
