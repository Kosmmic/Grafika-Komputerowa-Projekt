// dithering (brak/Bayer/Floyd-Steinberg) do dowolnej palety 256-kolorowej (format DG8)
#ifndef GK2026_DG8DITHER_H_INCLUDED
#define GK2026_DG8DITHER_H_INCLUDED

#include <SDL2/SDL.h>

// dithering=0: kazdy piksel -> indeks najblizszego koloru w palecie
void ditherBrakDG8(SDL_Color paleta[], int rozmiar, Uint8* wynik);

// dithering=1: tablica Bayera 4x4 jako przesuniecie przed wyszukaniem najblizszego koloru
void ditherBayerDG8(SDL_Color paleta[], int rozmiar, Uint8* wynik);

// dithering=2: Floyd-Steinberg, dyfuzja bledu R/G/B
void ditherFSDG8(SDL_Color paleta[], int rozmiar, Uint8* wynik);

#endif // GK2026_DG8DITHER_H_INCLUDED
