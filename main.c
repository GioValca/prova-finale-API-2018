// perfetto
//  main.c
//  ProgettoApi
//
//  Created by Giovanni Valcarenghi on 26/08/18.
//  Copyright © 2018 Giovanni Valcarenghi. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define L 128 // grandezza inziale array dinamico per transizioni


int a=0; // serve solo per scanf sennò dice che ignorocil ritorno
//struct delle transizioni
typedef struct node{
    int from;
    char read;
    char write;
    char move;
    int to;
    struct node *down;
} node;

typedef node *new_node;

//struct per struttura non determinismo (tutti gli stati non deterministici)
typedef struct nd{
    char *run_nsx;
    unsigned long dimmaxsx;
    char *run_ndx;
    unsigned long dimmaxdx;
    int stato;
    int indicerun;
    short int flag; // se 0 sono sul nastro di sx, 1 se sono su quello di dx
    short int loop; // questa trans è in loop se =1
    struct nd *next;
    struct nd *prec;
}nd;

typedef nd *new_node_nd;

//array dinamico
new_node *array; // array dinamico per transizioni
int *accettazione; // array dinamico per accettazione
int k=127; // numero di stato massimo che posso accettare nell'array fino a questo punto
int f=1; // numero di celle per array di accettazione
char *runtemp =NULL; // array dinamico stringhe run
int n=10; // grandezza iniziale array dinamico per stringhe run
int long max=0; // massimo numero di iterazioni
int risultato=3;

// dichiarazione funzione inserimento delle transizioni
void inserimentoincoda( int fromtemp, char readtemp, char writetemp, char movetemp, int totemp);

// dichiarazione funzione ingrandisce array per transizioni
new_node * ingrandisciarray( new_node * array, int fromtemp);

// dichiarazione funzione ingrandisci array per accettazione
int * ingrandisciaccettazione( int * accettazione, int acctemp);

// dichiarazione funzione mossa per duplicato
char avantimove;
void mossaduplico (new_node_nd duplico, char avantimove, new_node avanti);

// dichiarazione funzione fa mossa in pippo ( cioè faccio mossa sull'originale, i non determinismi sono duplico)
void mossapippo (new_node_nd pippo, char move_det, new_node avantidet);


