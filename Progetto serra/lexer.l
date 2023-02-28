/* Lexer per Progetto Serra - Gruppo La Bua, Caruso, La Mantia */

%option noyywrap nodefault yylineno
%{
# include "serra.h"
# include "parser.tab.h"
%}

%%
 /* Operazioni a singolo carattere: servono per i singoli apicetti nello scanner */
"-" { return yytext[0]; }
"=" { return yytext[0]; } 
"," { return yytext[0]; }
";" { return yytext[0]; }
"(" { return yytext[0]; }
")" { return yytext[0]; }
"." { return yytext[0]; }
"[" { return yytext[0]; }
"]" { return yytext[0]; }


 /* Parole chiavi */
"if"    {  return IF;   }
"then"  {  return THEN; }
"else"  { return ELSE;  }    
"while" { return WHILE; }
"do"    { return DO;    }


 /* Funzioni incorporate */
"sleep"                 { yylval.func = B_sleep; return FUNC;                   }
"print"                 { yylval.func = B_print; return FUNC;                   }
"readFile"              { yylval.func = B_readFile; return FUNC;                }
<<EOF>>                 {  if(flag==0) yyrestart (stdin); else yyterminate ();  }
                        
"bye"                   { return TERM;                                          }

 /* Funzioni per il tipo device */
"connect"               { yylval.func = B_connect; return FUNCDEV;      }
"reconnect"             { yylval.func = B_reconnect; return FUNCDEV;    }
"status"                { yylval.func = B_status; return FUNCDEV;       }
"switchOn"              { yylval.func = B_switchOn; return FUNCDEV;     }
"switchOff"             { yylval.func = B_switchOff; return FUNCDEV;    }
"delete"                { yylval.func = B_delete; return FUNCDEV;       }
"interval"              { yylval.func = B_interval; return INTERVAL;    }
"newDevice"             { yylval.func = B_insertDevice; return INSERT;  }

 /* ARROW necessario per le associazioni tra dispositivi*/
"->"                    { return ARROW;                                 }


 /* Modalità User */
"CMD"                   { return CMD;                                   }



"newArray"              { return ARRAY;                                 }
"integer"               { return INTEGER;                               }
"char"                  { return CHAR;                                  }
"device"                { return DEVICE;                                }
"add"                   { yylval.func = B_add; return ADD;              }
"get"                   { yylval.func = B_get; return GET;              }
"set"                   { yylval.func = B_set; return SET;              }
"remove"                { yylval.func = B_remove; return REMOVE;        }
"ret"                   { return RET;                                   }
"repeat"                { return REPEAT;                                }
 
 /* Funzioni senza parametri, funzionalità di sistema */
"clear"                 { yylval.func = B_clear; return SYSTEM;         }
"help"                  { yylval.func = B_help; return SYSTEM;          }


[2][0-9]{3}"."(0?[1-9]|1[012])"."(0?[1-9]|[12][0-9]|3[01])"."(0?[1-9]|1[0-9]|2[0-3])"."(0?[0-9]|[1-5][0-9]) { 
                        yylval.s = search(yytext, NULL); 
                        return DATA; 
}


 /* Stringhe alfanumeriche.*/
["][a-zA-Z][a-zA-Z0-9]*["] {            
                                int dimString= strlen(yytext)+1;
                                char subbuff[dimString-2];
                                memmove( subbuff, &yytext[1], dimString-3 );
                                subbuff[dimString-3] = '\0';
                                yylval.s = search(subbuff, NULL); 
                                return STRING; 
                           }


 /* SERVE PER DEFINIRE I SIMBOLI PER I NOMI DELLE FUNZIONI */
[a-zA-Z][a-zA-Z0-9]*  { yylval.s = search(yytext, NULL); return NAME;   }            

 /* Numeri in virgola mobile */
 /* Prende la prima cifra intera dopo la virgola. */
[0-9]+                { yylval.d = atof(yytext); return NUMBER;         }


 
[ \t]   /*IGNORA SPAZIO E TAB*/ 
\\\n    printf("continue... > ");    /* IGNORA CONTINUAZIONE DI LINEA A CAPO*/
\n      { return EOL; }

.       { yyerror("Carattere Sconosciuto %c\n", *yytext); }


%%