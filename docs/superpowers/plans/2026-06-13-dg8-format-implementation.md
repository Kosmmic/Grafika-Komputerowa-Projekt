# Format DG8 - Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Zaimplementować zapis/odczyt pliku `.dg8` (5 trybów koloru + dithering)
zgodnie z `docs/superpowers/specs/2026-06-13-dg8-format-design.md`, jako wyłącznie
nowy kod (nowe pliki, nowe funkcje, nowe klawisze) - bez modyfikacji istniejących
funkcji Funkcja1-9, MedianCut*16, Bayer/FloydSteinberg* itd.

**Architecture:** 3 nowe pary plików: `GK2026-DG8Paleta` (generowanie 256-elementowych
palet: narzucona, szarość, MedianCut256 BW/Color, paleta wykryta, nearest-color),
`GK2026-DG8Dither` (dithering Bayer/Floyd-Steinberg do dowolnej 256-elementowej
palety), `GK2026-DG8` (zapiszDG8/odczytajDG8, nagłówek, kolejność blokowa 8px,
Funkcja10 + obsługa klawiszy). Wszystkie tryby 1-5 są wewnętrznie ujednolicone:
każdy tryb daje 256-elementową paletę `SDL_Color[256]` w pamięci - dla trybów 1/2
jest ona generowana deterministycznie wzorem (i niezapisywana do pliku), dla
trybów 3/4/5 zapisywana jako 768B. Piksel = indeks najbliższego koloru w tej palecie.

**Tech Stack:** C++17, SDL2, operacje na lewej-górnej ćwiartce ekranu (320x200,
`getPixel`/`setPixel` z `GK2026-Funkcje.cpp`).

---

### Task 1: GK2026-DG8Paleta - generatory palet 256-kolorowych

**Files:**
- Create: `GK2026-DG8Paleta.h`
- Create: `GK2026-DG8Paleta.cpp`

- [ ] **Step 1: Utwórz `GK2026-DG8Paleta.h`**

```cpp
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
```

- [ ] **Step 2: Utwórz `GK2026-DG8Paleta.cpp`**

```cpp
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
```

- [ ] **Step 3: Sprawdź składnię (syntax-only, bez linkowania)**

Run: `cd "/Users/miloszbembnowicz/Studia/2 rok niestacjo/GK/GK_projekt" && g++ -std=c++17 -I/opt/homebrew/include -D_THREAD_SAFE -fsyntax-only GK2026-DG8Paleta.cpp`
Expected: brak wyjścia (sukces, brak błędów kompilacji)

- [ ] **Step 4: Commit**

```bash
cd "/Users/miloszbembnowicz/Studia/2 rok niestacjo/GK/GK_projekt"
git add GK2026-DG8Paleta.h GK2026-DG8Paleta.cpp
git commit -m "feat(dg8): generatory palet 256-kolorowych (narzucona, szarosc, MedianCut256, wykryta)"
```

---

### Task 2: GK2026-DG8Dither - dithering do dowolnej palety

**Files:**
- Create: `GK2026-DG8Dither.h`
- Create: `GK2026-DG8Dither.cpp`

- [ ] **Step 1: Utwórz `GK2026-DG8Dither.h`**

```cpp
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
```

- [ ] **Step 2: Utwórz `GK2026-DG8Dither.cpp`**

