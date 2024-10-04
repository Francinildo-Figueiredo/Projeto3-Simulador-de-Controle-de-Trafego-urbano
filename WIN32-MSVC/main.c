/*
 * FreeRTOS Kernel V10.0.1
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/******************************************************************************
 * This project provides two demo applications.  A simple blinky style project,
 * and a more comprehensive test and demo application.  The
 * mainCREATE_SIMPLE_BLINKY_DEMO_ONLY setting is used to select between the two.
 * The simply blinky demo is implemented and described in main_blinky.c.  The
 * more comprehensive test and demo application is implemented and described in
 * main_full.c.
 *
 * This file implements the code that is not demo specific, including the
 * hardware setup and FreeRTOS hook functions.
 *
 *******************************************************************************
 * NOTE: Windows will not be running the FreeRTOS demo threads continuously, so
 * do not expect to get real time behaviour from the FreeRTOS Windows port, or
 * this demo application.  Also, the timing information in the FreeRTOS+Trace
 * logs have no meaningful units.  See the documentation page for the Windows
 * port for further information:
 * http://www.freertos.org/FreeRTOS-Windows-Simulator-Emulator-for-Visual-Studio-and-Eclipse-MingW.html
 *

 *
 *******************************************************************************
 */

/* Standard includes. */
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include "time.h"
//#include <conio.h>

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* This project provides two demo applications.  A simple blinky style demo
application, and a more comprehensive test and demo application.  The
mainCREATE_SIMPLE_BLINKY_DEMO_ONLY setting is used to select between the two.

If mainCREATE_SIMPLE_BLINKY_DEMO_ONLY is 1 then the blinky demo will be built.
The blinky demo is implemented and described in main_blinky.c.

If mainCREATE_SIMPLE_BLINKY_DEMO_ONLY is not 1 then the comprehensive test and
demo application will be built.  The comprehensive test and demo application is
implemented and described in main_full.c. */
#define mainCREATE_SIMPLE_BLINKY_DEMO_ONLY	1

/* This demo uses heap_5.c, and these constants define the sizes of the regions
that make up the total heap.  heap_5 is only used for test and example purposes
as this demo could easily create one large heap region instead of multiple
smaller heap regions - in which case heap_4.c would be the more appropriate
choice.  See http://www.freertos.org/a00111.html for an explanation. */
#define mainREGION_1_SIZE	7201
#define mainREGION_2_SIZE	29905
#define mainREGION_3_SIZE	6407

/*-----------------------------------------------------------*/

/*
 * main_blinky() is used when mainCREATE_SIMPLE_BLINKY_DEMO_ONLY is set to 1.
 * main_full() is used when mainCREATE_SIMPLE_BLINKY_DEMO_ONLY is set to 0.
 */
//extern void main_blinky( void );
//extern void main_full( void );

/*
 * Only the comprehensive demo uses application hook (callback) functions.  See
 * http://www.freertos.org/a00016.html for more information.
 */
//void vFullDemoTickHookFunction( void );
//void vFullDemoIdleFunction( void );

/*
 * This demo uses heap_5.c, so start by defining some heap regions.  It is not
 * necessary for this demo to use heap_5, as it could define one large heap
 * region.  Heap_5 is only used for test and example purposes.  See
 * http://www.freertos.org/a00111.html for an explanation.
 */
static void  prvInitialiseHeap( void );

/*
 * Prototypes for the standard FreeRTOS application hook (callback) functions
 * implemented within this file.  See http://www.freertos.org/a00016.html .
 */
void vApplicationMallocFailedHook( void );
void vApplicationIdleHook( void );
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName );
void vApplicationTickHook( void );
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize );

/*
 * Writes trace data to a disk file when the trace recording is stopped.
 * This function will simply overwrite any trace files that already exist.
 */
static void prvSaveTraceFile( void );

/*-----------------------------------------------------------*/

