// metody do algorytmu kwantyzacji (MedianCut)
#ifndef GK2026_MEDIANCUT_H_INCLUDED
#define GK2026_MEDIANCUT_H_INCLUDED

#include <SDL2/SDL.h>
void MedianCutBW(int start, int koniec, int iteracja);
void MedianCutColor(int start, int koniec, int iteracja);

void paletaMedianCutBW();
void paletaMedianCutColor();

void sortujKubelekBW(int start, int koniec);
void sortujKubelekColor(int start, int koniec);

int znajdzSasiadaBW(Uint8 wartosc);
int znajdzSasiadaColor(SDL_Color kolor);

void narysujPalete4b(int px, int py, SDL_Color pal3b[]);

void Bayer();
void BayerKolor();
void FloydSteinbergBW();
void FloydSteinbergColor();
float clamping(float v);
void distributeError(float* tab, int x, int y, float blad);
#endif // GK2026_MEDIANCUT_H_INCLUDED
