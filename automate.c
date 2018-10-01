#include "automate.h"

char print[SIZE_BUFF];

char* read_buffer = NULL;

/***************************************************************
 * **************************************************************
 *      LES TROIS PRINCIPALES FONCTIONS DEMANDÉES
 * **************************************************************
 * **************************************************************/

bool appartenance(automate_s *m ,mot_s* w, char * buf)
{
    if(m != NULL && w != NULL)
    {
        bool matched = false;
        int i = 0;
        while(i < m->nb_init && !matched)
        {
            matched = step(m,m->etats_initiaux[i], w,buf);
            write(STDOUT_FILENO,"\n",1);
            i++;
        }
        return matched;
    }
    return false;
}

bool langage_vide(automate_s *m)
{
    if (m == NULL || m->nb_init == 0 || m->nb_final == 0){return true;}

    if (cherche_init_final(m)){return false;}

    int i = 0, j = 0;

    //MATRICE (ADJ)
    int **tab = malloc(m->nb_etats * sizeof(int *));
    for(i = 0;i < m->nb_etats;i++)
    {
        tab[i] = malloc(m->nb_etats * sizeof(int));
        for(int j = 0;j < m->nb_etats;j++) {tab[i][j] = -1;}
    }

    for(i = 0;i < m->nb_etats;i++)
    {
        for(j = 0;j < m->etats[i].nb_trans;j++)
        {
            if (m->etats[i].list_transitions[j].dest != i)
            {
                tab[i][m->etats[i].list_transitions[j].dest] = 1;
            }
        }
    }
    
    bool ret = false;
    for(i = 0;i < m->nb_init && !ret ; i++)
    {
        ret = (chemin_vers_final(m,tab, m->etats_initiaux[i]));
    }

    for(i = 0;i < m->nb_etats;i++){free(tab[i]);}
    free(tab);

    return !ret;
}

bool langage_infini(automate_s* m)
{
    
    if (!langage_vide(m))
    {
        int i,j;

        for(i = 0;i < m->nb_etats;i++)
        {
            for(j = 0;j < m->etats[i].nb_trans;j++)
            {
                if (i == m->etats[i].list_transitions[j].dest){return true;}
            }
        }

        int *tab = malloc(sizeof(int)* m->nb_etats);
        for(i = 0;i < m->nb_etats;i++){tab[i] = -1;}
        
        bool ret = false;
        for(i = 0;i < m->nb_etats && !ret;i++)
        {
            for(int k = 0;k < m->nb_etats;k++){tab[i] = -1;}
            tab[i] = 1;
            ret = est_dans_compsante_forttement_connexe(m,tab,i,0);
        }
        
        free(tab);
        return ret;        
    }
    return false;
}

/* *************************************************************************************** */

bool step(automate_s* m,int e,mot_s *w, char *t)
{
    bool ret = false;
    if ( w != NULL && e>= 0 && e<= m->nb_etats)
    {
        if (w->str_len == 0)
        {
            sprintf(t,"\t\t\t\t [-] ETAT [%d] RESTE À VERIFIER |%s| = %d   \n",e,w->string,w->str_len);
            print_and_clean(t,100);
            return (m->etats[e].final);
        }
        else
        {
            mot_s* tmp;
            int i = 0;
            while(i<m->etats[e].nb_trans && ret != true)
            {
                if (m->etats[e].list_transitions[i].etiquette == *w->string)
                {
                    sprintf(t,"\t\t\t\t [-] ETAT [%d] RESTE À VERIFIER |%s| = %d   \n",e,w->string,w->str_len);
                    print_and_clean(t,100);
                    tmp = mot_suiv(w);
                    ret = step(m,m->etats[e].list_transitions[i].dest,tmp,t);
                    free_mot(tmp);
                }
                i++;
            }
        }
    }
    return ret;
}

bool est_dans_compsante_forttement_connexe(automate_s *m,int* tab, int e, int n)
{
    if (n < m->nb_etats)
    {
        for(int i = 0;i < m->etats[e].nb_trans; i++)
        {
            if (apaprtient_tab(tab, m->etats[e].list_transitions[i].dest, m->nb_etats)){return true;}
        }

        bool ret = false;
        for(int i = 0;i < m->etats[e].nb_trans && !ret ; i++)
        {
            
            ret = est_dans_compsante_forttement_connexe(m,tab,m->etats[e].list_transitions[i].dest,n+1);
        }
        return ret;
    }
    return false;
}

