// funkcje do redukcji kolorow i tworzenia palet
#include "GK2026-Paleta.h"
#include "GK2026-Zmienne.h"
#include "GK2026-Funkcje.h"

// 4-bit (2R 1G 1B)
Uint8 z24Kdo4K(SDL_Color kolor) {
    // R: 0-3 (2 bity), G: 0-1 (1 bit), B: 0-1 (1 bit)
    Uint8 r = round(kolor.r * 3.0 / 255.0);
    Uint8 g = round(kolor.g * 1.0 / 255.0);
    Uint8 b = round(kolor.b * 1.0 / 255.0);
    return (r << 2) | (g << 1) | b;
}

SDL_Color z4Kdo24K(Uint8 v) {
    SDL_Color c;
    // Mapowanie z powrotem na 0-255
    c.r = round(((v >> 2) & 0x03) * 255.0 / 3.0);
    c.g = round(((v >> 1) & 0x01) * 255.0 / 1.0);
    c.b = round((v & 0x01) * 255.0 / 1.0);
    c.a = 255;
    return c;
}


Uint8 z24Kdo8K(SDL_Color kolor){
    Uint8 kolor8bit;
    Uint8 R, G, B;
    int nowyR, nowyG, nowyB;

    R = kolor.r;
    G = kolor.g;
    B = kolor.b;
    nowyR = round(R*7.0/255.0);
    nowyG = round(G*7.0/255.0);
    nowyB = round(B*3.0/255.0);

    kolor8bit = nowyR<<5 | nowyG << 2 | nowyB;
    return kolor8bit;
}

SDL_Color z8Kdo24K(Uint8 kolor8bit){

    SDL_Color kolor;
    int R,G,B;
    int nowyR, nowyG, nowyB;

    nowyR = (kolor8bit & 0xE0) >> 5;
    nowyG = (kolor8bit & 0x1C) >> 2;
    nowyB = (kolor8bit & 0x03);

    R = round(nowyR*255.0/7.0);
    G = round(nowyG*255.0/7.0);
    B = round(nowyB*255.0/3.0);

    kolor.r = R;
    kolor.g = G;
    kolor.b = B;
    kolor.a = 255;
    return kolor;
}

// Konwersja na 16 odcieni szaroœci (4 bity)
Uint8 z24Kdo4S(SDL_Color kolor) {
    Uint8 szary = round((0.299 * kolor.r + 0.587 * kolor.g + 0.114 * kolor.b));
    return round(szary * 15.0 / 255.0); // wynik 0-15
}

SDL_Color z4Sdo24K(Uint8 v) {
    SDL_Color c;
    Uint8 szary = round(v * 255.0 / 15.0);
    c.r = c.g = c.b = szary;
    c.a = 255;
    return c;
}

void narzuconaV1() {
    ::ileKolorow = 0;
    SDL_Color kolor;
    int R, G, B;
    int nowyR, nowyG, nowyB;

    for (int y = 0; y < wysokosc / 2; y++) {
        for (int x = 0; x < szerokosc / 2; x++) {
            kolor = getPixel(x, y);

            nowyR = kolor.r >> 5;
            nowyG = kolor.g >> 5;
            nowyB = kolor.b >> 6;

            R = nowyR <<5;
            G = nowyG <<5;
            B = nowyB <<6;
            setPixel(x + szerokosc / 2, y, R, G, B);
        }
    }
}

void narzuconaV2() {
    ::ileKolorow = 0;
    SDL_Color kolor;
    int R, G, B;
    int nowyR, nowyG, nowyB;

    for (int y = 0; y < wysokosc / 2; y++) {
        for (int x = 0; x < szerokosc / 2; x++) {
            kolor = getPixel(x, y);


            nowyR = kolor.r >> 5;
            nowyG = kolor.g >> 5;
            nowyB = kolor.b >> 6;

            R = nowyR * 255.0 / 7.0;
            G = nowyG * 255.0 / 7.0;
            B = nowyB * 255.0 / 3.0;

            setPixel(x, y + wysokosc / 2, R, G, B);
        }
    }
}

void narzuconaV3() {
    ::ileKolorow = 0;
    SDL_Color kolor;
    int R, G, B;
    int nowyR, nowyG, nowyB;

    for (int y = 0; y < wysokosc / 2; y++) {
        for (int x = 0; x < szerokosc / 2; x++) {
            kolor = getPixel(x, y);

            nowyR = round(kolor.r * 7.0 / 255.0);
            nowyG = round(kolor.g * 7.0 / 255.0);
            nowyB = round(kolor.b * 3.0 / 255.0);

            R = nowyR * 255.0 / 7.0;
            G = nowyG * 255.0 / 7.0;
            B = nowyB * 255.0 / 3.0;


            setPixel(x + szerokosc / 2, y + wysokosc / 2, R, G, B);
        }
    }
}

