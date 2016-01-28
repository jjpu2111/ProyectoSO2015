#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include <stdlib.h>
#define I 8
#define TRUE 1

/*Estructuras*/
/* Estamos planteando dos formas de acceder a nuestro modulo
 * 1: El que solicite un recurso llama al manejador con el IRQ y la PCB y nosotros activamos el recurso.
 * 2: El solicitante llama directamente al modulo al cual quiere acceder y nosotros nos encargamos de interrumpir la llamada. 
 * 
 * Cual les parece mas conveniente??  
 * 
 * En el codigo interrupciones_ejemplo.c se encuentra el funcionamiento de ambas opciones 
 */

typedef struct  
{
	int numero_proceso;
	int prioridad;
	int IRQ;
	int id_modulo;
} PCB;

sem_t IRQ[I]; //vector de semaforos de interrupciones

void manejadorInterrupciones(PCB id_pcb, int irqO);
int main()
{
int j; 

	for (j = 0; j < I; j++) //incializar los semaforos
		sem_init(&IRQ[j], 0, 0);
	
return 0;	
}


void manejadorInterrupciones(PCB id_pcb, int irqO)
{
		switch(irqO)
		{
			case 0: //IRQ0: Solicitud de tiempo de CPU. 
			sem_post(&IRQ[0]);
			break;
			case 1: //IRQ1: Solicitud de espacio en memoria principal.
			sem_post(&IRQ[1]);
			break;
			case 2: //IRQ2: Eliminación de procesos de la memoria principal.
			sem_post(&IRQ[2]);
			break;
			case 3: //IRQ3: Compactación de la memoria principal.
			sem_post(&IRQ[3]); 
			break;
			case 4: //IRQ4: Solicitud de espacio en disco.
			sem_post(&IRQ[4]);
			break;
			case 5: // IRQ5: Envío y recepción de datos a través de la tarjeta de red.
			sem_post(&IRQ[5]);
			break;
			case 6: //IRQ6: Solicitud de cierre de la tarjeta de red.
			sem_post(&IRQ[6]);
			break; 
			case 7:  //IRQ7: Solicitud de defragmentación del disco.
			sem_post(&IRQ[7]);
			break; 
			default:
			printf("Non valid interrupt handler\n");
			break;
		}
}
