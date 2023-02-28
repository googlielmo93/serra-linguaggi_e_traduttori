#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <time.h>
#include "serra.h"
#include <unistd.h>

static FILE *temp;


/* Tabella dei Simboli -> Hashing dei Simboli*/
static unsigned symhash(char *sym) {

  unsigned int hash = 0;
  unsigned c;

  while(c = *sym++)    hash = hash*9 ^ c;

  return hash;
}


unsigned char * symhashDev(char * nameSymbol){

  char devhash[DIMHASH];
  sprintf(devhash, "#%u", symhash(nameSymbol) % DIMHASH);

  return nameSymbol= strdup(strcat(nameSymbol, devhash));  
  /* in questa maniera il nome del device è il simbolo che sarebbe già presente nella tabella per causa della stringa,
   * pertanto lo concateniamo a un codice hash */
}



/*Nel programma non posso avere simboli uguali di tipi diversi per come è stata costruita la tabella di lookup. Un po come matlab, ci puo anche stare.*/
struct symbol *search(char* sym, char* type){
 /* puntatore alla cella corrispondente al simbolo cercato della tabella dei simboli dichiarata nell'header serra.h */
    struct symbol *symptr = &symtab[ symhash(sym) % DIMHASH ];
  int symcount = DIMHASH;      /* viene passata la dimensione della tabella per cercare in tutte le celle di questa il simbolo cercato */
    
  while(--symcount >= 0) {
    if(symptr->name && !strcmp(symptr->name, sym)) {        
 /* se trova il simbolo cercato ritorna il puntatore alla cella contenente il simbolo cercato */
        return symptr; 
    }

    if(!symptr->name) {             /* NUOVO SIMBOLO */ //se è nullo
      if(!type){    //se è nulloe
        //printf ("nuova\n");
         symptr->name = strdup(sym);
         symptr->value = 0;
         symptr->dev = NULL;
         symptr->func = NULL;
         symptr->syms = NULL;  //LISTA DI SIMBOLI
          return symptr;
      }
      else{
         //if(!strcmp(type, "searchSym")){
         if(!strcmp(type, "searchSym")){
             return NULL;
         }
      }
    }
    if(++symptr >= symtab + DIMHASH) {      /* Ricomincia da capo se la cella corrente è l'ultima */
        symptr = symtab;                       
    }
  }
  
  yyerror("Errore: Tabella piena\n");
  abort(); 

}

/*Definizione nodi albero*/

struct ast *newast(int nodetype, struct ast *l, struct ast *r)
{
  struct ast *a = malloc(sizeof(struct ast));
  
  if(!a) {
    yyerror("Spazio di memoria insufficiente\n");
    exit(0);
  }
  
  a->nodetype = nodetype;
  a->l = l;
  a->r = r;
  return a;
}



struct ast *newnum(double d)
{
  struct numval *a = malloc(sizeof(struct numval));
  
  if(!a) {
    yyerror("Spazio di memoria insufficiente\n");
    exit(0);
  }
  
  a->nodetype = 'K';
  a->number = d;
    
  return (struct ast *)a;
}


struct ast *newString(struct symbol *s)
{
  struct stringVal *a = malloc(sizeof(struct stringVal));
  
  if(!a) {
    yyerror("Spazio di memoria insufficiente\n");
    exit(0);
  }
  a->nodetype = 'C';
  a->s = s;
  
  return (struct ast *)a;
}


void update_lookup (char *sym, struct ast *ref, double tipo,double d){
 struct symbol *symptr = (&symtab[ symhash(sym) % DIMHASH ]);
 
  int symcount = DIMHASH;      /* viene passata la dimensione della tabella per cercare in tutte le celle di questa il simbolo cercato */
    
  while(--symcount >= 0) {

    if(!symptr->name) {        
 /* se trova il simbolo cercato ritorna il puntatore alla cella contenente il simbolo cercato */
        symptr->name = strdup(sym);
        double *z=malloc(sizeof(double));
        z[0]=tipo;
        symptr->value = (char *)z;
        symptr->arr = ref; //riferimento alla zona di memoria dove l'array è stato allocato
        symptr->dim = 0;
        symptr->syms = NULL;  //LISTA DI SIMBOLI
        
       printf("ARRAY CREATO CORRETTAMENTE:%s\n", symptr->name);
         
        break;

    }else{
        printf("QUESTO SIMBOLO GIA ESISTE, array non creato\n");
        break;
    }


    if(++symptr >= symtab + DIMHASH) {      /* Ricomincia da capo se la cella corrente è l'ultima */
        symptr = symtab;                       
    }
  }
  

}

//comodo: estrarre la sottostringa device senza id. Funzione di comodo per estrarre il nome del simbolo passato escludento l'id. 
//Utile nelle funzioni, negli array 
char *name_dev(char *arco){
    int y=0;
    char *d=malloc (strlen(arco)*sizeof (char));
    
    while (y<(strlen (arco) +1)){
        if (arco[y]=='#') {d[y]='\0'; break;}
        d[y]=arco[y];
        y++;
    }
    
    return d;
}


void newArray (struct symbol *nome, double dimensione, double tipo){
      char *v=nome->name ;
      v=symhashDev(name_dev(v));
      v=symhashDev(v);
    
    if (tipo==1){
        int *d=malloc(sizeof(int));
        update_lookup(v, (struct ast *)d, tipo, dimensione);

    }
    else if (tipo==2){
        struct symbol *d=malloc(sizeof(struct symbol));
        update_lookup(v, (struct ast *)d, tipo, dimensione);


    }else{
        struct device *d=malloc(sizeof(struct device));
        update_lookup(v, (struct ast  *)d, tipo, dimensione);
    }    
    
    
}

struct ast *newAstArray (struct symbol *nome, double dimensione, double tipo){

  struct defineArray *a = malloc(sizeof(struct defineArray));
  
  if(!a) {
    yyerror("Spazio di memoria insufficiente\n");
    exit(0);
  }
  a->nodetype = 'Z';
  a->dimensione = dimensione;
  a->tipo = tipo;
  a->nome = nome;
    
  return (struct ast *)a;

}


struct ast *newfunc(int functype, struct ast *l, struct ast *r, struct ast *t)
{
  struct funcBuiltIn *a = malloc(sizeof(struct funcBuiltIn));
  
  if(!a) {
    yyerror("Spazio di memoria insufficiente\n");
    exit(0);
  }
  a->nodetype = 'F';
  a->l = l;     //stringa
  a->r = r;      
  a->t = t;     //t: per la data in INTERVAL

  a->functype = functype;
  //printf("%d", functype);       //DEBUG
    
  return (struct ast *)a;
}

struct ast *newfuncArray(int functype, struct ast *l, struct symbol *r, struct ast* t)
{
  struct funcBuiltArray *a = malloc(sizeof(struct funcBuiltArray));
  
