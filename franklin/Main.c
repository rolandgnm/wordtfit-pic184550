/*###          Agosto/2015
  ### SEMB1 - Worst-fit portado para PIC18F4550
  ### Dupla: Franklin e Roland Gabriel
  ###
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
#define NUM_MAX_PROCESSOS 270

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
    int tamProcessos[NUM_MAX_PROCESSOS];
    BlocoMem vBlocoMem[NUM_MAX_BLOCOS_MEM];
#pragma udata


// setor de acesso rapido (96B) default no PIC
// e mantido no Linker.
#pragma udata accessram
    unsigned char cbuffer;              // buffer para tipo char.
    unsigned char sbuffer[TAM_NUM];     // buffer para vetor de char.
    int iNumProcessos = 0;              // guarda número de processos.
    int iNumBlocos = 0;                 // guarda número de blocos de memória.
    int iTamTodosProcessos = 0;         // tamanhos para casos em lote.
    int iTamTodosBlocos = 0;            //              //
    int i = 0, j = 0;                   // iteradores.
#pragma udata

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
void ordenarDecrescente(BlocoMem *vetorMem, int numBlocos);    // função para ordenar vetor de bloco de memória.
int getInt();                                       //Facilita leitura de inteiros a partir da USART

void main(void)
{


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
      // do vetor de tamanho de procesos.
      for(i=0;i<iNumProcessos;i++) tamProcessos[i] = iTamTodosProcessos;

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

    // Ordena vetor de Blocos de Memória de modo a ter
    // Maior posição na primeira posição.
    // %%%%% , %%%% , %%% , %% , %
    ordenarDecrescente(vBlocoMem, iNumBlocos);

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
                //
                printf("     %d                    %d               %d               %d                         %d%S",i+1, tamProcessos[i], vBlocoMem[j].id, vBlocoMem[j].tamanho,
                       (vBlocoMem[j].tamanho - tamProcessos[i]),CRLF);
               while(BusyUSART());
                //Registra alteracao no tamanho do bloco
                vBlocoMem[j].tamanho -= tamProcessos[i];

                //TODO: Reordena o vetor de BlocosMem.
                ordenarDecrescente(vBlocoMem, iNumBlocos);
                break;
            }
        }
        //Espaco nao encontrado
        if(j == iNumBlocos){

          printf("     %d                    %d      %S%S",i+1, tamProcessos[i],impossivel, CRLF);
        }
    }
        printf("\r\n\r\n\r\n" );
        while(1);

}
/*******/

//TODO:Configura USART
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

//TODO: Metodo de Ordenacao
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
  int iReturn=0;
	while(i<TAM_NUM){
			if(PIR1bits.RCIF){				//Checa se foi registrador foi carregado
				PIR1bits.RCIF = 0;			//Reinicializa Flag de recebimento
				cbuffer = RCREG;				//Recebe dado
				if(cbuffer == 0x0D){ 	//Checa se � Enter
					break;					//para iteracao
				} else {
				sbuffer[i++] = cbuffer;			//Poe na string para ser convertido
				putcUSART(cbuffer);				//imprime
        while(BusyUSART());
				}	}	}
  puts(CRLF);				//quebra linha
  while(BusyUSART());
  iReturn = atoi(sbuffer);
  while(i>=0) sbuffer[i--] = '\0';
  return iReturn;
}