void PaletaNarzucona(bool trybKolorowy){
    ::ileKolorow = 0;
    Uint8 dane4bit;
    SDL_Color kolor, nowyKolor;
     for (int y = 0; y < wysokosc; y++) {
        for (int x = 0; x < szerokosc; x++) {
            kolor = getPixel(x,y);
            if(trybKolorowy){
                dane4bit = z24Kdo4K(kolor);
                nowyKolor = z4Kdo24K(dane4bit);
            }else{
                dane4bit = z24Kdo4S(kolor);
                nowyKolor = z4Sdo24K(dane4bit);
            }

            setPixel(x+szerokosc/2, y+wysokosc/2, nowyKolor.r,nowyKolor.g,nowyKolor.b);
        }
    }
    SDL_UpdateWindowSurface(window);
    cout<<"Paleta narzucona narysowana"<<endl;

}

void narysujPalete(int px, int py, SDL_Color pal8[]){
    int x,y;
    for(int k=0; k<256; k++){
        y = k/32;
        x = k%32;

        for(int xx=0;xx<10;xx++){
            for(int yy=0;yy<10;yy++){
                setPixel(x*10+xx+px, y*10+yy+py, pal8[k].r,pal8[k].g,pal8[k].b);
            }
        }
    }
    cout<<"Paleta narysowana"<<endl;
}
void narysujPalete4Bit(int px, int py, SDL_Color pal[]) {
    for(int k=0; k<16; k++) { // tylko 16 kolorów
        int y = k / 4;
        int x = k % 4;
        for(int xx=0; xx<30; xx++) {
            for(int yy=0; yy<30; yy++) {
                setPixel(x*30+xx+px, y*30+yy+py, pal[k].r, pal[k].g, pal[k].b);
            }
        }
    }
    cout<<"Paleta narysowana"<<endl;
}

bool porownajKolory(SDL_Color kolor1, SDL_Color kolor2){
    if(kolor1.r!=kolor2.r)
        return false;
    if(kolor1.g!=kolor2.g)
        return false;
    if(kolor1.b!=kolor2.b)
        return false;
    return true;
}
int dodajKolor(SDL_Color kolor){
    int aktualnyKolor = ileKolorow;
    paleta8[aktualnyKolor] = kolor;
    if(ileKolorow<256){
        cout<<aktualnyKolor<<": [";
        cout<<(int)kolor.r<<","<<(int)kolor.g<<","<<(int)kolor.b;
        cout<<"]"<<endl;
    }
    ileKolorow++;
    return aktualnyKolor;
}
void czyscPalete(){
    for(int k=0;k<ileKolorow;k++)
        paleta8[k] = {0,0,0};
    ileKolorow = 0;
    for(int k=0;k<256;k++){
        paleta8s[k] = {0,0,0};
        paleta8k[k] = {0,0,0};
    }
}

int sprawdzKolor(SDL_Color kolor){
    if(ileKolorow > 0){
        for(int k=0;k<ileKolorow;k++){
            if(porownajKolory(kolor, paleta8[k]))
                return k;
        }
    }
    return dodajKolor(kolor);
}

void paletaWykryta(){
    czyscPalete();
    ::ileKolorow=0;
    int indexKoloru;
    SDL_Color kolor;

    for (int y = 0; y < wysokosc/2; y++) {
        for (int x = 0; x < szerokosc/2; x++) {
            kolor = getPixel(x, y);
            sprawdzKolor(kolor);
            if (ileKolorow > 16) break;
        }
        if (ileKolorow > 16) break;
    }

    cout << endl <<"ile wykrytych kolorow: "<<ileKolorow<<endl;

    if(ileKolorow <= 16){
        cout<<"Paleta spelnia ograniczenia 4 bit/piksel"<<endl;
        narysujPalete(0,310,paleta8);
        for(int y=0; y<wysokosc/2; y++){
            for(int x=0; x<szerokosc/2; x++){
                kolor = getPixel(x, y);
                int index = sprawdzKolor(kolor);
                SDL_Color kolorZPalety = paleta8[index];

                setPixel(x + szerokosc/2, y + wysokosc/2, kolorZPalety.r, kolorZPalety.g, kolorZPalety.b);
            }
        }
        cout<<"Paleta narysowana"<<endl;
    }
    else{
        cout<<"Paleta przekracza ograniczenia 4bit/piksel"<<endl;
        cout << "Redukuje kolory do narzuconej palety 4-bit (2R 1G 1B)..." << endl;

        for(int y = 0; y < wysokosc/2; y++) {
            for(int x = 0; x < szerokosc/2; x++) {
                kolor = getPixel(x, y);
                Uint8 dane4bit = z24Kdo4K(kolor);
                SDL_Color kolorZPalety = z4Kdo24K(dane4bit);

                setPixel(x + szerokosc/2, y + wysokosc/2, kolorZPalety.r, kolorZPalety.g, kolorZPalety.b);
            }
        }
        SDL_Color paletaWzorcowa[16];
        for(int i = 0; i < 16; i++) {
            paletaWzorcowa[i] = z4Kdo24K(i); // Wykorzystujemy Twoj¹ funkcjê konwertuj¹c¹
        }
        narysujPalete4Bit(0, 310, paletaWzorcowa);
        cout<<"Paleta narysowana"<<endl;
    }

    }

