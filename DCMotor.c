#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <stdbool.h>


//declaration
double B0 (double , double );
double Z1(double , double , double );
double B1 (double , double , double );
double Z0(double , double , double );
void usage( char *);     
void SignalHandler( int  );
double newvangulaire(double ,double,double , double);
double newintensite(double , double ,double );
void Miseajourdesdonnees(void);
void usage( char *);

void initialisationMoteur(
                double ,       
                double ,          
                double ,         
                double ,        
                double ,        
                double ,          
                double  );







//définitions
/* constantes */
#define COMMANDZONE  "CommandeZone"
#define STATEZONE  "StateZone"
#define STR_LEN  256   /* ->taille  de la zone */
#define NombreArgument 9;
#define REFRESH_RATE    10;

/*variables globales */

int GoOn=1     ;  /* ->controle d'execution de la boucle dans main() */
bool DCMotor=true ;
double z0;
double z1;
double b1;
double b0;
double Te;
double r;
double resistance;
double inductance;
double constanteelectrique;
double constantemoteur;
double coeffrottement;
double inertietotale;
double periodeechantillonnage;
char moteurencharge;
char nomZonedeCommande[STR_LEN];
char NomZoneEtat[STR_LEN];
int iFdState;
int iFdCommande;




// struct sigaction {
// void (*sa_handler) (int);
// sigset_t sa_mask;
// int sa_flags;
// };











typedef struct moteur{
    double intensite;
    double vRotation;
}moteur;


/*sharedVariable*/
moteur* sharemoteur;
double *sharetension;
double *shareintensite;
double *sharevrotation;


/* méthodes */


/*signal interseption*/
void SignalHandler( int signal )
{
	if( signal == SIGUSR1){
        DCMotor = !DCMotor;
        if (DCMotor){
            printf("ON\n");
            GoOn = 1;
        }
        else{
            printf("OFF\n");
            // printf("reset\n");
            // resetModel();
            GoOn = 0;
        }
         printf("fin du programme");
    }
    if( signal == SIGALRM)
    {
        //printf("Update\n");
        Miseajourdesdonnees();
    }; 
}

/*mise à jour du moteur*/

void Miseajourdesdonnees(void){
    double nouvellevrotation;  
    double nouvelleintensite;
    double u =*sharetension;       /* ->valeur courante de la commande          */
    double w=*sharevrotation;       /* ->valeur courante de la vitesse angulaire */
    double i =*shareintensite ;  


    nouvelleintensite = newintensite(i, w, i);
    nouvellevrotation= newvangulaire(z1, w, b1,i);

    *shareintensite = nouvelleintensite;
    *sharevrotation = nouvellevrotation;

 
}

/* aide de ce programme */
/*&&&&&&&&&&&&&&&&&&&&&&*/
void usage( char *PgmName )
{
    if( PgmName == NULL )
    {
        exit( -1 );
    };
    printf("%s <resistance> <inductance> <constanteelectrique> <constantemoteur> <coeffrottement> <inertietotale> <periodeechantillonnage> <moteurencharge> \n", PgmName );}

/*calcul b0*/
double B0 (double R, double z0){
    return 1/R * (1-z0);
}
/*calcul b1*/
double B1 (double km, double z1, double f){
    return km/f *(1-z1);
}

/*calcul z1*/
double Z1 (double te, double f, double j){
    double exposant = - te *f/j;
    return exp(exposant);
}

/*calcul z0*/
double Z0(double te, double r , double l){
    double exposant = -te*r/l;
    return exp(exposant);
} 

/* calcul nouvelle intensité*/
double newintensite(double oldintensite, double anciennevrotation,double anciennetension){
    return (z0*oldintensite-constanteelectrique*b0*anciennevrotation+b0*anciennetension);
}


/*calcul nouvelle vitesse angulaire*/
double newvangulaire(double z1,double anciennevrotation,double b1, double ancienneintensite){
    return (z1*anciennevrotation+b1*ancienneintensite);
}