void connect(automate_s* m, int e1, int e2,char c)
{
    if(e1 >= 0 && e2 >= 0 && e1< m->nb_etats && e2 < m->nb_etats && 
        m->etats[e1].current < m->etats[e1].nb_trans)
    {
        m->etats[e1].list_transitions[m->etats[e1].current].dest = e2;
        m->etats[e1].list_transitions[m->etats[e1].current].etiquette = c;
        m->etats[e1].current ++;
    }

    if (m->etats[e1].current >= m->etats[e1].nb_trans){m->etats[e1].current = 0;}
}


/* ****************************************************************************************
**          FONCTION QUI PERMET DE CHARGER L'AUTOMATE À PARTIR D'UN FICHIER              **
***************************************************************************************** */

automate_s* charger_automate(char * file_path)
{
    int fd = open(file_path,O_RDONLY);
    if(fd < 0){myperror("ERREUR DE L'OUVERTURE DU FICHIER");}
    
    int nb_etats = 0;
    
    read_buffer = malloc (SIZE_BUFF*sizeof(char));
    char *tmp = read_buffer;
    
    int file_stren = read(fd,read_buffer,SIZE_BUFF);
    if(fd < 0){myperror("ERREUR DE L'OUVERTURE DU FICHIER");}
    read_buffer[file_stren] = '\0';
    
    nb_etats = read_value('\n');
    if(nb_etats<=0) return NULL;

    automate_s* machine = malloc(sizeof(automate_s));
    machine->courant = 0;
    machine->nb_etats = nb_etats;
    machine->etats = malloc((nb_etats+1) * sizeof(etat_s));

    machine->nb_init  = 0;
    machine->nb_final = 0;
    
    int somme_trans = load_etats(machine);
    machine->etats_initiaux = malloc(sizeof(int)*machine->nb_init);
    machine->etats_finaux = malloc(sizeof(int)*machine->nb_final);

    int j = 0;
    for(int i = 0;i < machine->nb_etats && j < machine->nb_init;i++)
    {
        if (machine->etats[i].initial){machine->etats_initiaux[j] = i;j++;}
    }

    j = 0;
    for(int i = 0;i < machine->nb_etats && j < machine->nb_final;i++)
    {
        if (machine->etats[i].final){machine->etats_finaux[j] = i;j++;}    
    }

    load_trans(machine, somme_trans);

    close(fd);
    free(tmp);
    return machine;
}

int load_etats(automate_s* m)
{
    int somme_trans = 0;
    int etat = 0;
    int n_trans = 0;
    int init_final;

    for(int i = 0;i < m->nb_etats;i++)
    {
        etat = read_value(' ');
        n_trans = read_value(' ');
        m->etats[etat].nb_trans = (n_trans>0)?n_trans:0;
        
        if (m->etats[etat].nb_trans == 0){m->etats[etat].list_transitions = NULL;}
        else{m->etats[etat].list_transitions = malloc(n_trans*sizeof(transition_s));}

        init_final = read_value(' ');

        if (init_final == 1)
        {
            m->nb_init++;
            m->etats[etat].initial = true;
        }else{m->etats[etat].initial = false;}

        init_final = read_value('\n');

        if (init_final == 1)
        {
            m->nb_final ++;
            m->etats[etat].final = true;
        }else{m->etats[etat].final = false;}

        m->etats[etat].current = 0;
        somme_trans += n_trans;
    }
    return somme_trans;
}

void load_trans(automate_s* m, int n)
{
    int src = 0,dest = 0;
    char caract = '\0';
    for (int i = 0; i < n; i++)
    {
        read_buffer += 1;
        src = read_value(']');

        read_buffer += 1;
        caract = read_char('-');

        read_buffer += 1;
        dest = read_value(']');
        read_buffer += 1;
        connect(m, src, dest,caract);
    }
}

char * char_search(char c)
{
    char *tmpo = read_buffer;
    for (int i = 0; tmpo != NULL && tmpo[i]!='\0'; i++)
    {
        if (tmpo[i]==c)
        {
            tmpo[i] = '\0';
            read_buffer = read_buffer + i +1;
            return tmpo;
        }
    }
    return NULL;
}

int read_value(char c)
{
    char *tmp = char_search(c);
    if (tmp != NULL){return atoi(tmp);}
    return 0;
}

