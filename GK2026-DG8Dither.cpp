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