/* When configSUPPORT_STATIC_ALLOCATION is set to 1 the application writer can
use a callback function to optionally provide the memory required by the idle
and timer tasks.  This is the stack that will be used by the timer task.  It is
declared here, as a global, so it can be checked by a test that is implemented
in a different file. */
StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];

/* Notes if the trace is running or not. */
static BaseType_t xTraceRunning = pdTRUE;

/*-----------------------------------------------------------*/

// Semáforo binário para controlar seguir em frente e a direita e  no cruzamento índice 0 (Norte-Sul) e 1 (Leste-Oeste)
SemaphoreHandle_t semaforoFrenteDireita[2]; 
// Semáforo binário para controlar conversão a esquerda no cruzamento índice 0 (Norte-Leste), 1 (Sul-Oeste), 2 (Leste-Sul), 3 (Oeste-Norte)
SemaphoreHandle_t semaforoEsquerda[4]; 

void TaskCruzamento(void *param) {
	// Inicializa os semáforos
	for (int i = 0; i < 4; i++) {
		if (i < 2)
			semaforoFrenteDireita[i] = xSemaphoreCreateBinary(); 
		semaforoEsquerda[i] = xSemaphoreCreateBinary();
	}

	int idCruzamento = (int)param; // Converte o ponteiro de volta para um inteiro
	int delayViaNS = idCruzamento * 30000; // Tempo para percorrer entre cruzamentos Norte-Sul
	int delayViaEW = idCruzamento * 36000; // Tempo para percorrer entre cruzamentos Leste-Oeste

	// Os índices indicam a posição dos semáforos 0 = Norte, 1 = Sul, 2 = Leste, 3 = Oeste
	while (1) {
		// Fase NS-Straight e EW-Left
		xSemaphoreGive(semaforoFrenteDireita[0]);    // Libera a passagem para seguir em frente e a direita Norte-Sul
		xSemaphoreTake(semaforoFrenteDireita[1], 0); // Bloqueia seguir em frente e a direita Leste-Oeste
		for (int i = 0; i < 4; i++) {
			xSemaphoreTake(semaforoEsquerda[i], 0); // Bloquia a conversões a esquerda
		}
		//printf("Fluxo Norte-Sul e Sul-Norte\n");
		vTaskDelay(1000);

		xSemaphoreTake(semaforoFrenteDireita[0], 0); // Bloqueia semáforos Norte e Sul
		xSemaphoreGive(semaforoEsquerda[2]);         // Libera conversão a esquerda Leste-Sul
		//printf("Fluxo Leste-Sul\n");;
		vTaskDelay(1000);

		xSemaphoreTake(semaforoEsquerda[2], 0); // Bloquia a conversão a esquerda Leste-Sul
		xSemaphoreGive(semaforoEsquerda[3]);  // Libera conversão a esquerda Oeste-Norte
		//printf("Fluxo Oeste-Norte\n");
		vTaskDelay(1000);

		// Fase EW-Straight e NS-Left
		xSemaphoreTake(semaforoEsquerda[3], 0); // Bloquia a conversão a esquerda Oeste-Norte
		xSemaphoreGive(semaforoFrenteDireita[1]); // Libera a passagem para seguir em frente e a direita (Leste-Oeste)
		//printf("Fluxo Leste-Oeste e Oeste-Leste\n");
		vTaskDelay(1000);

		xSemaphoreTake(semaforoFrenteDireita[1], 0); // Bloqueia seguir em frente e a direita Leste-Oeste
		xSemaphoreGive(semaforoEsquerda[0]);  // Libera conversão a esquerda Norte-Leste
		//printf("Fluxo Norte-Leste\n");
		vTaskDelay(1000);

		xSemaphoreTake(semaforoEsquerda[0], 0); // Bloquia a conversão a esquerda Norte-Leste
		xSemaphoreGive(semaforoEsquerda[1]);  // Libera conversão a esquerda Sul-Oeste
		//printf("Fluxo Sul-Oeste\n");
		vTaskDelay(1000);
	}
}

typedef enum {
	FRENTE,
	DIREITA,
	ESQUERDA
} Direcao;

