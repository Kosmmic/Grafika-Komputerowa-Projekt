// metody do algorytmu kwantyzacji (MedianCut)
#include "GK2026-MedianCut.h"
#include "GK2026-Zmienne.h"
#include "GK2026-Funkcje.h"
#include "GK2026-Paleta.h"
#include <algorithm>
using namespace std;

void MedianCutBW(int start, int koniec, int iteracja){
    for(int i=0;i<iteracja;i++)cout <<" ";
    cout<<"start: "<<start<<", koniec: "<<koniec<<", iteracja: "<<iteracja<<endl;
    if (start >= koniec) {
        if(ileKubelkow < 16) {
            paleta8s[ileKubelkow++] = {0,0,0};
        }
        return;
    }
    if(iteracja>0){
        sortujKubelekBW(start,koniec);

        for(int i=0;i<iteracja;i++)cout<<" ";
        cout<<"Dzielimy kubelek na poziomie "<<iteracja<<endl;

        int srodek = (start+koniec)/2;
        MedianCutBW(start,srodek,iteracja-1);
        MedianCutBW(srodek+1, koniec, iteracja-1);
    }
    else{
            if(ileKubelkow < 16){
                long sumaBW = 0;
                for(int p = start; p <= koniec; p++){
                sumaBW += obrazekS[p];
            }

            Uint8 noweBW = sumaBW / (koniec - start +1);
            SDL_Color nowyKolor = {noweBW,noweBW,noweBW};

            paleta8s[ileKubelkow] = nowyKolor;
            cout<<"Kubelek "<<ileKubelkow<<": ";
            cout<<"(s:"<<start<<", k:"<<koniec<<", e:"<<(koniec+1-start)<<")";
            cout<<endl;
            cout<<"Kolor "<<ileKubelkow<<": ("<<(int)nowyKolor.r<<",";
            cout<<(int)nowyKolor.g<<","<<(int)nowyKolor.b<<")"<<endl;
            ileKubelkow++;
            }
    }
}
void MedianCutColor(int start, int koniec, int iteracja){
    if (start >= koniec) return;

    for(int i=0;i<iteracja;i++)cout <<" ";
    cout<<"start: "<<start<<", koniec: "<<koniec<<", iteracja: "<<iteracja<<endl;

    if (start >= koniec) {
        if(ileKubelkow < 16) {
            paleta8k[ileKubelkow++] = {0,0,0};
        }
        return;
    }
    if(iteracja>0){
        sortujKubelekColor(start,koniec);

        for(int i=0;i<iteracja;i++)cout<<" ";
        cout<<"Dzielimy kubelek na poziomie "<<iteracja<<endl;

        int srodek = (start+koniec)/2;
        MedianCutColor(start,srodek,iteracja-1);
        MedianCutColor(srodek+1, koniec, iteracja-1);
    }
    else{
            if(ileKubelkow < 16){
                long sumaR = 0, sumaG = 0, sumaB = 0;
                int liczbaPikseli = koniec - start + 1;

                for(int p = start; p <= koniec; p++){
                    sumaR += obrazekK[p].r;
                    sumaG += obrazekK[p].g;
                    sumaB += obrazekK[p].b;
                }

                SDL_Color sredniKolor = {
                    (Uint8)(sumaR / liczbaPikseli),
                    (Uint8)(sumaG / liczbaPikseli),
                    (Uint8)(sumaB / liczbaPikseli)
                };

                paleta8k[ileKubelkow] = sredniKolor;
                ileKubelkow++;
            }
    }
}

void paletaMedianCutBW(){
    int ileKubelkow = 0;
    int ileKolorow = 0;
    czyscPalete();
    SDL_Color kolor;
    int szary= 0;
    int numer =0;
    int indeks =0;
    for (int y=0;y<wysokosc/2;y++){
        for(int x=0;x<szerokosc/2;x++){
            kolor = getPixel(x,y);
            szary = 0.299*kolor.r + 0.587*kolor.g +0.114*kolor.b;
            if (numer < 64000) {
                obrazekS[numer] = szary;
                numer++;
            }
            setPixel(x+szerokosc/2,y,szary,szary,szary);
        }
    }
    if (numer > 0) {
        MedianCutBW(0, numer-1, 4);
    }
    for(int y=0;y<wysokosc/2;y++){
        for(int x=0;x<szerokosc/2;x++){
            szary = getPixel(x+szerokosc/2, y).r;
            indeks = znajdzSasiadaBW(szary);
            setPixel(x+szerokosc/2,y+wysokosc/2,
            paleta8s[indeks].r, paleta8s[indeks].g, paleta8s[indeks].b);
        }
    }
    narysujPalete4b(0, 250, paleta8s);
    SDL_UpdateWindowSurface(window);
}
void paletaMedianCutColor() {
    ::ileKubelkow = 0;
    czyscPalete();

    SDL_Color kolor;
    int numer = 0;
    int indeks = 0;

    for (int y = 0; y < wysokosc / 2; y++) {
        for (int x = 0; x < szerokosc / 2; x++) {
            kolor = getPixel(x, y);

            if (numer < 1000000) {
                obrazekK[numer] = kolor;
                numer++;
            }
            // Opcjonalny podgląd w prawej górnej ćwiartce
            setPixel(x + szerokosc / 2, y, kolor.r, kolor.g, kolor.b);
        }
    }
    if (numer > 0) {
        MedianCutColor(0, numer - 1, 4);
    }

    // 3. Mapujemy piksele na nową paletę (prawa dolna ćwiartka)
    for (int y = 0; y < wysokosc / 2; y++) {
        for (int x = 0; x < szerokosc / 2; x++) {
            kolor = getPixel(x, y);
            indeks = znajdzSasiadaColor(kolor);

            SDL_Color nowy = paleta8k[indeks];
            setPixel(x + szerokosc / 2, y + wysokosc / 2, nowy.r, nowy.g, nowy.b);
        }
    }
    narysujPalete4b(0, 250, paleta8k);
}

