// funkcje do redukcji kolorow i tworzenia palet
#ifndef GK2026_PALETA_H_INCLUDED
#define GK2026_PALETA_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <iostream>
#include <cmath>
using namespace std;

Uint8 z24Kdo8K(SDL_Color kolor);
SDL_Color z8Kdo24K(Uint8 v);

Uint8 z24Kdo4K(SDL_Color kolor);
SDL_Color z4Kdo24K(Uint8 v);

#endif // GK2026_PALETA_H_INCLUDED
