/*Algoritmo Interpolação de Lagrange*/
#include <p18f4550.h>
#include <stdio.h>
#include <stdlib.h>
#include <usart.h>
#include <string.h>


/*-------------------------------------
* Bits de configura��o do PIC18F4550
*--------------------------------------*/
// Oscilador e outras definicoes de registradores
#pragma config FOSC     = HS     // (8 MHz)
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

#define VETOR_PRINCIPAL 384
#define VETOR_AUXILIAR 6
#define TAM_NUM 5
#define CRLF "\r\n"

    struct BlocosDeMemoria {
    int id;
    int tamanho;
    } typedef BlocoMem;

//TOTAL 1200
#pragma udata DADOS_U
    int tamProcessos[100];
    BlocoMem vBlocoMem[100];
#pragma udata

#pragma udata accessram //setor de acesso rapido (96B)
    int i;

    //variaveis para ler e escrever na serial
    unsigned char cbuffer;
    unsigned char sbuffer[TAM_NUM];

#pragma udata
//int *p = tamProcessos;

//TOTAL 424
//#pragma idata
    char rom msgInicial[41] = "\n\tAlocacao de Memoria - Worst Fit\n\n\0";
  	char rom msgNumProcessos[56] = "Entre com o Numero de Processos(Se > 5, em LOTE !!!): \0";
    char rom msgNumBlocos[33] = "\nEntre com o Numero de Blocos: \0";
  	char rom msgTodosProcessos[50] = "\nEntre com um tamanho para TODOS os Processos: \0";
    char rom msgTodosBlocos[47] = "\nEntre com um tamanho para TODOS os Blocos: \0";
    char rom tamKdProcesso[38] = "Entre com o tamanho dos Processos:\n\0";
    char rom tamKdBloco[35] = "Entre com o tamanho dos Blocos:\n\0";
    char rom processo[11] = "Processo \0";
    char rom bloco[8] = "Bloco \0";
    char rom cabecalho[77] = "\nID Processo:\t Tamanho:\t ID Bloco:\tEspaco no Bloco:\tEspaco Restante:\n\0";
    char rom t[4] = "\t\0";
    char rom n[4] = "\n\0";
    char rom impossivel[20] = " IMPOSSIVEL ALOCAR\0";
  //#pragma idata



