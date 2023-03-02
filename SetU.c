#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h> 
#include <sys/time.h>  

#include <ctype.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>     

/* variables globales */

#define ZONECOMMANDERIGHT       "ZoneCommandeR"    /* ->nom de la zone partagee                 */
#define ZONECOMMANDELEFT      "ZoneCommandeL"
#define STR_LEN         256         /* ->taille par defaut des chaines           */
int test=0;
int  GoOn = 1;                


/* prototypes locaux */



double *szInStr;


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

  if(argc!=3){
      printf("Nombre arguments invalides");
      return (0);
  }


    void *Adresse;                      
    int  iArea;                 
    /*.Connection à la zone */
    if(strcmp("L",argv[2])==0){
        if( (iArea = shm_open(ZONECOMMANDELEFT, O_RDWR | O_CREAT, 0600)) < 0)
        {
            printf("Creation impossible \n");
            if( (iArea= shm_open(ZONECOMMANDELEFT, O_RDWR, STR_LEN)) < 0)
            {  
                fprintf(stderr,"ERREUR : ---> appel a shm_open()\n");
                fprintf(stderr,"         code  = %d (%s)\n", errno, (char *)(strerror(errno)));
                return( -errno );
            };
            
        };
        /* on attribue la taille a la zone partagee */
        ftruncate(iArea, STR_LEN);
        if( (Adresse = mmap(NULL, STR_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, iArea, 0 ))  == NULL)
        {
            fprintf(stderr,"ERREUR : ---> appel a mmap()\n");
            fprintf(stderr,"         code  = %d (%s)\n", errno, (char *)(strerror(errno)));
            return( -errno );
        };

    }
    else {
        if( (iArea = shm_open(ZONECOMMANDERIGHT, O_RDWR | O_CREAT, 0600)) < 0)
        {
            printf("Creation impossible\n");
            if( (iArea = shm_open(ZONECOMMANDERIGHT, O_RDWR, STR_LEN)) < 0)
            {  
                fprintf(stderr,"ERREUR : ---> appel a shm_open()\n");
                fprintf(stderr,"         code  = %d (%s)\n", errno, (char *)(strerror(errno)));
                return( -errno );
            };
        };
        /* on attribue la taille a la zone partagee */
        ftruncate(iArea, STR_LEN);
        /* tentative de mapping de la zone dans l'espace memoire */
        if( (Adresse = mmap(NULL, STR_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, iArea, 0 ))  == NULL)
        {
            fprintf(stderr,"ERREUR : ---> appel a mmap()\n");
            fprintf(stderr,"         code  = %d (%s)\n", errno, (char *)(strerror(errno)));
            return( -errno );
        };

    }

  
    szInStr = (double *)(Adresse);
    *szInStr=atof(argv[1]);
    printf("%f",*szInStr);




  return( 0 );
}