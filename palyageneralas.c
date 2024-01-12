#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <math.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "debugmalloc.h"
#include "palyageneralas.h"
#include "esemenyvezerles.h"
#include "menu.h"
#include "segedfuggvenyek.h"
#include "fajlkezeles.h"
//Beolvassa a textúrákat. A fájlszerkezet a palya_terkep alapján indexelhető.
SDL_Texture** textura_beolvasas(SDL_Renderer* renderer) {
    SDL_Texture** texturak = malloc(sizeof(SDL_Texture*) * 19);
    char seged[] = { 'i', 'm', 'g', '\\', '\\','t', ' ',' ', '.', 'p', 'n', 'g','\0' };
    for (int i = 0; i <= 18; i++) {
        seged[6] = (i / 10) + '0';
        seged[7] = (i % 10) + '0';
        SDL_Texture* tex = IMG_LoadTexture(renderer, seged);
        texturak[i] = tex;
        SDL_RenderPresent(renderer);
    }
    return texturak;
}
//dinamikusan foglal a feloldo tombnek és kinullázza az értékeit. Az alkotott mátrix mérete megegyezik a pálya méretével.
//Ha az adott érték nulla, akkor a mezo érintetlen. 1 ha zaszlo, 2 ha kerdojel, 3 ha feloldva.
int** feloldas_terkep_genereal(Palya palya) {
    int** terkep = (int**)malloc(sizeof(int*) * palya.magas);
    if (terkep == NULL) return NULL;
    for (int i = 0; i < palya.magas; i++)
    {
        terkep[i] = (int*)malloc(sizeof(int) * palya.szeles);
        if (terkep[i] == NULL) return NULL;
    }
    for (int i = 0; i < palya.magas; i++)
        for (int j = 0; j < palya.szeles; j++)
            terkep[i][j] = 0;
    return terkep;
}

//felrajzolja a felfedetlen blockokat a kijelzõre
void palya_rajzolo(Palya palya, int** terkep, SDL_Renderer* renderer, SDL_Texture* img) {
    for (int i = 0; i < palya.magas; i++)
        for (int j = 0; j < palya.szeles; j++) {
            SDL_Rect celterulet = { 30 + j * 30, 120 + i * 30, 30, 30 };
            SDL_RenderCopy(renderer, img, NULL, &celterulet);
            SDL_RenderPresent(renderer);
        }
}
//Lefoglalja dinamikusan a helyet a pályának, amelynek méretét megkapja paraméterben.
//Elsõként kinullázza a tartalmát, aztán egy ciklussal elhelyezi random pozíciókba az aknákat.
//Ha egy aknát elhelyez a program, a környezetében lévõ elemek értékét növeli 1-el.
//Tehát végeredményként a visszaküldött mátrix elemei azt a környezetükben lévõ aknák számát fogják jelenteni (0-8). Az akna értéke 9.
int** palya_general(Palya palya) {
    int** terkep = (int**)malloc(sizeof(int*) * palya.magas);
    if (terkep == NULL) return NULL;
    for (int i = 0; i < palya.magas; i++)
    {
        terkep[i] = (int*)malloc(sizeof(int) * palya.szeles);
        if (terkep[i] == NULL) return NULL;
    }
    for (int i = 0; i < palya.magas; i++)
        for (int j = 0; j < palya.szeles; j++)
            terkep[i][j] = 0;
    srand(time(NULL));
    int lerakott_akna = 0;
    int x, y;
    while (lerakott_akna < palya.aknaSz) {
        while (x = rand() % palya.magas, y = rand() % palya.szeles, terkep[x][y] == 9);
        //elhejezzuk az aknat
        terkep[x][y] = 9;
        //noveljuk a kornyezeteben levo elemeket 1-el figyelve arra, hogy ne fussunk ki a matrixbol és ne noveljuk olyan mezo erteket ahol akna van.
        if (x == 0) { //legfelso sor
            if (y == 0) {//bal sarok
                if (terkep[0][1] != 9)
                    terkep[0][1]++;
                for (int k = 0; k < 2; k++)
                    if (terkep[1][k] != 9)
                        terkep[1][k]++;
            }
            else if (y == palya.szeles - 1) { //jobb sarok
                if (terkep[0][y - 1] != 9)
                    terkep[0][y - 1]++;
                for (int k = y - 1; k <= y; k++)
                    if (terkep[1][k] != 9)
                        terkep[1][k]++;
            }
            else { //felso sor tobbi resze
                if (terkep[0][y - 1] != 9)
                    terkep[0][y - 1]++;
                if (terkep[0][y + 1] != 9)
                    terkep[0][y + 1]++;
                for (int k = y - 1; k <= y + 1; k++)
                    if (terkep[1][k] != 9)
                        terkep[1][k]++;
            }
        }
        else if (x == palya.magas - 1) {//legalso sor
            if (y == 0) {//bal sarok
                if (terkep[x][1] != 9)
                    terkep[x][1]++;
                for (int k = 0; k < 2; k++)
                    if (terkep[x - 1][k] != 9)
                        terkep[x - 1][k]++;
            }
            else if (y == palya.szeles - 1) { //jobb sarok
                if (terkep[x][y - 1] != 9)
                    terkep[x][y - 1]++;
                for (int k = y - 1; k <= y; k++)
                    if (terkep[x - 1][k] != 9)
                        terkep[x - 1][k]++;
            }
            else { //also sor tobbi resze
                if (terkep[x][y - 1] != 9)
                    terkep[x][y - 1]++;
                if (terkep[x][y + 1] != 9)
                    terkep[x][y + 1]++;
                for (int k = y - 1; k <= y + 1; k++)
                    if (terkep[x - 1][k] != 9)
                        terkep[x - 1][k]++;
            }
        }
        else if (y == 0) { //bal szel
            if (terkep[x][y + 1] != 9)
                terkep[x][y + 1]++;
            for (int k = y; k <= y + 1; k++)
                if (terkep[x - 1][k] != 9)
                    terkep[x - 1][k]++;
            for (int k = y; k <= y + 1; k++)
                if (terkep[x + 1][k] != 9)
                    terkep[x + 1][k]++;
        }
        else if (y == palya.szeles - 1) { //jobb szel
            if (terkep[x][y - 1] != 9)
                terkep[x][y - 1]++;
            for (int k = y - 1; k <= y; k++)
                if (terkep[x - 1][k] != 9)
                    terkep[x - 1][k]++;
            for (int k = y - 1; k <= y; k++)
                if (terkep[x + 1][k] != 9)
                    terkep[x + 1][k]++;
        }
        else {
            for (int k = y - 1; k <= y + 1; k++)
                if (terkep[x + 1][k] != 9)
                    terkep[x + 1][k]++;
            for (int k = y - 1; k <= y + 1; k++)
                if (terkep[x - 1][k] != 9)
                    terkep[x - 1][k]++;
            if (terkep[x][y - 1] != 9)
                terkep[x][y - 1]++;
            if (terkep[x][y + 1] != 9)
                terkep[x][y + 1]++;
        }
        lerakott_akna++;
    }

    return terkep;
}