void narysujPalete4b(int px, int py, SDL_Color pal[]){
    int x,y;
    for(int k=0;k<16;k++){
        y = k/4;
        x = k % 4;

        for (int xx=-1; xx<31; xx++){
            for(int yy=-1; yy<31; yy++){
                if(xx==-1 || xx==30 || yy==-1 || yy==30)
                    setPixel(x*30+xx+px, y*30+yy+py, 255, 255, 255);
            }
        }
        for (int xx=0; xx<30; xx++){
            for(int yy=0; yy<30; yy++){
                setPixel(x*30+xx+px, y*30+yy+py, pal[k].r,pal[k].g,pal[k].b);
            }
        }
    }
}

void sortujKubelekBW(int start, int koniec){
    std::sort(obrazekS + start, obrazekS + koniec + 1);
}
void sortujKubelekColor(int start, int koniec) {
    int minR = 255, maxR = 0, minG = 255, maxG = 0, minB = 255, maxB = 0;
    for (int i = start; i <= koniec; i++) {
        if (obrazekK[i].r < minR) minR = obrazekK[i].r; if (obrazekK[i].r > maxR) maxR = obrazekK[i].r;
        if (obrazekK[i].g < minG) minG = obrazekK[i].g; if (obrazekK[i].g > maxG) maxG = obrazekK[i].g;
        if (obrazekK[i].b < minB) minB = obrazekK[i].b; if (obrazekK[i].b > maxB) maxB = obrazekK[i].b;
    }
    int dR = maxR - minR, dG = maxG - minG, dB = maxB - minB;

    if (dR >= dG && dR >= dB) // Sortuj po Czerwonym
        sort(obrazekK + start, obrazekK + koniec + 1, [](SDL_Color a, SDL_Color b) { return a.r < b.r; });
    else if (dG >= dR && dG >= dB) // Sortuj po Zielonym
        sort(obrazekK + start, obrazekK + koniec + 1, [](SDL_Color a, SDL_Color b) { return a.g < b.g; });
    else // Sortuj po Niebieskim
        sort(obrazekK + start, obrazekK + koniec + 1, [](SDL_Color a, SDL_Color b) { return a.b < b.b; });
}

int znajdzSasiadaBW(Uint8 wartosc){
    int minimum = 1000;
    int indexMinimum = 0;
    int odleglosc = 0;

    for(int i=0;i<16;i++){
        odleglosc = abs(wartosc - paleta8s[i].r);
        if(odleglosc< minimum){
            minimum = odleglosc;
            indexMinimum = i;
        }
    }
    return indexMinimum;
}
int znajdzSasiadaColor(SDL_Color kolor){
    int minimum = 1000000;
    int index = 0;
    for(int i=0; i<16; i++){
        int dR = kolor.r - paleta8k[i].r;
        int dG = kolor.g - paleta8k[i].g;
        int dB = kolor.b - paleta8k[i].b;
        int odleglosc = dR*dR + dG*dG + dB*dB;
        if(odleglosc < minimum){
            minimum = odleglosc;
            index = i;
        }
    }
    return index;
}