```cpp
// dithering (brak/Bayer/Floyd-Steinberg) do dowolnej palety 256-kolorowej (format DG8)
#include "GK2026-DG8Dither.h"
#include "GK2026-DG8Paleta.h"
#include "GK2026-Zmienne.h"
#include "GK2026-Funkcje.h"

static float clampDG8(float v) {
    if (v < 0) return 0;
    if (v > 255) return 255;
    return v;
}

void ditherBrakDG8(SDL_Color paleta[], int rozmiar, Uint8* wynik) {
    int w = szerokosc / 2, h = wysokosc / 2;
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++)
            wynik[y * w + x] = (Uint8)najblizszyIndeksDG8(getPixel(x, y), paleta, rozmiar);
}

void ditherBayerDG8(SDL_Color paleta[], int rozmiar, Uint8* wynik) {
    int w = szerokosc / 2, h = wysokosc / 2;
    const float amplituda = 32.0f;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            SDL_Color k = getPixel(x, y);
            float prog = (bayer4x4[x % 4][y % 4] - 0.5f) * amplituda;
            SDL_Color przesuniety;
            przesuniety.r = (Uint8)clampDG8(k.r + prog);
            przesuniety.g = (Uint8)clampDG8(k.g + prog);
            przesuniety.b = (Uint8)clampDG8(k.b + prog);
            wynik[y * w + x] = (Uint8)najblizszyIndeksDG8(przesuniety, paleta, rozmiar);
        }
    }
}

static void rozlozBladDG8(float* tab, int x, int y, float blad, int w, int h) {
    if (x >= 0 && x < w && y >= 0 && y < h) tab[y * w + x] += blad;
}

void ditherFSDG8(SDL_Color paleta[], int rozmiar, Uint8* wynik) {
    int w = szerokosc / 2, h = wysokosc / 2;
    float* robR = new float[w * h];
    float* robG = new float[w * h];
    float* robB = new float[w * h];

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            SDL_Color k = getPixel(x, y);
            int idx = y * w + x;
            robR[idx] = k.r;
            robG[idx] = k.g;
            robB[idx] = k.b;
        }
    }

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int idx = y * w + x;
            SDL_Color stary;
            stary.r = (Uint8)clampDG8(robR[idx]);
            stary.g = (Uint8)clampDG8(robG[idx]);
            stary.b = (Uint8)clampDG8(robB[idx]);

            int i = najblizszyIndeksDG8(stary, paleta, rozmiar);
            wynik[idx] = (Uint8)i;
            SDL_Color nowy = paleta[i];

            float bladR = robR[idx] - nowy.r;
            float bladG = robG[idx] - nowy.g;
            float bladB = robB[idx] - nowy.b;

            rozlozBladDG8(robR, x + 1, y,     bladR * 7.0f / 16.0f, w, h);
            rozlozBladDG8(robR, x - 1, y + 1, bladR * 3.0f / 16.0f, w, h);
            rozlozBladDG8(robR, x,     y + 1, bladR * 5.0f / 16.0f, w, h);
            rozlozBladDG8(robR, x + 1, y + 1, bladR * 1.0f / 16.0f, w, h);

            rozlozBladDG8(robG, x + 1, y,     bladG * 7.0f / 16.0f, w, h);
            rozlozBladDG8(robG, x - 1, y + 1, bladG * 3.0f / 16.0f, w, h);
            rozlozBladDG8(robG, x,     y + 1, bladG * 5.0f / 16.0f, w, h);
            rozlozBladDG8(robG, x + 1, y + 1, bladG * 1.0f / 16.0f, w, h);

            rozlozBladDG8(robB, x + 1, y,     bladB * 7.0f / 16.0f, w, h);
            rozlozBladDG8(robB, x - 1, y + 1, bladB * 3.0f / 16.0f, w, h);
            rozlozBladDG8(robB, x,     y + 1, bladB * 5.0f / 16.0f, w, h);
            rozlozBladDG8(robB, x + 1, y + 1, bladB * 1.0f / 16.0f, w, h);
        }
    }

    delete[] robR;
    delete[] robG;
    delete[] robB;
}
```

- [ ] **Step 3: Sprawdź składnię**

Run: `cd "/Users/miloszbembnowicz/Studia/2 rok niestacjo/GK/GK_projekt" && g++ -std=c++17 -I/opt/homebrew/include -D_THREAD_SAFE -fsyntax-only GK2026-DG8Dither.cpp`
Expected: brak wyjścia (sukces)

- [ ] **Step 4: Commit**

```bash
cd "/Users/miloszbembnowicz/Studia/2 rok niestacjo/GK/GK_projekt"
git add GK2026-DG8Dither.h GK2026-DG8Dither.cpp
git commit -m "feat(dg8): dithering Bayer i Floyd-Steinberg do dowolnej palety 256-kolorowej"
```

---

### Task 3: GK2026-DG8 - zapis/odczyt pliku DG8 + Funkcja10

**Files:**
- Create: `GK2026-DG8.h`
- Create: `GK2026-DG8.cpp`

- [ ] **Step 1: Utwórz `GK2026-DG8.h`**

```cpp
// zapis/odczyt formatu DG8 (specyfikacja: docs/superpowers/specs/2026-06-13-dg8-format-design.md)
#ifndef GK2026_DG8_H_INCLUDED
#define GK2026_DG8_H_INCLUDED

#include <SDL2/SDL.h>

// 1=paleta narzucona kolor, 2=szarosc narzucona, 3=szarosc dedykowana,
// 4=paleta dedykowana (MedianCut), 5=paleta wykryta
extern int trybDG8;

// 0=brak, 1=Bayer4x4, 2=Floyd-Steinberg
extern int ditheringDG8;

void zapiszDG8(int tryb, int dithering, const char* nazwa);
void odczytajDG8(const char* nazwa);

void Funkcja10();
void zmienTrybDG8();
void zmienDitheringDG8();

#endif // GK2026_DG8_H_INCLUDED
```

