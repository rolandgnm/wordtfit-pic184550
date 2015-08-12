//###          Agosto/2015
//### SEMB1 - Codigo executado no ARM9 linux
//### Dupla: Franklin e Roland Gabriel
//###

#include <p18f4550.h>
#include <stdlib.h>
#include <string.h>

#define NUM_MAX_BLOCOS_MEM 10
#define NUM_MAX_PROCESSOS 30
#define Max 15
#define numElementos 12
#define MAX_INPUT 5
#define CRLF "\r\n" 

char *dado;
int i,a,b,c;
char msg[MAX_INPUT];

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

int getInt(){
	i=0;
	while(i<MAX_INPUT){
			if(PIR1bits.RCIF){				//Checa se foi registrador foi carregado
				PIR1bits.RCIF = 0;			//Reinicializa Flag de recebimento
				*dado = RCREG;				//Recebe dado
				if((char) *dado == 0x0D){ 	//Checa se é Enter
					puts(CRLF);				//quebra linha
					break;					//para iteracao
				} else {
				msg[i++] = *dado;			//Poe na string para ser convertido
				putc(*dado);				//imprime
				numeroLido = atoi(msg);		//imprime
				}
	 		}
	
		}
	return numeroLido;
}


void main()
{
	

	initEUSART();
     while(1){
		puts("Insira um numero: ");
		a = getInt();

		puts("\nInsira outro numero: ");
		b = getInt();
		
		c= a + b;
		itoa(c, msg);
		puts(msg);
		puts(CRLF);
	
	
	 } 

 

}
