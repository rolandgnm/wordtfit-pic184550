/*###
  ###         Agosto/2015
  ### SEMB1 - Worst-fit portado para PIC18F4550
  ### Dupla: Franklin e Roland Gabriel
  ###
  ### Principais estruturas:
  ### - Vetor Tamanho de procesos
  ### - Vetor de estruturas de int
  ###     equivalendo a espaço de um float.
  ###
  ### O programa aloca os processos de forma simbólica
  ### atribuindo valores resultado de subtração para
  ### cada bloco de memória.
  ### A alocação acontece sempre no bloco de memória que
  ### que terá maior espaço livre após a alocação.
  ###
  ###       Referências:
  ### Worst-fit: http://github.com/JoeVogel/Java-WorstFit
  ### shellSortDESC: http://rosettacode.org/wiki/Sorting_algorithms/Shell_sort#C
  ###   com simples alteração para tornar Decrescente.
  ###
*/

#include <p18f4550.h>
#include <stdio.h>
#include <stdlib.h>
#include <usart.h>
#include <string.h>

//Configura registradores do PIC
#pragma config FOSC     = HS
#pragma config IESO     = OFF
#pragma config PWRT     = OFF
#pragma config BORV     = 3
#pragma config WDT      = OFF
#pragma config WDTPS    = 32768
#pragma config MCLRE    = ON
#pragma config LPT1OSC  = OFF
#pragma config PBADEN   = OFF
#pragma config STVREN   = ON
#pragma config LVP      = OFF

// Limita a alocação de memória
#define NUM_MAX_BLOCOS_MEM 270
#define NUM_PROCESSOS_MODO_INTERATIVO 5 //Alterado para apenas trabalhar em modo INTERATIVO

// Tamanho maximo para buffer de entrada do teclado,
// suposto a aceitar entradas de até 5 algarismos,
// ou de 1 a 4 + tecla 'ENTER'.
#define TAM_NUM 5

// Define modo de quebra de linha,
// por conta de o simulador usar quebra diferente
// do PIC.
#define CRLF "\r\n"


// Depois dos últimos testes, ainda antes da
// demonstração, chegamos a conclusão de que o tratamento
// para chegar ao funcionamento pleno da computação
// se devia a evitar o uso de armazenamento em
// memória de programa por conta da necessidade
// de constantes escritas a mesma.
// Portanto, ficou mantido uso de estrutura.
struct BlocosDeMemoria {
int id;
int tamanho;
} typedef BlocoMem;

// Alocação de memmória para o vetores necessários.
// Poderíamos alterar para ceder mais espaço
// para o vetor de Blocos de memória, contudo
// preferimos manter o código da forma
// que foi demonstrado.
// No arquivo .LKR há mais comentários
// acerca da configuração de memória de dados.
#pragma udata DADOS_U
    int tamProcessos[NUM_PROCESSOS_MODO_INTERATIVO]; //Passa a ser usado apenas em modo Interativo
    BlocoMem vBlocoMem[NUM_MAX_BLOCOS_MEM];
#pragma udata


// setor de acesso rapido (96B) default no PIC
// e mantido no Linker.
#pragma udata accessram
    unsigned char cbuffer;              // buffer para tipo char.
    unsigned char sbuffer[TAM_NUM];     // buffer para vetor de char.
    int iNumProcessos = 0;              // guarda número de processos.
    int iNumBlocos = 0;                 // guarda número de blocos de memória.
    int iTamTodosProcessos = 0;         // tamanhos para casos em lote.         OBS: Passa a Guardar o valor para todos os Processos
    int iTamTodosBlocos = 0;            //              //
    int i = 0, j = 0;                   // iteradores.
    int counter = 0;
    int counter2 = 0;
#pragma udata

#pragma code high_vector=0x08
  void interrupt_at_high_vector(void) {
    _asm goto high_isr _endasm
      }
#pragma code
  
#pragma interrupt high_isr
  void high_isr(void) {
       if(INTCONbits.TMR0IF) {                // Timer 0 .. 
           
           TMR0H =  0x00;           
           TMR0L =  0x06;
           
          INTCONbits.TMR0IF=0;                    // clear bit IRQ
  
          counter++;
          
          if(counter == 99) {
              counter = 0;
              counter2++;  
          }        //if timer...
    }
  }
