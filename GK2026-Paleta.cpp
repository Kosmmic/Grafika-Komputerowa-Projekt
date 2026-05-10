// funkcje do redukcji kolorow i tworzenia palet
#include "GK2026-Paleta.h"
#include "GK2026-Zmienne.h"
#include "GK2026-Funkcje.h"

// 8-bit (3R 3G 2B)
Uint8 z24Kdo8K(SDL_Color kolor) {
	return (kolor.r & 0xE0) | ((kolor.g >> 3) & 0x1C) | (kolor.b >> 6);
}

SDL_Color z8Kdo24K(Uint8 v) {
	SDL_Color c;
	c.r = v & 0xE0;
	c.g = (v & 0x1C) << 3;
	c.b = (v & 0x03) << 6;
	c.a = 255;
	return c;
}

// 4-bit (2R 1G 1B)
Uint8 z24Kdo4K(SDL_Color kolor) {
	return ((kolor.r >> 6) << 2) | ((kolor.g >> 7) << 1) | (kolor.b >> 7);
}

SDL_Color z4Kdo24K(Uint8 v) {
	SDL_Color c;
	c.r = ((v >> 2) & 0x03) * 85;
	c.g = ((v >> 1) & 0x01) * 255;
	c.b = (v & 0x01) * 255;
	c.a = 255;
	return c;
}