typedef enum {
	N,
	S,
	E,
	W
} idSemaforo;

typedef enum {
	A,
	B,
	C,
	D
} idCruzamento;

typedef struct {
	int idVeiculo;
	idCruzamento cruzamentoAtual;
	idSemaforo semaforoAtual;
	Direcao direcao;
} Veiculo;

char trafegoBase[23][50] = {
		"          |       |          |       |          ",
		"          |   |   |          |   |   |          ",
		"          |       |          |       |          ",
		"          |___|   |          |___|   |          ", // Linha 3 // Colunas 12, 16, 31 e 35
		"----------         ----------         ----------",
		"                   |                  |         ", // Linha 5 // Colunas 20 e 39
		"- - - - -           - - - - -          - - - - -",
		"         |                   |                  ", // Linha 7 // Colunas 8 e 28
		"----------         ----------         ----------",
		"          |    ---|          |    ---|          ",
		"          |   |   |          |   |   |          ", // Linha 10
		"          |       |          |       |          ",
		"          |       |          |       |          ",
		"          |___|   |          |___|   |          ", // Linha 13
		"----------         ----------         ----------",
		"                   |                  |         ", // Linha 15
		"- - - - -           - - - - -          - - - - -",
		"         |                   |                  ", // Linha 17
		"----------         ----------         ----------",
		"          |    ---|          |    ---|          ", // Linha 19
		"          |   |   |          |   |   |          ",
		"          |       |          |       |          ",
		"          |   |   |          |   |   |          "
};

char trafego[23][50];

void limpaTrafego() {
	for (int i = 0; i < 23; i++) {
		for (int j = 0; j < 50; j++) {
			trafego[i][j] = trafegoBase[i][j];
		}
	}
}

void modificaTrafego(int lin, int col) {
	limpaTrafego();
	trafego[lin][col] = 'o';
}

