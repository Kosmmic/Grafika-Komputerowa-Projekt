# Format DG8 - design (etap4_1306)

Źródło: `GK2026-Projekt4b (1).pdf` (Specyfikacja DG8).

## Cel

Pełna implementacja zapisu/odczytu pliku `.dg8` (5 trybów koloru + dithering),
operująca na obrazie z lewej-górnej ćwiartki ekranu (320x200, `getPixel`/`setPixel`).

## Nagłówek (12 bajtów, zawsze)

| Pole         | Offset | Rozmiar | Uwagi |
|--------------|--------|---------|-------|
| Identyfikator| 0      | 2B      | 'D','G' |
| Szerokość    | 2      | 2B (Uint16) | |
| Wysokość     | 4      | 2B (Uint16) | |
| Tryb         | 6      | 1B (Uint8)  | 1-5 |
| Dithering    | 7      | 1B (Uint8)  | 0=brak,1=Bayer4x4,2=Floyd-Steinberg |
| Rozmiar danych| 8     | 4B (Uint32) | = szerokość*wysokość (1B/piksel) |

Tabela offsetów w PDF ("08 lub 768+08" dla rozmiaru danych) jest niezgodna z
przykładami liczbowymi na str. 8 (paleta zawsze od offsetu 12, dane od offsetu
12 albo 780 = 12+768). Przyjmujemy nagłówek **zawsze 12B**, a dla trybów 3/4/5
zaraz po nim **paleta 768B** (256 x RGB), potem dane pikseli.

## Tryby

1. **Paleta narzucona (kolor)** - 1B/piksel, format `RRRGGGBB`. Brak palety w pliku.
   Konwersja R/G: 3-bitowa wartość 0-7 -> {0,36,73,109,146,182,218,255}.
   Konwersja B: 2-bitowa wartość 0-3 -> {0,85,170,255}.
   Forward (RGB->3/3/2 bit): `round(skladowa * maxN / 255)`.
2. **Skala szarości narzucona** - 1B/piksel = luminancja `round(0.299R+0.587G+0.114B)`,
   0-255 wprost. Brak palety w pliku.
3. **Skala szarości dedykowana** - paleta 768B (256 wpisów RGB, r=g=b) wygenerowana
   MedianCut (256 kubełków, 8 poziomów podziału) na luminancjach obrazu. 1B/piksel
   = indeks do palety.
4. **Paleta dedykowana (kolor)** - paleta 768B wygenerowana MedianCut (256 kubełków,
   8 poziomów podziału) na kolorach RGB obrazu. 1B/piksel = indeks.
5. **Paleta wykryta** - jeśli obraz ma <=256 unikalnych kolorów RGB: paleta 768B
   = rzeczywiste kolory obrazu, 1B/piksel = indeks (obraz bez zmian). Jeśli obraz
   ma >256 unikalnych kolorów: fallback do trybu 4 (nagłówek zapisuje tryb=4).

## Dithering (pole w nagłówku, 0/1/2)

Stosowany jako preprocessing przed kwantyzacją do palety/wartości danego trybu.
Nowe, samodzielne funkcje operujące na dowolnej palecie (tablica `SDL_Color*` +
rozmiar) i dowolnej liczbie poziomów (256 dla trybu 1/2, rozmiar palety dla 3/4/5).
Nie modyfikujemy istniejących `Bayer`/`BayerKolor`/`FloydSteinbergBW`/`FloydSteinbergColor`
(używanych przez Funkcja5-8) - ryzyko regresji już ocenionej funkcjonalności.

- Bayer 4x4: próg z `bayer4x4[x%4][y%4]` dodawany do wartości/składowej przed
  wyszukaniem najbliższego koloru w palecie.
- Floyd-Steinberg: sekwencyjna dyfuzja błędu (jak istniejące FS), ale
  `znajdzNajblizszy` generalizowany do dowolnej palety/rozmiaru.

## Kolejność danych pikseli

Blokami po 8 pikseli, kolumnowo (str. 9 PDF): dla każdej kolumny x=0..w-1,
dla każdego pasma wierszy y0=0,8,16,..., zapisywane są piksele
(x,y0)...(x,y0+7). Obraz 320x200 - wysokość podzielna przez 8, brak paddingu.

## Nowe API

`GK2026-Pliki.h/.cpp`:
```cpp
void zapiszDG8(int tryb, int dithering, const char* nazwa);
void odczytajDG8(const char* nazwa); // czyta tryb z nagłówka, dispatch po trybie, rysuje wynik
```

`GK2026-Paleta.h/.cpp`:
```cpp
Uint8 z24KdoNarzucona8(SDL_Color kolor); // RRRGGGBB wg tabel powyżej
SDL_Color zNarzucona8do24K(Uint8 v);
Uint8 luminancja(SDL_Color kolor); // 0-255, do trybu 2 i baz dla 3
```

`GK2026-MedianCut.h/.cpp`:
```cpp
void paletaMedianCut256BW(SDL_Color paleta[256]);   // luminancje obrazu
void paletaMedianCut256Color(SDL_Color paleta[256]); // RGB obrazu
int znajdzNajblizszyW Palecie(SDL_Color kolor, SDL_Color paleta[], int rozmiar);
```
(nowe funkcje, 8 poziomów rekursji, niezależne od istniejących 16-kolorowych
wersji używanych w Funkcja7/8)

Dithering (nowy plik lub w MedianCut.cpp):
```cpp
void ditherBayerDoPalety(SDL_Color paleta[], int rozmiar, Uint8* wynikIndeksy);
void ditherFSDoPalety(SDL_Color paleta[], int rozmiar, Uint8* wynikIndeksy);
```

## Integracja UI

`GK2026-Funkcje.cpp`: nowa `Funkcja10()` - test zapisu/odczytu DG8:
zapisuje obraz z lewej-górnej ćwiartki w trybie+ditheringu wskazanym przez
globalne zmienne `trybDG8` (1-5) i `ditheringDG8` (0-2), do `obraz.dg8`,
następnie `odczytajDG8` rysuje wynik w prawej-dolnej ćwiartce.

`GK2026-Projekt.cpp`: klawisz `0` -> `Funkcja10()`. Klawisze do zmiany
`trybDG8`/`ditheringDG8` (np. `[`/`]` tryb, `-`/`=` dithering) z wypisaniem
aktualnych wartości na konsolę.

## Pliki dotknięte

- `GK2026-Zmienne.h/.cpp` - nowe globalne `trybDG8`, `ditheringDG8`
- `GK2026-Paleta.h/.cpp` - konwersje 8-bit narzucone + luminancja
- `GK2026-MedianCut.h/.cpp` - 256-kolorowy MedianCut (BW/Color) + nearest w
  dowolnej palecie + dithering do dowolnej palety
- `GK2026-Pliki.h/.cpp` - `zapiszDG8`/`odczytajDG8` + helper kolejności bloków 8px
- `GK2026-Funkcje.h/.cpp` - `Funkcja10`
- `GK2026-Projekt.cpp` - klawisze
