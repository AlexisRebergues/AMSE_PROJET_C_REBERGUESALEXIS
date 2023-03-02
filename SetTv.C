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

#define ZONETARGETRIGHT       "TARGET_R"    /* ->nom de la zone partagee                 */
#define ZONETARGETLEFT      "TARGET_L"
#define STR_LEN         256         /* ->taille par defaut des chaines           */
int test=0;
int  GoOn = 1;                
double *shareTv;

/* prototypes locaux */






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
    double Tv;
    char moteurencharge;

  if(argc!=3){
      printf("Nombre arguments invalides");
      return (0);
  }

     if( sscanf(argv[1],"%lf",&Tv) == 0 )
    {
        fprintf(stderr,"%s.main()  : ERREUR ---> l'argument #1 doit etre reel\n", argv[0]);
        usage(argv[0]);
        return( 0 );
    };
      if( sscanf(argv[2],"%s",&moteurencharge) == 0 )
    {
        fprintf(stderr,"%s.main()  : ERREUR ---> l'argument #1 doit etre reel\n", argv[0]);
        usage(argv[0]);
        return( 0 );
    };
                  
    int  iArea;                 
    /*.Connection à la zone */
    if(strcmp("L",argv[2])==0){
        if( (iArea = shm_open(ZONETARGETLEFT, O_RDWR | O_CREAT, 0600)) < 0)
        {
            printf("Creation impossible \n");
            if( (iArea= shm_open(ZONETARGETLEFT, O_RDWR, STR_LEN)) < 0)
            {  
                fprintf(stderr,"ERREUR : ---> appel a shm_open()\n");
                fprintf(stderr,"         code  = %d (%s)\n", errno, (char *)(strerror(errno)));
                return( -errno );
            };
            
        };
        /* on attribue la taille a la zone partagee */
        ftruncate(iArea, STR_LEN);
        if( (shareTv = (double*)mmap(NULL, STR_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, iArea, 0 ))  == NULL)
        {
            fprintf(stderr,"ERREUR : ---> appel a mmap()\n");
            fprintf(stderr,"         code  = %d (%s)\n", errno, (char *)(strerror(errno)));
            return( -errno );
        };

    }
    else {
        if( (iArea = shm_open(ZONETARGETRIGHT, O_RDWR | O_CREAT, 0600)) < 0)
        {
            printf("Creation impossible\n");
            if( (iArea = shm_open(ZONETARGETRIGHT, O_RDWR, STR_LEN)) < 0)
            {  
                fprintf(stderr,"ERREUR : ---> appel a shm_open()\n");
                fprintf(stderr,"         code  = %d (%s)\n", errno, (char *)(strerror(errno)));
                return( -errno );
            };
        };
        /* on attribue la taille a la zone partagee */
        ftruncate(iArea, STR_LEN);
        /* tentative de mapping de la zone dans l'espace memoire */
        if( (shareTv =(double*) mmap(NULL, STR_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, iArea, 0 ))  == NULL)
        {
            fprintf(stderr,"ERREUR : ---> appel a mmap()\n");
            fprintf(stderr,"         code  = %d (%s)\n", errno, (char *)(strerror(errno)));
            return( -errno );
        };

    }

  
    *shareTv = Tv;
    printf("nouvelle valeur de Tv%s = %lf ", &moteurencharge, Tv);
    return( 0 );   



  return( 0 );
}