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
/* ablak letrehozasa */
void sdl_init(char const* felirat, int szeles, int magas, SDL_Window** pwindow, SDL_Renderer** prenderer) {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        SDL_Log("Nem indithato az SDL: %s", SDL_GetError());
        exit(1);
    }
    SDL_Window* window = SDL_CreateWindow(felirat, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, szeles, magas, 0);
    if (window == NULL) {
        SDL_Log("Nem hozhato letre az ablak: %s", SDL_GetError());
        exit(1);
    }
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (renderer == NULL) {
        SDL_Log("Nem hozhato letre a megjelenito: %s", SDL_GetError());
        exit(1);
    }
    SDL_RenderClear(renderer);

    *pwindow = window;
    *prenderer = renderer;
}

typedef struct EgerInfo {
    int x, y;
    bool elozo;
}EgerInfo;
typedef struct PalyaKezelo{
    int** palya_terkep, **feloldva;
    bool vege, elso;
    int feloldasok_szama,  mode;
} PalyaKezelo;
void tisztitas(PalyaKezelo * pk, OraAdat * oa, int magas);
//Az eseményvezérlés központja. Felelős a smily gomb vezérléséért, a menüablakok nyitásáért zárásáért. Valamint biztosítja a helyes memóriakezelést.
int main(int argc, char* argv[]) {
    /* ablak letrehozasa és kezdeti beállítások */
    SDL_Window* window;
    SDL_Renderer* renderer;
    sdl_init("Aknakereső", 330, 420, &window, &renderer);
    SDL_SetRenderDrawColor(renderer, 192, 192, 192, 255);
    SDL_RenderClear(renderer);
    //A pálya vezérléséhez szükséges változók
    SDL_Texture** texturak = textura_beolvasas(renderer);
    int palyakod = 0;
    bool running = true;
    PalyaKezelo pk;
    pk.elso = true;
    pk.feloldasok_szama = 0;
    pk.vege = false;
    pk.mode = 0;
    Palya palyak[] = { {9,9, 10}, {16,16, 40}, {30,16, 99}, {9,9, 10} };
    SDL_Rect ct = { (palyak[palyakod].szeles*30+60) / 2 - 25, 30, 50, 50 };
     //0- aktív játkék, 1- játék menü, 2- help menü
    LboardAdat * ld = beolvasas(); //Leaderboard beolvasas
    //Eseménykezeléshez kell
    EgerInfo e;
    SDL_Event event;
    //Óra beállítása
    SDL_TimerID id;
    OraAdat  oa = {renderer, 330, 0};
    //Elvégzi a pálya generálását és a változók előkészítését.
    uj_jatek_general(window, renderer, palyak[0], texturak, &pk.palya_terkep, &pk.feloldva);
    //Eseménykezelés
    while (running) {
        while (SDL_PollEvent(&event)){
            switch (event.type)
            {
            case SDL_QUIT:
                running = false;
                break;
            //elmenti a program a gombnyomás információit és a smily gombot vezérli.
            case SDL_MOUSEBUTTONDOWN:
                e.x = event.button.x;
                e.y = event.button.y;
                if (event.button.button == SDL_BUTTON_LEFT){
                    if(palyan_van(event.button.x, event.button.y, palyak[palyakod]) && !pk.vege){
                        SDL_RenderCopy(renderer, texturak[15], NULL, &ct);
                        SDL_RenderPresent(renderer);
                    }
                    e.elozo = false;
                }
                else if (event.button.button == SDL_BUTTON_RIGHT)
                    e.elozo = true;
                break;
            case SDL_MOUSEBUTTONUP:
                //összehasonlítja a felengedést a lenyomás adataival, ha klikk-et állapít meg, akkor lekezeli. Vezérli a smily gombot is.
                if(!pk.vege){
                SDL_RenderCopy(renderer, texturak[14], NULL, &ct);
                SDL_RenderPresent(renderer);
                }
                //megegyezik a lenyomás és a felengedés pozíciója?
                if(e.x == event.button.x && e.y == event.button.y) {
                    if (e.elozo){ //ha jobb klikk
                        if (!pk.vege) jobb_klikk_esemenykezelo(e.x, e.y, renderer, pk.feloldva, palyak[palyakod], texturak);
                    }
                    else if (pk.mode == 0) {
                        if (palyan_van(event.button.x, event.button.y, palyak[palyakod]) && !pk.vege) { //rajtavagyunk a pálya területén és fut még a játék?
                            if(pk.elso){ //amennyiben ez az aktuális játék első kattintása
                                id = SDL_AddTimer(1000, idozit, &oa); //akkor óra indul.
                                pk.elso = false;
                            }
                            //a balklikk eseménykezelője egy int függvény. A szam változó rögzíti a feloldott mezők számát. -1 tér vissza ha aknát oldottunk fel.
                            int szam = palyan_bal_klikk(event.button.x, event.button.y, renderer, pk.feloldva, pk.palya_terkep, palyak[palyakod], texturak);
                            if (szam == -1){ //vesztes jatek
                                SDL_RenderCopy(renderer, texturak[16], NULL, &ct);
                                SDL_RenderPresent(renderer);
                                pk.vege = true;
                                SDL_RemoveTimer(id);
                            }
                            else if(osszes_feloldva(palyak[palyakod], pk.feloldasok_szama + szam)) {//nyertes jatek.
                                SDL_RenderCopy(renderer, texturak[17], NULL, &ct);
                                SDL_RenderPresent(renderer);
                                pk.vege = true;
                                SDL_RemoveTimer(id);
                                if(palyakod < 3){
                                    if(Leaderboard_kezelo(renderer, ld, palyakod, oa.tickszam)){//Amennyiben a játék az eddigi legeredményesebb a szinten.
                                        tisztitas(&pk, &oa, palyak[palyakod].magas);
                                        uj_jatek_general(window,renderer, palyak[palyakod], texturak, &pk.palya_terkep, &pk.feloldva);
                                    }
                                }
                            }
                            else {//ha nincs speciális eset, léptetjük tovább a játékot.
                                pk.feloldasok_szama += szam;
                            }
                        }
                        else if (event.button.x >= (palyak[palyakod].szeles*30+60) / 2 - 25 && event.button.x <=  (palyak[palyakod].szeles*30+60) / 2 + 25 && event.button.y >= 30 && event.button.y <= 80) { //smiley gomb
                            tisztitas(&pk, &oa, palyak[palyakod].magas);
                            uj_jatek_general(window,renderer,palyak[palyakod],texturak, &pk.palya_terkep, &pk.feloldva);
                            SDL_RemoveTimer(id);
                        }
                        else if (event.button.x <= 55 && event.button.y < 50) { //jatek menu
                            SDL_SetWindowSize(window, 330, 420);
                            pk.mode = 1;
                            jatek_menu_general(renderer, palyakod, texturak);
                            pk.vege = true;
                            SDL_RemoveTimer(id);
                        }
                        else if(event.button.x > 55 && event.button.x < 100 && event.button.y < 50){ //Help menu
                            SDL_SetWindowSize(window, 330, 420);
                            help_ful_general(renderer);
                            pk.mode = 2;
                            pk.vege = true;
                            SDL_RemoveTimer(id);
                        }
                    }
                    else if (pk.mode > 0) {//amennyiben nem játékban vagyunk
                        int elozo = palyakod;
                        bool uj_jatek = false;
                        if(pk.mode == 1){//játék menü
                        if (event.button.x >= 50 && event.button.x <= 440) {

                            if (event.button.y >= 50 && event.button.y <= 75) {//új játék
                                    uj_jatek = true;
                            }
                            else if (event.button.y >= 90 && event.button.y < 115) {//első szint
                                palyakod = 0;
                                uj_jatek = true;
                            }
                            else if (event.button.y >= 130 && event.button.y < 155) {//második szint
                                palyakod = 1;
                                uj_jatek = true;
                            }
                            else if (event.button.y >= 170 && event.button.y < 195) {//harmadik szint
                                palyakod = 2;
                                uj_jatek = true;
                            }
                            else if (event.button.y >= 210 && event.button.y < 235) {//Egyedi szint adatbekéréssel
                                palyakod = 3;
                                palyak[palyakod].magas = egyedi_adat_bekero("Magassag (9-24)",renderer, 3, 9, 24);
                                palyak[palyakod].szeles = egyedi_adat_bekero("Szeles (9-30)",renderer, 3, 9, 30);
                                int hatar = (palyak[palyakod].magas*palyak[palyakod].szeles > 300)? 300: palyak[palyakod].magas*palyak[palyakod].szeles-1;
                                palyak[palyakod].aknaSz = egyedi_adat_bekero("Aknak (10-300)",renderer, 4, 10, hatar);
                                uj_jatek = true;
                            }
                            else if (event.button.y >= 250 && event.button.y < 275){// Legjobb eredmények
                                Lboard_mutato(renderer, ld);
                                pk.mode = 2;
                            }
                            else if (event.button.y >= 290 && event.button.y < 315){ //Kilépés a játékból
                                tisztitas(&pk, &oa, palyak[palyakod].magas);
                                for (int i = 0; i < 19; i++)
                                    SDL_DestroyTexture(texturak[i]);
                                    free(texturak);
                                    free(ld);
                                    SDL_RemoveTimer(id);
                                    SDL_Quit();
                                    return 0;
                                }
                            }
                        }
                        else if(pk.mode == 2) uj_jatek = true;//help menü
                        if(uj_jatek){
                                tisztitas(&pk, &oa, palyak[elozo].magas);
                                SDL_Rect ujrect = { (palyak[palyakod].szeles*30+60) / 2 - 25, 30, 50, 50 };
                                ct = ujrect;
                                oa.palyaszel  = palyak[palyakod].szeles*30+60;
                                oa.tickszam =0;
                                uj_jatek_general(window,renderer,palyak[palyakod],texturak, &pk.palya_terkep, &pk.feloldva);
                        }
                    }
                }
                break;
            }
        }
    }
    //dinamikusan foglalt területek felszabadítása és az SDL ablak bezárása.
    tisztitas(&pk, &oa, palyak[palyakod].magas);
    for (int i = 0; i < 19; i++)
        SDL_DestroyTexture(texturak[i]);
    free(texturak);
    free(ld);
    SDL_RemoveTimer(id);
    SDL_Quit();
    return 0;
}

//Alapállapotba helyezi kifejezetten a játékmenethez szükséges változókat.
void tisztitas(PalyaKezelo * pk, OraAdat * oa, int magas){
    for (int i = 0; i < magas; i++) {
        free(pk->palya_terkep[i]);
        free(pk->feloldva[i]);
    }
    free(pk->palya_terkep);
    free(pk->feloldva);
    pk->vege = false;
    pk->feloldasok_szama = 0;
    pk->mode = 0;
    pk->elso = true;
    oa->tickszam =0;
}
