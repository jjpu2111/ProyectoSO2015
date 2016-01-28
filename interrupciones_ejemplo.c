#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include <stdlib.h>
//#include "principal.h"
#define I 8
#define TRUE 1

/*Estructuras*/

typedef struct  
{
	int numero_proceso;
	int prioridad;
	int IRQ;
	int id_modulo;
} PCB;

sem_t IRQ[I]; //vector de semaforos de interrupciones

void manejadorInterrupciones(PCB id_pcb, int irqO);
void funcionA(PCB hi);
void funcionB(PCB hix);
int main()
{
	int pid;
	pid= fork();
	int j; 
	PCB hi;
	
	for (j = 0; j < I; j++) //incializar los semaforos
		sem_init(&IRQ[j], 0, 0);
	hi.numero_proceso=2;
	hi.IRQ=9;
	if(pid==0)
	{
		manejadorInterrupciones(hi, 6);
		/*
		 * En este caso, el proceso llama al manejador de interrupciones con el IRQ del
		 * servicio que desea obtener, y el manejador se encarga de llamar y activar al 
		 * modulo correspondiente 
		 * (Opcion 1)
		 */
	}
	else
	{
		funcionB(hi);
		/*
		 * En este caso, el proceso llama directamnete al servicio que desea obtener
		 * y el manejador se encarga de hacer la interrupcion y luego activar el 
		 * modulo del servicio
		 * (Opcion 2)
		 */
	}
	

	return 0;	
}

void funcionA(PCB hi)
{
	/*
	 * (Opcion 1)
	 * El proceso llamo al manejador, el manejador llama a la funcion y 
	 * activa el semaforo
	 */ 
	sem_wait(&IRQ[6]);// Esperando la activacion por parte del manejador
	printf("Hola soy la funcion A\n");
	
}

void funcionB(PCB hix)
{
	/*
	 * (Opcion 2)
	 * La funcion fue llamada directamente por el proceso.
	 */ 
	manejadorInterrupciones(hix, 3);// El manejador interrumpiendo la llamada al servicio
	sem_wait(&IRQ[3]); // En espera de la activacion por parte del manejador
	printf("Hola soy la funcion B\n");
	
}

void manejadorInterrupciones(PCB id_pcb, int irqO)
{

	
		
		switch(irqO)
		{
		
			case 0:
			sem_post(&IRQ[0]);
			break;
			case 1:
			sem_post(&IRQ[1]);
			break;
			case 2:
			sem_post(&IRQ[2]);
			break;
			case 3:
			sem_post(&IRQ[3]); // Activacion del servicio solicitado
			break;
			case 4:
			sem_post(&IRQ[4]);
			break;
			case 5:
			sem_post(&IRQ[5]);
			break;
			case 6:
			sem_post(&IRQ[6]); //Activacion del servicio solicitado por el proceso
			funcionA(id_pcb);  //LLamada al modulo correspondiente 
			break; 
			case 7:
			sem_post(&IRQ[7]);
			
			break; 
			default:
			printf("Non valid interrupt handler\n");
			break;
		}
}



