// funkcje do operacji na plikach
#include "GK2026-Funkcje.h"
#include "GK2026-Zmienne.h"
#include "GK2026-Paleta.h"
#include "GK2026-MedianCut.h"
#include "GK2026-Pliki.h"

using namespace std;

void zapiszPlik() {
	SDL_Color kolor;
	Uint16 szerokoscObrazka = szerokosc / 2;
	Uint16 wysokoscObrazka = wysokosc / 2;
	Uint8 ileBitow = 24;
	char identyfikator[] = "DG";

	cout << "Zapisujemy plik 'obrazRGB.bin' uzywajac metody write()" << endl;
	ofstream wyjscie("obrazRGB.bin", ios::binary);
	wyjscie.write((char*)&identyfikator, sizeof(char) * 2);
	wyjscie.write((char*)&szerokoscObrazka, sizeof(Uint16));
	wyjscie.write((char*)&wysokoscObrazka, sizeof(Uint16));
	wyjscie.write((char*)&ileBitow, sizeof(Uint8));

	for (int y = 0; y < wysokoscObrazka; y++) {
		for (int x = 0; x < szerokoscObrazka; x++) {
			kolor = getPixel(x, y);
			wyjscie.write((char*)&kolor, sizeof(Uint8) * 3);
		}
	}

	wyjscie.close();
	SDL_UpdateWindowSurface(window);
}

void odczytajPlik() {
	SDL_Color kolor;
	Uint16 szerokoscObrazka = 0;
	Uint16 wysokoscObrazka = 0;
	Uint8 ileBitow = 0;
	char identyfikator[3] = "  ";

	cout << "Odczytujemy plik 'obrazRGB.bin' uzywajac metody read()" << endl;
	ifstream wejscie("obrazRGB.bin", ios::binary);

	wejscie.read((char*)&identyfikator, sizeof(char) * 2);
	wejscie.read((char*)&szerokoscObrazka, sizeof(Uint16));
	wejscie.read((char*)&wysokoscObrazka, sizeof(Uint16));
	wejscie.read((char*)&ileBitow, sizeof(Uint8));

	cout << "id: " << identyfikator << endl;
	cout << "szerokosc: " << szerokoscObrazka << endl;
	cout << "wysokosc: " << wysokoscObrazka << endl;
	cout << "ile bitow: " << (int)ileBitow << endl;

	for (int y = 0; y < wysokoscObrazka; y++) {
		for (int x = 0; x < szerokoscObrazka; x++) {
			wejscie.read((char*)&kolor, sizeof(Uint8) * 3);
			setPixel(x + (szerokosc / 2), y, kolor.r, kolor.g, kolor.b);
		}
	}

	SDL_UpdateWindowSurface(window);
}

void zapiszPlik8() {
	SDL_Color kolor;
	Uint8 kolor8bit;
	Uint16 szerokoscObrazka = szerokosc / 2;
	Uint16 wysokoscObrazka = wysokosc / 2;
	Uint8 ileBitow = 8;
	char identyfikator[] = "DG";

	cout << "Zapisujemy plik 'obraz8.bin' uzywajac metody write()" << endl;
	ofstream wyjscie("obraz8.bin", ios::binary);
	wyjscie.write((char*)&identyfikator, sizeof(char) * 2);
	wyjscie.write((char*)&szerokoscObrazka, sizeof(Uint16));
	wyjscie.write((char*)&wysokoscObrazka, sizeof(Uint16));
	wyjscie.write((char*)&ileBitow, sizeof(Uint8));

	for (int y = 0; y < wysokoscObrazka; y++) {
		for (int x = 0; x < szerokoscObrazka; x++) {
			kolor = getPixel(x, y);
			kolor8bit = z24Kdo8K(kolor);
			wyjscie.write((char*)&kolor8bit, sizeof(Uint8));
		}
	}

	wyjscie.close();
	SDL_UpdateWindowSurface(window);
}