void printaTrafego() {
	// Habilita escape codes no Windows
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD mode = 0;
	GetConsoleMode(hConsole, &mode);
	SetConsoleMode(hConsole, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

	while (1) {
		// Move o cursor para a posição superior esquerda do console
		printf("\033[H");
		for (int i = 0; i < 23; i++) {
			printf("%s\n", trafego[i]);
		}
		vTaskDelay(10);
	}
}

void animacaoViaNS(int lin, int col, int sig) {
	for (int i = 0; i < 5; i++) {
		modificaTrafego(lin + i*sig, col); // sig = 1 -> sentido Norte-Sul, sig = -1 -> sentido Sul-Norte
		vTaskDelay(600);
	}
}

void animacaoViaEW(int lin, int col, int sig) {
	for (int i = 0; i < 10; i++) {
		modificaTrafego(lin, col + i*sig); // sig = 1 -> sentido Oeste-Leste, sig = -1 -> sentido Leste-Oeste 
		vTaskDelay(360);
	}
}

void animacaoSairN(int lin, int col) {
	for (int i = 0; i < 2; i++) {
		vTaskDelay(100);
		modificaTrafego(lin - 4 * i, col);
		vTaskDelay(100);
	}
	modificaTrafego(-1, -1); // O veículo foi embora
}

void animacaoSairW(int lin, int col) {
	for (int i = 0; i < 3; i++) {
		vTaskDelay(100);
		modificaTrafego(lin, col - 4*i);
		vTaskDelay(100);
	}
	modificaTrafego(-1, -1); // O veículo foi embora
}

SemaphoreHandle_t mutexVeiculos;

void TaskVeiculo(void *param){
	srand(time(NULL));
	Veiculo *veiculo = (Veiculo*)param; // Pega os dados do veículo
	
	// Eu tenho 48 combinações possíveis a partir da condição inicial do veículo 4*4*3 = 48
	while (1) {
		if (xSemaphoreTake(mutexVeiculos, portMAX_DELAY) == pdTRUE) {
			if (veiculo->cruzamentoAtual == A && veiculo->semaforoAtual == N) { // Cruzamento A e semáforo Norte
				modificaTrafego(3, 12);
				vTaskDelay(300);
				if (veiculo->direcao == FRENTE) {
					// Espera pelo sinal do semáforo
					if (xSemaphoreTake(semaforoFrenteDireita[0], portMAX_DELAY)) {
						modificaTrafego(6, 12);
						vTaskDelay(100);
						animacaoViaNS(8, 12, 1);
						veiculo->cruzamentoAtual = C;
						modificaTrafego(13, 12);
						veiculo->direcao = rand() % 3; // Próxima direção do veículo
					}
					vTaskDelay(100); // Espera antes de tentar novamente
				}
				else if (veiculo->direcao == DIREITA) {
					if (xSemaphoreTake(semaforoFrenteDireita[0], portMAX_DELAY)) {
						animacaoSairW(5, 12);
						vTaskDelete(NULL);
					}
					vTaskDelay(100); // Espera antes de tentar novamente
				}
				else if (veiculo->direcao == ESQUERDA) {
					if (xSemaphoreTake(semaforoEsquerda[0], portMAX_DELAY)) {
						modificaTrafego(7, 12);
						vTaskDelay(100);
						animacaoViaEW(7, 16, 1);
						veiculo->cruzamentoAtual = B;
						veiculo->semaforoAtual = W;
						modificaTrafego(7, 28);
						veiculo->direcao = rand() % 3;
					}
					vTaskDelay(100); // Espera antes de tentar novamente
				}
			}
			
			else if (veiculo->cruzamentoAtual == A && veiculo->semaforoAtual == S) { // Cruzamento A e semáforo Sul
				modificaTrafego(10, 16);
				vTaskDelay(300);
				if (veiculo->direcao == FRENTE) {
					if (xSemaphoreTake(semaforoFrenteDireita[0], portMAX_DELAY)) {
						animacaoSairN(6, 16);
						vTaskDelete(NULL);
					}
					vTaskDelay(100); // Espera antes de tentar novamente
				}
				else if (veiculo->direcao == DIREITA) {
					if (xSemaphoreTake(semaforoFrenteDireita[0], portMAX_DELAY)) {
						modificaTrafego(7, 16);
						vTaskDelay(100);
						animacaoViaEW(7, 18, 1);
						veiculo->cruzamentoAtual = B;
						veiculo->semaforoAtual = W;
						modificaTrafego(7, 28);
						veiculo->direcao = rand() % 3;
					}
					vTaskDelay(100); // Espera antes de tentar novamente
				}
				else if (veiculo->direcao == ESQUERDA) {
					if (xSemaphoreTake(semaforoEsquerda[1], portMAX_DELAY)) {
						animacaoSairW(5, 16);
						vTaskDelete(NULL);
					}
					vTaskDelay(100); // Espera antes de tentar novamente
				}
			}
			else if (veiculo->cruzamentoAtual == A && veiculo->semaforoAtual == E) { // Cruzamento A e semáforo Leste
				modificaTrafego(5, 20);
				vTaskDelay(300);
				if (veiculo->direcao == FRENTE) {
					if (xSemaphoreTake(semaforoFrenteDireita[1], portMAX_DELAY)) {
						animacaoSairW(5, 14);
						vTaskDelete(NULL);
					}
					vTaskDelay(100); // Espera antes de tentar novamente
				}
				else if (veiculo->direcao == DIREITA) {
					if (xSemaphoreTake(semaforoFrenteDireita[1], portMAX_DELAY)) {
						animacaoSairN(5, 16);
						vTaskDelete(NULL);
					}
					vTaskDelay(100); // Espera antes de tentar novamente
				}
				else if (veiculo->direcao == ESQUERDA) {
					if (xSemaphoreTake(semaforoEsquerda[2], portMAX_DELAY)) {
						modificaTrafego(5, 12);
						vTaskDelay(100);
						animacaoViaNS(8, 12, 1);
						veiculo->cruzamentoAtual = C;
						veiculo->semaforoAtual = N;
						modificaTrafego(13, 12);
						veiculo->direcao = rand() % 3;
					}
					vTaskDelay(100); // Espera antes de tentar novamente
				}
			}
			else if (veiculo->cruzamentoAtual == A && veiculo->semaforoAtual == W) { // Cruzamento A e semáforo Oeste
				modificaTrafego(7, 8);
				vTaskDelay(300);
				if (veiculo->direcao == FRENTE) {
					if (xSemaphoreTake(semaforoFrenteDireita[1], portMAX_DELAY)) {
						modificaTrafego(7, 14);
						vTaskDelay(100);
						animacaoViaEW(7, 18, 1);
						veiculo->cruzamentoAtual = B;
						veiculo->semaforoAtual = W;
						modificaTrafego(7, 28);
						veiculo->direcao = rand() % 3;
					}
					vTaskDelay(100); // Espera antes de tentar novamente
				}
				else if (veiculo->direcao == DIREITA) {
					if (xSemaphoreTake(semaforoFrenteDireita[1], portMAX_DELAY)) {
						modificaTrafego(7, 12);
						vTaskDelay(100);
						animacaoViaNS(8, 12, 1);
						veiculo->cruzamentoAtual = C;
						veiculo->semaforoAtual = N;
						modificaTrafego(13, 12);
						veiculo->direcao = rand() % 3;
					}
					vTaskDelay(100); // Espera antes de tentar novamente
				}
				else if (veiculo->direcao == ESQUERDA) {
					if (xSemaphoreTake(semaforoEsquerda[1], portMAX_DELAY)) {
						modificaTrafego(7, 16);
						vTaskDelay(100);
						animacaoSairN(5, 16);
						vTaskDelete(NULL);
					}
					vTaskDelay(100); // Espera antes de tentar novamente
				}
			}
			// Libera o semáforo
			xSemaphoreGive(mutexVeiculos);
		}
	}
}

int main( void )
{
	setlocale(LC_ALL, "Portuguese");
	/* This demo uses heap_5.c, so start by defining some heap regions.  heap_5
	is only used for test and example reasons.  Heap_4 is more appropriate.  See
	http://www.freertos.org/a00111.html for an explanation. */
	prvInitialiseHeap();

	/* Initialise the trace recorder.  Use of the trace recorder is optional.
	See http://www.FreeRTOS.org/trace for more information. */
	vTraceEnable( TRC_START );

	limpaTrafego();
	mutexVeiculos = xSemaphoreCreateMutex(); // Inicializa o semáforo de contagem

	Veiculo veiculo1 = {.idVeiculo = 1, .cruzamentoAtual = A, .semaforoAtual = N, .direcao = FRENTE};
	Veiculo veiculo2 = { .idVeiculo = 2, .cruzamentoAtual = A, .semaforoAtual = S, .direcao = FRENTE };
	Veiculo veiculo3 = { .idVeiculo = 3, .cruzamentoAtual = A, .semaforoAtual = E, .direcao = FRENTE };
	Veiculo veiculo4 = { .idVeiculo = 4, .cruzamentoAtual = A, .semaforoAtual = W, .direcao = FRENTE };

	xTaskCreate(TaskCruzamento, (signed char*)"Cruzamento", configMINIMAL_STACK_SIZE, (void*)NULL, 1, NULL);

	xTaskCreate(TaskVeiculo, (signed char*)"Veiculo", configMINIMAL_STACK_SIZE, &veiculo1, 1, NULL);
	xTaskCreate(TaskVeiculo, (signed char*)"Veiculo", configMINIMAL_STACK_SIZE, &veiculo2, 1, NULL);
	xTaskCreate(TaskVeiculo, (signed char*)"Veiculo", configMINIMAL_STACK_SIZE, &veiculo3, 1, NULL);
	xTaskCreate(TaskVeiculo, (signed char*)"Veiculo", configMINIMAL_STACK_SIZE, &veiculo4, 1, NULL);

	xTaskCreate(printaTrafego, (signed char*)"PrintarTrafego", configMINIMAL_STACK_SIZE, (void*)NULL, 1, NULL);
	
	vTaskStartScheduler(); 
	for (;;);
	return 0;
}
/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer or semaphore is created.  It is also called by various parts of the
	demo application.  If heap_1.c, heap_2.c or heap_4.c is being used, then the
	size of the	heap available to pvPortMalloc() is defined by
	configTOTAL_HEAP_SIZE in FreeRTOSConfig.h, and the xPortGetFreeHeapSize()
	API function can be used to query the size of free heap space that remains
	(although it does not provide information on how the remaining heap might be
	fragmented).  See http://www.freertos.org/a00111.html for more
	information. */
	vAssertCalled( __LINE__, __FILE__ );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
	/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
	to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
	task.  It is essential that code added to this hook function never attempts
	to block in any way (for example, call xQueueReceive() with a block time
	specified, or call vTaskDelay()).  If application tasks make use of the
	vTaskDelete() API function to delete themselves then it is also important
	that vApplicationIdleHook() is permitted to return to its calling function,
	because it is the responsibility of the idle task to clean up memory
	allocated by the kernel to any task that has since deleted itself. */

	/* Uncomment the following code to allow the trace to be stopped with any
	key press.  The code is commented out by default as the kbhit() function
	interferes with the run time behaviour. */
	/*
		if( _kbhit() != pdFALSE )
		{
			if( xTraceRunning == pdTRUE )
			{
				vTraceStop();
				prvSaveTraceFile();
				xTraceRunning = pdFALSE;
			}
		}
	*/

	#if ( mainCREATE_SIMPLE_BLINKY_DEMO_ONLY != 1 )
	{
		/* Call the idle task processing used by the full demo.  The simple
		blinky demo does not use the idle task hook. */
		vFullDemoIdleFunction();
	}
	#endif
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected.  This function is
	provided as an example only as stack overflow checking does not function
	when running the FreeRTOS Windows port. */
	vAssertCalled( __LINE__, __FILE__ );
}
/*-----------------------------------------------------------*/

