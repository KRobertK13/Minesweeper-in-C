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
//Az időzítő függvénye. Ez végzi az óra léptetését.
Uint32 idozit(Uint32 ms, void * oraadat){
    OraAdat * oa = (OraAdat*)oraadat;
    if(oa->tickszam < 999) oa->tickszam += 1;
    char *seged = Konvertal_sztringge(oa->tickszam);
    felirat_general(seged, 22, oa->palyaszel-75, 30, oa->rend, true);
    free(seged);
    return ms;
}
//Ahogy a nevében is benne van, felfesti az új pályát és elvégzi a beállításokat.
void uj_jatek_general(SDL_Window* window, SDL_Renderer * renderer, Palya palya, SDL_Texture ** texturak, int *** palya_terkep, int *** feloldva ){
        //beállítja az ablakot a játékhoz
        SDL_SetRenderDrawColor(renderer, 192, 192, 192, 255);
        SDL_RenderClear(renderer);
        SDL_SetWindowSize(window, palya.szeles*30+60, palya.magas*30+150);
        //menü felrajzolás
        felirat_general("Game", 18, 10, 0, renderer, false);
        felirat_general("Help", 18, 60, 0, renderer, false);
        //mûszerfal felrajzolás
        char *seged = Konvertal_sztringge(palya.aknaSz);
        felirat_general(seged, 22, 30, 30, renderer, true);
        free(seged);
        SDL_Rect ct = { (palya.szeles*30+60) / 2 - 25, 30, 50, 50 };
        SDL_RenderCopy(renderer, texturak[14], NULL, &ct);
        felirat_general("0", 22, palya.szeles*30+60-75, 30, renderer, true);
        //új dinamikus pályaadatok generálása és pályarajzolás
        *palya_terkep = palya_general(palya);
        *feloldva = feloldas_terkep_genereal(palya);
        palya_rajzolo(palya, *palya_terkep, renderer, texturak[11]);
        SDL_RenderPresent(renderer);
}

//Lekezeli a játék közben elsütött jobb klikkeket.
void jobb_klikk_esemenykezelo(int x, int y, SDL_Renderer* renderer, int** feloldva, Palya palya, SDL_Texture** texturak) {
    if (x >= 30 && x <= palya.szeles * 30 + 30 && y >= 120 && y <= 120 + palya.magas * 30) { // ha a pálya területére kattintunk
        x = Xkoordinata(x);
        y = Ykoordinata(y);
        SDL_Rect celterulet = { 30 + x * 30, 120 + y * 30, 30, 30 };
        if (feloldva[y][x] < 2) {
            feloldva[y][x]++;
            SDL_RenderCopy(renderer, texturak[11+feloldva[y][x]], NULL, &celterulet);
            char *seged = Konvertal_sztringge(palya.aknaSz-zaszloszamlalo(palya,feloldva));
            felirat_general(seged, 22, 30, 30, renderer, true);
            free(seged);
        }
        else if (feloldva[y][x] == 2) {
            SDL_RenderCopy(renderer, texturak[11], NULL, &celterulet);
            feloldva[y][x] = 0;
        }
        SDL_RenderPresent(renderer);
    }
}