void odczytajPlik8() {
	SDL_Color kolor;
	Uint8 kolor8bit = 0;
	Uint16 szerokoscObrazka = 0;
	Uint16 wysokoscObrazka = 0;
	Uint8 ileBitow = 0;
	char identyfikator[3] = "  ";

	cout << "Odczytujemy plik 'obraz8.bin' uzywajac metody read()" << endl;
	ifstream wejscie("obraz8.bin", ios::binary);

	wejscie.read((char*)&identyfikator, sizeof(char) * 2);
	wejscie.read((char*)&szerokoscObrazka, sizeof(Uint16));
	wejscie.read((char*)&wysokoscObrazka, sizeof(Uint16));
	wejscie.read((char*)&ileBitow, sizeof(Uint8));

	cout << "id: " << identyfikator << endl;
	cout << "szerokosc: " << szerokoscObrazka << endl;
	cout << "wysokosc: " << wysokoscObrazka << endl;
	cout << "ile bitow: " << (int)ileBitow << endl;

	for (int y = 0; y < wysokoscObrazka; y++) {
		for (int x = 0; x < szerokoscObrazka; x++) {
			wejscie.read((char*)&kolor8bit, sizeof(Uint8));
			kolor = z8Kdo24K(kolor8bit);
			setPixel(x + (szerokosc / 2), y, kolor.r, kolor.g, kolor.b);
		}
	}

	SDL_UpdateWindowSurface(window);
}

static void pakuj4Bit(Uint8* pixelVals, Uint8* bytes) {
	for (int i = 0; i < 4; i++) bytes[i] = 0;
	for (int i = 0; i < 8; i++) {
		for (int bitPlane = 0; bitPlane < 4; bitPlane++) {
			if (pixelVals[i] & (1 << (3 - bitPlane))) {
				bytes[bitPlane] |= (1 << (7 - i));
			}
		}
	}
}

static void rozpakuj4Bit(Uint8* bytes, Uint8* pixelVals) {
	for (int i = 0; i < 8; i++) pixelVals[i] = 0;
	for (int bitPlane = 0; bitPlane < 4; bitPlane++) {
		for (int i = 0; i < 8; i++) {
			if (bytes[bitPlane] & (1 << (7 - i))) {
				pixelVals[i] |= (1 << (3 - bitPlane));
			}
		}
	}
}

void zapiszPlik4() {
	SDL_Color kolor;
	Uint16 szerokoscObrazka = szerokosc / 2;
	Uint16 wysokoscObrazka = wysokosc / 2;
	Uint8 ileBitow = 4;
	char identyfikator[] = "DG";

	cout << "Zapisujemy plik 'obraz4.bin' uzywajac metody write()" << endl;
	ofstream wyjscie("obraz4.bin", ios::binary);
	wyjscie.write((char*)&identyfikator, sizeof(char) * 2);
	wyjscie.write((char*)&szerokoscObrazka, sizeof(Uint16));
	wyjscie.write((char*)&wysokoscObrazka, sizeof(Uint16));
	wyjscie.write((char*)&ileBitow, sizeof(Uint8));

	int totalPixels = szerokoscObrazka * wysokoscObrazka;
	for (int pixelIdx = 0; pixelIdx < totalPixels; pixelIdx += 8) {
		Uint8 pixelVals[8] = {};
		for (int i = 0; i < 8; i++) {
			int py = (pixelIdx + i) / szerokoscObrazka;
			int px = (pixelIdx + i) % szerokoscObrazka;
			kolor = getPixel(px, py);
			pixelVals[i] = z24Kdo4K(kolor);
		}
		Uint8 bytes[4];
		pakuj4Bit(pixelVals, bytes);
		wyjscie.write((char*)bytes, 4);
	}

	wyjscie.close();
	SDL_UpdateWindowSurface(window);
}