- [ ] **Step 2: Utwórz `GK2026-DG8.cpp`**

```cpp
// zapis/odczyt formatu DG8 (specyfikacja: docs/superpowers/specs/2026-06-13-dg8-format-design.md)
#include "GK2026-DG8.h"
#include "GK2026-DG8Paleta.h"
#include "GK2026-DG8Dither.h"
#include "GK2026-Zmienne.h"
#include "GK2026-Funkcje.h"
#include <fstream>
#include <iostream>

using namespace std;

int trybDG8 = 1;
int ditheringDG8 = 0;

void zapiszDG8(int tryb, int dithering, const char* nazwa) {
    int w = szerokosc / 2;
    int h = wysokosc / 2;

    SDL_Color paleta[256];
    bool zapisPalety = false;

    switch (tryb) {
        case 1:
            paletaNarzucona256(paleta);
            break;
        case 2:
            paletaSzarosc256(paleta);
            break;
        case 3:
            paletaMedianCut256BW(paleta);
            zapisPalety = true;
            break;
        case 4:
            paletaMedianCut256Color(paleta);
            zapisPalety = true;
            break;
        case 5:
            if (!paletaWykryta256(paleta)) {
                tryb = 4;
                paletaMedianCut256Color(paleta);
            }
            zapisPalety = true;
            break;
    }

    Uint8* indeksy = new Uint8[w * h];
    if (dithering == 1) ditherBayerDG8(paleta, 256, indeksy);
    else if (dithering == 2) ditherFSDG8(paleta, 256, indeksy);
    else ditherBrakDG8(paleta, 256, indeksy);

    ofstream wyjscie(nazwa, ios::binary);

    char id[2] = {'D', 'G'};
    Uint16 szer = (Uint16)w;
    Uint16 wys = (Uint16)h;
    Uint8 trybZapis = (Uint8)tryb;
    Uint8 dith = (Uint8)dithering;
    Uint32 rozmiarDanych = (Uint32)(w * h);

    wyjscie.write(id, 2);
    wyjscie.write((char*)&szer, sizeof(Uint16));
    wyjscie.write((char*)&wys, sizeof(Uint16));
    wyjscie.write((char*)&trybZapis, sizeof(Uint8));
    wyjscie.write((char*)&dith, sizeof(Uint8));
    wyjscie.write((char*)&rozmiarDanych, sizeof(Uint32));

    if (zapisPalety) {
        for (int i = 0; i < 256; i++) {
            Uint8 rgb[3] = {paleta[i].r, paleta[i].g, paleta[i].b};
            wyjscie.write((char*)rgb, 3);
        }
    }

    for (int x = 0; x < w; x++) {
        for (int blok = 0; blok < h; blok += 8) {
            for (int r = 0; r < 8; r++) {
                int y = blok + r;
                Uint8 v = indeksy[y * w + x];
                wyjscie.write((char*)&v, sizeof(Uint8));
            }
        }
    }

    wyjscie.close();
    delete[] indeksy;

    cout << "Zapisano '" << nazwa << "': " << w << "x" << h
         << ", tryb=" << (int)trybZapis << ", dithering=" << (int)dith
         << ", rozmiar danych=" << rozmiarDanych << endl;
}

void odczytajDG8(const char* nazwa) {
    ifstream wejscie(nazwa, ios::binary);

    char id[2];
    Uint16 szer = 0, wys = 0;
    Uint8 tryb = 0, dithering = 0;
    Uint32 rozmiarDanych = 0;

    wejscie.read(id, 2);
    wejscie.read((char*)&szer, sizeof(Uint16));
    wejscie.read((char*)&wys, sizeof(Uint16));
    wejscie.read((char*)&tryb, sizeof(Uint8));
    wejscie.read((char*)&dithering, sizeof(Uint8));
    wejscie.read((char*)&rozmiarDanych, sizeof(Uint32));

    cout << "Odczytano '" << nazwa << "': " << szer << "x" << wys
         << ", tryb=" << (int)tryb << ", dithering=" << (int)dithering
         << ", rozmiar danych=" << rozmiarDanych << endl;

    SDL_Color paleta[256];
    if (tryb >= 3 && tryb <= 5) {
        for (int i = 0; i < 256; i++) {
            Uint8 rgb[3];
            wejscie.read((char*)rgb, 3);
            paleta[i] = {rgb[0], rgb[1], rgb[2], 255};
        }
    } else if (tryb == 1) {
        paletaNarzucona256(paleta);
    } else {
        paletaSzarosc256(paleta);
    }

    int w = szer, h = wys;
    for (int x = 0; x < w; x++) {
        for (int blok = 0; blok < h; blok += 8) {
            for (int r = 0; r < 8; r++) {
                Uint8 idx;
                wejscie.read((char*)&idx, sizeof(Uint8));
                int y = blok + r;
                SDL_Color kolor = paleta[idx];
                setPixel(x + szerokosc / 2, y + wysokosc / 2, kolor.r, kolor.g, kolor.b);
            }
        }
    }

    wejscie.close();
}

void Funkcja10() {
    cout << "Funkcja10: zapis/odczyt DG8, tryb=" << trybDG8 << ", dithering=" << ditheringDG8 << endl;
    zapiszDG8(trybDG8, ditheringDG8, "obraz.dg8");
    odczytajDG8("obraz.dg8");
    SDL_UpdateWindowSurface(window);
}

void zmienTrybDG8() {
    trybDG8++;
    if (trybDG8 > 5) trybDG8 = 1;
    cout << "trybDG8 = " << trybDG8 << endl;
}

void zmienDitheringDG8() {
    ditheringDG8++;
    if (ditheringDG8 > 2) ditheringDG8 = 0;
    cout << "ditheringDG8 = " << ditheringDG8 << endl;
}
```