#pragma code

// Armazena Strings em memoria de programa
const rom char msgInicial[] = "###   Alocacao de Memoria - Worst Fit   ###";
const rom char msgNumProcessos[] = "Entre com o Numero de Processos(Se > 5, em LOTE !!!): ";
const rom char msgNumBlocos[] = "Entre com o Numero de Blocos: ";
const rom char msgTodosProcessos[] = "Entre com um tamanho para TODOS os Processos: ";
const rom char msgTodosBlocos[] = "Entre com um tamanho para TODOS os Blocos: ";
const rom char tamCadaProcesso[] = "Entre com o tamanho dos Processos: ";
const rom char tamCadaBloco[] = "Entre com o tamanho dos Blocos: ";
const rom char processo[] = "Processo";
const rom char bloco[] = "Bloco";
const rom char cabecalho[] = "ID Processo:     Tamanho:     ID Bloco:     Espaco no Bloco:     Espaco Restante:";
const rom char impossivel[] = " IMPOSSIVEL ALOCAR";


// Protótipos de funções definidas após a main()
void inicializarSerial(void);                       // configura pinagem para habilitar USART.
/*OBSOLETO!*/void ordenarDecrescente(BlocoMem *vetorMem, int numBlocos); /*OBSOLETO!*/    // função para ordenar vetor de bloco de memória.
int getInt();                                       //Facilita leitura de inteiros a partir da USART
void iniciaContador(void);
void interrupt_at_high_vector(void);
void high_isr(void);
void alocarModoInterativo(); //Metodo que aloca processos ao modo original.
void alocarModoLote(); //Metodo que descarta uso de vetor para Processos.
void shellSortDESC(BlocoMem *a, int n);

void main(void)
{
        RCONbits.IPEN = 1;
        INTCONbits.GIE = 1;
    
        INTCONbits.TMR0IE = 1;
        INTCON2bits.TMR0IP = 1;

	// Iniciliza a serial
	inicializarSerial();


	// USART out -> Introducao +  mensagem numero de Processos
  // obs: usamos um While(BusyUSART()) para cada envio para USART.
  // "###   Alocacao de Memoria - Worst Fit   ###"
  // "Entre com o Numero de Processos(Se > 5, em LOTE !!!): "
  printf("\t\t%S%S%S", msgInicial, CRLF, msgNumProcessos);
  while(BusyUSART());

  // USART in -> Le numero de Processos
  // função getInt() faz todo o trabalho de feedback ao usuario
  // ao digitar cada dígito, finalizando com ENTER.
	iNumProcessos = getInt();

  // USART out -> mensagem / in-> numero de BlocosDeMemoria
  // "Entre com o Numero de Blocos: "
  printf("%S",msgNumBlocos);
  while(BusyUSART());
  // USART in -> Le numero de Blocos
  iNumBlocos = getInt();



  if(iNumProcessos > 5) {
      //Ativa modo com iteração da entrada. trabalho em lote.

      // "Entre com um tamanho para TODOS os Processos: "
      printf("%S", msgTodosProcessos);
      while(BusyUSART());
      // Recebe tamanho de todos os processos.
      iTamTodosProcessos = getInt();

      //Itera atribuição de tamanho para cada posição
      // do vetor de tamanho de procesos.            OBS: Valor será persistido em variável única e utilizado no momento e alocação.
      // for(i=0;i<iNumProcessos;i++) tamProcessos[i] = iTamTodosProcessos;

      //Quebra de linha simples.
      printf("%S", CRLF); //new line
      while(BusyUSART());

      // "Entre com um tamanho para TODOS os Blocos: "
      printf("%S", msgTodosBlocos);
      while(BusyUSART());

      // Recebe tamanho para todos os Blocos.
      iTamTodosBlocos = getInt();
      // atribui tamanho para cada bloco.
      for(i=0;i<iNumBlocos;i++) {
          vBlocoMem[i].tamanho = iTamTodosBlocos;
          vBlocoMem[i].id = i+1;
        }

  } else {

         // Para até 5 Processos, diálogo é lançado
         // para definir cada proceso e bloco.

        // Introduz atribuição até 5 elementos.
        //"Entre com o tamanho dos Processos: "
        printf("%S%S", tamCadaProcesso, CRLF);
        while(BusyUSART());

        // iNumProcessos sempre <= 5
        for(i=0;i<iNumProcessos;i++) {
            // "Processo i: "
            printf("%S %d: ",processo, i+1);
            while(BusyUSART());
            // Atribui valor a partir do buffer.
            tamProcessos[i] = getInt();
        }

        // "Entre com o tamanho dos Blocos: "
        printf("%S%S",tamCadaBloco, CRLF);
        while(BusyUSART());

        for(i=0;i<iNumBlocos;i++) {
            // "Bloco i: "
            printf("%S %d: ",bloco, i+1);
            while(BusyUSART());
            // Atribui valores necessário para preencher struct.
            vBlocoMem[i].tamanho = getInt();
            vBlocoMem[i].id = i+1; // Início em 1.
        }
	}

    //## Fim de entrada, início da computação ##
     iniciaContador();

  if (iNumBlocos > 5) {
    alocarModoLote();

  } else {
    shellSortDESC(vBlocoMem, iNumBlocos);
    alocarModoInterativo();
  }


    //Encerra programa.
    printf("%d.%d Segundos", counter2, counter);
    printf("!!! Programa Encerrado !!!" );
    while(1);

}
// Fim de main()




