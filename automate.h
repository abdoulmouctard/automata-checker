#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include "structures.h"

#define SIZE_BUFF 1024
extern char print[SIZE_BUFF];

automate_s* charger_automate(char * file_path);
void connect(automate_s* m, int e1, int e2,char c);
int read_value(char c);
void load_trans(automate_s* m, int n);
int load_etats(automate_s* machine);
void myperror();
bool appartenance(automate_s *m ,mot_s* w, char*);
void free_etat(etat_s *e);
void free_automate(automate_s* m);

bool langage_vide(automate_s *m);
bool langage_infini(automate_s* m);
void status_langage_infini(automate_s* m);

void affiche(automate_s* m, char* t);
void print_and_clean(char* p,int n);
void status_appartenance(bool matched, mot_s *mot);
void status_langage_vide(automate_s* m);
int menu();
bool again(bool *b);
mot_s* input(char *s, mot_s* m);
bool cherche_init_final(automate_s* m);
bool step(automate_s* m,int e,mot_s *w, char *t);
bool est_dans_compsante_forttement_connexe(automate_s *m,int* tab, int e, int n);
mot_s* mot_suiv(mot_s * w);
bool apaprtient_tab(int *t, int n, int max);
bool chemin_vers_final(automate_s* m,int **t, int e);
char read_char(char c);
void free_mot(mot_s *w);