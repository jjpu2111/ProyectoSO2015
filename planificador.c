/* NOTA 

Prioridades: 0 - 5 
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

/* Procedimientos */
void creador_procesos(pthread_t procesos[], PCB procesos_P0[], PCB procesos_P1[], PCB procesos_P2[], PCB procesos_P3[], PCB procesos_P4[], PCB procesos_P5[]);
void *planificador(PCB procesos_P0[], PCB procesos_P1[], PCB procesos_P2[], PCB procesos_P3[], PCB procesos_P4[], PCB procesos_P5[]);

/* Principal */
int main(void)
{
	/* Cola de procesos por prioridad */
	PCB procesos_P0[N], procesos_P1[N], procesos_P2[N], procesos_P3[N], procesos_P4[N], procesos_P5[N];
	/* Cola de hilos, estos son los que podran ejecutarse */
	pthread_t procesos[N];
	/* Variables */
	int i;

	/* Semilla para el srand */
	srand(time(NULL));

	/* Pongo en 0s los semaforos para bloquear los procesos */
	for (i = 0; i < N; i++)
			sem_init(&sem_procesos[i], 0, 1);

	/* Se crean los procesos */
	creador_procesos(procesos, procesos_P0, procesos_P1, procesos_P2, procesos_P3, procesos_P4, procesos_P5);
	
	printf("press enter to continue\n");
	getchar();

	return 0;
}

/* ------------------------------------------------------------------------------------ */

void creador_procesos(pthread_t procesos[], PCB procesos_P0[], PCB procesos_P1[], PCB procesos_P2[], PCB procesos_P3[], PCB procesos_P4[], PCB procesos_P5[])
{
	interrupcion(0);
	semaforo(0);
	int i, prioridad, idmodulo;
	for (i = 0; i < N; i++)
	{
		prioridad = rand()MOD 5; /* 0 - 5 */
		
		if(prioridad == 0)
		{	
			idmodulo = rand()MOD 3; /* 0 - 3 */
			procesos_P0[i].numero_proceso= i;
			procesos_P0[i].prioridad = prioridad;
			procesos_P0[i].IRQ = 0;
			procesos_P0[i].id_modulo = idmodulo;
			if(pthread_create(&procesos[i], NULL, (void *) &planificador, (void *) &procesos_P0[i]))
				printf("ERROR: Creacion del hilo incorrecta %d\n", i );
			else
				printf("AVISO: Hilo %d creado exitosamente\n", i);
		}
		else
		{
			idmodulo = 4 + rand()%(3); /* 4 - 6 */
			if(prioridad == 1)
			{
				procesos_P1[i].numero_proceso= i;
				procesos_P1[i].prioridad = prioridad;
				procesos_P1[i].IRQ = 0;
				procesos_P1[i].id_modulo = idmodulo;
				if(pthread_create(&procesos[i], NULL, (void *) &planificador, (void *) &procesos_P1[i]))
					printf("ERROR: Creacion del hilo incorrecta %d\n", i );
				else
					printf("AVISO: Hilo %d creado exitosamente\n", i);
				
			}
			else if(prioridad == 2)
			{
				procesos_P2[i].numero_proceso = i;
				procesos_P2[i].prioridad = prioridad;
				procesos_P2[i].IRQ = 0;
				procesos_P2[i].id_modulo = idmodulo;
				if(pthread_create(&procesos[i], NULL, (void *) &planificador, (void *) &procesos_P2[i]))
					printf("ERROR: Creacion del hilo incorrecta %d\n", i);
				else
					printf("AVISO: Hilo %d creado exitosamente\n", i);
			}
			else if(prioridad == 3)
			{
				procesos_P3[i].numero_proceso = i;
				procesos_P3[i].prioridad = prioridad;
				procesos_P3[i].IRQ = 0;
				procesos_P3[i].id_modulo = idmodulo;
				if(pthread_create(&procesos[i], NULL, (void *) &planificador, (void *) &procesos_P3[i]))
					printf("ERROR: Creacion del hilo incorrecta %d\n", i);
				else
					printf("AVISO: Hilo %d creado exitosamente\n", i);
			}
			else if(prioridad == 4)
			{
				procesos_P4[i].numero_proceso = i;
				procesos_P4[i].prioridad = prioridad;
				procesos_P4[i].IRQ = 0;
				procesos_P4[i].id_modulo = idmodulo;
				if(pthread_create(&procesos[i], NULL, (void *) &planificador, (void *) &procesos_P4[i]))
					printf("ERROR: Creacion del hilo incorrecta %d\n", i);
				else
					printf("AVISO: Hilo %d creado exitosamente\n", i);
			}
			else
			{
				procesos_P5[i].numero_proceso = i;
				procesos_P5[i].prioridad = prioridad;
				procesos_P5[i].IRQ = 0;
				procesos_P5[i].id_modulo = idmodulo;
				if(pthread_create(&procesos[i], NULL, (void *) &planificador, (void *) &procesos_P5[i]))
					printf("ERROR: Creacion del hilo incorrecta %d\n", i);
				else
					printf("AVISO: Hilo %d creado exitosamente\n", i);
			}
		}
	}
}

void *planificador(PCB procesos_P0[], PCB procesos_P1[], PCB procesos_P2[], PCB procesos_P3[], PCB procesos_P4[], PCB procesos_P5[])
{
	int band = 1;
	
	printf("hola\n");
}

