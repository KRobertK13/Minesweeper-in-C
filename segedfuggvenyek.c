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
//A kattintás x ponja alapján a pálya x koordinátáját számítja ki.
int Xkoordinata(int x){
    x -= 30;
    return x / 30;
}
//A kattintás y ponja alapján a pálya y koordinátáját számítja ki.
int Ykoordinata(int y){
    y -= 120;
    return y / 30;
}
//Megszámolja a lerakott zászlók számát a pályán.
int zaszloszamlalo(Palya palya, int** feloldva){
    int zaszlok_szama = 0;
    for(int i = 0; i < palya.magas; i++){
        for(int j = 0; j < palya.szeles; j++)
            if( feloldva[i][j] == 1)
                zaszlok_szama++;
    }
    return zaszlok_szama;
}
//Megnézi, hogy az összes mezõ fel van e oldva, amely nem akna. Gyakorlatilag megnézi, hogy nyertünk-e.
bool osszes_feloldva(Palya p, int feloldva) {
    return feloldva == p.magas * p.szeles - p.aknaSz;
}
//Megvizsgálja, hogy a kattintás a pálya területét érte-e.
bool palyan_van(int x, int y, Palya palya) {
    return (x >= 30 && x < palya.szeles * 30 + 30 && y >= 120 && y < 120 + palya.magas * 30);
}
//Egy pointert küld vissza, tehát figyelni kell a memória kezelésre. Átkonvertálja elõjeles sztringé a kapott számot. Ez kell a számlálónak, hogy kiírja a számlálás aktuális értékét.
char * Konvertal_sztringge(int szam){
    int n;
    int s = abs(szam);
    if(s < 10) n = 3;
    else if(s >= 10 && s < 100) n = 4;
    else n =5;
    char* tarolo = malloc(n * sizeof(char));
    int i;
    for (i = n-2; i > 0; --i){
        tarolo[i] = (s % 10) + '0';
        s /= 10;
    }
    tarolo[n-1] = '\0';
    if(szam >= 0) tarolo[0] = ' ';
    else tarolo[0] = '-';
    return tarolo;
}
//Egy kapott sztringbõl készít számot. 0 megy vissza, ha a formátum hibás.
int Konvertal_egessze(char string[]){
    int hossz = 0;
    while(string[hossz] != '\0')
        hossz++;
    int szam = 0;
    int seged = 0;
    int index = 0;
    while(index < hossz){
        seged = string[index]- '0';
        if(seged > 10) return 0;
        else szam += seged * pow((double)(10),(double)(hossz-(index+1)));
        index++;
    }
    return szam;
}