// Configura USART
void inicializarSerial()
{
	RCSTAbits.SPEN = 1;
	TRISCbits.TRISC7 = 1;
	TRISCbits.TRISC6 = 1;
	TXSTAbits.TXEN = 1;
	RCSTAbits.CREN = 1;

	OpenUSART(	USART_TX_INT_OFF &
				USART_RX_INT_OFF &
				USART_ASYNCH_MODE &
				USART_EIGHT_BIT &
				USART_CONT_RX &
				USART_BRGH_HIGH,51);
}

// Metodo de Ordenacao
// Devido a nossa dificuldade com configurações iniciais
// permenece método de ordenação prévio.
void ordenarDecrescente(BlocoMem *vetorMem, int numBlocos) {
    int i,j;
    for (i = 0; i < numBlocos; i++) {
        for (j = i + 1; j < numBlocos; j++) {
            if (vetorMem[i].tamanho < vetorMem[j].tamanho) {
                BlocoMem temp = vetorMem[i];
                vetorMem[i] = vetorMem[j];
                vetorMem[j] = temp;
            }
        }
    }
}

//TODO:Funcao facilita leitura de inteiros
int getInt(){
	int i=0;
  int iReturn=0;                  //inteiro a ser retornado.
	while(i<TAM_NUM){               // Atende ao buffer de 5 char
			if(PIR1bits.RCIF){				  //Checa se foi registrador foi carregado
				PIR1bits.RCIF = 0;			  //Reinicializa Flag de recebimento
				cbuffer = RCREG;				  //Recebe dado
				if(cbuffer == 0x0D){ 	    //Checa se foi ENTER
					break;					        //para iteracao
				} else {
				sbuffer[i++] = cbuffer;		//Poe na string para ser convertido
				putcUSART(cbuffer);				//imprime
        while(BusyUSART());
				}	}	}
  puts(CRLF);				              //quebra linha
  while(BusyUSART());
  iReturn = atoi(sbuffer);        //Armazena return
  while(i>=0) sbuffer[i--] = '\0';//limpa para uso futuro
  return iReturn;
}