  if(!a) {
    yyerror("Spazio di memoria insufficiente\n");
    exit(0);
  }
  a->nodetype = 'M';
  a->l = l;     //stringa
  a->r = r;      
  a->t = t;     //t: per la data in INTERVAL

  a->functype = functype;
  //printf("%d", functype);       //DEBUG
    
  return (struct ast *)a;
}

struct ast *newfuncSystem(int functype)
{
  struct funcBuiltInSystem *a = malloc(sizeof(struct funcBuiltInSystem));
  
  if(!a) {
    yyerror("Spazio di memoria insufficiente\n");
    exit(0);
  }
  a->nodetype = 'O';
  a->functype = functype;
  //printf("%d", functype);       //DEBUG
  return (struct ast *)a;
}



struct ast * newDev(struct symbol *ps, struct argsList *l)
{
    
 
  struct device *d = malloc(sizeof(struct device));
  struct argsList *lpt;
  
  if(!d) {
    yyerror("Spazio di memoria insufficiente\n");
    exit(0);
  }
  
  char *nameSymbol;
  char *devNameList;
        
  nameSymbol= ps->name;

  nameSymbol= symhashDev(nameSymbol);
  
  struct symbol *symbolDev= NULL; 
  symbolDev = search(nameSymbol, "searchSym");

  if(symbolDev==NULL)    //SE IL DISPOSITIVO NON ESISTE
  {
       struct symbol *sym= search(nameSymbol, NULL);            //INSERIMENTO DEL SIMBOLO DEL DISPOSITIVO CON #CODICEHASH AGGIUNTO
       d->nodetype = 'D';
       d->status = 0;  //LO PONGO CON STATO SPENTO DI DEFAULT
       d->s= sym;
       d->l = (struct ast *)l;
      d->c=0;
       //printf("il fratello è:%s", (((struct argsList *) (d->l))->sym)->name);
       printf("Dispositivo inserito con successo con ID: %s \n", nameSymbol);

      struct ast *reco= (struct ast *)d;
      (sym->dev)=reco;
      
       if(l!= NULL) { 

          struct symbol *ptrSymDevices= search(nameSymbol, "searchSym");     //ptrSymDevices valore di ritorno
         
          printf("%s connesso con ->   ", nameSymbol); 
          int countDeviceUnknown = 0;

          printf("[");

          for( lpt=l; lpt; lpt = lpt->next){
                 devNameList = lpt->sym->name;

                 const char strchSearchChar= '#';
                 if( !strchr(devNameList,strchSearchChar))
                        devNameList = symhashDev(devNameList);

                 if(strcmp(devNameList, nameSymbol)){     //SOLO SE DIVERSI
                
                     printf(" [%s] ", devNameList); 

                     if( !search(devNameList, "searchSym")){   
                      /* SE ptrSymDevices->dev NON È SETTATO, cioè se dev, puntatore ad un nodo struttura device è NULL, ALLORA BISOGNA CREARE ANCORA IL DEVICE, ANCHE SE IL SIMBOLO ESISTE GIÀ, COSA SCONTATA perchè creato in fase di parsing dalla regolaargsListDevice  */
                           printf("* ", ptrSymDevices->dev);
                            countDeviceUnknown++;
                     }

                 
                 }else{ 
                     printf(" itself ");
                 }
                 
                 
                 if(lpt->next != NULL) 
                          printf("-");

          }
          
          printf("]\n"); 

          if(countDeviceUnknown > 0)    printf("Devices con (*) sconosciuti, inserire devices\n");

       }
    
  }else{
    
    printf("Dispositivo già Esistente con ID: %s\n", nameSymbol);
      d->s=symbolDev;
      d->c=1;
  }
    d->nodetype = 'D';
  return (struct ast *)d;

}



struct ast *newcall(struct symbol *s, struct ast *l)
{
  struct userfunc *a = malloc(sizeof(struct userfunc));
  
  if(!a) {
    yyerror("Spazio di memoria insufficiente\n");
    exit(0);
  }
  a->nodetype = 'U';
  a->l = l; //argomento funzione
  a->s = s; //nome funzione, finisce nella tabella dei simboli
  return (struct ast *)a;
}


struct ast *newref(struct symbol *s)
{
  struct symref *a = malloc(sizeof(struct symref));
  
  if(!a) {
    yyerror("Spazio di memoria insufficiente\n");
    exit(0);
  }
  a->nodetype = 'N';
  a->s = s;
  return (struct ast *)a;
}


/*Nelle assegnazioni le scelt3 che sono state fatte riguardano l'associare al value della tabella dei simboli il riferimento. Che saranno tutti interpretati come puntatori a char anche se in realtà non lo sono */

struct ast *newasgn(struct symbol *s, struct ast *v)
{
  struct symasgn *a = malloc(sizeof(struct symasgn));
  
  if(!a) {
    yyerror("Spazio di memoria insufficiente\n");
    exit(0);
  }
  a->nodetype = '=';
  a->s = s;
  a->v = v;
  return (struct ast *)a;
}


//Nodo per gli if, else, do while
struct ast *newContent(int nodetype, struct ast *cond, struct ast *tl, struct ast *el)
{
  struct content *a = malloc(sizeof(struct content));
  
  if(!a) {
    yyerror("Spazio di memoria insufficiente\n");
    exit(0);
  }
  a->nodetype = nodetype;
  a->cond = cond;
  a->tl = tl;
  a->el = el;
  return (struct ast *)a;
}

struct ast *newRepeat(int nodetype, double cond, struct ast *tl)
{
  struct contentRepeat *a = malloc(sizeof(struct contentRepeat));
  
  if(!a) {
    yyerror("Spazio di memoria insufficiente\n");
    exit(0);
  }
  a->nodetype = nodetype;
  a->cond = cond;
  a->tl = tl;

  return (struct ast *)a;
}

//Struttura momentanera usata:
//- nelle funzioni per contenere la  lista dei nomi dele variabili nel momento in cui la funzione viene definita
//- nella creazione di un oggetto device per la lista dei device a cui è collegato
struct argsList *newargsList(struct symbol *sym, struct argsList *next)
{
  struct argsList *sl = malloc(sizeof(struct argsList));
  
  if(!sl) {
    yyerror("Spazio di memoria insufficiente\n");
    exit(0);
  }
  
  sl->sym = sym;
  (sl->sym)->name=sym->name;
  sl->next = next;
  return sl;
}

//Usato nelle funzioni:
//-consente di aggiornare la lista dei parametri nel momento in cui viene ridefinita la funzione. Usata infatti nel nodo ast defNewFunc
void argsListfree(struct argsList *sl)
{
  struct argsList *nsl;

  while(sl) {
    nsl = sl->next;
    free(sl);
    sl = nsl;
  }
}