void initialisationMoteur(
                double r,       
                double l,          
                double Ke,         
                double Km,        
                double f,        
                double j,          
                double Te )
{
    
    z0 = Z0(Te,r,l);
    z1 = Z1(Te, f, j);
     b0 = B0(r, z0);
    b1 = B1(Km, z1, f);
 
   
}


int main(int argc, char *argv[], char *envp[]){
    double* moteur;
    struct sigaction sa; 
    struct sigaction sa_old;  
    struct itimerval    sTime; 
    sigset_t  mask;   
    int     iLoops = 0;    


    /*implementation des valeurs*/

    if (sscanf(argv[1],"%lf",&resistance)==0){
        fprintf(stderr,"%s : Erreur sur la valeur de la resistance",argv[1]);
    }

      if (sscanf(argv[2],"%lf",&inductance)==0){
        fprintf(stderr,"%s : Erreur sur la valeur de l'inductance",argv[2]);
    }
      if (sscanf(argv[3],"%lf",&constanteelectrique)==0){
        fprintf(stderr,"%s : Erreur sur la valeur de la constante electrique",argv[3]);
    }
      if (sscanf(argv[4],"%lf",&constantemoteur)==0){
        fprintf(stderr,"%s : Erreur sur la valeur de la constante moteur",argv[4]);
    }
      if (sscanf(argv[5],"%lf",&coeffrottement)==0){
        fprintf(stderr,"%s : Erreur sur la valeur du coef de frottement",argv[5]);
    }
      if (sscanf(argv[6],"%lf",&inertietotale)==0){
        fprintf(stderr,"%s : Erreur sur la valeur de l'inertie totale",argv[6]);
    }
      if (sscanf(argv[7],"%lf",&periodeechantillonnage)==0){
        fprintf(stderr,"%s : Erreur sur la valeur de la periode d'échantillonnage",argv[7]);
    }
      if (sscanf(argv[8],"%c",&moteurencharge)==0){
        fprintf(stderr,"%s : Erreur sur la valeur du moteur pris en charge",argv[8]);
    }

    printf("%lf %lf %lf %lf %lf %lf %lf %lf ", resistance, inductance, constanteelectrique, constantemoteur, coeffrottement, inertietotale, periodeechantillonnage);




    /* Creation de zones partagées*/

    sprintf(nomZonedeCommande,"%s%c",COMMANDZONE,moteurencharge);
    sprintf(NomZoneEtat,"%s%c",STATEZONE,moteurencharge);
    printf("%s \n",nomZonedeCommande);
    /*Zone Commande*/

    if(( iFdCommande= shm_open(nomZonedeCommande, O_RDWR,  0600)) < 0)
    {
        if(( iFdCommande = shm_open(nomZonedeCommande, O_RDWR | O_CREAT, 0600)) < 0){
        fprintf(stderr,"%s.main() :  ERREUR ---> appel a shm_open() #1\n", nomZonedeCommande);
        fprintf(stderr,"             code = %d (%s)\n", errno, (char *)(strerror(errno)));
        exit( -errno );
        };
    }
    else
    {
        printf("LIEN  de la zone %s\n", nomZonedeCommande);
    };
    /* ajustement de la taille de la zone partagee */    
    if( ftruncate(iFdCommande, STR_LEN) < 0 )
    {
        fprintf(stderr,"%s.main() :  ERREUR ---> appel a ftruncate() #1\n", nomZonedeCommande);
        fprintf(stderr,"             code = %d (%s)\n", errno, (char *)(strerror(errno)));
        exit( -errno );
    };
    /* obtention d'un pointeur sur la zone et transtypage */
    if((sharetension= (double *)(mmap(NULL, STR_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, iFdCommande, 0))) == MAP_FAILED )
    {
        fprintf(stderr,"%s.main() :  ERREUR ---> appel a mmap() #1\n", argv[0]);
        fprintf(stderr,"             code = %d (%s)\n", errno, (char *)(strerror(errno)));
        exit( -errno );
    };
        /*Zone Etat*/

    if(( iFdState= shm_open(NomZoneEtat, O_RDWR,  0600)) < 0)
    {
        if(( iFdState = shm_open(NomZoneEtat, O_RDWR | O_CREAT, 0600)) < 0){
            fprintf(stderr,"%s.main() :  ERREUR ---> appel a shm_open() #1\n", NomZoneEtat);
            fprintf(stderr,"             code = %d (%s)\n", errno, (char *)(strerror(errno)));
            exit( -errno );}
    }
    else
    {
        printf("LIEN  de la zone %s\n", NomZoneEtat);
    };
    /* ajustement de la taille de la zone partagee */    
    if( ftruncate(iFdState, STR_LEN) < 0 )
    {
        fprintf(stderr,"%s.main() :  ERREUR ---> appel a ftruncate() #1\n", NomZoneEtat);
        fprintf(stderr,"             code = %d (%s)\n", errno, (char *)(strerror(errno)));
        exit( -errno );
    };
    /* obtention d'un pointeur sur la zone et transtypage */
    if((moteur= (double *)(mmap(NULL, STR_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, iFdState, 0))) == MAP_FAILED )
    {
        fprintf(stderr,"%s.main() :  ERREUR ---> appel a mmap() #1\n", argv[0]);
        fprintf(stderr,"             code = %d (%s)\n", errno, (char *)(strerror(errno)));
        exit( -errno );
    };

    shareintensite=&moteur[0];
    sharevrotation=&moteur[1];
    initialisationMoteur(resistance,inductance,constanteelectrique,constantemoteur,coeffrottement, inertietotale,periodeechantillonnage);

    //printf("%lf %lf %lf %lf", z0,z1,b0,b1);
    // printf("sharedintensite %lf ",*shareintensite);
    memset(&sa,0,sizeof(struct sigaction));
    sigemptyset( &mask );
    sa.sa_mask = mask;
    sa.sa_handler = SignalHandler;
    sa.sa_flags = 0;
    if( sigaction(SIGALRM, &sa, &sa_old) < 0 ){
        fprintf(stderr,"%s.main() :  ERREUR ---> appel a sigaction() #1\n", argv[0]);
        fprintf(stderr,"             code = %d (%s)\n", errno, (char *)(strerror(errno)));
        exit( -errno );
    };
    if( sigaction(SIGUSR1, &sa, &sa_old) < 0 ){
        fprintf(stderr,"%s.main() :  ERREUR ---> appel a sigaction() #1\n", argv[0]);
        fprintf(stderr,"             code = %d (%s)\n", errno, (char *)(strerror(errno)));
        exit( -errno );
    };

    sTime.it_interval.tv_sec = (int)(periodeechantillonnage);
    sTime.it_interval.tv_usec = (int)((periodeechantillonnage - (int)(periodeechantillonnage))*1e6);
    
    sTime.it_value.tv_sec = (int)(periodeechantillonnage);
    sTime.it_value.tv_usec = (int)((periodeechantillonnage - (int)(periodeechantillonnage))*1e6);

    if( setitimer( ITIMER_REAL, &sTime, NULL) < 0 ){
        fprintf(stderr,"%s.main() :  ERREUR ---> appel a setitimer() \n", argv[0]);
        fprintf(stderr,"             code = %d (%s)\n", errno, (char *)(strerror(errno)));
        exit( -errno );
    };


    while( GoOn){
        pause();
        int reste = iLoops % (int)REFRESH_RATE;
        if( reste == 0){
             printf("u = %lf w = %lf i = %lf side = %c\n", *sharetension, *sharevrotation, *shareintensite, moteurencharge);
        }
        iLoops++;
    }
    return 0;

};
    



    

    