//A pályát érő bal klikkeket kezeli, tehát a mezőfelfedés a feladata. Ő hívja meg a felfedo rekurzív függvényt is. A visszatérési értéke a feloldott fezők száma. Amennyiben aknára kattintunk -1-el tér vissza.
int palyan_bal_klikk(int x, int y, SDL_Renderer* renderer, int** feloldva, int** palya_terkep, Palya palya, SDL_Texture** texturak) {
    x = Xkoordinata(x);
    y = Ykoordinata(y);
    int szam = 0;
    if ((feloldva[y][x] == 0 || feloldva[y][x] == 2)) {
        SDL_Rect celterulet = { 30 + x * 30, 120 + y * 30, 30, 30 };
        if (palya_terkep[y][x] == 0) //ha nincs a környezetben akna
           szam = felfedo(palya_terkep, feloldva, renderer, texturak, palya.magas - 1, palya.szeles - 1, x, y);
        else if (palya_terkep[y][x] == 9) {//ha akna
            SDL_RenderCopy(renderer, texturak[9], NULL, &celterulet);
            for (int i = 0; i < palya.szeles; i++) {
                for (int j = 0; j < palya.magas; j++) {
                    if (palya_terkep[j][i] == 9) {
                        SDL_Rect celterulet = { 30 + i * 30, 120 + j * 30, 30, 30 };
                        SDL_RenderCopy(renderer, texturak[10], NULL, &celterulet);
                    }
                }
            }
            SDL_RenderCopy(renderer, texturak[9], NULL, &celterulet);
            szam = -1;
        }
       else {//ha nem különleges eset
            SDL_RenderCopy(renderer, texturak[palya_terkep[y][x]], NULL, &celterulet);
            feloldva[y][x] = 3;
            szam = 1;
        }
        SDL_RenderPresent(renderer);
    }
    return szam;
}
//kapott pozícióba, kapott méretű, sztringet ír. Utolsó paramétere megkérdi, hogy a hátteret fedje-e le. Ez akkor fontos, ha egy másik felirat pozíciójába szeretnénk írni.
void felirat_general(char text[], int fontsize, int posX, int posY, SDL_Renderer* renderer, bool shaded) {
    SDL_Color fekete = { 0, 0, 0,255 };
    SDL_Surface* felirat;
    SDL_Texture* felirat_t;
    TTF_Init();
    TTF_Font* font = TTF_OpenFont("segoeui.ttf", fontsize);
    felirat = TTF_RenderUTF8_Solid(font, text, fekete);
    //mivel szöveg felülírásra használom, így a különbözõ szóhosszok miatt nem lenne minden esetben teljes átfedeés a TTF...Shaded-del dolgozva a szövegek közt. Erre a boxRGBA nyújt megoldást
    if(shaded) boxRGBA(renderer, posX, posY, posX+100, posY+fontsize+5, 192, 192, 192, 255);
    felirat_t = SDL_CreateTextureFromSurface(renderer, felirat);
    SDL_Rect hova = { posX, posY, felirat->w, felirat->h };
    SDL_RenderCopy(renderer, felirat_t, NULL, &hova);
    SDL_FreeSurface(felirat);
    SDL_DestroyTexture(felirat_t);
    TTF_CloseFont(font);
    SDL_RenderPresent(renderer);
}

//rekurziv algoritmus amelyet akkor hívunk meg, ha üres mezõre kattintunk, azaz ahol a környezõ aknák száma 0.
//A metódus felfedi a lehetõ legnagyobb egybefüggõ területet a kattintott block környezetében, amit n>0 értékû mezõk határolnak.
//A metódus figyel arra, hogy a már felfedett mezõkön ne indítson vizsgálatot és ne futson ki a pályahatárokon kívülre.
int felfedo(int** terkep, int** felfedve, SDL_Renderer* renderer, SDL_Texture** tex, int mh, int szh, int x, int y) {
    int szam = 1;
    SDL_Rect celterulet = { 30 + x * 30, 120 + y * 30, 30, 30 };
    SDL_RenderCopy(renderer, tex[terkep[y][x]], NULL, &celterulet);
    SDL_RenderPresent(renderer);
    felfedve[y][x] = 3;
    if (terkep[y][x] == 0) {
        if (y > 0)
            if (felfedve[y - 1][x] < 3)
                szam += felfedo(terkep, felfedve, renderer, tex, mh, szh, x , y-1);
        if (x > 0)
            if (felfedve[y][x - 1] < 3)
                szam += felfedo(terkep, felfedve, renderer, tex, mh, szh, x-1, y);
        if (y < mh)
            if (felfedve[y + 1][x] < 3)
                szam += felfedo(terkep, felfedve, renderer, tex, mh, szh, x, y+1);
        if (x < szh)
            if (felfedve[y][x + 1] < 3)
                szam += felfedo(terkep, felfedve, renderer, tex, mh, szh, x+1, y);
        if (x > 0 && y > 0)
            if (felfedve[y - 1][x - 1] < 3)
                szam += felfedo(terkep, felfedve, renderer, tex, mh, szh, x - 1, y - 1);
        if (x < szh && y > 0)
            if (felfedve[y - 1][x + 1] < 3)
                szam += felfedo(terkep, felfedve, renderer, tex, mh, szh, x + 1, y - 1);
        if (x > 0 && y < mh)
            if (felfedve[y + 1][x - 1] < 3)
                szam += felfedo(terkep, felfedve, renderer, tex, mh, szh, x - 1, y + 1);
        if (x < szh && y < mh)
            if (felfedve[y + 1][x + 1] < 3)
                szam += felfedo(terkep, felfedve, renderer, tex, mh, szh, x + 1, y + 1);
    }
    return szam;
}
