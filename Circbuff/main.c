#include "stm32f10x.h"
#include "cmsis_os.h"
#include "uart.h"

void Thread1 (void const *argument);
void Thread2 (void const *argument);
void Thread3 (void const *argument);
void Thread4 (void const *argument);
void Thread5 (void const *argument);
osThreadDef(Thread1, osPriorityNormal, 1, 0);
osThreadDef(Thread2, osPriorityNormal, 1, 0);
osThreadDef(Thread3, osPriorityNormal, 1, 0);
osThreadDef(Thread4, osPriorityNormal, 1, 0);
osThreadDef(Thread5, osPriorityNormal, 1, 0);
osThreadId T1;
osThreadId T2;
osThreadId T3;
osThreadId T4;
osThreadId T5;

osMessageQId Q_LED;
osMessageQDef (Q_LED,0x16,unsigned char);
osEvent  result;

osMutexId mutex;
osMutexDef(mutex);
osSemaphoreId item_semaphore;                         // Semaphore ID
osSemaphoreDef(item_semaphore);                       // Semaphore definition
osSemaphoreId space_semaphore;                         // Semaphore ID
osSemaphoreDef(space_semaphore);                       // Semaphore definition

long int x=0;
long int i=0;
long int j=0;
long int k=0;
long int l=0;
const unsigned int size = 7;
unsigned char buffer[size];
unsigned int tail = 0;
unsigned int head = 0;

void put(unsigned char an_item){
	osSemaphoreWait(space_semaphore, osWaitForever);
	osMutexWait(mutex, osWaitForever);
	buffer[tail] = an_item;
	tail = (tail + 1) % size;
	osMutexRelease(mutex);
	osSemaphoreRelease(item_semaphore);
}

unsigned char get(){
	unsigned int rr = 0xff;
	osSemaphoreWait(item_semaphore, osWaitForever);
	osMutexWait(mutex, osWaitForever);
	rr = buffer[head];
	head = (head + 1) % size;
	osMutexRelease(mutex);
	osSemaphoreRelease(space_semaphore);
	return rr;
}

int loopcount = 10;

void Thread1 (void const *argument) 
{
	unsigned char item = 0x30;
	for(i=0; i<loopcount; i++){
		put(item++);
	}
}

void Thread2 (void const *argument) 
{
	unsigned int data = 0x00;
	for(j=0; j<loopcount; j++){
		data = get();
		osMessagePut(Q_LED,data,osWaitForever);
	}
}

void Thread3 (void const *argument) 
{
	unsigned int data2 = 0x00;
	for(k=0; k<loopcount; k++){
		data2 = get();
		osMessagePut(Q_LED,data2,osWaitForever);
	}
}

void Thread4 (void const *argument) 
{
	unsigned int data3 = 0x00;
	for(l=0; l<20; l++){
		data3 = get();
		osMessagePut(Q_LED,data3,osWaitForever);
	}
}

void Thread5(void const *argument)
{
	for(;;){
		result = 	osMessageGet(Q_LED,osWaitForever);
		SendChar(result.value.v);
	}
}

int main (void) 
{
	osKernelInitialize ();                    // initialize CMSIS-RTOS
	USART1_Init();
	item_semaphore = osSemaphoreCreate(osSemaphore(item_semaphore), 0);
	space_semaphore = osSemaphoreCreate(osSemaphore(space_semaphore), size);
	mutex = osMutexCreate(osMutex(mutex));	
	
	Q_LED = osMessageCreate(osMessageQ(Q_LED),NULL);
	
	T1 = osThreadCreate(osThread(Thread1), NULL);
	T2 = osThreadCreate(osThread(Thread2), NULL);
	T3 = osThreadCreate(osThread(Thread3), NULL);
	T4 = osThreadCreate(osThread(Thread4), NULL);
	T5 = osThreadCreate(osThread(Thread5), NULL);
 
	osKernelStart ();                         // start thread execution 
}
