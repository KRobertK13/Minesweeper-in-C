#ifndef FAJLKEZELES_H_INCLUDED
#define FAJLKEZELES_H_INCLUDED

LboardAdat * beolvasas();
bool Leaderboard_kezelo(SDL_Renderer * renderer, LboardAdat * lba, int index, int szam);
void kiiras(LboardAdat * lba);

#endif // FAJLKEZELES_H_INCLUDED
