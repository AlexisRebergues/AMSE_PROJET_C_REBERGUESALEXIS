#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h> 
#include <sys/time.h>  
#include <sys/stat.h> 
#include <ctype.h>
#include <fcntl.h>
#include <sys/mman.h>
   
/* variables globales */

typedef struct moteur{
  double i;
  double w;
}moteur;
#define STATEZONE  "STATE"    /* ->nom de la zone partagee pour STATEL et STATER                */
#define STR_LEN         256         /* ->taille par defaut des chaines           */
#define NB_ARGS         1               /* ->nombre d'arguments a passer en ligne de commande                            */

// Variable mémoire partagée
moteur* etatmoteur;



/* declarations */

void usage( char *);           /* ->aide de ce programme                */



/* aide de ce programme */

void usage( char *pgm_name )
{
  if( pgm_name == NULL )
  {
    exit( -1 );
  };
}



/* programme principal */

int main( int argc, char *argv[])
{
    char    moteurencharge;                       /* ->caractere pour identifier le moteur            */
    char    NomZoneEtat[STR_LEN];       /* ->nom de la zone contenant la commande           */
    int     iArea;                       /* ->descripteur pour la zone de commande           */
    double  *moteur;                    /* ->pointeur sur le moteur */

    if( argc != NB_ARGS)
    {
        usage(argv[0]);
        return( 0 );
    };


    /* recuperation des arguments */

    if( sscanf(argv[1],"%c",&moteurencharge) == 0 )
    {
        fprintf(stderr,"%s.main()  : ERREUR ---> l'argument doit etre un char\n", argv[0]);
        usage(argv[0]);
        return( 0 );
    };
    sprintf(NomZoneEtat,"%s%c", STATEZONE, moteurencharge);

    /* lien / creation aux zones de memoire partagees */
    sprintf(NomZoneEtat,"%s%c", STATEZONE, moteurencharge);
    /* zone de commande */
    if(( iArea = shm_open(NomZoneEtat, O_RDWR, 0600)) < 0 )
    {
        fprintf(stderr,"%s.main() :  ERREUR ---> appel a shm_open() \n", argv[0]);
        fprintf(stderr,"             code = %d (%s)\n", errno, (char *)(strerror(errno)));
        exit( -errno );
    }
    else
    {
        printf("LIEN a la zone %s\n", NomZoneEtat);
    };
    if( ftruncate(iArea, 2 * sizeof(double)) < 0 )
    {
        fprintf(stderr,"%s.main() :  ERREUR ---> appel a ftruncate() #1\n", argv[0]);
        fprintf(stderr,"             code = %d (%s)\n", errno, (char *)(strerror(errno)));
        exit( -errno );
    };
    if((moteur = (double *)(mmap(NULL, 2 * sizeof(double), PROT_READ | PROT_WRITE, MAP_SHARED, iArea, 0))) == MAP_FAILED )
    {
        fprintf(stderr,"%s.main() :  ERREUR ---> appel a mmap() #1\n", argv[0]);
        fprintf(stderr,"             code = %d (%s)\n", errno, (char *)(strerror(errno)));
        exit( -errno );
    };

    /* Remise à zéro */

    memset(moteur,0, sizeof(etatmoteur));


  return( 0 );
}