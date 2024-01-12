#ifndef PALYAGENERALAS_H_INCLUDED
#define PALYAGENERALAS_H_INCLUDED
typedef struct Palya {
    int szeles, magas, aknaSz;
} Palya;
void palya_rajzolo(Palya palya, int** terkep, SDL_Renderer * renderer, SDL_Texture* img);
int** palya_general(Palya palya) ;
int** feloldas_terkep_genereal(Palya palya);
SDL_Texture** textura_beolvasas(SDL_Renderer* renderer);
#endif // PALYAGENERALAS_H_INCLUDED
