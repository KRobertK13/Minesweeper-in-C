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
//A kapott adatok mellé bekér egy nevet amely legfeljebb 11 karakter hosszú. Majd kiírja fájlba az eredményt. Támaszkodik a nev_bekero és a kiiras függvényekre.
bool Leaderboard_kezelo(SDL_Renderer * renderer, LboardAdat * lba, int index, int szam){
    if(lba[index].ido > szam){
        nev_bekero("Adja meg a nevet!", renderer, 11, &lba[index].nev);
        lba[index].ido = szam;
        kiiras(lba);
        return true;
    }
    return false;
}
//Frissíti a leaderboard fájlt a kapott LboardAdat tömb alapján
void kiiras(LboardAdat * lba){
    FILE* fp;
    fp = fopen("leaderboard.txt", "wt");
    for(int i = 0; i< 3; i++){
        fprintf(fp,"%s\n",lba[i].nev);
        fprintf (fp, "%d\n", lba[i].ido);
    }
    fclose(fp);
}
//Beolvassa a leaderboard fájlt a kapott LboardAdat tömb alapján egy LboardAdat típusú dinamikusan foglalt struktúra tömbbe. Visszatérési értéke az erre mutató pointer, így free-elni is kell.
LboardAdat * beolvasas(){
    LboardAdat * lba = (LboardAdat*)malloc(sizeof(LboardAdat)*3);
    FILE* fp;
    fp = fopen("leaderboard.txt", "r");
    if(fp == NULL) return NULL;
    for(int i = 0; i< 3; i++){
        fscanf(fp,"%s",&lba[i].nev);
        fscanf (fp, "%d", &lba[i].ido);
    }
    fclose(fp);
    return lba;
}