//Collega nella struct symbol passata come parametro i riferimenti alla argsList e all'AST che definisce
void defSymRef(struct symbol *name, struct argsList *syms, struct ast *func, struct ast*ret)
{
  if(name->syms){ argsListfree(name->syms); }
  //if(name->func) treefree(name->func);
  
  name->syms = syms;
  //name->func = func;
  name->ret=ret;
}

//nodo ast di una funzione contente in syms la lista dei parametri passati, in func la catena di espressioni da eseguire, 
//in ret l'espressione del valore di ritorno
struct ast* defFunction (struct symbol *name, struct argsList *syms, struct ast *func, struct ast*ret)
{
  if(name->syms){ argsListfree(name->syms); }
  if(name->func) treefree(name->func);
  
  name->syms = syms;
  name->func = func;
  name->ret=ret;
    
  struct ast *a = ( malloc(sizeof(struct ast)) ); //fittizzio
  a->nodetype='T'; //fittizzio
  return a;
    
}

//funzione che esegue le operazioni dl linguaggio e dal quale parte lo scorrimento dell'albero
char* eval(struct ast *a)
{
  char *v, *a1, *a2;
  int h;
  double n;
  struct symbol *s;
  struct ast *c;
  if(!a) {
    yyerror("Errore interno, null eval; la funzione non restituisce alcun parametro");
    return 0;
  }

    switch(a->nodetype) {      
          

        /* Number constant, ritorna il numero inserito */      
      case 'K':
                v= malloc(sizeof((((struct numval *)a)->number)));
                sprintf(v, "%1.0f", ((struct numval *)a)->number);
                s=search("ans", NULL);
                s->value = v;
                break;

        /* String constant, token STRING, ritorna la stringa inserita */
     case 'C':  
                v= strdup(((struct stringVal *)a)->s->name);
                s=search("ans", NULL);
                s->value = v; 
                break;   

        /* Symref, token NAMe, ritorna il valore contenuto nel symbol name creato*/
     case 'N':
              if((((struct symref *)a)->s->value))
                  v=strdup((((struct symref *)a)->s->value));
              else{
                  v=malloc(sizeof(char *));
                  v[0]='0';     //valore di default, se associo null rischio segmentazione
              }
              break; 

        /* NewAsgn, assignment, production: NAMe = exp : ritorna come stringa il valore risultato a destra dell'espressione*/
     case '=': 
              v=eval(((struct symasgn *)a)->v);
              ((struct symasgn *)a)->s->value = v;
              if(((struct symasgn *)a)->s->value){
                v= strdup( ((struct symasgn *)a)->s->value); //assegno il valore a value. In realtà assegno il riferimento a value. Poi posso stampare.
              }else{
                  v=NULL;
              }
              break;

        /* control content */
        /* null if/else/do expressions allowed in the grammar, so check for them; ritorna il valore del corpo dell'if */
      case 'I': 
            v=malloc(sizeof(char));
           if( eval( ((struct content *)a)->cond) !=NULL){
              // printf ("a->condASAS\n");
               if( ((struct content *)a)->tl) {
                    v = eval( ((struct content *)a)->tl);
              } else{
                   v[0] = '0';        /* a default value */
               }
           } else {
                if( ((struct content *)a)->el) {
                    v = eval(((struct content *)a)->el);
                } else{
                    v[0] = '0';        /* a default value */
                }
            }
           break;
            

     case 'W':
         v=malloc(sizeof(char));
        v[0] = '0';        /* a default value */
            
        if( ((struct content *)a)->tl) {
          while( eval(((struct content *)a)->cond))
                     v = eval(((struct content *)a)->tl);
        }

        break;        /* last value is value */

     case 'R': 
            v=malloc(sizeof(char));
            v[0] = '0';        /* a default value */
            n= ((struct contentRepeat *)a)->cond;

            if( ((struct content *)a)->tl && n!=0){
                for (int j=0; j<(int)n; j++){
                    eval(((struct content *)a)->tl);
                }
            }
            return NULL;
            
     case 'L': eval(a->l); v = eval(a->r); break;

     case 'F': //ritorna sempre stringhe o null se l'operazione non è annegabile
               v = (char *) callbuiltin((struct funcBuiltIn *)a);
               if (((struct funcBuiltIn *)a)->functype < 9 && ((struct funcBuiltIn *)a)->functype >1){
                   v= v? name_dev (((struct symbol *)v)->name) : NULL;
                }
                else
                    return v;
            
            /*AGGIUNTA: return ((struct symbol *)v)->name;*/ 
            break;

     case 'O': v = (char *) callbuiltinSystem((struct funcBuiltInSystem *)a); break;
            
     case 'U': v = calluser((struct userfunc *)a); break;

     case 'Z':  
            newArray(((struct defineArray *)a)->nome, ((struct defineArray *)a)->dimensione, ((struct defineArray *)a)->tipo);  
            return NULL; break; 

     case 'D': return "D"; break;
     
     case 'M':  v = (char *) callbuiltinArray((struct funcBuiltArray *)a); break;
            
     case 'T': printf("funzione definita"); return NULL; 
     default: printf("Errore interno: bad node %c\n", a->nodetype);
  }
  return v;
}

//Funzione embedded che verifica il ping col device passato come parametro. 
//Funzione di comodo usata da richiamare, in altre funzioni /switchOn, off e interval
struct ast *connect(char *v){
    
     printf("Ricerca del dispositivo %s in corso...\n", v);
     char *n=symhashDev(v);
     struct symbol *symDev= search(n, "searchSym");

     if(symDev){
        printf("Dispositivo Esistente\nRichiesta connessione...\n\n");     //INSERIRE ENUM CON CODICE DISPOSITIVO COME HTTP 200 AD ESEMPIO
     }else{
        printf("Dispositivo %s: Non Esistente\n\n", n);  
        return NULL;
     }

     return (struct ast *) symDev;
    
}

//Accensione del dispositivo:
//Descrizione: L'accensione del dispositivo è stata intesa come la variazione dello status dell'ogetto del device. L'ogetto device è memorizzato all'interno la tabella dei simboli nel campo Dev (è stato aggiunto una cosa piccola dentro search, chiedere conferma a vincenzo). Quindi bisogna effettuare una connect (si puo richiamare dentro il case di B_connect, confrontarsi) che restituisce il simbolo. Se la connessione è andata a buon fine si va a modificare il campo Dev.
struct symbol *switchOn(char *v){
    
    printf("\t\t\tVerifica in corso della connessione del dispositivo....\n");
    struct symbol *sym= (struct symbol *) connect(v);

    if(sym){
        ((struct device *) (sym->dev))->status =1;
        printf("\t\t\tLa connect è andata a buon fine e il dispositivo è stato acceso\n\n");
    }else{
        printf("\t\t\tDispositivo inesistente e quindi non acceso\n\n");
        return NULL;
    }
    return sym;
        
}