int main(int argc, const char * argv[]) { //---------------------- MAIN
    
    
    //creo e inizializzo array dinamico transizioni
    int i=0;
    array= (new_node *) malloc((L)*sizeof(new_node));
    for (i=0; i<=(L-1); i++) {
        array[i]=NULL;
    }
    
    // setta il  file fp da cui partire ********************************************
    //lettura da input
    char transtemp[5];
    i=0;
    int contacaratteri=0;
    int fromtemp=999;
    char readtemp='@';
    char writetemp='@';
    char movetemp='@';
    int totemp=999;
    FILE *fp;
    fp=stdin;
    
    //verifico inizi con tr
    a=fscanf(fp,"%s",transtemp);
    
    if (strcmp(transtemp, "tr")==0) {
        
        do {
            contacaratteri= fscanf(fp,"%d %c %c %c %d", &fromtemp, &readtemp, &writetemp, &movetemp, &totemp);
            // se ho letto 5 caratteri allora è una transizione e quindi la salvo
            if (contacaratteri==5) inserimentoincoda(fromtemp, readtemp, writetemp, movetemp, totemp);
            else {
                //printf ("transizioni finite");     serviva solo per il debug
                break;
            }
        } while (contacaratteri==5);
        //quando sarà uscito dal while vuol dire che c'è 'acc', lo verifico
        a=fscanf(fp,"%s", transtemp);
        if (strcmp(transtemp, "acc")==0){
            int j=0;
            accettazione= (int *) malloc((1)*sizeof(int));
            for (j=0; j<1; j++) {
                accettazione[j]='9';
            }
            j=0;
            int acctemp=0; // variabile che mi serve solo per passare il valore alla funzione
            do{
                if (j==f) {
                    contacaratteri=fscanf(fp,"%d", &acctemp);
                    
                    if (contacaratteri==0) {
                        break;
                    }
                    else {
                        accettazione=ingrandisciaccettazione(accettazione, acctemp);
                        j++;
                    }
                }
                else {
                    contacaratteri=fscanf(fp,"%d", &accettazione[j]);
                    j++;
                }
                
            }while (contacaratteri==1);
            //esce quando sono finiti gli stati di accettazione
            a=fscanf(fp,"%s", transtemp);
            if (strcmp(transtemp, "max")==0){
                a=fscanf(fp,"%ld",&max);
                a=fscanf(fp,"%s", transtemp);  // *************** si può togliere
                if (strcmp(transtemp, "run")==0) {
                    
                    // ** INIZIO PROCEDIMENTO SCASSA CAZZO
                    // inizializzazioni che prima erano dentro la risoluzione ma che prendendo una nuova stringa run non vanno rimesse
                    
                    //inizializzo la testa
                    new_node_nd head;
                    new_node_nd coda;
                    new_node_nd pippo; // puntatore per scorrere in next tra gli stati nd
                    new_node_nd duplico;// quando duplico nd creo subito lista
                    new_node_nd pippob; // serve solo a cancellare pippo e ricordarmi del pippo->next
                    new_node avanti; // puntatore per scendere in down
                    new_node avantidet=NULL; // serve per ricordarmi di tutta transizione avanti in caso di det
                    int d;
                    int dtemp;
                    int qualeif=0; // serve per capire in che caso sono nelle free
                    
                    int y=0; // serve al for per scorrere stati di accettazione
                    head=malloc(sizeof(nd));
                    head->prec=NULL;
                    pippo=malloc(sizeof(nd));
                    pippo->prec=head;
                    head->next=pippo;
                    char *ingrandiscisx;
                    ingrandiscisx=malloc(sizeof(char));
                    
                    head->dimmaxsx=0;
                    head->dimmaxsx=0;
                    head->loop=0;
                    head->stato=0;
                    head->indicerun=0;
                    
                    char ch=getc(fp);
                    short int saltatutto=0;
                    char * run;
                    
                    
                    while ((ch=getc(fp))!=EOF) {
                        //*** inizio lettura run
                        saltatutto=0;
                        int len=100;
                        runtemp=malloc(100*sizeof(char));
                        i=0;
                        do {
                            if (i+1>len) {
                                runtemp=realloc(runtemp, (len+100)*sizeof(char));
                                len=len+100;
                            }
                            if (ch=='\n') { // vuol dire che c'è una stringa vuota, solo \n
                                saltatutto=1;
                                free(runtemp);
                            }
                            else {
                                runtemp[i]=ch;
                                ch=getc(fp);
                                i++;
                            }
                        } while (ch!='\n'&&ch!=EOF);
                        
                        
                        if (saltatutto==0) {
                            // ho tutta la stringa con anche \n
                            run=malloc((i+1)*sizeof(char));
                            for (len=0; len<i; len++) {
                                run[len]=runtemp[len];
                            }
                            run[i]='\0'; // metto carattere terminatore
                            
                            len=100;
                            free(runtemp);
                            
                            risultato=3;
                            
                            // fine lettura nastro run
                            // setto la testa con le uniche cose che ho per adesso
                            pippo->run_ndx=run;
                            pippo->run_nsx=malloc(sizeof(char));
                            pippo->run_nsx[0]='\0';
                            pippo->dimmaxdx=(i); // non conto lo \0
                            pippo->dimmaxsx=0;
                            pippo->flag=1; // parto sempre dal nastro di dx
                            pippo->stato=0;
                            pippo->indicerun=0;
                            pippo->loop=0;
                            pippo->next=NULL;
                            coda=pippo;
                            
                            d=1; // conto quanto lunga la mia fila di stati nd, lunghezza iniziale 1
                            dtemp=d;
                            // adesso ho in run la prima stringha
                            
                            // verifico se la stringa è accettata
                            
                            i=0; // conto se ho fatto una mossa in uno stato
                            int q=1; // q serve per arrivare a d
                            //tutte cose da dichiarare sopra e inizializzare qua
                            char write_det=0; // salvo gli stati in caso di non ancora non determinismo, gli scrivo alla fine di avanti null
                            char move_det=0; // inzializzati solo perchè sennò rompe
                            int to_det=0;
                            
                            int numfree=0; // devo tenerne conto per il for che finisce anche con d, e non posso toglierlo subito perchè sono ancora dentro il ciclo
                            int numadd=0;
                            
                            
                            // ciclo grande, ogni iterazione è un passetto in più su tutte le mosse
                            for (int long m=0; m<max && risultato!=0 && risultato!=1; m++) {
                                pippo=head->next; // riparto sempre da head
                                numfree=0;
                                numadd=0;
                                // la coda al secondo giro di for deve essere l'ultima cella pippo, quindi non la rinizializzo
                                i=0;
                                q=1;
                                
                                while (q<=d && risultato!=1) { // mi sto muovendo tra gli stati nd, non metto risultato 0 perchè tanto se succede ho gia finito
                                    i=0; // conta numero di nd ( se i=1, c'è solo un determinismo) che ci sono in uno stato con la corrente configurazione
                                    if (pippo->loop==0) {
                                        
                                        avanti=array[pippo->stato]; // vado nelle transizioni che partono da pippo->stato
                                        if (pippo->stato<=k){
                                            if (avanti!=NULL && risultato!=1){
                                                while (avanti!=NULL && risultato!=1) {
                                                    if ((pippo->flag==1 && (pippo->run_ndx)[pippo->indicerun]==avanti->read) || (pippo->flag==0 && (pippo->run_nsx)[pippo->indicerun]==avanti->read))  {
                                                        
                                                        //controllo su loop del tipo 5 a a S 5
                                                        if (avanti->move=='S' && avanti->from==avanti->to && avanti->read==avanti->write) {
                                                            // allora questa transizione mi porta in un loop, la salto e setto risultato a 4, per ricordarmi che questo è U
                                                            risultato=4;
                                                            
                                                        }
                                                        
                                                        else {
                                                            i++;
                                                            if (i<=1) {
                                                                // sono ancora in un caso di determinismo
                                                                write_det=avanti->write;
                                                                move_det=avanti->move;
                                                                to_det=avanti->to;
                                                                avantidet=avanti;
                                                            }
                                                            else {
                                                                //sono in caso di non determinismo, es i=2, uno lo faccio fuori con l'originale che modifico dopo, e l'altro l'ho appena duplicato
                                                                //duplico tutto
                                                                
                                                                numadd=numadd+1;
                                                                duplico=NULL;
                                                                duplico=malloc(sizeof(nd)); // lista per nuovo nd
                                                                duplico->next=NULL;
                                                                duplico->prec=NULL;
                                                                duplico->dimmaxdx=pippo->dimmaxdx;
                                                                duplico->dimmaxsx=pippo->dimmaxsx;
                                                                duplico->flag=pippo->flag;
                                                                duplico->stato=pippo->stato;
                                                                duplico->indicerun=pippo->indicerun;
                                                                duplico->run_ndx= malloc(((pippo->dimmaxdx)+1)*sizeof(char));
                                                                duplico->run_nsx=malloc(((pippo->dimmaxsx)+1)*sizeof(char));
                                                                strcpy(duplico->run_ndx, pippo->run_ndx);
                                                                duplico->run_ndx[pippo->dimmaxdx]='\0';
                                                                strcpy(duplico->run_nsx, pippo->run_nsx);
                                                                duplico->run_nsx[pippo->dimmaxsx]='\0';
                                                                duplico->loop=0;
                                                                // concateno duplico e vado avanti di uno in coda
                                                                coda->next=duplico;
                                                                duplico->prec=coda;
                                                                coda=coda->next;
                                                                
                                                                //faccio mossa con il duplicato
                                                                duplico->stato=avanti->to;
                                                                if (duplico->flag==1) {
                                                                    (duplico->run_ndx)[duplico->indicerun]=avanti->write;
                                                                }
                                                                else if (duplico->flag==0){
                                                                    (duplico->run_nsx)[duplico->indicerun]=avanti->write;
                                                                }
                                                                avantimove=avanti->move;
                                                                mossaduplico(duplico, avantimove, avanti);
                                                                
                                                                
                                                            }// fine duplicazione per nd
                                                        } // chiuso else per duplicazione
                                                    }
                                                    avanti=avanti->down;
                                                }//-------- fine while avanti !=null
                                            }
                                            else { // cioè avanti è subito NULL, no transizioni uscenti da quello stato
                                                //guardo se stato di array =NULL è di accettazione
                                                for ( y=0; y<f; y++) {
                                                    if (pippo->stato==accettazione[y]) {
                                                        risultato=1;
                                                    }
                                                }
                                            }
                                        } // fine if pippo->stato <k
                                    } // fine if pippo->loop==0
                                    
                                    if (i>=1) {// vuol dire che non mi sono bloccato in questa configurazione pippo
                                        // faccio mossa sull'originale
                                        pippo->stato=to_det;
                                        if (pippo->flag==1) {
                                            (pippo->run_ndx)[pippo->indicerun]=write_det;
                                        }
                                        else if (pippo->flag==0){
                                            (pippo->run_nsx)[pippo->indicerun]=write_det;
                                        }
                                        else printf("errore");
                                        
                                        mossapippo (pippo, move_det, avantidet);
                                        pippo=pippo->next;
                                        
                                    }
                                    
                                    // esle, i=0, cioè se in questa cella nd non ho fatto nessuna mossa, la cancello, perchè sono bloccato
                                    // elimino la cella pippo
                                    else {
                                        
                                        numfree=numfree+1;
                                        dtemp=d+numadd;
                                        dtemp=d-numfree;
                                        if (dtemp==0) {
                                            // vuol dire che è l'ultima cella rimasta, faccio un procedimento diverso perchè sennno dopo accedevano a null->next
                                            // qua non cancello pippo, perchè tanto almeno uno mi servirà, però lo inizializzo
                                            pippo->next=NULL;
                                            pippo->indicerun=-2;
                                            free(pippo->run_ndx);
                                            free(pippo->run_nsx);
                                            pippo->run_nsx=NULL;
                                            pippo->run_ndx=NULL;
                                            pippo->dimmaxdx=0;
                                            pippo->dimmaxsx=0;
                                            pippo->flag=1;
                                            pippo->stato=-2;
                                            coda=pippo;
                                            if (risultato==3) {
                                                risultato=0;
                                            }
                                            else if (risultato==4){
                                                risultato=4;
                                            }
                                        }
                                        else{
                                            qualeif=0;
                                            if (pippo==head->next) { // vuol dire che pippo è la prima cella,
                                                
                                                
                                                head->next=pippo->next;
                                                pippo->next=NULL;
                                                pippo->prec=NULL;
                                                free(pippo->run_ndx);
                                                pippo->run_ndx=NULL;
                                                free(pippo->run_nsx);
                                                pippo->run_nsx=NULL;
                                                free(pippo);
                                                pippo=head->next;
                                                pippo->prec=head;
                                                qualeif=1;
                                                
                                            }
                                            if (pippo->next==NULL && qualeif==0) { // vuol dire che è l'ultima cella
                                                coda=(pippo->prec);
                                                coda->next=NULL;
                                                pippo->prec=NULL;
                                                free(pippo->run_ndx);
                                                pippo->run_ndx=NULL;
                                                free(pippo->run_nsx);
                                                pippo->run_nsx=NULL;
                                                free (pippo);
                                                qualeif=1;
                                                // non rinizializzo pippo perchè tanto visto che sono arrivato all'ultima cella ripartirò dalla prima cella nd
                                                pippo=head->next;
                                            }
                                            if (qualeif==0){ // caso in cui è in mezzo la cella da cancellare
                                                (pippo->prec)->next=pippo->next;
                                                (pippo->next)->prec=pippo->prec;
                                                pippob=pippo->next;
                                                pippo->next=NULL;
                                                pippo->prec=NULL;
                                                free(pippo->run_ndx);
                                                pippo->run_ndx=NULL;
                                                free(pippo->run_nsx);
                                                pippo->run_nsx=NULL;
                                                free(pippo);
                                                pippo=pippob;
                                            }
                                        }
                                        
                                    }
                                    
                                    // pippo l'ho già mandato avanti in base al caso in cui era
                                    q++;
                                    
                                }// fine while q<=d
                                
                                d=d+numadd;
                                d=d-numfree;
                            }// fine FOR una mossa per tutti i nd
                            
                            if (risultato==1) {
                                printf("1\n");
                                //libera tutto spazio
                                // duplico non lo inizializzo, tanto quando lo uso faccio già tutto
                                pippo=coda; // parto dalla coda, il primo pippo e head non li elimino, tanto lo riuso
                                while (pippo!=head->next) {
                                    coda=pippo->prec;
                                    coda->next=NULL;
                                    pippo->next=NULL;
                                    pippo->prec=NULL;
                                    free(pippo->run_ndx);
                                    pippo->run_ndx=NULL;
                                    free(pippo->run_nsx);
                                    pippo->run_nsx=NULL;
                                    free(pippo);
                                    pippo=coda;
                                }
                                // finito qua inizializzo pippo, che è l'unica cella rimasta
                                pippo->next=NULL;
                                pippo->indicerun=-2;
                                free(pippo->run_ndx);
                                free(pippo->run_nsx);
                                pippo->dimmaxdx=0;
                                pippo->dimmaxsx=0;
                                pippo->stato=-2;
                                head->next=pippo;
                                
                            }
                            if (risultato==0) {
                                printf("0\n");
                            }
                            if (risultato==4) {
                                printf("U\n");
                            }
                            if (risultato==3) {
                                printf("U\n");
                                //libera tutto spazio
                                // duplico non lo inizializzo, tanto quando lo uso faccio già tutto
                                pippo=coda; // parto dalla coda, il primo pippo e head non li elimino, tanto lo riuso
                                while (pippo!=head->next) {
                                    coda=pippo->prec;
                                    coda->next=NULL;
                                    pippo->next=NULL;
                                    pippo->prec=NULL;
                                    free(pippo->run_ndx);
                                    pippo->run_ndx=NULL;
                                    free(pippo->run_nsx);
                                    pippo->run_nsx=NULL;
                                    free(pippo);
                                    pippo=coda;
                                }
                                // finito qua inizializzo pippo, che è l'unica cella rimasta
                                pippo->next=NULL;
                                pippo->indicerun=-2;
                                free(pippo->run_ndx);
                                free(pippo->run_nsx);
                                pippo->dimmaxdx=0;
                                pippo->dimmaxsx=0;
                                pippo->stato=-2;
                                head->next=pippo;
                                
                            }
                            
                        }// fine if saltatutto
                        
                    } // while EOF per far girare run
                    
                    
                }// fine if 'run'
                else {
                    //printf("non inizia con 'run'");
                }
                
            }//fine if 'max'
            else {
                //printf("\nERRORE: non inizia con 'max'\n");
            }
        }//fine if per 'acc'
        else {
            //printf("\nERRORE: non inizia con 'acc'\n");
        }
    }//fine if per 'tr'
    else {
        //printf("\nERRORE: non inizia con 'tr'\n");
    }
    
    
    
    
} //---------------------------------- FINE MAIN


