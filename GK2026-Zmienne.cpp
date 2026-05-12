// zmienne globalne
#include "GK2026-Zmienne.h"

SDL_Window* window = NULL;
SDL_Surface* screen = NULL;

SDL_Color paleta8[szerokosc*wysokosc];
int ileKolorow = 0;
int ileKubelkow = 0;
Uint8 obrazekS[320*200];
SDL_Color obrazekK[320*200];
SDL_Color paleta8k[256];
SDL_Color paleta8s[256];
float bayer4x4[4][4] = {
    { 0.0/16.0,  8.0/16.0,  2.0/16.0, 10.0/16.0},
    {12.0/16.0,  4.0/16.0, 14.0/16.0,  6.0/16.0},
    { 3.0/16.0, 11.0/16.0,  1.0/16.0,  9.0/16.0},
    {15.0/16.0,  7.0/16.0, 13.0/16.0,  5.0/16.0}
};