- [ ] **Step 3: Sprawdź składnię**

Run: `cd "/Users/miloszbembnowicz/Studia/2 rok niestacjo/GK/GK_projekt" && g++ -std=c++17 -I/opt/homebrew/include -D_THREAD_SAFE -fsyntax-only GK2026-DG8.cpp`
Expected: brak wyjścia (sukces)

- [ ] **Step 4: Commit**

```bash
cd "/Users/miloszbembnowicz/Studia/2 rok niestacjo/GK/GK_projekt"
git add GK2026-DG8.h GK2026-DG8.cpp
git commit -m "feat(dg8): zapiszDG8/odczytajDG8 z naglowkiem i kolejnoscia blokowa 8px"
```

---

### Task 4: Podłączenie klawiszy w GK2026-Projekt.cpp

**Files:**
- Modify: `GK2026-Projekt.cpp:5` (includes)
- Modify: `GK2026-Projekt.cpp:88-91` (obsługa klawiszy)

- [ ] **Step 1: Dodaj include**

W `GK2026-Projekt.cpp`, po linii `#include "GK2026-MedianCut.h"` (linia 5) dodaj:

```cpp
#include "GK2026-DG8.h"
```

- [ ] **Step 2: Dodaj obsługę klawiszy przed blokiem `SDLK_b`**

W `GK2026-Projekt.cpp`, znajdź blok:

```cpp
                if (event.key.keysym.sym == SDLK_b)
                    czyscEkran(0, 0, 0);
                else
                    break;
```

Dodaj bezpośrednio PRZED nim (jako kolejne niezależne `if`, w stylu istniejących klawiszy 1-9):

```cpp
                if (event.key.keysym.sym == SDLK_0)
                    Funkcja10();
                if (event.key.keysym.sym == SDLK_LEFTBRACKET)
                    zmienTrybDG8();
                if (event.key.keysym.sym == SDLK_RIGHTBRACKET)
                    zmienDitheringDG8();
```

Wynikowy fragment (kontekst):

```cpp
                if (event.key.keysym.sym == SDLK_l)
                    ladujBMP("obrazek9.bmp", 0, 0);
                if (event.key.keysym.sym == SDLK_0)
                    Funkcja10();
                if (event.key.keysym.sym == SDLK_LEFTBRACKET)
                    zmienTrybDG8();
                if (event.key.keysym.sym == SDLK_RIGHTBRACKET)
                    zmienDitheringDG8();
                if (event.key.keysym.sym == SDLK_b)
                    czyscEkran(0, 0, 0);
                else
                    break;
```

- [ ] **Step 3: Commit**

```bash
cd "/Users/miloszbembnowicz/Studia/2 rok niestacjo/GK/GK_projekt"
git add GK2026-Projekt.cpp
git commit -m "feat(dg8): klawisze 0/[/] do testu zapisu/odczytu DG8 i zmiany trybu/ditheringu"
```

---

### Task 5: Rejestracja nowych plików w build.sh i .cbp