struct symbol *switchOff(char *v){
    
    printf("\t\t\tVerifica in corso della connessione del dispositivo....\n");
    struct symbol *sym= (struct symbol *) connect(v);

    if(sym){
        ((struct device *) (sym->dev))->status =0;
        printf("\t\t\tIl dispositivo è spento\n\n");
    }else{
        printf("\t\t\tDispositivo inesistente\n\n");
        return NULL;
    }
    return sym;
        
}

void *annaffia(void *x) {
    
    struct thread_time *y= (struct thread_time *)x;
    
    sleep (y->seconds);
    
    printf("\nAVVIO OPERAZIONE:\n");

        //Operazioni:
    char *temp=malloc((strlen(y->v)+1)*sizeof(char));
    temp=strcpy(temp, y->v);

    struct symbol *h= switchOn(y->v);
   // printf ("SLEEEEEP:%f\n", y->r);
    if(h){
        sleep(y->r); //time.h, 1000 è 1 secondo, aspettiamo min secondi
        switchOff(temp);
    }
    printf ("L'OPERAZIONE DI INTERVAL è CONCLUSA, IL THREAD è TERMINATO\n");
    printf("\n>");
}


void *brute_force (void *x){
    char *v=(char *)x;
    sleep (1+rand()%30);
    connect(name_dev(v));
}


//calcola la differenza tra la data passata e la data corrente (in secondi)
float calc_diff_date (int anno, int mese, int giorno, int ora, int minuti){
    
    //conversione della data in time_t.
    struct tm info;

   info.tm_year = anno - 1900;
   info.tm_mon = mese - 1;
   info.tm_mday = giorno;
   info.tm_hour = ora;
   info.tm_min = minuti;
   info.tm_sec = 1;
   info.tm_isdst = -1;

   time_t parametro = mktime(&info);
     
    
    //calcolo dell'ora corrente.
   time_t rawtime;
   struct tm *tminfo;

   time ( &rawtime ); //calcolo in secondi della data corrente.

   return (difftime(parametro, time ( &rawtime )));

}

//Per come è stata costrutia la data il quinto carattere sarà il punto:  nel nostro linguaggio l'unico punto si puo avere con le date, non con altri tipi,
int control_date (char src[]){
    
    if (strlen(src)>4 && src[4]=='.') return 1;
    return 0;
    
}

float data_op (char src[]){

    if ( control_date(src)==0) return -1;
   
    int i=0;
    char *part;
    char **x=malloc (4*sizeof(char **));
    /* Nested calls to strtok do not work as desired */
    part = strtok(src, ".");
    while (part != NULL)
    {
        x[i]=part;
        part = strtok(NULL, ".");
        i++;
    }
    //printf ("x:%s,%s,%s,%s\n", x[0], x[1], x[2], x[3]);

    //printf ("%i,%i,%i,%i\n", atoi(x[0]), atoi(x[1]), atoi(x[2]), atoi(x[3]));
   
    float n= calc_diff_date(atoi(x[0]), atoi(x[1]), atoi(x[2]), atoi(x[3]), atoi(x[4]));
       // printf("%f seconds have passed since the beginning of the month.\n", n);
    //free(x);
    n= (n>=0 ? n: -1);
   
    return n;//return n;

}



//Funzione attivata se: il nome del device è una stringa, se i secondi sono non negativi e se il numero di secondi in cui si deve attivare il device non è 0
void interval (int r, char *v, float seconds){
    pthread_t threads;

    struct thread_time *n=malloc (sizeof (struct thread_time));
    n->r=r;
    n->v=v;
    n->seconds=seconds;
    
    pthread_create (&threads, NULL, annaffia, (void *)n);

    
}

void reconnect (char *nome){
    pthread_t threads[2];
    
    for (int i=0; i<2; i++){
        pthread_create (&(threads[i]), NULL, brute_force, (void *)nome);
    }

}

/*Cancella file*/
void cancella_device (struct symbol *syms){
    
    /*Passo come parametro il simbolo che voglio cancellare. Cancellare vuol dire cancellare il riferimento. Eliminare un elemento dalla tabella dei simboli vuol dire lasciare un buco. Dato che la tabella dei simboli ha una dimensione prefissata. Ponendolo a null non riesco piu ad accedervi. Rimane un campo null nella tabella dei simboli che eventualmente puo essere riutilizzato in futuro per scrivere. */

    (syms->name)=NULL;

}
    

/*Il file è presupposto sia sempre lo stesso e quindi non c'è bisogno di passarlo come parametro. Usata in delete.*/
void scrivi_file (struct symbol *syms){
        
    struct device *d=(struct device *) (syms->dev);
    //printf("symbol:%s, %i, %i\n",syms->name, syms->value, d->status);
    
    FILE *fd;
    char *x=(syms->name);

                /* apre il file in scrittura */
    fd=fopen("Device.txt", "a");
    if( fd==NULL ) {
            perror("Errore in apertura del file");
            exit(1);
    }


          /* scrive il numero */
    fprintf(fd, "%s\n", x);


                /* chiude il file */
    fclose(fd);


}



void delete (char *v){
    
    char *nameSymbol= symhashDev(v);
    struct symbol *symo=  search(nameSymbol, "searchSym");
    
    //struct argsList *n= (symo->syms);
    
    //printf("E' collegato a:%s\n",(n->sym)->name); 
    //printf("E' collegato a:%s\n",(n->sym)->name);
    
    if(symo){
        scrivi_file(symo);
        cancella_device (symo);
    }
    

}



