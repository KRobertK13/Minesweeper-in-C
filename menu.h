#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED
typedef struct LboardAdat{
 char nev[12];
 int ido;
}LboardAdat;

void jatek_menu_general(SDL_Renderer* renderer, int palyakod, SDL_Texture** texturak);
void help_ful_general(SDL_Renderer* renderer);
void Lboard_mutato(SDL_Renderer * renderer, LboardAdat * lba);
int egyedi_adat_bekero(char felirat[],SDL_Renderer * renderer, int hossz, int also, int felso);
void nev_bekero(char felirat[],SDL_Renderer * renderer, int hossz, char beirando[]);
bool input_text(char *dest, size_t hossz, SDL_Rect teglalap, SDL_Color hatter, SDL_Color szoveg, TTF_Font *font, SDL_Renderer *renderer);
#endif // MENU_H_INCLUDED
