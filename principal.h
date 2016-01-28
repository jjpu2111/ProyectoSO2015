IDmodulo: 0 - 6

IDmodulo 0 = Tarjeta de red
IDmodulo 1 = Servidor de impresion
IDmodulo 2 = Demonio de compactacion
IDmodulo 3 = Antivirus y Firewall
IDmodulo 4 = Procesos que soliciten CPU
IDmodulo 5 = Procesos que soliciten espacio en disco
IDmodulo 6 = Procesos que envien y soliciten datos a traves de la tarjeta de red */  


/* Librerias */
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include <stdlib.h>

#define MOD %
#define AND &&
#define OR ||
#define N 10

/* Etructuras */
sem_t mutex, atom, sem_procesos[N]; /* 2 semaforos sencillos y 1 cola de semaforos */

typedef struct  
{
	int numero_proceso;
	int prioridad;
	int IRQ;
	int id_modulo;
} PCB;



#endif