/*�rea que define o tamanho de mem�ria alocada para os vetores iniciais*/
/*
#pragma romdata DATAX
const rom float x[VETOR_PRINCIPAL] =
		   {0.,1.,2.,3.,4.,5.,6.,7.,8.,9.,10.,11.,12.,
			13.,14.,15.,16.,17.,18.,19.,20.,21.,22.,23.,24.,
			25.,26.,27.,28.,29.,30.,31.,32.,33.,34.,35.,36.,
			37.,38.,39.,40.,41.,42.,43.,44.,45.,46.,47.,48.,
			49.,50.,51.,52.,53.,54.,55.,56.,57.,58.,59.,60.,
			61.,62.,63.,64.,65.,66.,67.,68.,69.,70.,71.,72.,
            73.,74.,75.,76.,77.,78.,79.,80.,81.,82.,83.,84.,
			85.,86.,87.,88.,89.,90.,91.,92.,93.,94.,95.,96.,
			97.,98.,99.,100.,101.,102.,103.,104.,105.,106.,107.,108.,
			109.,110.,111.,112.,113.,114.,115.,116.,117.,118.,119.,120.,
			121.,122.,123.,124.,125.,126.,127.,128.,129.,130.,131.,	132.,
		    133.,134.,135.,136.,137.,138.,139.,140.,141.,142.,143.,144.,
			145.,146.,147.,148.,149.,150.,151.,152.,153.,154.,155.,156.,
			157.,158.,159.,160.,161.,162.,163.,164.,165.,166.,167.,168.,
			169.,170.,171.,172.,173.,174.,175.,176.,177.,178.,179.,180.,
			181.,182.,183.,184.,185.,186.,187.,188.,189.,190.,191.,192.,
			193.,194.,195.,196.,197.,198.,199.,200.,201.,202.,203.,204.,
			205.,206.,207.,208.,209.,210.,211.,212.,213.,214.,215.,216.,
			217.,218.,219.,220.,221.,222.,223.,224.,225.,226.,227.,228.,
			229.,230.,231.,232.,233.,234.,235.,236.,237.,238.,239.,240.,
			241.,242.,243.,244.,245.,246.,247.,248.,249.,250.,251.,252.,
			253.,254.,255.,256.,257.,258.,259.,260.,261.,262.,263.,264.,
			265.,266.,267.,268.,269.,270.,271.,272.,273.,274.,275.,276.,
			277.,278.,279.,280.,281.,282.,283.,284.,285.,286.,287.,288.,
			289.,290.,291.,292.,293.,294.,295.,296.,297.,298.,299.,300.,
			301.,302.,303.,304.,305.,306.,307.,308.,309.,310.,311.,312.,
			313.,314.,315.,316.,317.,318.,319.,320.,321.,322.,323.,324.,
			325.,326.,327.,328.,329.,330.,331.,332.,333.,334.,335.,336.,
			337.,338.,339.,340.,341.,342.,343.,344.,345.,346.,347.,348.,
			349.,350.,351.,352.,353.,354.,355.,356.,357.,358.,359.,360.,
			361.,362.,363.,364.,365.,366.,367.,368.,369.,370.,371.,372.,
			373.,374.,375.,376.,377.,378.,379.,380.,381.,382.,383.};
#pragma romdata

#pragma romdata DATAY
const rom float y[VETOR_PRINCIPAL] =
		    {3.70439,3.78823,1.33636,4.94159,1.18643,82.71557,
	        99.54162,77.81941,91.74496,58.02974,57.54860,43.43887,
	        64.60021,46.32540,49.72529,67.42367,35.20597,55.19687,
	        70.06395,111.90639,93.35977,46.05606,46.37604,45.77379,
	        3.62884,2.15454,1.09399,4.78827,1.38145,117.45736,
	        117.93512,112.67571,92.28173,48.97979,60.84288,50.97737,
	        68.67294,48.24578,56.88278,68.81634,64.17157,43.99588,
	        113.22461,72.55680,117.20805,25.11175,42.54589,58.46992,
			1.09326,2.12933,3.42153,1.63076,3.04914,117.26584,
			66.31967,84.18746,117.86518,36.82274,62.73909,48.36075,
			68.22959,54.69229,60.98038,40.81870,53.81025,46.42337,
			73.22207,89.04848,113.55361,49.75285,58.94000,57.21378,
			1.49209,2.52826,2.92159,1.99546,2.90504,92.88083,
			66.59077,70.62356,90.94414,44.58142,56.31236,36.64184,
			49.74453,63.79418,46.95276,44.05808,49.39597,69.07668,
			84.39739, 83.36733,117.15848,46.46170,56.54927,50.07679,
			8.78674, 4.76208,1.87580,8.93194,9.84913,79.72916,
			80.67797,1.02911,105.97831,42.24153,45.80032,39.11855,
			46.98218,64.73236,47.50777,47.50120,61.31783,59.82965,
			97.01690,92.86451,100.03872,53.47676,45.08257,40.29724,
			1.17818,2.28851,7.21012,2.08823,9.92795,114.70823,
			89.52983,110.87244,80.88866,42.76993,41.59173,69.31875,
			69.13236,55.68271,46.29440,52.00012,47.97056,62.00336,
			103.73257,115.46924,70.50487,45.30505,31.63784,49.24166,
			1.54520,5.55203,8.52763,4.22529,6.88590,107.00053,
			77.90802,104.15741,77.07306,46.26307,47.90226,47.42976,
			46.70043,63.77651,69.04682,41.49764,66.46166,57.14738,
			118.08252,91.12425,111.00932,32.71311,44.28337,34.93989,
			2.04520,6.20479,1.17515,2.73232,2.68511,75.58591,
			106.59347,77.05086,102.13579,54.87944,60.72788,60.48774,
			51.13307,62.05659,52.41536,55.48826,44.51350,63.96906,
			112.54468, 67.53957,71.60108,30.53714,39.82132,27.04641,
			2.98750,3.32024,2.54505,3.50361,5.90335,91.01019,
			70.75974,65.80628,110.94266,53.51937,69.78840,40.05870,
			56.02625,64.87466,59.89761,36.39665,48.05514,56.34986,
			112.66951,66.20389,84.20279,40.28365,41.40094,53.86164,
			3.55556,9.64141,3.90208,8.42668,1.36501,102.50847,
			111.33002,102.12837,73.88463,58.22661,56.09830,35.21822,
			69.89594,64.04078,66.50324,45.18126,39.00801,42.52164,
			74.58013,68.88247,94.31696,29.33126,50.39574,41.80056,
			3.40995,8.30611,9.80945,5.02518,2.65476,95.65513,
			67.89477,66.97169,77.65441,45.20685,64.18325,60.70318,
			63.15490,67.43899,57.47517,66.36837,67.48319,49.78987,
			119.10328,101.65764,101.82969,29.75412,43.79614,27.15478,
			1.22403,9.30839,4.00674,3.92754,3.11514,114.42311,
			115.10536,111.69593,88.45974,50.51457,51.66247,46.71994,
			47.16158,50.54379,53.86872,45.68879,47.99086,35.79056,
			73.62137,104.34571,68.27064,44.87873,44.49204,29.28443,
			9.94917,8.16451,9.42947,2.30429,1.77931,67.45271,
			106.01536,93.56141,106.33993,66.82969,35.74610,56.75425,
			48.49634,65.21591,65.36423,51.23599,63.90016,64.44303,
			95.37784,98.56479,104.30943,54.15159,33.92579,30.10237,
			5.31589,3.91944,7.41518,7.37179,6.07273,68.66767,
			98.28917,100.24173,75.07158,41.10486,54.08275,40.73027,
			58.22124,47.71185,45.26840,35.63875,66.29298,68.56731,
			79.94481,118.23518,66.88661,46.40015,36.85682,36.63649,
			5.43684,8.012005,6.76342,5.54881,6.93289,76.49527,
			88.30664,71.13927,96.47198,62.74221,35.22243,54.33807,
			64.29392,48.91301,53.03774,38.65431,45.65192,65.36231,
			93.98323,96.13746,97.57894,52.11006,47.51148,29.04268,
			8.56982,7.66494,8.84172,8.35864,1.97540,115.98692,
			101.22980,108.11245,73.97923,65.44365,45.23138,65.16940,
			62.92194,64.81122,63.68278,56.49815,36.33445,61.74986,
			118.60826,92.55926,104.31152,29.65341,33.93534,42.28892};
#pragma romdata
/*******/

