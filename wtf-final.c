//###          Agosto/2015
//### SEMB1 - Codigo executado no ARM9 linux
//### Dupla: Franklin e Roland Gabriel
//###

#include <p18f4550.h>
#include <stdlib.h>

#define NUM_MAX_BLOCOS_MEM 10
#define NUM_MAX_PROCESSOS 30
#define Max 15
#define numElementos 12
#define NUM_ALGARISMOS 5

char *dado;
int i;
char msg[5];

char dadoAux;

int numeroLido;

char str[64]; 
int  convint;  
int  atoi( const char * s );  
 

/*Configura e habilita a comunicação serial*/


#define FOSCC 8000000
#define BAUDRATE 9600
#define SPBRG_VALUE FOSCC/(4* (BAUDRATE+1))

void initEUSART( void ){

	TRISCbits.TRISC7 = 1;
	TRISCbits.TRISC6 = 0;
	
	TXSTAbits.CSRC = 0;			//Don't care
	TXSTAbits.TX9 = 0;			//Selects 8-bit transmission
	TXSTAbits.TXEN = 1;			//Transmit enabled
	TXSTAbits.SYNC = 0;			//Asynchronous mode
	TXSTAbits.SENDB = 0;		//Sync Break transmission completed
	TXSTAbits.BRGH = 0; 		//high speed

	RCSTA = 0;					//Clear the register
	RCSTAbits.SPEN = 1;			//Serial port enabled (configures RX/DT and TX/CK pins as serial port pins)
	RCSTAbits.RX9 = 0;			//Selects 8-bit reception
	RCSTAbits.SREN = 0;			//Don't care
	RCSTAbits.CREN = 1;			//Enables receiver
	RCSTAbits.ADDEN = 0; 		//Don't care

	BAUDCONbits.BRG16 = 0;		//16-bit Baud Rate Generator – SPBRG only (Compatible mode), SPBRGH value ignored 
 	BAUDCONbits.WUE = 0;		//RX pin not monitored or rising edge detected
	BAUDCONbits.ABDEN = 0;		//Baud rate measurement disabled or completed


	SPBRG =  12 ;//SPBRG_VALUE % 256;		//Value 8 LSB for the baud rate
	SPBRGH =  12; //; SPBRG_VALUE / 256;		//Value 8 MSB for the baud rate

}

void putc(unsigned char X)
{
	TXREG = X;					//transfere o byte para o registrador de saida
	while(!TXSTAbits.TRMT);				//espera o registrador de saida esvaziar
}



void puts(rom char *X)
{
	while(*X)
	{
		putc(*X);
		X++;
	}
}


void putf(char *X, int L)
{	
	while(L)
	{
		putc(*X);
		X++;
		L--;
	}
}


void main()
{
	initEUSART();
     while(1){
	 	while(i<NUM_ALGARISMOS){
			if(PIR1bits.RCIF){
				PIR1bits.RCIF = 0;	
				*dado = RCREG ;
				msg[i++] = *dado;
				puts("dskjfhdsfdskj\r\nfjk      d");
				numeroLido = atoi(msg);
	 		}
		}
		
	 } 
 
	  

/*

    BlocoMem vBlocoMem[NUM_MAX_BLOCOS_MEM];
    int tamProcessos[NUM_MAX_PROCESSOS],
        i,j,
        numBlocos,
        numProcessos,
        tamTodosBlocos,
        tamTodosProcessos;

    printf("\n\tAlocacao de Memoria - Worst Fit\n\n");

//TODO: Area de entrada de dados
    printf("Entre com o Numero de Processos(Se > 5, em LOTE !!!):");
    //scanf("%d",&numProcessos);
    numProcessos = 10;

    printf("Entre com o Numero de Blocos:");
    //scanf("%d",&numBlocos);
    numBlocos = 10;

    //TODO: Condicional que define o modo de tratamento:
    if(numProcessos <= 5) {
        //atÃ© 5 Processos definindo as condicoes de cada parte,
        printf("Entre com o tamanho dos Processos:\n");
        for(i=0;i<numProcessos;i++) {
            printf("Processo %d:",i+1);
            //scanf("%d",&tamProcessos[i]);}
}

        printf("Entre com o tamanho dos Blocos:\n");
        for(i=0;i<numBlocos;i++) {
            printf("Bloco %d:",i+1);
            //scanf("%d",&vBlocoMem[i].tamanho);
            vBlocoMem[i].id = i+1;
        }

    } else {
        //ou em lote, definindo os tamanhos para todos.
        printf("Entre com um tamanho para TODOS os Processos:");
        //scanf("%d",&tamTodosProcessos);
        tamTodosProcessos = 10;

        for(i=0;i<numProcessos;i++) tamProcessos[i] = tamTodosProcessos;

        printf("Entre com um tamanho para TODOS os blocos:");
        //scanf("%d",&tamTodosBlocos);
        tamTodosBlocos = 20;
        for(i=0;i<numBlocos;i++) {
            vBlocoMem[i].tamanho = tamTodosBlocos;
            vBlocoMem[i].id = i+1;
        }

    }
//Fim de area de entrada de dados

    //TODO: OrdenaÃ§Ã£o inicial da memÃ³ria
    ordenar(vBlocoMem, numBlocos);
    //Imprime cabecalho da lista output.
    printf("\nID Processo:\t Tamanho:\t ID Bloco:\tEspaco no Bloco:\tEspaco Restante:\n");

//TODO: Regiao critica.
//    Para cada processo.
    for(i=0;i<numProcessos;i++) {

        //Varre vetor de blocos de memÃ³ria ordenado descrescentemente
        for(j=0;j<numBlocos;j++) {

            //Ha espaco?
            if(vBlocoMem[j].tamanho >= tamProcessos[i]){
                //Imprime linha output
                printf("\t%d\t\t\t\t%d\t\t\t%d\t\t\t%d\t\t\t\t\t%d\n",i+1, tamProcessos[i], vBlocoMem[j].id, vBlocoMem[j].tamanho,
                       (vBlocoMem[j].tamanho - tamProcessos[i]));
                //Registra alteracao no tamanho do bloco
                vBlocoMem[j].tamanho -= tamProcessos[i];

                //TODO: Reordena o vetor de BlocosMem.
                ordenar(vBlocoMem,numBlocos);
                break;
            }
        }
        //Espaco nao encontrado
        if(j == numBlocos){
            printf("\t%d\t\t\t\t%d\t IMPOSSIVEL ALOCAR\n",i+1, tamProcessos[i]);
        }
    }
//FIM de regiao critica

    //encerra sw
    printf("\n");
    //getchar();
    return(0);
*/

}
