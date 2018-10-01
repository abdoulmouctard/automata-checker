#include "automate.h"

char print[SIZE_BUFF];




int main()
{
    bool matched;
    automate_s * machine = NULL;
    bool refresh = true;

    char str[SIZE_BUFF];
    int choix;
    
    do
    {
        if (refresh)
        {
            free_automate(machine);
            machine = charger_automate("config");
        }

        affiche(machine, print);
        choix = menu();
        if(choix == 1)
        {
            mot_s * mot = input(str, NULL);
            matched =  appartenance(machine,mot,print);
            status_appartenance(matched,mot);        
        }else if(choix == 2)
        {
            status_langage_vide(machine);
        }else if(choix == 3)
        {
            status_langage_infini(machine);            
        }else{break;}

    } while(again(&refresh));

    sprintf(print,"\e[0m\e[1;25m\n\t\t\t::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n\t\t\t::\t\t            BYE                                   ::\n\t\t\t::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");
    print_and_clean(print,300);

    free_automate(machine);
    return EXIT_SUCCESS;
}