/*Prot�tipo de fun��es*/
void inicializarSerial(void);
void ordenar(BlocoMem *vetorMem, int numBlocos);

/******/

//TODO:TESTE
int getInt(){
	i=0;
	while(i<TAM_NUM){
			if(PIR1bits.RCIF){				//Checa se foi registrador foi carregado
				PIR1bits.RCIF = 0;			//Reinicializa Flag de recebimento
				cbuffer = RCREG;				//Recebe dado
				if(cbuffer == 0x0D){ 	//Checa se � Enter
					puts(CRLF);				//quebra linha
          while(BusyUSART());
					break;					//para iteracao
				} else {
				sbuffer[i++] = cbuffer;			//Poe na string para ser convertido
				putcUSART(cbuffer);				//imprime
        while(BusyUSART());
				}	}	}
   return atoi(sbuffer);
}

//TODO:FIM

/*Fun��o principal*/
void main(void)
{


    int numProcessosi = 0;
    int numBlocosi = 0;
    int tamTodosProcessosi = 0;
    int tamTodosBlocosi = 0;

    int i = 0, j = 0;


    char numProcessos[TAM_NUM];
    char numBlocos[TAM_NUM];
    char tamTodosProcessos[TAM_NUM];
    char tamTodosBlocos[TAM_NUM];

    char valor[5] = "   \0";
    char valor2[5] = "   \0";
    char valor3[5] = "   \0";



	//TODO:Iniciliza a serial
	inicializarSerial();

	//TODO: INTRO +  mensagem -> numero de Processos
  printf("\t\t%S%S%S", msgInicial, CRLF, msgNumProcessos);
  while(BusyUSART());

  //TODO:Le numero de Processos
	numProcessosi = getInt();

  //TODO:mensagem, le -> numero de BlocosDeMemoria
  printf("%S",msgNumBlocos);
  while(BusyUSART());

	while(!DataRdyUSART());
	getsUSART(numBlocos, TAM_NUM);
	putsUSART(numBlocos);

	//Convers�o do x buscado para um valor int
	numBlocosi = atoi(numBlocos);

    if(numProcessosi > 5) {

    putsUSART(msgTodosProcessos);

	while(!DataRdyUSART());
	getsUSART(tamTodosProcessos, TAM_NUM);
	putsUSART(tamTodosProcessos);

	//Convers�o do x buscado para um valor int
	tamTodosProcessosi = atoi(tamTodosProcessos);



    putsUSART(msgTodosBlocos);

	while(!DataRdyUSART());
	getsUSART(tamTodosBlocos, TAM_NUM);
	putsUSART(tamTodosBlocos);

	//Convers�o do x buscado para um valor int
	tamTodosBlocosi = atoi(tamTodosBlocos);

    } else {

         //at� 5 Processos definindo as condicoes de cada parte,


        putsUSART(tamKdProcesso);

        for(i=0;i<numProcessosi;i++) {
            putsUSART(processo);
            valor[0] = i + 1 + '0'; //Convers�o INT para CHAR
            putsUSART(valor);

            while(!DataRdyUSART());
	        getsUSART(valor, 1);
	        putsUSART(valor);

            tamProcessos[i] = atoi(valor);
        }

        putsUSART(tamKdBloco);




        for(i=0;i<numBlocosi;i++) {

            putsUSART(bloco);
            valor2[0] = i + 1 + '0'; //Convers�o INT para CHAR
            putsUSART(valor2);

            while(!DataRdyUSART());
	        getsUSART(valor2, 1);
	        putsUSART(valor2);

            vBlocoMem[i].tamanho = atoi(valor2);
            vBlocoMem[i].id = i+1;
        }
    }

    ordenar(vBlocoMem, numBlocosi);

    putsUSART(cabecalho);

    for(i=0;i<numProcessosi;i++) {

        //Varre vetor de blocos de mem�ria ordenado descrescentemente
        for(j=0;j<numBlocosi;j++) {

            //Ha espaco?
            if(vBlocoMem[j].tamanho >= tamProcessos[i]){
                //Imprime linha output

                putsUSART(t);

                itoa(i+1,valor3);
                putsUSART(valor3);

                putsUSART(t);
                putsUSART(t);
                putsUSART(t);
                putsUSART(t);

                itoa(tamProcessos[i],valor3);
                putsUSART(valor3);

                putsUSART(t);
                putsUSART(t);
                putsUSART(t);

                itoa(vBlocoMem[j].id,valor3);
                putsUSART(valor3);

                putsUSART(t);
                putsUSART(t);
                putsUSART(t);

               itoa(vBlocoMem[j].tamanho,valor3);
                putsUSART(valor3);

                putsUSART(t);
                putsUSART(t);
                putsUSART(t);
                putsUSART(t);
                putsUSART(t);

               itoa(vBlocoMem[j].tamanho - tamProcessos[i],valor3);
                putsUSART(valor3);

                //Registra alteracao no tamanho do bloco
                vBlocoMem[j].tamanho -= tamProcessos[i];

                //TODO: Reordena o vetor de BlocosMem.
                ordenar(vBlocoMem,numBlocos);
                break;
            }
        }
        //Espaco nao encontrado
        if(j == numBlocosi){

            putsUSART(t);

            itoa(i+1,valor3);
            putsUSART(valor3);

            putsUSART(t);
            putsUSART(t);
            putsUSART(t);
            putsUSART(t);

            itoa(tamProcessos[i],valor3);
            putsUSART(valor3);

            putsUSART(impossivel);

            putsUSART(n);
        }
    }

        return(0);





/*

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
        //at� 5 Processos definindo as condicoes de cada parte,
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

    //TODO: Ordena��o inicial da mem�ria
    ordenar(vBlocoMem, numBlocos);
    //Imprime cabecalho da lista output.
    printf("\nID Processo:\t Tamanho:\t ID Bloco:\tEspaco no Bloco:\tEspaco Restante:\n");

//TODO: Regiao critica.
//    Para cada processo.
    for(i=0;i<numProcessos;i++) {

        //Varre vetor de blocos de mem�ria ordenado descrescentemente
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

 */

}
/*******/

/*Configura e habilita a comunica��o serial*/
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
/******/

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
