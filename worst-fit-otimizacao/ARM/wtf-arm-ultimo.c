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
  ###
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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
#define CRLF "\n"


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


// Protótipos de funções definidas após a main()
void ordenarDecrescente(BlocoMem *vetorMem, int numBlocos);    // função para ordenar vetor de bloco de memória.



int main(void)
{

  // Armazena Strings em memoria de programa
  const char msgInicial[] = "###   Alocacao de Memoria - Worst Fit   ###";
  const char msgNumProcessos[] = "Entre com o Numero de Processos(Se > 5, em LOTE !!!): ";
  const char msgNumBlocos[] = "Entre com o Numero de Blocos: ";
  const char msgTodosProcessos[] = "Entre com um tamanho para TODOS os Processos: ";
  const char msgTodosBlocos[] = "Entre com um tamanho para TODOS os Blocos: ";
  const char tamCadaProcesso[] = "Entre com o tamanho dos Processos: ";
  const char tamCadaBloco[] = "Entre com o tamanho dos Blocos: ";
  const char processo[] = "Processo";
  const char bloco[] = "Bloco";
  const char cabecalho[] = "ID Processo:     Tamanho:     ID Bloco:     Espaco no Bloco:     Espaco Restante:";
  const char impossivel[] = " IMPOSSIVEL ALOCAR";

  // Alocação de memmória para o vetores necessários.
  // Poderíamos alterar para ceder mais espaço
  // para o vetor de Blocos de memória, contudo
  // preferimos manter o código da forma
  // que foi demonstrado.
  // No arquivo .LKR há mais comentários
  // acerca da configuração de memória de dados.
      int tamProcessos[NUM_MAX_PROCESSOS];
      BlocoMem vBlocoMem[NUM_MAX_BLOCOS_MEM];


  // setor de acesso rapido (96B) default no PIC
  // e mantido no Linker.
      unsigned char cbuffer;              // buffer para tipo char.
      unsigned char sbuffer[TAM_NUM];     // buffer para vetor de char.
      int iNumProcessos = 0;              // guarda número de processos.
      int iNumBlocos = 0;                 // guarda número de blocos de memória.
      int iTamTodosProcessos = 0;         // tamanhos para casos em lote.
      int iTamTodosBlocos = 0;            //              //
      int i = 0, j = 0;                   // iteradores.
      int counter = 0;                    // contadores
      int counter2 = 0;
      clock_t clkInicio, clkFim;
      double exeTime;





	// USART out -> Introducao +  mensagem numero de Processos
  // obs: usamos um While(BusyUSART()) para cada envio para USART.
  // "###   Alocacao de Memoria - Worst Fit   ###"
  // "Entre com o Numero de Processos(Se > 5, em LOTE !!!): "
  printf("\t\t%s%s%s", msgInicial, CRLF, msgNumProcessos);

  // USART in -> Le numero de Processos
  // função getInt() faz todo o trabalho de feedback ao usuario
  // ao digitar cada dígito, finalizando com ENTER.
	scanf("%d", &iNumProcessos);
  printf("\n" );

  // USART out -> mensagem / in-> numero de BlocosDeMemoria
  // "Entre com o Numero de Blocos: "
  printf("%s",msgNumBlocos);
  // USART in -> Le numero de Blocos
  scanf("%d", &iNumBlocos);
  printf("\n" );



  if(iNumProcessos > 5) {
      //Ativa modo com iteração da entrada. trabalho em lote.

      // "Entre com um tamanho para TODOS os Processos: "
      printf("%s", msgTodosProcessos);
      // Recebe tamanho de todos os processos.
      scanf("%d", &iTamTodosProcessos);
      printf("\n" );

      //Itera atribuição de tamanho para cada posição
      // do vetor de tamanho de procesos.
      for(i=0;i<iNumProcessos;i++) tamProcessos[i] = iTamTodosProcessos;

      //Quebra de linha simples.
      printf("%s", CRLF); //new line

      // "Entre com um tamanho para TODOS os Blocos: "
      printf("%s", msgTodosBlocos);

      // Recebe tamanho para todos os Blocos.
      scanf("%d", &iTamTodosBlocos);
      printf("\n" );

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
        printf("%s%s", tamCadaProcesso, CRLF);

        // iNumProcessos sempre <= 5
        for(i=0;i<iNumProcessos;i++) {
            // "Processo i: "
            printf("%s %d: ",processo, i+1);
            // Atribui valor a partir do buffer.
            scanf("%d", &tamProcessos[i]);
            printf("\n" );

        }

        // "Entre com o tamanho dos Blocos: "
        printf("%s%s",tamCadaBloco, CRLF);

        for(i=0;i<iNumBlocos;i++) {
            // "Bloco i: "
            printf("%s %d: ",bloco, i+1);
            // Atribui valores necessário para preencher struct.
            scanf("%d", &vBlocoMem[i].tamanho );
            printf("\n" );

            vBlocoMem[i].id = i+1; // Início em 1.
        }
	}

    //## Fim de entrada, início da computação ##

    // Ordena vetor de Blocos de Memória de modo a ter
    // Maior posição na primeira posição.
    // %%%%% , %%%% , %%% , %% , %
    // obs: Desse ponto em diante creditamos o usuário
    // JoeVogel @ Github pela disponibilidade da lógica
    // usada em Java, que foi adaptada para C e depois PIC.

    //TODO Inicia Contador
    clkInicio = clock();

    ordenarDecrescente(vBlocoMem, iNumBlocos);

    // "ID Processo:     Tamanho:     ID Bloco:     Espaco no Bloco:     Espaco Restante: "
  printf("%s%s", cabecalho, CRLF);

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
               printf("     %d              %d            %d               %d                    %d%s",i+1, tamProcessos[i], vBlocoMem[j].id, vBlocoMem[j].tamanho,
                      (vBlocoMem[j].tamanho - tamProcessos[i]),CRLF);

                //Registra alteracao no tamanho do bloco
                vBlocoMem[j].tamanho -= tamProcessos[i];

                // Reordena o vetor de BlocosMem.
                ordenarDecrescente(vBlocoMem, iNumBlocos);
                break;
            }
        }

        // Não há espaço em nenhum bloco para aquele processo.
        // Confirma que chegou ao fim do vetor.
        if(j == iNumBlocos){
          // "5        20         IMPOSSIVEL ALOCAR"
         printf("     %d                    %d      %s%s",i+1, tamProcessos[i],impossivel, CRLF);
        }
    }

    //Encerra programa.
    clkFim = clock();
    exeTime = (double)(clkFim - clkInicio) / CLOCKS_PER_SEC;
    printf("%.6f Segundos\n", exeTime);
    printf("!!! Programa Encerrado !!!" );
    return 0;

}
// Fim de main()





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
