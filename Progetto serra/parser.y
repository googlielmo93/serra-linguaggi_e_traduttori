/* Parser per Progetto Serra - Gruppo La Bua, Caruso, La Mantia */

%{
#  include <stdio.h>
#  include <stdlib.h>
#  include "serra.h"



%}

%union {
  struct ast *a;
  double d;
  char * str;
  struct symbol *s;       /* Specifica quale simbolo si sta passando*/
  struct argsList *sl;
  int  func;              /* Specifica quale funzione built-in deve essere eseguita */
}

/* declare tokens */
%token <d> NUMBER
%token <str> STRING
%token <s> NAME
%token <func> FUNC
%token <func> SYSTEM
%token <func> FUNCDEV
%token <func> INTERVAL
%token <func> INSERT
%token <str> ARROW
%token <s> DATA
%token <func> ADD
%token <func> GET
%token <func> SET
%token <func> REMOVE
%token <func> SLEEP

%token <func> ARRAY
%token <d> INTEGER
%token <d> CHAR
%token <d> DEVICE


%token EOL
%token <str> TERM
%token IF THEN ELSE WHILE DO CMD HELP RET REPEAT

%type <a> exp stmt listStmt explistStmt
%type <a> liste
%type <a> type
%type <sl> argsList
%type <sl> argsListDevice
%type <d> nameType
%type <a> function

%start exec

%%

exec:  /* nothing */
    | exec stmt EOL                             { run ($2);                                     }
    | exec function EOL                         { eval($2);  treefree($2);  printf("\n> ");     }                       
    | exec TERM EOL                             { fclose (yyin);                                }    
    | exec error EOL                            { printf("\n> "); yyerrok;                      }                       
;

 
stmt: IF exp THEN listStmt                      { $$ = newContent('I', $2, $4, NULL);           }
    | IF exp THEN listStmt ELSE listStmt        { $$ = newContent('I', $2, $4, $6);             }
    | WHILE exp DO listStmt                     { $$ = newContent('W', $4, $2, NULL);           }
    | REPEAT NUMBER DO listStmt                 { $$ = newRepeat('R', $2, $4);                  }
    | exp
;


function:   CMD NAME '(' argsList ')' '=' listStmt RET exp ';'    { $$ = defFunction ($2, $4, $7, $9);      }
          | CMD NAME '(' argsList ')' '=' listStmt                { $$ = defFunction ($2, $4, $7, NULL);    }                                       
;


listStmt:  /* nothing */     { $$ = NULL;                                 }
        | stmt ';' listStmt  { $$= ($3==NULL ? $1 : newast('L', $1, $3)); }
;

exp:
    '(' exp ')'                         { $$ = $2;                                         }  
   | type
   | FUNC exp                           { $$ = newfunc($1, $2, NULL, NULL);                }                                 
   | FUNCDEV exp                        { $$ = newfunc($1, $2, NULL, NULL);                }
   | SYSTEM                             { $$ = newfuncSystem($1);                          }                                       
   | NAME                               { $$ = newref($1);                                 }                
   | NAME '=' exp                       { $$ = newasgn($1, $3);                            }               
   | INTERVAL exp '-' exp '-' exp       { $$ = newfunc($1, $2, $4, (struct ast *)$6);      }
   | NAME '(' explistStmt ')'           { $$ = newcall($1, $3);                            }
   | liste
;


liste: ARRAY nameType NAME '(' NUMBER ')'          { $$ = newAstArray($3, $5, $2);                                  }
        | NAME ARROW  ADD '=' type                 { $$ = newfuncArray($3, $5, $1, NULL);                           }
        
        | NAME ARROW GET                           { $$ = newfuncArray($3, NULL, $1, NULL);                         }
                                            
        | NAME ARROW  GET '=' NUMBER               { $$ = newfuncArray($3,  newnum($5), $1, NULL);                  }
                                            
        | NAME ARROW  SET '=' type ',' NUMBER      { $$ = newfuncArray($3, $5, $1, newnum($7));                     }
                                                      
        | NAME ARROW REMOVE                        { $$ = newfuncArray($3, NULL, $1, NULL);                         }
       
;

nameType: CHAR {$$=2;} | INTEGER {$$=1;} | DEVICE {$$=3;}

;

type:
          INSERT STRING                                 { defSymRef((struct symbol *)$2, NULL, NULL, NULL); $$ = newDev((struct symbol *)$2,NULL);  }
        | NUMBER                                        { $$ = newnum($1);                                                                          }
        | STRING                                        { $$ = newString((struct symbol *)$1);                                                      }
        | INSERT STRING ARROW '[' argsListDevice ']'    { defSymRef((struct symbol *)$2, $5, NULL, NULL);   $$ = newDev((struct symbol *)$2,$5);    }
        | DATA                                          { $$ = newString((struct symbol *)$1);}
;


//con 1 parametro comportamento standard: $$=$1
explistStmt: exp 
           | exp ',' explistStmt  { $$ = newast('L', $1, $3); }         
;


argsList: NAME              { $$ = newargsList($1, NULL); }
        | NAME ',' argsList { $$ = newargsList($1, $3);   }
;


argsListDevice: STRING                     { $$ = newargsList((struct symbol *)$1, NULL); }
               | STRING ',' argsListDevice { $$ = newargsList((struct symbol *)$1, $3);   }
;


%%