void Bayer() {
    for (int y = 0; y < wysokosc; y++) {
        for (int x = 0; x < szerokosc; x++) {
            SDL_Color kolor = getPixel(x, y);
            float szary = 0.299 * kolor.r + 0.587 * kolor.g + 0.114 * kolor.b;

            // Mno¿nik progu dla 16 poziomów
            float m = 17.0 * (bayer4x4[x % 4][y % 4] - 0.5);
            float wynik = szary + m;

            if (wynik < 0) wynik = 0; if (wynik > 255) wynik = 255;

            if (wynik < 0) {
                wynik = 0;
            }
            if (wynik > 255) {
                wynik = 255;
            }

            Uint8 v = (Uint8)(wynik / 17);
            SDL_Color n = z4Sdo24K(v);
            setPixel(x + szerokosc / 2, y, n.r, n.g, n.b);
        }
    }
}
void BayerKolor() {
    for (int y = 0; y < wysokosc; y++) {
        for (int x = 0; x < szerokosc; x++) {
            SDL_Color c = getPixel(x, y);
            float progi = bayer4x4[x % 4][y % 4] - 0.5;
            float r = c.r + 85.0 * progi;
            float g = c.g + 255.0 * progi;
            float b = c.b + 255.0 * progi;

            SDL_Color n;
            n.r = (r<0)?0:(r>255)?255:(Uint8)(round(r/85.0)*85);
            n.g = (g<0)?0:(g>255)?255:(Uint8)(round(g/255.0)*255);
            n.b = (b<0)?0:(b>255)?255:(Uint8)(round(b/255.0)*255);

            setPixel(x + szerokosc / 2, y + wysokosc / 2, n.r, n.g, n.b);
        }
    }
}
void FloydSteinbergBW() {
    float* roboczy = new float[(szerokosc/2) * (wysokosc/2)];
    int nr = 0;
    for(int y=0; y<wysokosc/2; y++) {
        for(int x=0; x<szerokosc/2; x++) {
            SDL_Color c = getPixel(x, y);
            roboczy[y * (szerokosc/2) + x] = 0.299*c.r + 0.587*c.g + 0.114*c.b;
        }
    }

    for(int y=0; y<wysokosc/2; y++) {
        for(int x=0; x<szerokosc/2; x++) {
            float staryKolor = roboczy[y * (szerokosc/2) + x];

            int indeks = znajdzSasiadaBW((Uint8)clamping(staryKolor));
            float nowyKolor = paleta8s[indeks].r;

            setPixel(x + szerokosc/2, y + wysokosc/2, (Uint8)nowyKolor, (Uint8)nowyKolor, (Uint8)nowyKolor);

            float blad = staryKolor - nowyKolor;

            distributeError(roboczy, x + 1, y,     blad * 7.0/16.0);
            distributeError(roboczy, x - 1, y + 1, blad * 3.0/16.0);
            distributeError(roboczy, x,     y + 1, blad * 5.0/16.0);
            distributeError(roboczy, x + 1, y + 1, blad * 1.0/16.0);
        }
    }
    delete[] roboczy;
}
void FloydSteinbergColor() {
    float* robR = new float[(szerokosc/2) * (wysokosc/2)];
    float* robG = new float[(szerokosc/2) * (wysokosc/2)];
    float* robB = new float[(szerokosc/2) * (wysokosc/2)];

    for(int y=0; y<wysokosc/2; y++) {
        for(int x=0; x<szerokosc/2; x++) {
            SDL_Color c = getPixel(x, y);
            int idx = y * (szerokosc/2) + x;
            robR[idx] = (float)c.r;
            robG[idx] = (float)c.g;
            robB[idx] = (float)c.b;
        }
    }

    for(int y=0; y<wysokosc/2; y++) {
        for(int x=0; x<szerokosc/2; x++) {
            int idx = y * (szerokosc/2) + x;

            SDL_Color staryKolor;
            staryKolor.r = (Uint8)clamping(robR[idx]);
            staryKolor.g = (Uint8)clamping(robG[idx]);
            staryKolor.b = (Uint8)clamping(robB[idx]);

            int indeks = znajdzSasiadaColor(staryKolor);
            SDL_Color nowyKolor = paleta8k[indeks];


            setPixel(x + szerokosc/2, y + wysokosc/2, nowyKolor.r, nowyKolor.g, nowyKolor.b);

            float bladR = robR[idx] - nowyKolor.r;
            float bladG = robG[idx] - nowyKolor.g;
            float bladB = robB[idx] - nowyKolor.b;

            distributeError(robR, x + 1, y,     bladR * 7.0/16.0);
            distributeError(robR, x - 1, y + 1, bladR * 3.0/16.0);
            distributeError(robR, x,     y + 1, bladR * 5.0/16.0);
            distributeError(robR, x + 1, y + 1, bladR * 1.0/16.0);

            distributeError(robG, x + 1, y,     bladG * 7.0/16.0);
            distributeError(robG, x - 1, y + 1, bladG * 3.0/16.0);
            distributeError(robG, x,     y + 1, bladG * 5.0/16.0);
            distributeError(robG, x + 1, y + 1, bladG * 1.0/16.0);

            distributeError(robB, x + 1, y,     bladB * 7.0/16.0);
            distributeError(robB, x - 1, y + 1, bladB * 3.0/16.0);
            distributeError(robB, x,     y + 1, bladB * 5.0/16.0);
            distributeError(robB, x + 1, y + 1, bladB * 1.0/16.0);
        }
    }

    delete[] robR;
    delete[] robG;
    delete[] robB;
}

float clamping(float v) {
    if (v < 0) return 0;
    if (v > 255) return 255;
    return v;
}
void distributeError(float* tab, int x, int y, float blad) {
    if (x >= 0 && x < szerokosc/2 && y >= 0 && y < wysokosc/2) {
        tab[y * (szerokosc/2) + x] += blad;
    }
}
