#!/bin/bash
cd "$(dirname "$0")"
g++ -std=c++17 -I/opt/homebrew/include -D_THREAD_SAFE \
    GK2026-Zmienne.cpp GK2026-Paleta.cpp GK2026-Pliki.cpp \
    GK2026-Funkcje.cpp GK2026-MedianCut.cpp GK2026-Projekt.cpp \
    GK2026-DG8Paleta.cpp GK2026-DG8Dither.cpp GK2026-DG8.cpp \
    -o GK_projekt -L/opt/homebrew/lib -lSDL2 2>&1
