// generatory 256-elementowych palet i wyszukiwanie najblizszego koloru (format DG8)
#ifndef GK2026_DG8PALETA_H_INCLUDED
#define GK2026_DG8PALETA_H_INCLUDED

#include <SDL2/SDL.h>

// tryb 1: paleta narzucona kolor (RRRGGGBB), wartosci wg tabel ze specyfikacji DG8
void paletaNarzucona256(SDL_Color paleta[256]);

// tryb 2: skala szarosci narzucona, paleta[v] = {v,v,v}
void paletaSzarosc256(SDL_Color paleta[256]);

// indeks najblizszego koloru w palecie (odleglosc euklidesowa^2 w RGB)
int najblizszyIndeksDG8(SDL_Color kolor, SDL_Color paleta[], int rozmiar);

// tryb 5: rzeczywiste kolory obrazu, jesli <=256 unikalnych. Zwraca false jesli >256
bool paletaWykryta256(SDL_Color paleta[256]);

// tryb 3: 256-kolorowy MedianCut na luminancji obrazu
void paletaMedianCut256BW(SDL_Color paleta[256]);

// tryb 4: 256-kolorowy MedianCut na kolorach RGB obrazu
void paletaMedianCut256Color(SDL_Color paleta[256]);

#endif // GK2026_DG8PALETA_H_INCLUDED
