#include <stdbool.h>

typedef struct transition transition_s;

typedef struct etat
{
    transition_s *list_transitions;
    int nb_trans;

    bool initial;
    bool final; 
    
    int current;
} etat_s;

struct transition
{
    int dest;
    char etiquette;    
};

typedef struct automate
{
    etat_s * etats;
    int nb_etats;
    
    int * etats_initiaux;
    int nb_init;
    
    int * etats_finaux;
    int nb_final;
    
    int courant;
}automate_s;

typedef struct mot
{
    char *string;
    int str_len;
}mot_s;