//funzione ingrandisce e inizializza array
new_node * ingrandisciarray( new_node * array, int fromtemp){
    array=realloc(array, (fromtemp+1)*sizeof(new_node));
    for (k=k+1; k<=fromtemp; k++) {
        array[k]=NULL;
    }
    k=fromtemp;
    return array;
}

//funzione ingrandisce e inizializza stati accettazione
int * ingrandisciaccettazione( int * accettazione, int acctemp){
    accettazione=realloc(accettazione, (f+1)*sizeof(int));
    accettazione[f]=acctemp;
    f=f+1;
    return accettazione;
}


// funzione inserisce transizioni
void inserimentoincoda( int fromtemp, char readtemp, char writetemp, char movetemp, int totemp){
    new_node temp;
    new_node avanti;
    temp=malloc(sizeof(node));
    temp->down=NULL;
    //inserisco i dati
    temp->from=fromtemp;
    temp->move=movetemp;
    temp->to=totemp;
    temp->read=readtemp;
    temp->write=writetemp;
    
    if ((temp->from)<=k) {  // se entro allora c'e spazio nell'array
        if (array[temp->from]==NULL) {
            array[temp->from]=temp;
        }
        else{
            avanti=array[temp->from];
            while (avanti->down!=NULL) {
                avanti=avanti->down;
            }
            avanti->down=temp;
        }
    }
    else {
        //alloco spazio e ripeto quello su, nel caso sia =NULL visto che l'ho dovuto creare nuovo
        array =ingrandisciarray (array, fromtemp);
        array[temp->from]=temp;
    }
}


