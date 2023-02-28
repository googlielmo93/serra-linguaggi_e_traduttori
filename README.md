# Serra
Un linguaggio per l’Ambient Intelligence (AMI), tesina per la materia “Linguaggi e Traduttori” nell'anno accademico 2020/2021

## Autori

- [La Mantia Vincenzo](https://github.com/googlielmo93)
- [Caruso Mario](https://github.com/warioevolution)
- [Gaetano La Bua]()


### Descrizione del progetto

Il progetto **Serra**, nasce dalla volontà di progettare e realizzare, un linguaggio di programmazione di facile utilizzo per la gestione di una classica serra. Il team avendo valutato la crescente richiesta del mercato di dispositivi non invasivi in grado di affiancare i lavoratori durante attività più o meno complesse, ha deciso, come ambito di sviluppo per il progetto, di scegliere l’Ambient Intelligence (AMI). Quando si parla di AMI ci si riferisce ad ambienti elettronici, con a bordo una sostanziale componente software, sensibili e reattivi alla presenza di persone, si ha quindi un ambiente, in cui attraverso una adeguata sensoristica e un opportuno sistema di gestione software, si interconnettono tra loro, dando così una “voce”, macchinari e strumenti di vario genere, permettendo di interagire con questi, direttamente e da remoto, riducendo il carico di lavoro e aumentando l’usabilità.
<br><br>
<img src="https://user-images.githubusercontent.com/10675526/221908120-4a61ad60-c589-47c0-93d1-dd4c6c287a1e.png" width="600"/>
<br><br>



Alla base dell’idea dell’AMI ci sono due concetti fondamentali:
- I dispositivi inseriti non devono essere invasivi
- L’utilizzo deve essere User-Friendly.
Il nostro progetto si è concentrato sul secondo aspetto, ossia sullo sviluppo di un linguaggio
per lo sviluppo di un Ambient Intelligence, rendendo semplice la programmazione e facile
l’utilizzo.
In particolare, si è deciso di sviluppare, un linguaggio per l’automazione di sistemi per la
cura di giardini, orti e serre. A questo progetto abbiamo dato il nome **“SERRA”**.


### Analisi dei requisiti
L’obiettivo nel nostro lavoro è stato quello di rendere il linguaggio il più semplice possibile all’utilizzatore finale. L’utente non si deve rende che verrà usate come struttura dati, ad esempio, una struct device che contiene le informazioni del device, ma semplicemente si occuperà di inserire stringhe ( o al più numeri ) con la quale inserire un nuovo device.
Un'altra peculiarità del linguaggio SERRA, è la flessibilità.
Allo scopo di rendere il programma il più adattabile alle proprie esigenze, si dà la possibilità all’utente di generare nuovi comandi runtime per semplificare possibili operazioni frequenti.

In particolare:
- A livello di giardinaggio le operazioni previste sono:
  - Creazione di una istanza oggetto device;
  - Variazione dello status del device e quindi accenderlo;
  - Variazione dello status del device per un intervallo di tempo;
  - Eliminazione di un dei device.
- A livello di linguaggio classico sono previsti:
  - Cicli iterativi;
  - Definizioni e assegnazioni variabili;
  - Comandi Runtime;
  - Istruzioni Condizionali;
  - Esecuzione di programma scritto su file;

### Descrizione file di utilizzo
Di seguito un  elenco dei file del progetto con una breve descrizione associata:
- Lexer.l -> contiene il lexer
- Parser.y -> contiene il parser
- Lex.yy.c , parser.tab.h -> sono file generati eseguendo il lexer e bison
- Serra.c , Serra.h -> file C e header
- Tutti i file innaffia e irrigazione (saranno descritti in seguito) sono esempi di utilizzo del linguaggio
- Il file DB contiene l’elenco dei device (e sarà caricato all’avvio del linguaggio)
- Library->contiene funzioni di sistema di libreria (e sarà caricato all’avvio del linguaggio)
- Exec.sh -> file di comodo per eseguire tutti i file C, lexer e bison

##Esecuzione del programma

```
$./exec.sh
$./serra
```

<img src="https://user-images.githubusercontent.com/10675526/221912112-22141ecd-a254-489a-999a-f7d0ad2ce890.png" width="700"/>

<br>
## Caratteristiche del Linguaggio
### Grammatica
Per realizzare un analizzatore lessicale è stata utilizzata la libreria Flex. L’analizzatore lessicale si occupa di estrarre i token che definiranno la grammatica del nostro linguaggio.
I token estratti verranno a loro volta presi in input da Bison che si occuperà di effettuare un'analisi sintattica del linguaggio occupandosi delle precedenze tra operatori/token.
La grammatica che è stata realizzata è, secondo la classificazione di Chomsky, di tipo 2 (libera dal contesto) dato che tutti i simboli non terminali (o produzioni) della nostra grammatica vengono tradotte in una sequenza di simboli/o terminali/e e/o non terminali/e:
La caratteristica dei linguaggi di tipo 2 è che possono essere espressi sottoforma di un automa a stati finiti con uno stack di dimensione infinita (in seguito ne rappresenteremo una forma generale di tale diagramma).

### Analizzatore lessicale
È stato realizzato utilizzando la libreria Flex. Tramite Flex vengono individuati i token e quindi la sequenza di caratteri digitati dall’utente in fase di esecuzione del programma.
Nella seconda sezione del lexer sono state individuate le seguenti definizioni regolari da usare con i corrispondenti token. I token individuati, che ritroviamo anche in bison, sono:

<img src="https://user-images.githubusercontent.com/10675526/221913809-ea0b198b-efcc-499e-a0b7-85dfdaf69bc6.png" height="500"/>


#### Descrivendoli nel dettaglio:
**Definizioni regolari:**
- Il token NUMBER è individuato dalla seguente definizione regolare:
  NUMBER -> [0-9]+
  - Ex: 1 , 2, 3, 44
  Tale token consente di inserire nel nostro lessico tutti i numeri interi positivi rappresentabili nel nostro sistema.
- Il token EOL è invece individuato dalla seguente espressione regolare:
  EOL -> \n { return EOL; }
  - Ex: [invio]
  Corrisponderà al terminatore di ogni istruzione. Un'istruzione nel programma termina nel momento in cui l’utente digita invio
- Il token NAME è usato invece per individuare un carattere seguito da una qualsiasi stringa di lettere e numeri (se presente).
  Consente di fatto di individuare le variabili memorizzate nel sistema.
  E’ usato anche per individuare non solo i nomi di variabili ma anche array.
  Verrà anche utilizzato nella definizione di funzioni vedremo.
  NAME -> [a-zA-Z][a-zA-Z0-9]*
  - Ex: ciao , DEVICE1, device2 , d2
- Il token STRING viene utilizzato per individuare le stringhe nel nostro programma nel formato “ciao”:
  STRING-> ["][a-zA-Z][a-zA-Z0-9]*["]
  - Ex: “ciao “, “DEVICE1”, “device2” , “d2”
- Il token DATA viene utilizzato per individuare le date nel nostro programma. Per semplicità stiamo considerando solo il secolo corrente nell’analisi delle date:
  DATE-> [2][0-9]{3}"."(0?[1-9]|1[012])"."(0?[1-9]|[12][0-9]|3[01])"."(0?[1-9]|1[0-9]|2[0-3])"."(0?[0-9]|[1-5][0-9])
  - Ex: 2021.01.01.9, 2021.11.01.9, 2021.01.31.9, 2020.01.01.9
  - Errori sintattici: 2021.01.01.90, 2021.25.01.9, 3021.01.01.9, 2021.13.01.9
Di fatto questi sono i token principali delle definizioni regolari necessari nel nostro linguaggio. 
Tramite i token identificati infatti la nostra grammatica consente di individuare finora con 3 token diversi:
- Tutti i possibili numeri (token number),
- Tutte le possibili stringhe tra virgolette (token string),
- Tutte le possibili date,
- Tutte i possibili nomi di variabili e funzioni (token name);

Notare che la differenza tra le stringhe e le variabili stanno che le prime iniziano con le “”, le
seconde invece no.
Vuol dire che:
  - “Ciao” verrà riconosciuto col token STRING
  - Ciao verrà riconosciuto col token NAME

Nel nostro sistema di giardinaggio si prevede infatti che tutte le funzioni embedded, tutte le funzioni Runtime e tutte le operazioni necessarie pretendano il riconoscimento solo di questi tre tipi di token.
I token successivi consentiranno di individuare parole chiavi del linguaggio, il quale significato è evidente:
- I token IF, THEN, ELSE sono individuati dalle espressioni regolari “if”, “then”, “else”.
  Ovviamente consentono di individuare i caratteri che consentiranno con l’analisi sintattica di effettuare le operazioni condizionali
- I token DO, WHILE sono individuati analogamente dalle espressioni regolari “do”, “while”.
- I token REPEAT individuano un ciclo iterativo da ripetere un certo numero di volte fissato come parametro
Altri token che individuano parole chiavi sono state inseriti a individuare funzioni di sistema che sono:
- SYSTEM
  - clear individuata dall’espressione “clear”;
  - help individuata dall’espressione “help”;
- FUNC
  - readFile individuata dall’espressione “readFile”.

Gli altri token ADD, REMOVE, GET, SET (che individuano rispettivamente le espressioni:
add, get, set e remove) sono le operazioni base utilizzati per la gestione di liste e array come tipi compositi del linguaggio. In seguito, verrà fatta un'analisi approfondita di tali token.
- L’array avrà un tipo definito per cui sono presenti i token:
  - CHAR che identifica l’espressione “char”
  - INTEGER che identifica l’espressione “integer”
  - DEVICE che identifica l’espressione “device”
- Il token ARRAY che individua l’espressione “newArray” necessaria per la creazione dell’array
- Il token add verrà riconosciuto nel momento in cui l’utente digita add
- Il token remove analogamente verrà riconosciuto con l’espressione remove
- Set e get analogamente con le espressioni set e get
Gli ultimi token che descriviamo sono quelli che individuano parole chiavi necessari a definire funzioni embedded:
- FUNCDEV individua tutte le funzioni embedded necessarie a collegare i device per avviare le operazioni di giardinaggio.
  Tali funzioni sono individuate dalle espressioni:
    “connect”, “reconnect”, “switchOn”, “switchOff”, “status” e “archive”
- INTERVAL individua l’espressione “interval” e anche questa è una funzione di tipo
  FUNCDEV (è stato preferito però inserirla con un token differente perché al contrario delle altre funzioni, come vedremo nell’analisi sintattica prevede tre parametri).
- FUNC individua invece tutte le funzioni di sistema generali non strettamente legata ai device, per esempio la print. Il token verrà riconosciuto quando l’utente scriverà: print o clear.

Tutti i token finora descritti sono quelli utilizzati per effettuare l’analisi lessicale del nostro linguaggio.
In funzione di questi token l’analizzatore sintattico si occuperà di valutare le sentence indicate dall’utente.
Quindi coi nostri token in generale individuiamo:
- Funzioni di sistema (clear, help e readFile),
- Funzioni di device (connect, reconnect, status, interval ecc),
- Operazioni sui tipi compositi (add, get, remove e set),
- Costanti (stringhe e numeri),
- Variabili e funzioni runtime (individuate da una lettera seguite da una sequenza di lettere e numeri).

I restanti digit, ovviamente, non verranno riconosciuti dall’utente e verranno segnalati come caratteri sconosciuti e quindi come errore lessicale.

Nella seguente tabella riassumiamo quanto spiegato in precedenza:


<img src="https://user-images.githubusercontent.com/10675526/221917413-55d76ae1-7fec-43c1-b9cb-e0bab12dd3c3.png" height="600"/>

## Descrizione del Parser
### Analizzatore sintattico
È stato realizzato utilizzando la libreria Bison.
Lo scopo nel nostro parser è individuato dalla produzione exec. Derivando più volte tale produzione riusciamo ad ottenere l’analisi sintattica del nostro linguaggio.-
Per effettuare tale analisi scegliamo di effettuare un'analisi discendente del nostro albero sintattico.
Le principali produzioni che definiscono la nostra grammatica sono le seguenti:
 Exec: rappresenta lo scopo. Ogni Statement verrà eseguita tramite tale produzione. 
Da essa infatti avviene lo scorrimento dell’albero sintattico.
- Statement: È un simbolo non terminale che consente l’individuazione della nostra istruzione.
Tale produzione si occuperà infatti di riconoscere opportunamente il comando definito dall’utente.

Riconosce quindi:
  - If, then, else;
  - Do, while;
  - Repeat;
  - Espressioni.
- Exp: È un simbolo non terminale che consente l’individuazione delle:
  - Variabili, funzioni di sistema/embedded e funzioni runtime.
- Type: In type sono stati inseriti tutti i tipi considerati primitivi nel linguaggio, o comunque derivanti da una combinazione di stringhe e numeri.
Di conseguenza un type è:
  - Una stringa, un numero, una data, una sequenza particolare di parole chiavi per creare un device
Per comprendere meglio, analizziamo l’albero sintattico delle diverse espressioni regolari:
**EXP:**
  È un simbolo non terminale per individuare tutte le espressioni presenti nel linguaggio.
  Un exp/espressione può essere:
  - Un simbolo non terminale type.
  Type è un simbolo non terminale del linguaggio che identifica i simboli terminali rappresentati dai tipi primitivi e/o compoisiti del linguaggio.
  Type può quindi essere:
    - Un numero (simbolo terminale): vuol dire che l’utente ha digitato un numero riconosciuto dal token NUMBER
      Exp -> Type; Type-> NUMBER
        - 2 3 5
    - Una stringa (simbolo terminale): vuol dire che l’utente ha digitato una stringa riconosciuto dal token STRING
      Exp-> Type; Type-> STRING
      - “ciao” “pippo”
    - Definizione di un tipo device: per la definizione di un device sono state usate due espressioni:
      - La prima prevede di accostare due simboli terminali rappresentati da
      INSERT (individuato dalla parola chiave “newDevice”) e STRING:
        tale istruzione sarà quindi del tipo:
        EXP-> Type; Type-> INSERT STRING
- NewDevice “pippo”
      - La seconda consente non solo di creare un device ma anche di indicare
      che è collegato ad altri device:
      EXP-> Type; Type->INSERT STRING ARROW '[' argsListDevice ']'
- NewDevic “pippo” -> [“ciao”]
Quindi un'espressione è un type che può esssere o una stringa o un intero o un device, che sono i tipi base previsti nel linguaggio.
- Una variabile (simbolo terminale): vuol dire che l’utente ha digitato un nome di variabile riconosciuto dal token NAME
Exp->NAME
  - Ciao pippo
- Una funzione embedded (simbolo terminale) e un ulteriore espressione (simbolo non terminale): la funzione embedded è individuata dal token FUNCDEV che è riconosciuto nel momento in cui l’utente avrà richiamato una delle funzioni di sistema riportate nella figura precedente (connect, reconnect, interval ecc).
Tale funzione pretenderà dei parametri che potranno essere rappresentati da un ulteriore espressione.
L’espressione exp (che stiamo descrivendo in tale sezione) può essere: una stringa, un numero, ecc.
Per cui le funzioni embedded (connect, etc) verranno richiamate prendendo come parametri o i token STRING, NAME, NUMBER, etc, oppure magari contenendo altre funzioni come parametri al loro interno. In quest’ultimo caso verrà eseguita la funzione più interna per poi valutare la funzione più esterna. Sarà compito
dell’albero controllare l’eventuale errore di conseguenza nel passaggio dei parametri.
E’ stata prevista in quest’ultimo caso la possibilità di annegare più funzioni embedded (vedremo in segutio):
Exp->FUNCDEV exp
  - Esempio in cui passo come parametro un simbolo terminale:
    - Connect “ciao”
    - Connect “pippo”
    - Connect “pluto”
    - SwitchOn “nome”
    - SwitchOff 1
    - Connect 2
    - SwitchOn connect “pippo”
  - Esempio in cui passo come parametro un simbolo non terminale:
    - Connect switchOff “pluto”: passo come parametro una funzione embedded.
    - Connect var: passo come parametro una variabile.
    - SwitchOn connect “pippo”: esempio di annidamento in cui passo come parametro di connect la stringa “pippo”, e come parametro di switchOn il valore restituito dall’espressione connect “pippo”.
Questo ultimo aspetto è possibile perché nella nostra grammatica abbiamo considerato come un'espressione anche i parametri da passare alla funzione, e non come un simbolo terminale.
- Una funzione di sistema (simbolo terminale) seguita da un'espressione: è analogo alla funzione embedded spiegata in precedenza. Ancora una volta, i parametri di questa funzione possono essere una qualsiasi espressione.
  Exp-> FUNC exp
- Una variabile (simbolo terminale) seguito da un uguale (simbolo terminale) seguito da un'espressione (simbolo non terminale): una variabile (che è semplicemente una
stringa) è individuata infatti dal token descritto nella sezione dell’analisi lessicale.
E’seguita dal simbolo terminale uguale seguita da un'espressione.
Ancora una volta, (come nel caso delle funzioni embedded) tale espressione può rappresentare: un
numero, una stringa, una chiamata a funzione embedded, ecc.
  Exp-> NAME '=' exp
  - Esempio in cui passo come parametro un simbolo non terminale:
    - NomeVariabile = connect “pippo”
    - NomeVaribaile = switchOn “pluto”
    - NomeVariabile = NomeVariabile = 2
  - Esempio in cui passo come parametro un simbolo terminale:
    - NomeVariabile = “pippo”
    - NomeVariabile = “pluto”
    - NomeVariabile = 1
    - Interval: analogo alle funzioni embedded
    - Una espressione può infine anche essere una lista. La lista è un ultriore simbolo non terminale che consente di individuare tutte le operazioni possibili con un linguaggio.
Una lista infatti può essere:
  - Creazione array: per creare l’array usa i token ARRAY NAME NUMBER e il simbolo non terminale nameType:
    - Il simbolo non terminale nameType identifica i simboli terminali per effettuare la tipizzazione dll’array. Infatti, può essere rappresentato dai token:
- Char
- Integer
- Device
    - Esempio: newArray integer nomeArray (2);
- NewArray integer sono identificati dai token ARRAY e
  INTEGER (descritti in precedenza). NomeArray è invece individuato dal token nomeArray
  - Add: Un NAME seguito da ARROW seguito da ADD seguito da = seguito dal simbolo non terminale type (descritto in precedenza, identifica: stringhe, interi e device):
    Esempio: nomeArray -> add = 2
- NomeArray è individuato dal token NAME, -> token arrow, add dal token ADD, invce 2 dal simbolo non terminale type (che abbiamo potere essere individuata dai token: string,
number e newDevice)
  - Set:
     Esempio: nomeArray -> set = 1,2
- I token sono gli stessi di add solo che l’ultimo parametro (2) individua il simbolo termibnale NUMBER
  - Remove: Analoga a add ma non pretende come parametro un'espressione type:
    - Esempio: nomeArray->remove
- Individuata dai token NAME, REMOVE E ARROW
  EXEC (esecuzione del comando):
Una volta individuata l’espressione exp e l’istruzione stmt si potrà risalire quindi allo scopo exec da cui si inizierà a scorrere l’albero per poter eseguire l’istruzione individuata:
  EXEC -> exec stmt EOL
L'exec (esecuzione del comando) è dunque un simbolo non terminale dato da una sequenza di un comando eseguito in precedenza (exec), da una nuova istruzione (stmt) e dall’inizio (EOL). Di conseguenza il ritorno a capo cioè il carattere EOL individua la fine di ogni comando.
In generale, quindi, un comando sarà seguito a partire da questa espressione. Si specifica, però, che l’istruzione da eseguire a partire dalla quale scorrere l’albero è contenuto nella statement.

...

...

...

## Per maggiori approfondimenti si consiglia di scaricare il file "TESINA.pdf" contenuto nel repository.