void vApplicationTickHook( void )
{
	/* This function will be called by each tick interrupt if
	configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h.  User code can be
	added here, but the tick hook is called from an interrupt context, so
	code must not attempt to block, and only the interrupt safe FreeRTOS API
	functions can be used (those that end in FromISR()). */
	#if ( mainCREATE_SIMPLE_BLINKY_DEMO_ONLY != 1 )
	{
		vFullDemoTickHookFunction();
	}
	#endif /* mainCREATE_SIMPLE_BLINKY_DEMO_ONLY */
}
/*-----------------------------------------------------------*/

void vApplicationDaemonTaskStartupHook( void )
{
	/* This function will be called once only, when the daemon task starts to
	execute	(sometimes called the timer task).  This is useful if the
	application includes initialisation code that would benefit from executing
	after the scheduler has been started. */
}
/*-----------------------------------------------------------*/

void vAssertCalled( unsigned long ulLine, const char * const pcFileName )
{
static BaseType_t xPrinted = pdFALSE;
volatile uint32_t ulSetToNonZeroInDebuggerToContinue = 0;

	/* Called if an assertion passed to configASSERT() fails.  See
	http://www.freertos.org/a00110.html#configASSERT for more information. */

	/* Parameters are not used. */
	( void ) ulLine;
	( void ) pcFileName;

	printf( "ASSERT! Line %ld, file %s, GetLastError() %ld\r\n", ulLine, pcFileName, GetLastError() );

 	taskENTER_CRITICAL();
	{
		/* Stop the trace recording. */
		if( xPrinted == pdFALSE )
		{
			xPrinted = pdTRUE;
			if( xTraceRunning == pdTRUE )
			{
				vTraceStop();
				prvSaveTraceFile();
			}
		}

		/* You can step out of this function to debug the assertion by using
		the debugger to set ulSetToNonZeroInDebuggerToContinue to a non-zero
		value. */
		while( ulSetToNonZeroInDebuggerToContinue == 0 )
		{
			__asm{ NOP };
			__asm{ NOP };
		}
	}
	taskEXIT_CRITICAL();
}
/*-----------------------------------------------------------*/

