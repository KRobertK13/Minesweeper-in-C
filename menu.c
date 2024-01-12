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
//a játék menüt mutatja hívásra.
void jatek_menu_general(SDL_Renderer* renderer, int palyakod, SDL_Texture** texturak) {
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    char feliratok[7][13] = { "New game", "Beginner", "Intermediate", "Expert", "Custom", "Best times", "Exit" };
    for (int i = 0; i < 7; i++)
        felirat_general(feliratok[i], 22, 50, 50 + i * 40, renderer, false);
    SDL_Rect celterulet = { 250, 90+palyakod*40, 30, 30 };
    SDL_RenderCopy(renderer, texturak[18], NULL, &celterulet);
    SDL_RenderPresent(renderer);
}
//A help menüt mutatja meg hívásra
void help_ful_general(SDL_Renderer* renderer){
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    felirat_general("Minesweeper overview", 20, 10, 10, renderer, false);
    felirat_general("-Locate all the mines as quickly as possible", 16, 20,50, renderer, false);
    felirat_general("without uncovering any of them.", 16, 28,70, renderer, false);
    felirat_general("-If you uncover a mine, you lose the game.", 16, 20,95, renderer, false);
    felirat_general("About Minesweeper", 20, 10, 135, renderer, false);
    felirat_general("Version 1.0", 16, 20, 175, renderer, false);
    felirat_general("Made by Kovacs Robert Kristof", 16, 20, 195, renderer, false);
    SDL_RenderPresent(renderer);
}
//hívásra megmutatja a leaderboardot.
void Lboard_mutato(SDL_Renderer * renderer, LboardAdat * lba){
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    char stringek[3][13] = {"Beginner","Intermediate","Expert"};
    felirat_general("Leaderboard", 22, 10,10, renderer, false);
    for(int i = 0; i < 3; i++){
        felirat_general(stringek[i], 18, 50,50+i*75, renderer, false);
        felirat_general(lba[i].nev, 18, 70,75+i*75, renderer, false);
        char * seged = Konvertal_sztringge(lba[i].ido);
        felirat_general(seged, 18, 70,100+i*75, renderer, false);
        free(seged);
        SDL_RenderPresent(renderer);
    }
    SDL_RenderPresent(renderer);
}
//Egy paraméterként kapott char tömbbe betölti a 3. paraméter hosszúságú sztringet. A felületet teljesen felülírja. Az input_text függvényre támaszkodik.
void nev_bekero(char felirat[],SDL_Renderer * renderer, int hossz, char beirando[]){
    SDL_SetRenderDrawColor(renderer, 192, 192, 192, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    felirat_general(felirat, 22, 10, 10, renderer, true);
    TTF_Font* font = TTF_OpenFont("segoeui.ttf", 18);
    char nyers[hossz];
    SDL_Color feher = {255, 255, 255, 255}, fekete = { 0, 0, 0 };
    SDL_Rect r = {50,50,120,30};
    if(input_text(nyers, hossz, r, feher, fekete, font, renderer)){

            int index =0;
            while(index < hossz-1){
                beirando[index] = nyers[index];
                index++;
            }
            beirando[hossz-1] = '\0';
    }
    TTF_CloseFont(font);
}
//Alsó és felsõ határ közötti int értékek beolvasására használatos. Az input_text függvényre támaszkodik. A felületet teljesen felülírja.
int egyedi_adat_bekero(char felirat[],SDL_Renderer * renderer, int hossz, int also, int felso){
    SDL_SetRenderDrawColor(renderer, 192, 192, 192, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    felirat_general(felirat, 22, 10, 10, renderer, true);
    TTF_Font* font = TTF_OpenFont("segoeui.ttf", 18);
    char nyers[hossz];
    SDL_Color feher = {255, 255, 255, 255}, fekete = { 0, 0, 0 };
    SDL_Rect r = {50,50,50,30};
    int bejott = 0;
    while(bejott > felso || bejott < also){
        if(input_text(nyers, hossz, r, feher, fekete, font, renderer)){
            nyers[hossz-1] = '\0';
            bejott = Konvertal_egessze(nyers);
        }
    }
    TTF_CloseFont(font);
    return bejott;
}
/* Beolvas egy szoveget a billentyuzetrol.
 * A rajzolashoz hasznalt font es a megjelenito az utolso parameterek.
 * Az elso a tomb, ahova a beolvasott szoveg kerul.
 * A masodik a maximális hossz, ami beolvasható.
 * A visszateresi erteke logikai igaz, ha sikerult a beolvasas. */
bool input_text(char *dest, size_t hossz, SDL_Rect teglalap, SDL_Color hatter, SDL_Color szoveg, TTF_Font *font, SDL_Renderer *renderer) {
    /* Ez tartalmazza az aktualis szerkesztest */
    char composition[SDL_TEXTEDITINGEVENT_TEXT_SIZE];
    composition[0] = '\0';
    /* Ezt a kirajzolas kozben hasznaljuk */
    char textandcomposition[hossz + SDL_TEXTEDITINGEVENT_TEXT_SIZE + 1];
    /* Max hasznalhato szelesseg */
    int maxw = teglalap.w - 2;
    int maxh = teglalap.h - 2;

    dest[0] = '\0';

    bool enter = false;
    bool kilep = false;

    SDL_StartTextInput();
    while (!kilep && !enter) {
        /* doboz kirajzolasa */
        boxRGBA(renderer, teglalap.x, teglalap.y, teglalap.x + teglalap.w - 1, teglalap.y + teglalap.h - 1, hatter.r, hatter.g, hatter.b, 255);
        rectangleRGBA(renderer, teglalap.x, teglalap.y, teglalap.x + teglalap.w - 1, teglalap.y + teglalap.h - 1, szoveg.r, szoveg.g, szoveg.b, 255);
        /* szoveg kirajzolasa */
        int w;
        strcpy(textandcomposition, dest);
        strcat(textandcomposition, composition);
        if (textandcomposition[0] != '\0') {
            SDL_Surface *felirat = TTF_RenderUTF8_Blended(font, textandcomposition, szoveg);
            SDL_Texture *felirat_t = SDL_CreateTextureFromSurface(renderer, felirat);
            SDL_Rect cel = { teglalap.x, teglalap.y, felirat->w < maxw ? felirat->w : maxw, felirat->h < maxh ? felirat->h : maxh };
            SDL_RenderCopy(renderer, felirat_t, NULL, &cel);
            SDL_FreeSurface(felirat);
            SDL_DestroyTexture(felirat_t);
            w = cel.w;
        } else {
            w = 0;
        }
        /* kurzor kirajzolasa */
        if (w < maxw) {
            vlineRGBA(renderer, teglalap.x + w + 2, teglalap.y + 2, teglalap.y + teglalap.h - 3, szoveg.r, szoveg.g, szoveg.b, 192);
        }
        /* megjeleniti a képernyon az eddig rajzoltakat */
        SDL_RenderPresent(renderer);

        SDL_Event event;
        SDL_WaitEvent(&event);
        switch (event.type) {
            /* Kulonleges karakter */
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_BACKSPACE) {
                    int textlen = strlen(dest);
                    do {
                        if (textlen == 0) {
                            break;
                        }
                        if ((dest[textlen-1] & 0x80) == 0x00)   {
                            /* Egy bajt */
                            dest[textlen-1] = 0x00;
                            break;
                        }
                        if ((dest[textlen-1] & 0xC0) == 0x80) {
                            /* Bajt, egy tobb-bajtos szekvenciabol */
                            dest[textlen-1] = 0x00;
                            textlen--;
                        }
                        if ((dest[textlen-1] & 0xC0) == 0xC0) {
                            /* Egy tobb-bajtos szekvencia elso bajtja */
                            dest[textlen-1] = 0x00;
                            break;
                        }
                    } while(true);
                }
                if (event.key.keysym.sym == SDLK_RETURN) {
                    enter = true;
                }
                break;

            /* A feldolgozott szoveg bemenete */
            case SDL_TEXTINPUT:
                if (strlen(dest) + strlen(event.text.text) < hossz) {
                    strcat(dest, event.text.text);
                }

                /* Az eddigi szerkesztes torolheto */
                composition[0] = '\0';
                break;

            /* Szoveg szerkesztese */
            case SDL_TEXTEDITING:
                strcpy(composition, event.edit.text);
                break;
        }
    }

    /* igaz jelzi a helyes beolvasast; = ha enter miatt allt meg a ciklus */
    SDL_StopTextInput();
    return enter;
}