**Files:**
- Modify: `build.sh`
- Modify: `GK2026-Projekt.cbp`

- [ ] **Step 1: Zaktualizuj `build.sh`**

Obecna treść:

```bash
#!/bin/bash
cd "$(dirname "$0")"
g++ -std=c++17 -I/opt/homebrew/include -D_THREAD_SAFE \
    GK2026-Zmienne.cpp GK2026-Paleta.cpp GK2026-Pliki.cpp \
    GK2026-Funkcje.cpp GK2026-MedianCut.cpp GK2026-Projekt.cpp \
    -o GK_projekt -L/opt/homebrew/lib -lSDL2 2>&1
```

Zmień na:

```bash
#!/bin/bash
cd "$(dirname "$0")"
g++ -std=c++17 -I/opt/homebrew/include -D_THREAD_SAFE \
    GK2026-Zmienne.cpp GK2026-Paleta.cpp GK2026-Pliki.cpp \
    GK2026-Funkcje.cpp GK2026-MedianCut.cpp \
    GK2026-DG8Paleta.cpp GK2026-DG8Dither.cpp GK2026-DG8.cpp \
    GK2026-Projekt.cpp \
    -o GK_projekt -L/opt/homebrew/lib -lSDL2 2>&1
```

- [ ] **Step 2: Zaktualizuj `GK2026-Projekt.cbp`**

Znajdź blok:

```xml
		<Unit filename="GK2026-Paleta.cpp" />
		<Unit filename="GK2026-Paleta.h" />
```

Dodaj PO nim (przed `<Unit filename="GK2026-Pliki.cpp" />`):

```xml
		<Unit filename="GK2026-DG8.cpp" />
		<Unit filename="GK2026-DG8.h" />
		<Unit filename="GK2026-DG8Dither.cpp" />
		<Unit filename="GK2026-DG8Dither.h" />
		<Unit filename="GK2026-DG8Paleta.cpp" />
		<Unit filename="GK2026-DG8Paleta.h" />
```

(Kolejność alfabetyczna jak resztę listy `<Unit>` w pliku.)

- [ ] **Step 3: Commit**

```bash
cd "/Users/miloszbembnowicz/Studia/2 rok niestacjo/GK/GK_projekt"
git add build.sh GK2026-Projekt.cbp
git commit -m "build(dg8): dodaj nowe pliki DG8 do build.sh i Code::Blocks"
```

---

### Task 6: Pełna kompilacja i test manualny

**Files:** brak zmian (weryfikacja)

- [ ] **Step 1: Zbuduj projekt**

Run: `cd "/Users/miloszbembnowicz/Studia/2 rok niestacjo/GK/GK_projekt" && ./build.sh`
Expected: brak błędów kompilacji/linkowania, powstaje plik wykonywalny `GK_projekt`

- [ ] **Step 2: Test manualny (instrukcja dla użytkownika)**

Uruchom `./GK_projekt`. Wczytaj obrazek klawiszem `a` (lub innym a-l, zależnie od
dostępnych plików `obrazek*.bmp`), tak aby lewa-górna ćwiartka zawierała obraz.

Następnie:
- `[` - zmienia `trybDG8` 1->2->3->4->5->1 (wypisuje wartość w konsoli)
- `]` - zmienia `ditheringDG8` 0->1->2->0 (wypisuje wartość w konsoli)
- `0` - zapisuje `obraz.dg8` w aktualnym trybie/ditheringu i odczytuje go z
  powrotem do prawej-dolnej ćwiartki

Sprawdź dla każdego trybu (1-5) i każdego ditheringu (0-2), że obraz w prawej-
dolnej ćwiartce wygląda sensownie (paleta narzucona/szarość/dedykowana/wykryta,
widoczny wzór ditheringu dla 1/2). Sprawdź w konsoli, że `rozmiar danych` =
320*200 = 64000, oraz że dla trybu 5 na obrazku z dużą liczbą kolorów następuje
fallback na tryb=4 (komunikat w konsoli pokaże `tryb=4`).

- [ ] **Step 3: Zweryfikuj plik binarny (opcjonalnie)**

Run: `cd "/Users/miloszbembnowicz/Studia/2 rok niestacjo/GK/GK_projekt" && xxd -l 16 obraz.dg8`
Expected: pierwsze 2 bajty `4447` (ASCII "DG"), bajty 2-3 = `4001` (320 little-endian),
bajty 4-5 = `c800` (200 little-endian), bajt 6 = tryb (01-05), bajt 7 = dithering (00-02)