static void prvSaveTraceFile( void )
{
FILE* pxOutputFile;

	fopen_s( &pxOutputFile, "Trace.dump", "wb");

	if( pxOutputFile != NULL )
	{
		fwrite( RecorderDataPtr, sizeof( RecorderDataType ), 1, pxOutputFile );
		fclose( pxOutputFile );
		printf( "\r\nTrace output saved to Trace.dump\r\n" );
	}
	else
	{
		printf( "\r\nFailed to create trace dump file\r\n" );
	}
}
/*-----------------------------------------------------------*/

static void  prvInitialiseHeap( void )
{
/* The Windows demo could create one large heap region, in which case it would
be appropriate to use heap_4.  However, purely for demonstration purposes,
heap_5 is used instead, so start by defining some heap regions.  No
initialisation is required when any other heap implementation is used.  See
http://www.freertos.org/a00111.html for more information.

The xHeapRegions structure requires the regions to be defined in start address
order, so this just creates one big array, then populates the structure with
offsets into the array - with gaps in between and messy alignment just for test
purposes. */
static uint8_t ucHeap[ configTOTAL_HEAP_SIZE ];
volatile uint32_t ulAdditionalOffset = 19; /* Just to prevent 'condition is always true' warnings in configASSERT(). */
const HeapRegion_t xHeapRegions[] =
{
	/* Start address with dummy offsets						Size */
	{ ucHeap + 1,											mainREGION_1_SIZE },
	{ ucHeap + 15 + mainREGION_1_SIZE,						mainREGION_2_SIZE },
	{ ucHeap + 19 + mainREGION_1_SIZE + mainREGION_2_SIZE,	mainREGION_3_SIZE },
	{ NULL, 0 }
};

	/* Sanity check that the sizes and offsets defined actually fit into the
	array. */
	configASSERT( ( ulAdditionalOffset + mainREGION_1_SIZE + mainREGION_2_SIZE + mainREGION_3_SIZE ) < configTOTAL_HEAP_SIZE );

	/* Prevent compiler warnings when configASSERT() is not defined. */
	( void ) ulAdditionalOffset;

	vPortDefineHeapRegions( xHeapRegions );
}
/*-----------------------------------------------------------*/

