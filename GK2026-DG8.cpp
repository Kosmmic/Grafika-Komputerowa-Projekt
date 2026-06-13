// zapis i odczyt plikow w formacie DG8 (wszystkie 5 trybow palety + dithering)
#include "GK2026-DG8.h"
#include "GK2026-Zmienne.h"
#include "GK2026-Funkcje.h"
#include "GK2026-DG8Paleta.h"
#include "GK2026-DG8Dither.h"

#include <fstream>
#include <iostream>
using namespace std;

int trybDG8 = 1;
int ditheringDG8 = 0;

static void zbudujPaletaDG8(int& tryb, SDL_Color paleta[256]) {
    if (tryb == 1) {
        paletaNarzucona256(paleta);
    } else if (tryb == 2) {
        paletaSzarosc256(paleta);
    } else if (tryb == 3) {
        paletaMedianCut256BW(paleta);
    } else if (tryb == 5) {
        if (!paletaWykryta256(paleta)) {
            tryb = 4;
            paletaMedianCut256Color(paleta);
        }
    } else {
        paletaMedianCut256Color(paleta);
    }
}

void zapiszDG8(int tryb, int dithering, const char* nazwa) {
    SDL_Color paleta[256];
    zbudujPaletaDG8(tryb, paleta);

    int szerokoscObrazka = szerokosc / 2;
    int wysokoscObrazka = wysokosc / 2;
    Uint8 indeksy[320 * 200];

    if (dithering == 1)
        ditherBayerDG8(paleta, 256, indeksy);
    else if (dithering == 2)
        ditherFSDG8(paleta, 256, indeksy);
    else
        ditherBrakDG8(paleta, 256, indeksy);

    char identyfikator[] = "DG";
    Uint16 szer = szerokoscObrazka;
    Uint16 wys = wysokoscObrazka;
    Uint8 trybZapis = tryb;
    Uint8 ditheringZapis = dithering;
    Uint32 rozmiarDanych = szerokoscObrazka * wysokoscObrazka;

    ofstream wyjscie(nazwa, ios::binary);
    wyjscie.write((char*)identyfikator, sizeof(char) * 2);
    wyjscie.write((char*)&szer, sizeof(Uint16));
    wyjscie.write((char*)&wys, sizeof(Uint16));
    wyjscie.write((char*)&trybZapis, sizeof(Uint8));
    wyjscie.write((char*)&ditheringZapis, sizeof(Uint8));
    wyjscie.write((char*)&rozmiarDanych, sizeof(Uint32));

    if (tryb >= 3) {
        for (int i = 0; i < 256; i++) {
            Uint8 rgb[3] = {paleta[i].r, paleta[i].g, paleta[i].b};
            wyjscie.write((char*)rgb, 3);
        }
    }

    for (int x = 0; x < szerokoscObrazka; x++) {
        for (int blockStart = 0; blockStart < wysokoscObrazka; blockStart += 8) {
            for (int r = 0; r < 8; r++) {
                int y = blockStart + r;
                Uint8 indeks = indeksy[y * szerokoscObrazka + x];
                wyjscie.write((char*)&indeks, sizeof(Uint8));
            }
        }
    }

    wyjscie.close();
    cout << "Zapisano plik '" << nazwa << "' (tryb " << (int)trybZapis << ", dithering " << (int)ditheringZapis << ")" << endl;
}

void odczytajDG8(const char* nazwa) {
    char identyfikator[3] = "  ";
    Uint16 szer = 0, wys = 0;
    Uint8 tryb = 0, dithering = 0;
    Uint32 rozmiarDanych = 0;

    ifstream wejscie(nazwa, ios::binary);
    wejscie.read((char*)identyfikator, sizeof(char) * 2);
    wejscie.read((char*)&szer, sizeof(Uint16));
    wejscie.read((char*)&wys, sizeof(Uint16));
    wejscie.read((char*)&tryb, sizeof(Uint8));
    wejscie.read((char*)&dithering, sizeof(Uint8));
    wejscie.read((char*)&rozmiarDanych, sizeof(Uint32));

    cout << "id: " << identyfikator << endl;
    cout << "szerokosc: " << szer << endl;
    cout << "wysokosc: " << wys << endl;
    cout << "tryb: " << (int)tryb << endl;
    cout << "dithering: " << (int)dithering << endl;

    SDL_Color paleta[256];
    if (tryb == 1) {
        paletaNarzucona256(paleta);
    } else if (tryb == 2) {
        paletaSzarosc256(paleta);
    } else {
        for (int i = 0; i < 256; i++) {
            Uint8 rgb[3];
            wejscie.read((char*)rgb, 3);
            paleta[i] = {rgb[0], rgb[1], rgb[2], 255};
        }
    }

    for (int x = 0; x < szer; x++) {
        for (int blockStart = 0; blockStart < wys; blockStart += 8) {
            for (int r = 0; r < 8; r++) {
                int y = blockStart + r;
                Uint8 indeks;
                wejscie.read((char*)&indeks, sizeof(Uint8));
                SDL_Color kolor = paleta[indeks];
                setPixel(x + szerokosc / 2, y + wysokosc / 2, kolor.r, kolor.g, kolor.b);
            }
        }
    }

    SDL_UpdateWindowSurface(window);
    cout << "Odczytano plik '" << nazwa << "'" << endl;
}

void Funkcja10() {
    zapiszDG8(trybDG8, ditheringDG8, "obrazDG8.bin");
    odczytajDG8("obrazDG8.bin");
}

void zmienTrybDG8() {
    trybDG8 = (trybDG8 % 5) + 1;
    cout << "trybDG8 = " << trybDG8 << endl;
}

void zmienDitheringDG8() {
    ditheringDG8 = (ditheringDG8 + 1) % 3;
    cout << "ditheringDG8 = " << ditheringDG8 << endl;
}
