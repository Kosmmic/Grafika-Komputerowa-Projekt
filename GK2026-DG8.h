// zapis i odczyt plikow w formacie DG8 (wszystkie 5 trybow palety + dithering)
#ifndef GK2026_DG8_H_INCLUDED
#define GK2026_DG8_H_INCLUDED

#include <SDL2/SDL.h>

extern int trybDG8;
extern int ditheringDG8;

void zapiszDG8(int tryb, int dithering, const char* nazwa);
void odczytajDG8(const char* nazwa);

void Funkcja10();
void zmienTrybDG8();
void zmienDitheringDG8();

#endif // GK2026_DG8_H_INCLUDED
