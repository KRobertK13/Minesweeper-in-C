#ifndef ESEMENYVEZERLES_H_INCLUDED
#define ESEMENYVEZERLES_H_INCLUDED
typedef struct OraAdat {
    SDL_Renderer * rend;
    int palyaszel;
    int tickszam;
}OraAdat;
int palyan_bal_klikk(int x, int y, SDL_Renderer* renderer, int** feloldva, int** palya_terkep, Palya palya, SDL_Texture** texturak);
int felfedo(int** terkep, int** felfedve, SDL_Renderer* renderer, SDL_Texture** tex, int mh, int szh, int x, int y);
void jobb_klikk_esemenykezelo(int x, int y, SDL_Renderer* renderer, int** feloldva, Palya palya, SDL_Texture** texturak);
void felirat_general(char text[], int fontsize, int posX, int posY, SDL_Renderer* renderer, bool shaded);
void uj_jatek_general(SDL_Window* window, SDL_Renderer * renderer, Palya palya, SDL_Texture ** texturak, int *** palya_terkep, int *** feloldva );
Uint32 idozit(Uint32 ms, void * oraadat);

#endif // ESEMENYVEZERLES_H_INCLUDED
