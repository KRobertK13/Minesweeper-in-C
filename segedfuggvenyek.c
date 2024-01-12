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
//A kattint�s x ponja alapj�n a p�lya x koordin�t�j�t sz�m�tja ki.
int Xkoordinata(int x){
    x -= 30;
    return x / 30;
}
//A kattint�s y ponja alapj�n a p�lya y koordin�t�j�t sz�m�tja ki.
int Ykoordinata(int y){
    y -= 120;
    return y / 30;
}
//Megsz�molja a lerakott z�szl�k sz�m�t a p�ly�n.
int zaszloszamlalo(Palya palya, int** feloldva){
    int zaszlok_szama = 0;
    for(int i = 0; i < palya.magas; i++){
        for(int j = 0; j < palya.szeles; j++)
            if( feloldva[i][j] == 1)
                zaszlok_szama++;
    }
    return zaszlok_szama;
}
//Megn�zi, hogy az �sszes mez� fel van e oldva, amely nem akna. Gyakorlatilag megn�zi, hogy nyert�nk-e.
bool osszes_feloldva(Palya p, int feloldva) {
    return feloldva == p.magas * p.szeles - p.aknaSz;
}
//Megvizsg�lja, hogy a kattint�s a p�lya ter�let�t �rte-e.
bool palyan_van(int x, int y, Palya palya) {
    return (x >= 30 && x < palya.szeles * 30 + 30 && y >= 120 && y < 120 + palya.magas * 30);
}
//Egy pointert k�ld vissza, teh�t figyelni kell a mem�ria kezel�sre. �tkonvert�lja el�jeles sztring� a kapott sz�mot. Ez kell a sz�ml�l�nak, hogy ki�rja a sz�ml�l�s aktu�lis �rt�k�t.
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
//Egy kapott sztringb�l k�sz�t sz�mot. 0 megy vissza, ha a form�tum hib�s.
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