void odczytajPlik4() {
	SDL_Color kolor;
	Uint16 szerokoscObrazka = 0;
	Uint16 wysokoscObrazka = 0;
	Uint8 ileBitow = 0;
	char identyfikator[3] = "  ";

	cout << "Odczytujemy plik 'obraz4.bin' uzywajac metody read()" << endl;
	ifstream wejscie("obraz4.bin", ios::binary);

	wejscie.read((char*)&identyfikator, sizeof(char) * 2);
	wejscie.read((char*)&szerokoscObrazka, sizeof(Uint16));
	wejscie.read((char*)&wysokoscObrazka, sizeof(Uint16));
	wejscie.read((char*)&ileBitow, sizeof(Uint8));

	cout << "id: " << identyfikator << endl;
	cout << "szerokosc: " << szerokoscObrazka << endl;
	cout << "wysokosc: " << wysokoscObrazka << endl;
	cout << "ile bitow: " << (int)ileBitow << endl;

	int totalPixels = szerokoscObrazka * wysokoscObrazka;
	for (int pixelIdx = 0; pixelIdx < totalPixels; pixelIdx += 8) {
		Uint8 bytes[4];
		wejscie.read((char*)bytes, 4);

		Uint8 pixelVals[8];
		rozpakuj4Bit(bytes, pixelVals);

		for (int i = 0; i < 8; i++) {
			int py = (pixelIdx + i) / szerokoscObrazka;
			int px = (pixelIdx + i) % szerokoscObrazka;
			kolor = z4Kdo24K(pixelVals[i]);
			setPixel(px + (szerokosc / 2), py, kolor.r, kolor.g, kolor.b);
		}
	}

	SDL_UpdateWindowSurface(window);
}

void zapiszPlik4Niestandardowy() {
	SDL_Color kolor;
	Uint16 szerokoscObrazka = szerokosc / 2;
	Uint16 wysokoscObrazka = wysokosc / 2;
	Uint8 ileBitow = 4;
	char identyfikator[] = "DG";

	cout << "Zapisujemy plik 'obraz4n.bin' (niestandardowy)" << endl;
	ofstream wyjscie("obraz4n.bin", ios::binary);
	wyjscie.write((char*)&identyfikator, sizeof(char) * 2);
	wyjscie.write((char*)&szerokoscObrazka, sizeof(Uint16));
	wyjscie.write((char*)&wysokoscObrazka, sizeof(Uint16));
	wyjscie.write((char*)&ileBitow, sizeof(Uint8));

	for (int bandRow = 0; bandRow < wysokoscObrazka / 8; bandRow++) {
		for (int x = 0; x < szerokoscObrazka; x++) {
			Uint8 pixelVals[8] = {};
			for (int r = 0; r < 8; r++) {
				kolor = getPixel(x, bandRow * 8 + r);
				pixelVals[r] = z24Kdo4K(kolor);
			}
			Uint8 bytes[4];
			pakuj4Bit(pixelVals, bytes);
			wyjscie.write((char*)bytes, 4);
		}
	}

	wyjscie.close();
	SDL_UpdateWindowSurface(window);
}

void odczytajPlik4Niestandardowy() {
	SDL_Color kolor;
	Uint16 szerokoscObrazka = 0;
	Uint16 wysokoscObrazka = 0;
	Uint8 ileBitow = 0;
	char identyfikator[3] = "  ";

	cout << "Odczytujemy plik 'obraz4n.bin' (niestandardowy)" << endl;
	ifstream wejscie("obraz4n.bin", ios::binary);

	wejscie.read((char*)&identyfikator, sizeof(char) * 2);
	wejscie.read((char*)&szerokoscObrazka, sizeof(Uint16));
	wejscie.read((char*)&wysokoscObrazka, sizeof(Uint16));
	wejscie.read((char*)&ileBitow, sizeof(Uint8));

	cout << "id: " << identyfikator << endl;
	cout << "szerokosc: " << szerokoscObrazka << endl;
	cout << "wysokosc: " << wysokoscObrazka << endl;
	cout << "ile bitow: " << (int)ileBitow << endl;

	for (int bandRow = 0; bandRow < wysokoscObrazka / 8; bandRow++) {
		for (int x = 0; x < szerokoscObrazka; x++) {
			Uint8 bytes[4];
			wejscie.read((char*)bytes, 4);

			Uint8 pixelVals[8];
			rozpakuj4Bit(bytes, pixelVals);

			for (int r = 0; r < 8; r++) {
				kolor = z4Kdo24K(pixelVals[r]);
				setPixel(x + (szerokosc / 2), bandRow * 8 + r, kolor.r, kolor.g, kolor.b);
			}
		}
	}

	SDL_UpdateWindowSurface(window);
}