void alocarModoInterativo () {
  // "ID Processo:     Tamanho:     ID Bloco:     Espaco no Bloco:     Espaco Restante: "
   printf("%S%S", cabecalho, CRLF);
   while(BusyUSART());

  // Para cada Processo
  for(i=0;i<iNumProcessos;i++) {

      // Percorre vetor de blocos a procura do maior espaço eestante após a alocação.
      // Melhor caso: Posição inicial conterá maior espaço após alocacção.
      // Pior cas0: Não há espaço para o Processo. Percorre todo o vetor de blocos.
      for(j=0;j<iNumBlocos;j++) {

          // Há espaço?
          if(vBlocoMem[j].tamanho >= tamProcessos[i]){
              // Imprime linha output
              // "1      250       1      300        50"
               printf("     %d              %d            %d               %d                    %d%S",i+1, tamProcessos[i], vBlocoMem[j].id, vBlocoMem[j].tamanho,
                      (vBlocoMem[j].tamanho - tamProcessos[i]),CRLF);
              while(BusyUSART());

              //Registra alteracao no tamanho do bloco
              vBlocoMem[j].tamanho -= tamProcessos[i];

              // Reordena o vetor de BlocosMem.
              shellSortDESC(vBlocoMem, iNumBlocos);
              break;
          }
      }

      // Não há espaço em nenhum bloco para aquele processo.
      // Confirma que chegou ao fim do vetor.
      if(j == iNumBlocos){
        // "5        20         IMPOSSIVEL ALOCAR"
         printf("     %d                    %d      %S%S",i+1, tamProcessos[i],impossivel, CRLF);
      }
  }
}

void alocarModoLote () {
  // "ID Processo:     Tamanho:     ID Bloco:     Espaco no Bloco:     Espaco Restante: "
   printf("%S%S", cabecalho, CRLF);
   while(BusyUSART());

  // Para cada Processo
  for(i=0;i<iNumProcessos;i++) {

      // Percorre vetor de blocos a procura do maior espaço restante após a alocação.
      // Melhor caso: Posição inicial conterá maior espaço após alocacção.
      // Pior cas0: Não há espaço para o Processo. Percorre todo o vetor de blocos.
      for(j=0;j<iNumBlocos;j++) {

          // Há espaço?
          if(vBlocoMem[j].tamanho >= iTamTodosProcessos){
              // Imprime linha output
              // "1      250       1      300        50"
               printf("     %d              %d            %d               %d                    %d%S",i+1, iTamTodosProcessos, vBlocoMem[j].id, vBlocoMem[j].tamanho,
                      (vBlocoMem[j].tamanho - iTamTodosProcessos),CRLF);
              while(BusyUSART());

              //Registra alteracao no tamanho do bloco
              vBlocoMem[j].tamanho -= iTamTodosProcessos;

              // Reordena o vetor de BlocosMem.
              shellSortDESC(vBlocoMem, iNumBlocos);
              break;
          }
      }

      // Não há espaço em nenhum bloco para aquele processo.
      // Confirma que chegou ao fim do vetor.
      if(j == iNumBlocos){
        // "5        20         IMPOSSIVEL ALOCAR"
         printf("     %d                    %d      %S%S",i+1, iTamTodosProcessos,impossivel, CRLF);
      }
  }
}

void shellSortDESC (BlocoMem *a, int n) {
    int h, i, j;
    BlocoMem t;
    for (h = n; h /= 2;) {
        for (i = h; i < n; i++) {
            t = a[i];
            for (j = i; j >= h && t.tamanho > a[j - h].tamanho; j -= h) {
                a[j] = a[j - h];
            }
            a[j] = t;
        }
    }
}

void iniciaContador() {
    
   T0CONbits.T08BIT = 1;   // 0 = 16 bit mode, 1 = 8 bit mode
   T0CONbits.T0CS = 0;     // Clock Source: 0 = Fcy, 1 = pulses on T0CKI pin
   T0CONbits.T0SE = 0;     // Select Edge: 0 =  low to high, 1 = high to low
   T0CONbits.PSA = 0;      // 1 = bypass the prescaler. 0 = prescaler per the T0PS2:0 bits
   T0CONbits.T0PS2 = 1;  // Prescaler Selection: 000 = 2, 001 = 4, 
   T0CONbits.T0PS1 = 0;  //  010 = 8, 011 = 16, 100 = 32, 
   T0CONbits.T0PS0 = 1;  // 101 = 64, 110= 128, 111 = 256
   
   TMR0H =  0x00;           
   TMR0L =  0x63;
   
   INTCONbits.TMR0IF = 0; // reset the interrupt flag
   
   T0CONbits.TMR0ON = 1;   // 0 = turn timer off, 1 = turn timer on
      
}
