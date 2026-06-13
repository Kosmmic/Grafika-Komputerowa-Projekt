// generatory 256-elementowych palet i wyszukiwanie najblizszego koloru (format DG8)
#include "GK2026-DG8Paleta.h"
#include "GK2026-Zmienne.h"
#include "GK2026-Funkcje.h"
#include <algorithm>
#include <climits>
#include <cmath>

void paletaNarzucona256(SDL_Color paleta[256]) {
    const Uint8 tabRG[8] = {0, 36, 73, 109, 146, 182, 218, 255};
    const Uint8 tabB[4]  = {0, 85, 170, 255};
    for (int v = 0; v < 256; v++) {
        int r = (v >> 5) & 0x07;
        int g = (v >> 2) & 0x07;
        int b = v & 0x03;
        paleta[v] = {tabRG[r], tabRG[g], tabB[b], 255};
    }
}

void paletaSzarosc256(SDL_Color paleta[256]) {
    for (int v = 0; v < 256; v++) {
        paleta[v] = {(Uint8)v, (Uint8)v, (Uint8)v, 255};
    }
}

int najblizszyIndeksDG8(SDL_Color kolor, SDL_Color paleta[], int rozmiar) {
    int najlepszy = 0;
    int najlepszaOdl = INT_MAX;
    for (int i = 0; i < rozmiar; i++) {
        int dr = kolor.r - paleta[i].r;
        int dg = kolor.g - paleta[i].g;
        int db = kolor.b - paleta[i].b;
        int odl = dr * dr + dg * dg + db * db;
        if (odl < najlepszaOdl) {
            najlepszaOdl = odl;
            najlepszy = i;
        }
    }
    return najlepszy;
}

bool paletaWykryta256(SDL_Color paleta[256]) {
    int w = szerokosc / 2, h = wysokosc / 2;
    int ile = 0;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            SDL_Color k = getPixel(x, y);
            bool znaleziono = false;
            for (int i = 0; i < ile; i++) {
                if (paleta[i].r == k.r && paleta[i].g == k.g && paleta[i].b == k.b) {
                    znaleziono = true;
                    break;
                }
            }
            if (!znaleziono) {
                if (ile == 256) return false;
                paleta[ile++] = k;
            }
        }
    }
    for (int i = ile; i < 256; i++) paleta[i] = {0, 0, 0, 255};
    return true;
}

static void medianCutBW256(Uint8* dane, int start, int koniec, int poziom, SDL_Color paleta[], int* licznik) {
    if (poziom == 0 || start >= koniec) {
        long suma = 0;
        for (int i = start; i <= koniec; i++) suma += dane[i];
        Uint8 sredni = (Uint8)(suma / (koniec - start + 1));
        paleta[(*licznik)++] = {sredni, sredni, sredni, 255};
        return;
    }
    std::sort(dane + start, dane + koniec + 1);
    int srodek = (start + koniec) / 2;
    medianCutBW256(dane, start, srodek, poziom - 1, paleta, licznik);
    medianCutBW256(dane, srodek + 1, koniec, poziom - 1, paleta, licznik);
}

void paletaMedianCut256BW(SDL_Color paleta[256]) {
    int w = szerokosc / 2, h = wysokosc / 2;
    Uint8* dane = new Uint8[w * h];
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            SDL_Color k = getPixel(x, y);
            dane[y * w + x] = (Uint8)round(0.299 * k.r + 0.587 * k.g + 0.114 * k.b);
        }
    }
    int licznik = 0;
    medianCutBW256(dane, 0, w * h - 1, 8, paleta, &licznik);
    delete[] dane;
}

static void medianCutColor256(SDL_Color* dane, int start, int koniec, int poziom, SDL_Color paleta[], int* licznik) {
    if (poziom == 0 || start >= koniec) {
        long sumaR = 0, sumaG = 0, sumaB = 0;
        int n = koniec - start + 1;
        for (int i = start; i <= koniec; i++) {
            sumaR += dane[i].r;
            sumaG += dane[i].g;
            sumaB += dane[i].b;
        }
        paleta[(*licznik)++] = {(Uint8)(sumaR / n), (Uint8)(sumaG / n), (Uint8)(sumaB / n), 255};
        return;
    }
    int minR = 255, maxR = 0, minG = 255, maxG = 0, minB = 255, maxB = 0;
    for (int i = start; i <= koniec; i++) {
        minR = std::min(minR, (int)dane[i].r); maxR = std::max(maxR, (int)dane[i].r);
        minG = std::min(minG, (int)dane[i].g); maxG = std::max(maxG, (int)dane[i].g);
        minB = std::min(minB, (int)dane[i].b); maxB = std::max(maxB, (int)dane[i].b);
    }
    int dR = maxR - minR, dG = maxG - minG, dB = maxB - minB;
    if (dR >= dG && dR >= dB)
        std::sort(dane + start, dane + koniec + 1, [](SDL_Color a, SDL_Color b) { return a.r < b.r; });
    else if (dG >= dR && dG >= dB)
        std::sort(dane + start, dane + koniec + 1, [](SDL_Color a, SDL_Color b) { return a.g < b.g; });
    else
        std::sort(dane + start, dane + koniec + 1, [](SDL_Color a, SDL_Color b) { return a.b < b.b; });

    int srodek = (start + koniec) / 2;
    medianCutColor256(dane, start, srodek, poziom - 1, paleta, licznik);
    medianCutColor256(dane, srodek + 1, koniec, poziom - 1, paleta, licznik);
}

void paletaMedianCut256Color(SDL_Color paleta[256]) {
    int w = szerokosc / 2, h = wysokosc / 2;
    SDL_Color* dane = new SDL_Color[w * h];
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++)
            dane[y * w + x] = getPixel(x, y);

    int licznik = 0;
    medianCutColor256(dane, 0, w * h - 1, 8, paleta, &licznik);
    delete[] dane;
}