// funzione fa mossa in duplico
void mossaduplico (new_node_nd duplico, char avantimove, new_node avanti){
    if (duplico->flag==1){ // sono nel natro di dx
        
        if (avantimove=='R') {
            if ((duplico->indicerun +1)==duplico->dimmaxdx) {
                // allora devo espandere l'array
                // verifica loop
                if (avanti->from==avanti->to && avanti->read=='_') {
                    // allora sono in caso di loop
                    duplico->loop=1;
                    risultato=4;
                }
                else {
                    duplico->run_ndx=realloc(duplico->run_ndx, (duplico->dimmaxdx+31)*sizeof(char));
                    strcat(duplico->run_ndx, "______________________________\0");
                    // problema, magari in realloc mi accoda dei caratteri alla mia stringa, e poi con il cui rimangono spazzi in mezzo
                    duplico->dimmaxdx=strlen(duplico->run_ndx);
                }
            }
            duplico->indicerun=duplico->indicerun+1;
        }
        
        if (avantimove=='L') {
            if ((duplico->indicerun)==0) {
                duplico->flag=0;
                if (duplico->dimmaxsx==0){
                    //espando già l'array di sx perchè al turno dopo sennò non legge niente dalla prima cella di runsx
                    duplico->run_nsx=realloc(duplico->run_nsx, 31*sizeof(char));
                    strcpy(duplico->run_nsx, "______________________________\0");
                    duplico->dimmaxsx=30;
                }
            }
            else {
                duplico->indicerun=duplico->indicerun-1;
            }
        }
    }
    else { // allora il flag è 0
        
        if (avantimove=='R') {
            if ((duplico->indicerun)==0) {
                // passo ad array di dx
                duplico->flag=1;
            }
            else {
                duplico->indicerun=duplico->indicerun-1;
            }
        }
        
        if (avantimove=='L') {
            if ((duplico->indicerun +1)==duplico->dimmaxsx) {
                //allora devo espandere l'array di sx
                // verifica loop
                if (avanti->from==avanti->to && avanti->read=='_') {
                    // allora sono in caso di loop
                    duplico->loop=1;
                    risultato=4;
                }
                else {
                    duplico->run_nsx=realloc(duplico->run_nsx, (duplico->dimmaxsx+31)*sizeof(char));
                    strcat(duplico->run_nsx, "______________________________\0");
                    duplico->dimmaxsx=strlen(duplico->run_nsx);
                }
            }
            duplico->indicerun=duplico->indicerun+1;
        }
    }
    
    // verifico se finito in stato di accettazione
    for ( int y=0; y<f; y++) {
        if (duplico->stato==accettazione[y]) {
            risultato=1;
        }
    }
}



