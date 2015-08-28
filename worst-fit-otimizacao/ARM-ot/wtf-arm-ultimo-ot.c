//###          Agosto/2015
//### SEMB1 - Codigo executado no ARM9 linux
//### Dupla: Franklin e Roland Gabriel
//###

#include <stdio.h>
#define NUM_MAX_BLOCOS_MEM 1000
#define NUM_MAX_PROCESSOS 3000

//Define tipo de dado.
struct BlocosDeMemoria {
    int id;
    int tamanho;
} typedef BlocoMem;

//Ordena memoria de forma decrescente.
void ordenar(BlocoMem *vetorMem, int numBlocos) {
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

int main()
{

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
    scanf("%d",&numProcessos);

    printf("Entre com o Numero de Blocos:");
    scanf("%d",&numBlocos);

    //TODO: Condicional que define o modo de tratamento:
    if(numProcessos <= 5) {
        //até 5 Processos definindo as condicoes de cada parte,
        printf("Entre com o tamanho dos Processos:\n");
        for(i=0;i<numProcessos;i++) {
            printf("Processo %d:",i+1);
            scanf("%d",&tamProcessos[i]);}

        printf("Entre com o tamanho dos Blocos:\n");
        for(i=0;i<numBlocos;i++) {
            printf("Bloco %d:",i+1);
            scanf("%d",&vBlocoMem[i].tamanho);
            vBlocoMem[i].id = i+1;
        }

    } else {
        //ou em lote, definindo os tamanhos para todos.
        printf("Entre com um tamanho para TODOS os Processos:");
        scanf("%d",&tamTodosProcessos);
        for(i=0;i<numProcessos;i++) tamProcessos[i] = tamTodosProcessos;

        printf("Entre com um tamanho para TODOS os blocos:");
        scanf("%d",&tamTodosBlocos);
        for(i=0;i<numBlocos;i++) {
            vBlocoMem[i].tamanho = tamTodosBlocos;
            vBlocoMem[i].id = i+1;
        }

    }
//Fim de area de entrada de dados

    //TODO: Ordenação inicial da memória
    ordenar(vBlocoMem, numBlocos);
    //Imprime cabecalho da lista output.
    printf("\nID Processo:\t Tamanho:\t ID Bloco:\tEspaco no Bloco:\tEspaco Restante:\n");

//TODO: Regiao critica.
//    Para cada processo.
    for(i=0;i<numProcessos;i++) {

        //Varre vetor de blocos de memória ordenado descrescentemente
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
    getchar();
    return(0);

}