/* configUSE_STATIC_ALLOCATION is set to 1, so the application must provide an
implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
used by the Idle task. */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
/* If the buffers to be provided to the Idle task are declared inside this
function then they must be declared static - otherwise they will be allocated on
the stack and so not exists after this function exits. */
static StaticTask_t xIdleTaskTCB;
static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

	/* Pass out a pointer to the StaticTask_t structure in which the Idle task's
	state will be stored. */
	*ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

	/* Pass out the array that will be used as the Idle task's stack. */
	*ppxIdleTaskStackBuffer = uxIdleTaskStack;

	/* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
	Note that, as the array is necessarily of type StackType_t,
	configMINIMAL_STACK_SIZE is specified in words, not bytes. */
	*pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
/*-----------------------------------------------------------*/

/* configUSE_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
application must provide an implementation of vApplicationGetTimerTaskMemory()
to provide the memory that is used by the Timer service task. */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize )
{
/* If the buffers to be provided to the Timer task are declared inside this
function then they must be declared static - otherwise they will be allocated on
the stack and so not exists after this function exits. */
static StaticTask_t xTimerTaskTCB;

	/* Pass out a pointer to the StaticTask_t structure in which the Timer
	task's state will be stored. */
	*ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

	/* Pass out the array that will be used as the Timer task's stack. */
	*ppxTimerTaskStackBuffer = uxTimerTaskStack;

	/* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
	Note that, as the array is necessarily of type StackType_t,
	configMINIMAL_STACK_SIZE is specified in words, not bytes. */
	*pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