// funzione fa mossa in pippo ( cioè faccio mossa sull'originale, i non determinismi sono duplico)
void mossapippo (new_node_nd pippo, char move_det, new_node avantidet){
    if (pippo->flag==1){ // sono nel natro di dx
        
        if (move_det=='R') {
            if ((pippo->indicerun +1)==pippo->dimmaxdx) {
                // allora devo espandere l'array
                // verifica loop
                if (avantidet->from==avantidet->to && avantidet->read=='_') {
                    // allora sono in caso di loop
                    pippo->loop=1;
                    risultato=4;
                }
                else {
                    pippo->run_ndx=realloc(pippo->run_ndx, ((pippo->dimmaxdx+31)*sizeof(char)));
                    strcat(pippo->run_ndx, "______________________________\0");
                    // problema, magari in realloc mi accoda dei caratteri alla mia stringa, e poi con il cui rimangono spazzi in mezzo
                    pippo->dimmaxdx=strlen(pippo->run_ndx);
                }
            }
            pippo->indicerun=pippo->indicerun+1;
        }
        
        if (move_det=='L') {
            if ((pippo->indicerun)==0) {
                pippo->flag=0;
                if (pippo->dimmaxsx==0){
                    //espando già l'array di sx perchè al turno dopo sennò non legge niente dalla prima cella di runsx
                    pippo->run_nsx=realloc(pippo->run_nsx, 31*sizeof(char));
                    strcpy(pippo->run_nsx, "______________________________\0");
                    pippo->dimmaxsx=30;
                }
            }
            else {
                pippo->indicerun=pippo->indicerun-1;
            }
        }
    }
    else { // allora il flag è 0
        
        if (move_det=='R') {
            if ((pippo->indicerun)==0) {
                // passo ad array di dx
                pippo->flag=1;
            }
            else {
                pippo->indicerun=pippo->indicerun-1;
            }
        }
        
        if (move_det=='L') {
            if ((pippo->indicerun +1)==pippo->dimmaxsx) {
                //allora devo espandere l'array di sx
                // verifica loop
                if (avantidet->from==avantidet->to && avantidet->read=='_') {
                    // allora sono in caso di loop
                    pippo->loop=1;
                    risultato=4;
                }
                else {
                    pippo->run_nsx=realloc(pippo->run_nsx, (pippo->dimmaxsx+31)*sizeof(char));
                    strcat(pippo->run_nsx, "______________________________\0");
                    pippo->dimmaxsx=strlen(pippo->run_nsx);
                }
            }
            pippo->indicerun=pippo->indicerun+1;
        }
    }
    
    // verifico stato di accettazione
    for ( int y=0; y<f; y++) {
        if (pippo->stato==accettazione[y]) {
            risultato=1;
        }
    }
}