void helpMessage(){

    printf("                                 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
    printf("                                 |                                   |\n");  
    printf("                                 |    ** Manuale di istruzione **    |\n");          
    printf("                                 |                                   |\n");  
    printf("                                 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n\n\n");

    printf("                ---------------------------------------------------------------------\n");
    printf("                                   Elenco comandi base con esempi :\n");
    printf("                ---------------------------------------------------------------------\n");


    printf(" ---------------------------\n");
    printf(" |  Inserimento Stringa :  |\n");                          
    printf(" ---------------------------\n");
    printf("-> \"Testo Stringa\" [INVIO]\n\n");
    printf("-   newString return struct ast * pointerSimbolo\n\n");
    printf("-   Esempio: 'Sono una Stringa'\n\n\n");


    printf(" ----------------------\n");
    printf(" |  Stampa Stringa :  |\n");                          
    printf(" ----------------------\n");
    printf("->  sintassi: print “Stringa” [ INVIO ]\n\n");
    printf("-   callbuiltin return char * pointerSymbol\n\n");
    printf("-   Note: Anche l’inserimento dell’istruzione sopra produce la stampa.\n\n");
    printf("-   Esempio: print “Ciao sono una Stringa” [ INVIO ]\n\n\n");


    printf(" ----------------------------------------------\n");
    printf(" |  Inserimento Semplice Nuovo Dispositivo :  |\n");                          
    printf(" ----------------------------------------------\n");
    printf("->  sintassi: newDev “Nome Device” [INVIO]\n\n");
    printf("-   newDev return struct ast * pointerDevice\n\n");
    printf("-   Esempio: newDevice “dev1” [ INVIO ]\n\n\n");


    printf(" ---------------------------------------------------------------------------\n");
    printf(" |  Inserimento Nuovo Dispositivo con Collegamenti ad altri Dispositivi :  |\n");                          
    printf(" ---------------------------------------------------------------------------\n");
    printf("->  sintassi: newDev “Nome Device” → [ “Nome Device 1”, “Nome Device 2”, …] [ INVIO ]\n\n");
    printf("-   newDev return struct ast * pointerDevice\n\n");
    printf("-   Esempio: newDevice “dev1” → [“dev2”, “dev3”, “dev4”] [ INVIO ]\n\n\n");


    printf(" ----------------------------------------\n");
    printf(" |  Richiesta Status con Dispositivo :  |\n");                          
    printf(" ----------------------------------------\n");
    printf("->  sintassi: status “Nome Device” [ INVIO ]\n\n");
    printf("-   callbuiltin return struct ast * pointerSymbol\n\n");
    printf("-   Esempio: status “dev1” [ INVIO ]\n\n\n");


    printf(" ---------------------------------------------\n");
    printf(" |  Richiesta Connessione con Dispositivo :  |\n");                          
    printf(" ---------------------------------------------\n");
    printf("->  sintassi: connect “Nome Device” [ INVIO ]\n\n");
    printf("-   callbuiltin return struct ast * pointerSymbol\n\n");
    printf("-   Esempio: connect “dev1” [ INVIO ]\n\n\n");


    printf(" -----------------------------------------------\n");
    printf(" |  Richiesta Riconnessione con Dispositivo :  |\n");                          
    printf(" -----------------------------------------------\n");
    printf("->  sintassi: reconnect “Nome Device” [ INVIO ]\n\n");
    printf("-   callbuiltin return struct ast * pointerSymbol\n\n");
    printf("-   Esempio: reconnect “dev1” [ INVIO ]\n\n\n");


    printf(" -------------------------------------\n");
    printf(" |  Accensione status dispositivo :  |\n");                          
    printf(" -------------------------------------\n");
    printf("->  sintassi: switchOn ”StringaNomeDevice”\n\n");
    printf("-   callbuiltin return struct ast * pointerSymbol\n\n");
    printf("-   Esempio: switchOn ”dev1”  [INVIO]\n\n\n");


    printf(" ------------------------------\n");
    printf(" |  Off status dispositivo :  |\n");                          
    printf(" ------------------------------\n");
    printf("->  sintassi: switchOff ”StringaNomeDevice”\n\n");
    printf("-   callbuiltin return struct ast * pointerSymbol\n\n-   Esempio: Esempio:");
    printf("->  switchOff ”dev1”  [INVIO]\n\n\n");


    printf(" ---------------------------------------------------------------------------\n");
    printf(" |  Accensione dello status del device per un certo intervallo di tempo :  |\n");                          
    printf(" ---------------------------------------------------------------------------\n");
    printf("->  sintassi: interval ”NomeDevice”-Secondi");
    printf("-   callbuiltin return struct ast * pointerSymbol\n\n");
    printf("-   Note: E' un unica stringa dove il primo parametro identifica il device da accendere,\n");
    printf("    il secondo definisce per quanti secondi deve stare acceso.");
    printf("-   Esempio: (”pippo,10”, ”paperino,20”)  [INVIO]\n\n");
    printf("-   Altro Esempio:  interval ”pippo”-10  [INVIO]\n\n\n");


    printf(" ----------------------------------\n");
    printf(" |  Eliminazione di un device :   |\n");                          
    printf(" ----------------------------------\n");
    printf("->  sintassi: delete ”NomeDevice”\n\n");
    printf("-   null\n\n");
    printf("-   Esempio: > delete ”pippo”  [INVIO]\n\n\n");


    printf(" ------------------------------------------------------\n");
    printf(" |  Definizioni variabili e assegnazioni variabili :  |\n");                          
    printf(" ------------------------------------------------------\n");
    printf("->  sintassi: +-\n\n");
    printf("-   Note: Come in Matlab\n\n");
}

/*Funzione di comodo per gli array*/
 struct symbol *find (char *v){
    
    //io conosco il nome dell'array
    v=name_dev (v);
    v=symhashDev(v);
    v=symhashDev(v);
    // printf ("ricerco:%s\n", v);
    char *nome_array= v;
    //tipo array:
    struct symbol *s=search(nome_array, "searchSym");
    return s;
}

void remove_array (struct funcBuiltArray *f){
    
    struct symbol *s= find (f->r->name);

    if (s!=NULL && ((double *)(s)->value) !=NULL ){
        
        double *tipo= (double *)(s)->value;
        double dimensione= s->dim;
        
        if (dimensione > 0){
            if (tipo[0]==1){

                int *x= (int *) (s)->arr;
                int *d=malloc((dimensione -1)* sizeof(int));

                for (int i=0; i<(int)  (dimensione-1) ; i++)
                    d[i]=x[i];

                //libero la vecchia area di memoria
                free (x);
                s->arr = (struct ast *) d;
                s->dim = dimensione -1;
            }

            if (tipo[0]==2){

                struct symbol *x= (struct symbol *) (s)->arr;
                struct symbol *d=malloc((dimensione -1)* sizeof(struct symbol));

                for (int i=0; i<(int)  (dimensione-1) ; i++)
                    (d[i]).name =  (x[i]).name;
                free (x);
                s->arr = (struct ast *) d;
                s->dim = dimensione -1;
            }


            if (tipo[0]==3){

                struct device *x= (struct device *) (s)->arr;
                struct device *d=malloc((dimensione -1)* sizeof(struct device));

                for (int i=0; i<(int)  (dimensione-1) ; i++)
                    d[i] =  x[i];
                free (x);
                s->arr = (struct ast *) d;
                s->dim = dimensione -1;
            }


        }
    }
    

    
}


void set_array (struct funcBuiltArray *f){
    
    struct symbol *s= find (f->r->name);
    
    if (s!=NULL && ((double *)(s)->value) !=NULL ){
        double *tipo= (double *)s->value;


        //dimensone array:
        double dimensione= s->dim;
        double indice= ((struct numval *) (f->t))->number;

        if (indice >= 0 && dimensione>0 && indice < dimensione){

            if (tipo[0]==1 && ((f->l)->nodetype)==75){
                int *x= (int *) s->arr;
                double valore= ((struct numval *) (f->l))->number;
                x[(int)indice]= (int) valore;

            }


             if (tipo[0]==2 && ((f->l)->nodetype)==67){
                struct symbol *x= (struct symbol *) s->arr;
                char *valore= (((struct stringVal *) (f->l))->s)->name;
                (x[(int)indice]).name= name_dev(valore);

            }


            // if (tipo[0]==3 && ((f->l)->nodetype)!=67 && ((f->l)->nodetype)!=75){
             //   struct device *x= (struct device *) s->dev;
            //    struct device *valore= (struct device *) (f->l);
             //   x[(int)indice]= *valore;
           //      
           // }
        }
    
    }
    
}


char *get_index (struct funcBuiltArray *f){
    
    char charray[255];
    struct symbol *s= find (f->r->name);
    
    //SE è UN ARRAY E SE IL SIMBOLO ESISTE
    if (s!=NULL && ((double *)(s)->value) !=NULL ){
        
        double *tipo= (double *)s->value;
        
        //dimensone array:
        double dimensione= s->dim;
        double valore= ((struct numval *) (f->l))->number;
        
        if (valore >= 0 && dimensione>0 && valore < dimensione){
            
            if (tipo[0]==1){
                int *x= (int *) s->arr;
                int c=x[(int)valore];
                sprintf(charray, "%i", c);
                return charray;
            }

            if (tipo[0]==2 ){
                struct symbol *x= (struct symbol *) s->arr;
                return name_dev((x[(int)valore]).name);
            }

            if (tipo[0]==3){
                struct device *x= (struct device *) s->arr;
                return ((x[(int)valore]).s )->name;
            }
        }
    } 
    return NULL;
}


void get_array (struct funcBuiltArray *f){
    
    struct symbol *s= find (f->r->name);
    
    if (s!=NULL && ((double *)(s)->value) !=NULL ){
        double *tipo= (double *)s->value;

        //dimensone array:
        double dimensione= s->dim;

        if (tipo[0]==1){
            
            int *x= (int *) s->arr;    
            for (int i=0; i<(int)  dimensione ; i++)
                printf("%i\n",x[i]);

        }

        if (tipo[0]==2){

            struct symbol *x= (struct symbol *) s->arr;
            for (int i=0; i<(int)  dimensione ; i++)
                printf("%s\n",name_dev((x[i]).name) );   

        }

        if (tipo[0]==3){

            struct device *x= (struct device *) s->arr;
            for (int i=0; i<(int)  dimensione ; i++)
                printf("%s\n",((x[i]).s)->name);
        } 
    }
}


void add_array(struct funcBuiltArray *f){
    struct symbol *s= find ( f->r->name);
    //SE è UN ARRAY E SE IL SIMBOLO ESISTE
    if (s!=NULL && ((double *)(s)->value) !=NULL ){    //intero
        
        double *tipo= (double *)(s)->value;
        //dimensone array:
        double dimensione= s->dim;

        //tipo 1 è intero (memorizzata nella creazione dell'array), nodo 75 è k cioe number costant
        if (tipo[0]==1 && ((f->l)->nodetype)==75){

            int *x= (int *) s->arr;

            //valor da insrire nell'array:
            double valore= ((struct numval *) (f->l))->number;
            //double valore = ((double *) eval (f->l))[0];

            //occupo una nuova area di meemoria:
            int *d=malloc((dimensione +1)* sizeof(int));

            //copio nella nuova area di memoria:
            for (int i=0; i<(int)  dimensione ; i++)
                d[i]=x[i];
            d[(int)dimensione]= (int) valore;
            free (x);
            s->arr = (struct ast *) d;
            s->dim = dimensione +1;

        }
        
        //tipo 2 è stringa (memorizzata nella creazione dell'array), nodo 67 è K cioe string costant
        if (tipo[0]==2 && ((f->l)->nodetype)==67){
            
            //dev contiene il riferimnto all'array. Rifrimnto alla strattura
            struct symbol *x= (struct symbol *) s->arr;

            //stringa da insrir
            char *valore= (((struct stringVal *) (f->l))->s)->name;

            //nuovo array
            struct symbol *d=malloc((dimensione +1)* sizeof(struct symbol));

            for (int i=0; i<(int)  dimensione ; i++)
                (d[i]).name =  name_dev((x[i]).name);
            (d[(int)dimensione]).name= valore;
            free (x);
            s->arr = (struct ast *) d;
            s->dim = dimensione +1;

        }

        //tipo 3 identifica un device, essendo diverso da 67 e 75 identifica un device
        if (tipo[0]==3 && ((f->l)->nodetype)!=67 && ((f->l)->nodetype)!=75){
             struct device *x= (struct device *) s->arr;

            //device da insrire
            struct device *valore= (struct device *) (f->l);

            //nuovo array
            struct device *d=malloc((dimensione +1)* sizeof(struct device));

  
            for (int i=0; i<(int)  dimensione ; i++){
                d[i] =  x[i];
                //nel caso in cui cancella il device resta null nella locazione di memoria:
                
            }
            if ((*valore).c ==0) {
                d[(int)dimensione]= *valore;
                free (x);
                s->arr = (struct ast *) d;
                s->dim = dimensione +1;
            }
        }

    }

    
}

//Funzione in cui vengono gestite le funzioni embedded del device (FUNCDEV) e generiche (FUN)
struct ast * callbuiltin(struct funcBuiltIn *f)
{

  enum builtFunc functype = f->functype;
  //char *v = strdup(eval(f->l));
  char *v, *x, *m, *z;
  double r;
  FILE *fconfig, *let;
    
  struct symbol *symDev;

  switch(functype) {
   case B_print:
          //v= (strdup(eval(f->l)) != NULL ) ? strdup(eval(f->l)) : '/';
         //v=strdup(eval(f->l));
         x=eval(f->l);
         v= x ? strdup(name_dev(x)):strdup("0");
         //printf("display = %c", v);
         if(atoi(v) || isalpha(v[0])){
             printf("Display: %s\n", v);
             return (struct ast *)v;
        } else{
              printf("ERROR: Il parametro passato alla funzione printf non è nè un numero nè una stringa\n");
              return NULL;
        }
              // return NULL;
        break;

   case B_connect:
         x=eval(f->l);
         v= x ? strdup(name_dev(x)):strdup("0");
          //v=strdup(eval(f->l));
         printf("Ricerca del dispositivo %s in corso...\n", v);
         v=symhashDev(v);
         symDev= search(v, "searchSym");
         //printf("Sto ritornando symDev:%s\n", symDev->name);
          
         if(symDev){
            printf("Dispositivo Esistente\nRichiesta connessione...\n");     //INSERIRE ENUM CON CODICE DISPOSITIVO COME HTTP 200 AD ESEMPIO
         }else{
            printf("Dispositivo %s: Non Esistente\n", v);  
            //return "inesistente";
            return NULL;
         }
         //printf("Sto ritornando v:%s\n",  symDev->name);
         return (struct ast *) symDev;
         break;

   case B_reconnect:
         //v=strdup(eval(f->l));
         x=eval(f->l);
         v= x ? strdup(name_dev(x)):strdup("0");
         printf("Ricerca del dispositivo %s in corso...\n", v);
         v=symhashDev(v);
         symDev= search(v, "searchSym");

         if(symDev){
            printf("Dispositivo Esistente\nRichiesta Riconnessione...\n");     //INSERIRE ENUM CON CODICE DISPOSITIVO COME HTTP 200 AD ESEMPIO
         }else{
            printf("Dispositivo %s: Non Esistente\n", v);  
            //return "inesistente";
            if (v[0]!='0') reconnect (v);
            return NULL;
         }
         //statusDevice(symDev);
         return (struct ast *) symDev;
         break;

   case B_status:
         x=eval(f->l);
         v= x ? strdup(name_dev(x)):strdup("0");
          // v=strdup(eval(f->l));
         v=symhashDev(v);
         printf("Richiesta status in corso per il device %s...\n", v);
         symDev= search(v, "searchSym");

         if(symDev){
            printf("Dispositivo %s:  connesso, status ", v);
            if (((struct device *)(symDev->dev))->status ==1){
                printf ("accesso\n");
                
            }else{
                printf ("spento\n");
                return NULL;
            }
             //restituisce lo status
         }else{
            printf("Dispositivo %s:  Non Esistente\n", v);  
            //return "inesistente";
            return NULL;
         }
         //statusDevice(symDev);
         return (struct ast *) symDev;
         break;
         
    case B_switchOn:
         x=eval(f->l);
         v= x ? strdup(name_dev(x)):strdup("0");
         //v=strdup(eval(f->l));
          printf ("inizia switchOn\n");
         symDev=switchOn(v);
         //Controllo se tutto è andato bene, altrimenti col valore di return di switchOn facciamo prima
         //symDev= search(v, "searchSym");
         //printf ("Il dispositivo è stato aggiornato con stato: %i.",((struct device *) (symDev->dev))->status); 
          if (! symDev){ return NULL;} //return "inesistente"; }
         return (struct ast *) symDev;
         
    case B_switchOff:
         x=eval(f->l);
         v= x ? strdup(name_dev(x)):strdup("0");
         //v=strdup(eval(f->l));
         symDev= switchOff(v);
         //symDev= search(v, "searchSym");
         //int status=((struct device *) (symDev->dev))->status;
         //printf ("Il dispositivo è stato aggiornato con stato: %i.\n",status); 
         if (! symDev){  return NULL;}//return "inesistente"; }
         return (struct ast *) symDev;
          
    case B_interval:
         //per il nome del device scorro l'albero:
         x=eval(f->l);
         v= x ? strdup(name_dev(x)):strdup("0");

         //secondo parametro: numero di secondi in cui sta attivo il device
         r= atoi (eval(f->r));
         r= r ? r : 0;
          
         //terzo parametro: data inizio.
         m=eval(f->t);
         m= m? strdup(m):strdup("0");
          
         //calcolo in secondi della differenza tra la data odierna e la data passata come parametro
         float seconds=data_op (m);
          
         printf("\n \t IL DEVICE %s SI ATTIVERA' PER %g SECONDI, ALLE %f\n", v, r, seconds);
         if (seconds<0 || ( v[0]>47 && v[0]<58 )|| r==0) //o mi dà NULL come nome di device, o mi dà un numero che è NULLO/0 o i secondi sono negativi
            printf ("I dati passati in input non necessitano dell'attivazione del device\n");   
         else {
              z=malloc (strlen(v)*sizeof(char));
              strcpy(z,v);
              interval(r, z, seconds);
          }
         
         v=symhashDev(v);
         symDev= search(v, "searchSym");
         if (! symDev){  return NULL;}//return "inesistente"; }
         return (struct ast *) symDev;
         //break;
          
    case B_delete:
         x=eval(f->l);
         v= x ? strdup(name_dev(x)):strdup("0");
         // v=strdup(eval(f->l));
         delete(v);
         return NULL; //void è la funzione
         break;
          
   case B_sleep:
         x=eval(f->l);
         v= x ? strdup(name_dev(x)):strdup("0");
         if (isalpha(v[0])){
             return NULL;
         }
         sleep(atoi(v));
         return NULL;
          
    case  B_readFile:
         x=eval(f->l);
         v= x ? strdup(x):strdup("0");
            //v=strdup(eval(f->l));
        
         let=fopen(v, "r"); //esistenza file
          
         if (let!=NULL){
                yyin=let;             
         }else{
                printf("file inesistente\n");
         }
               
         return NULL;
         break;
    
    default:
         yyerror("Funzione built-in sconosciuta %d\n", functype);
         return (struct ast *) 0;
 }
  return NULL;
}


struct ast*callbuiltinArray(struct funcBuiltArray *f){
  enum builtArray functype = f->functype;
    
  struct symbol *symDev;

  switch(functype) {
          
    case B_add:       
         add_array(f);
         return NULL;
         break;
    
    case B_get:
         if ((f->l)==NULL){
            get_array (f);
            return NULL;
         }else{
            return (struct ast *) ( get_index (f) ) ;
         }
         break;
    
    case B_set:
          set_array (f);
          return NULL;
          break;
          
    case B_remove:
          remove_array (f);
          return NULL;
          break;   
          
  }
  return NULL;
      
}


/*FUNZIONI di sistema (SYSTEM)*/
struct ast * callbuiltinSystem(struct funcBuiltInSystem *f)
{
  enum builtFuncSystem functype = f->functype;

  switch(functype) {
          
    case B_clear:
     system("clear");
     return NULL;
     break;
         
    case B_help:
     helpMessage();
     return NULL;
     break;
         
    default:
     yyerror("Funzione di sistema built-in sconosciuta %d\n", functype);
 }

}


//Gestione delle funzioni utente
static char * calluser(struct userfunc *f)
{
    
  struct symbol *fn = f->s;   /* Nome Funzione */
  struct argsList *sl;        /* Argomenti Lista */
  struct ast *args = f->l;    /* Argomento attuale: passato nella chiamata a funzione */
  char **oldval, **newval;      /* Memorizza il valore dell'argomento */
  char *v;
  int nargs;
  int i;

  //con defFunc lo setti a 1 e quindi varia. Se non lo setti a 1 non è consideato come funzione
  if(!fn->func) {   
    yyerror("Chiamata ad una funzione insesistente", fn->name);
    return 0;
  }

  /* Contatore Argomenti */
  sl = fn->syms;
  for(nargs = 0; sl; sl = sl->next)
    nargs++;

  /* Crea Spazio di Memoria adeguato per gli Argomenti */
  newval = (char **)malloc(nargs * sizeof(char **));  
  oldval = (char **)malloc(nargs * sizeof(char **));

  if(!newval) {
    yyerror("Fuori Spazio in %s", fn->name); return 0;
  }
  /* Valuta gli argomenti */
  for(int j = 0; j< nargs; j++) {
    int i= j;
    if(!args) {
      yyerror("Troppo pochi argomenti nella chiamata per %s", fn->name);
      free(newval);
      return 0;
    }

    if(args->nodetype == 'L') {        /* se è una lista di nodi */
      char *h= eval(args->l);
      h= h ? h:strdup("0");
      newval[i]=(char *)malloc(strlen(h) * sizeof(char *));
      sprintf(newval[i], "%s",h);
      args = args->r;
    } else { 

      char *h= eval(args);
      h= h ? h:strdup("0");
      newval[i]=(char *)malloc(strlen(h) * sizeof(char *));
      sprintf(newval[i], "%s", h); 
      args = NULL;
    }

  }

  /* salvare i vecchi valori degli argomenti, per assegnarne di nuovi */
  sl = fn->syms; 
    
  for(i = 0; i < nargs; i++) {
     struct symbol *s = sl->sym;

     char *n= s->value  ? strdup( s->value ):strdup("0");
     oldval[i]=(char *)malloc(strlen(n) * sizeof(char *));
    
     (s->value)=(char *)malloc(strlen( newval[i]) * sizeof(char *));
     sprintf(oldval[i], "%s", n);
     sprintf( (s->value), "%s", newval[i]);

     sl = sl->next;
  }

    
 //Function corpo
 eval(fn->func);
    
 //valore di ritorno:
 struct symbol *p;
 v= (struct symref *)(fn->ret) ? (((struct symref *)(fn->ret))->s)->name : NULL;
 if (v){
     v= eval (fn->ret);
     p=search ((((struct symref *)(fn->ret))->s)->name , NULL); 
     p->value=v;   
 }
    
 sl = fn->syms;
 for(i = 0; i < nargs; i++) {
    struct symbol *s = sl->sym;
    sl = sl->next;
    if(v!=NULL && s==p) continue;
    sprintf(s->value, "%s", oldval[i]);
 }

 free (oldval);
 free(newval);
 return v;
}


//Liberare l'area di memoria da nodi pendenti non eseguiti nel caso di: funzioni, do-while, if e uguaglianze. Da ricontrollare.
void treefree(struct ast *a)
{
  switch(a->nodetype) {

    /* due nodi figli */
  case 'L': 
    treefree(a->r);
    /* senza nodi figlio */
  case 'K': case 'N': case 'C':case 'F':case 'O':case 'U': case 'D': case 'Z': case 'T': case 'M':
    break;
  
  case '=':
    free( ((struct symasgn *)a)->v);
    break;

  case 'I': case 'W': 
    free( ((struct content *)a)->cond);
    if( ((struct content *)a)->tl) free( ((struct content *)a)->tl);
    if( ((struct content *)a)->el) free( ((struct content *)a)->el);
    break;
 
  case 'R':
    if( ((struct contentRepeat *)a)->tl) free( ((struct contentRepeat *)a)->tl);
    break;
          
  default: printf("Errore interno: free bad node %c\n\n>", a->nodetype);
  
  }
  
  free(a);

}

//Funzione che redirige nel canale error.
void yyerror(char *s, ...)
{
  va_list ap;
  va_start(ap, s);

  fprintf(stderr, "%d: Errore: ", yylineno);
  vfprintf(stderr, s, ap);
  fprintf(stderr, "\n");
  va_end(ap);
}

//Messaggio di presentazione:
void welcomeMessage(){
    printf("\n");
    printf("                  +-+-+-+-+-+-+-+ +-+-+ +-+-+-+\n");
    printf("                  |W|e|l|c|o|m|e| |t|o| |t|h|e|  \n");                          
    printf("                  +-+-+-+-+-+-+-+ +-+-+ +-+-+-+\n");
    printf("   ad88888ba   88888888888  88888888ba   88888888ba          db \n");           
    printf("  d8'     '8b  88           88      '8b  88      '8b        d88b          \n"); 
    printf("  Y8,          88           88      ,8P  88      ,8P       d8'`8b          \n");
    printf("  `Y8aaaaa,    88aaaaa      88aaaaaa8P'  88aaaaaa8P'      d8'  `8b         \n");
    printf("    `aaaayy8   88aaaaa      88''''88'    88''''88h       8Yaaaaaa8b       \n");
    printf("          `8b  88           88    `8b    88    `8b      d8''''''''8b       \n");
    printf("  Y8a     a8P  88           88     `8b   88     `8b    d8'        `8b      \n");
    printf("   YY88888PP   88888888888  88      `8b  88      `8b  d8'          `8b     \n");
    printf("            +-+-+-+-+-+-+-+-+-+-+-+ +-+-+-+-+-+-+-+-+-+ \n");
    printf("            |p|r|o|g|r|a|m|m|i|n|g| |l|a|n|g|u|a|g|e|!|  \n");                          
    printf("            +-+-+-+-+-+-+-+-+-+-+-+ +-+-+-+-+-+-+-+-+-+ \n\n\n");
    printf("      +-+-+-+-+-+-+ +-+-+-+-+ +-+-+ +-+-+-+ +-+-+-+-+ +-+-+-+ \n");
    printf("      | |W|r|i|t|e| |h|e|l|p| |i|f| |y|o|u| |n|e|e|d| |i|t| |\n");                          
    printf("      +-+-+-+-+-+-+ +-+-+-+-+ +-+-+ +-+-+-+ +-+-+-+-+ +-+-+-+\n");
    printf("\n");
}

//funzione di avvio nella produzione: exec stmt EOL
void run (struct ast *r){
    char *valEval;
    valEval = eval(r);

    if(valEval != NULL){
        if(!strcmp(valEval,"D")){  
            printf("Operazione di inserimento dispositivo completata con successo\n\n> "); 
                                                                
        }else if ( r->nodetype=='K' || r->nodetype=='C'){       
            printf("%s\n> ", valEval);
                                                                
        }else{
            printf("\n> ");
                                                                
        }
                                                          
    }else{
        printf("\n> ");
    }

    treefree(r);
}


void db(){
    
    FILE *db=fopen ("db", "r");
    
    if (db != NULL){
        yyin=db;
        yyparse(yyin);
    }
}
void lib(){
    
    FILE *lb=fopen ("library", "r");
    
    if (lb != NULL){
        yyin=lb;
    }

}


//Avvio del parser e lexer:
int main()
{
    welcomeMessage();
    printf(">");
    search("ans", NULL);
    
    flag=1;
    db();
    flag=0;
    lib();
    
    return yyparse();

}