char read_char(char c)
{
    char *tmp = char_search(c);
    if (tmp != NULL){return *tmp;}
    return '\0';
}


/* ****************************************************************************************
**          QUELQUES FONCTIONS UTILES DANS LA LOGIQUE DES AUTOMATES                      **
***************************************************************************************** */

bool cherche_init_final(automate_s* m)
{
    for(int i = 0;i <m->nb_etats; i++)
    {
        if (m->etats[i].initial && m->etats[i].final) return true;
    }
    return false;
}


bool chemin_vers_final(automate_s* m,int **t, int e)
{
    bool ret = false;
    if (e >= 0 && e< m->nb_etats)
    {
        if (m->etats[e].final){return true;}

        for(int i = 0;i < m->nb_etats && !ret ;i++)
        {
            if(t[e][i] == 1)
            {
                t[e][i] = -1;
                ret = chemin_vers_final(m,t,i);
            }

            // if (i == m->nb_etats -1) return false;
        }   
        return ret;
    }
    return ret;
}

bool apaprtient_tab(int *t, int n, int max)
{
    for(int i = 0;i < max; i++)
    {
        if (t[i] == n) return true;
    }
    return false;
}

mot_s* charge_mot(char* str)
{
    mot_s *word   = malloc(sizeof(mot_s));
    word->string  = malloc(sizeof(char)*(strlen(str)+1));
    word->str_len = strlen(str);
    strcpy(word->string, str);
    word->string[word->str_len] = '\0';
    return word;
}

mot_s* mot_suiv(mot_s * w)
{
    if(w != NULL)
    {
        mot_s *word = malloc(sizeof(mot_s));
        word->string = malloc(sizeof(char)*(w->str_len-1));
        word->str_len = w->str_len-1;
        strncpy(word->string,w->string+1,w->str_len-1);
        word->string[word->str_len] = '\0';
        return word;
    }
    return NULL;
}
/* *************************************************************************************** */



/**
 * FONCTION UTILISÉE UNIQUEMENT LORS DES ERREURS DANS LA LECTURE DU FICHIER 
 **/ 
void  myperror(char *s)
{
    sprintf(print, "\n\n\n\n\t\t\t******* %s ******* \n\n",s);
    print_and_clean(print,300);   
    exit(EXIT_FAILURE);
}




/****************************************************************************
 *                  LIBERATION DE LA MEMOIRE 
 * **************************************************************************/

void free_automate(automate_s* m)
{
    if (m != NULL)
    {
        for(int i= 0; i < m->nb_etats; i++){free_etat(m->etats+i);}
        free(m->etats_initiaux);
        free(m->etats_finaux);
        free(m);
    }
}
void free_etat(etat_s *e)
{
    if (e != NULL){free(e->list_transitions);}
}


void free_mot(mot_s *w)
{
    if (w != NULL)
    {
        if (w->string != NULL){free(w->string);}
        free(w);
    }
}


/** ********************************************************************************************************
 *  ******************** TOUT CE QUI CONCERNE L'AFFICHAGE, LES INTERRACTIONS AVEC L'UTILISATEUR ************
 *  *******************************************************************************************************/


void affiche(automate_s* m, char* t)
{
    system("clear");
    sprintf(t,"\n\e[0m\e[1;25m\t\t\t::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");
    print_and_clean(t,300);
    sprintf(t,"\e[1;25m\t\t\t:::::::::::::::::::: AFFICHAGE DE L'AUTOMATE :::::::::::::::::::::::\n");
    print_and_clean(t,100);
    sprintf(t,"\e[0m\e[1;25m\t\t\t::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n\n");
    print_and_clean(t,300);

    for(int i = 0; m != NULL && i < m->nb_etats ;i++)
    {
        sprintf(t,"\t\t\t   [+] ETAT [%d] : {%d trans sortantes, %s , %s }\n",i,m->etats[i].nb_trans, (m->etats[i].initial)?"initial":"non initial",(m->etats[i].final)?"final":"non final" );
        print_and_clean(t,100);
        
        for (int j = 0; j < m->etats[i].nb_trans; j++)
        {
            sprintf(t, "\t\t\t\t  [%d]---%c-->[%d] \n",i,m->etats[i].list_transitions[j].etiquette,m->etats[i].list_transitions[j].dest);
            print_and_clean(t,300);
        }
    }
    
    sprintf(t,"\n\t\t\t::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");
    print_and_clean(t,300);
    sprintf(t,"\t\t\t::\e[1;32m Vous pouvez mettre à jour le fichier de config,sans recompiler \e[0m\e[1;25m::");
    print_and_clean(t,300);
    sprintf(t,"\n\t\t\t::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");
    print_and_clean(t,300);
}


void print_and_clean(char* p,int n)
{
    write(STDOUT_FILENO,p, n);
    for(int i = 0;i < SIZE_BUFF;i++){p[i] = '\0';}
}


int menu()
{
    int choix = -1;
    char tmp[313]={'\0'};

    sprintf(print," \e[1;34m\t\t\t  [*] MENU \n \t\t\t     [1] APPARTENANCE \n  \t\t\t     [2] LANGAGE VIDE \n \t\t\t     [3] LANGAGE INFINI \n\t\t\t     [4] QUITTER \n ");
    print_and_clean(print,200);
    
    do
    {
        sprintf(print," \e[1;33m\t\t\t     [*] VOTRE CHOIX (1-4) : ");
        print_and_clean(print,50);
        fgets(tmp, 313, stdin);
        
        for(int i = 0; i < 313;i++)
            if (tmp[i] == '\n'){tmp[i] = '\0';break;}
        
        choix = atoi(tmp);
    } while(choix <=0 || choix > 4);

    return choix;
}


bool again(bool *b)
{
    sprintf(print, "\t\t\t\e[1;33m  [*] Voulez-vous continuer [O/N] : ");
    print_and_clean(print,50);
    char tmp[SIZE_BUFF];
    fgets(tmp,SIZE_BUFF,stdin);
    if (tmp[0] == 'O' || tmp[0] == 'o')
    {

        sprintf(print, "\t\t\t\e[1;33m  [*] Voulez-vous rafraîchir le fichier de configuration [O/N] : ");
        print_and_clean(print,100);
        fgets(tmp,SIZE_BUFF,stdin);
        *b = (tmp[0] == 'O' || tmp[0] == 'o')?true:false;
        return true;
    }

    return false;
}

void status_appartenance(bool matched, mot_s *mot)
{
    if(matched)
    {
        sprintf(print, "\n\t\t\t\t  \e[1;32m** LE MOT <<%s>> EST ACCEPTÉ PAR L'AUTOMATE ** \n\n",mot->string);
    }else
    {
        sprintf(print, "\n\t\t\t\t\e[1;31m**  LE MOT <<%s>> N'EST PAS ACCEPTÉ PAR L'AUTOMATE ** \n\n",mot->string);
    }
    print_and_clean(print,200);
}

void status_langage_vide(automate_s* m)
{
    if (langage_vide(m))
    {
        sprintf(print, "\n\t\t\t\t\e[1;31m **  LE LANGAGE DE CET AUTOMATE EST BIEN VIDE ** \n\n");    
    }else
    {
        sprintf(print, "\n\t\t\t\t\e[1;32m **  LE LANGAGE DE CET AUTOMATE N'EST PAS VIDE ** \n\n");
    }
    print_and_clean(print,200);
}

void status_langage_infini(automate_s* m)
{

    if (langage_infini(m))
    {
        sprintf(print, "\n\t\t\t\t\e[1;31m **  LE LANGAGE DE CET AUTOMATE EST INIFINI ** \n\n");    
    }else
    {
        sprintf(print, "\n\t\t\t\t\e[1;31m **  LE LANGAGE DE CET AUTOMATE EST FINI ** \n\n");
    }
    print_and_clean(print,200);
}


mot_s* input(char *s, mot_s* m)
{
    int size = 0;
    do
    {
        sprintf(print," \e[1;33m\t\t\t  [*] ENTREZ LE MOT À VERIFIER : ");
        print_and_clean(print,50);
        fgets(s,SIZE_BUFF,stdin);
        size = strlen(s)-1;
        s[size] = '\0';
        sprintf(print,"\t\t\t\e[0m\e[1;34m  [*] VOUS AVEZ SAISI : \e[2;32m|%s|\e[0m\e[1;1m\n",s);
        print_and_clean(print,200);
    } while(size < 0);

    if(m == NULL)
    {
        m = malloc(sizeof(mot_s));
        m->string = malloc(size*sizeof(char));
    }else
    {
        free(m->string);
        m->string = malloc(size*sizeof(char));
    }
    m->str_len = size;
    strcpy(m->string,s); 

    return m;
}
