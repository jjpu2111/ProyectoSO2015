/* 
NOTA 

Prioridades: 0 - 5 
ID_modulo: 0 - 6

ID_modulo 0 = Tarjeta de red
ID_modulo 1 = Servidor de impresion
ID_modulo 2 = Demonio de compactacion
ID_modulo 3 = Antivirus y Firewall
ID_modulo 4 = Procesos que soliciten CPU
ID_modulo 5 = Procesos que soliciten espacio en disco
ID_modulo 6 = Procesos que envien y soliciten datos a traves de la tarjeta de red;
*/  

/* POLITICA DE PRIORIDADES SIN DESALOJO */

/* Librerias */
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include <stdlib.h>

/* Define */
#define N 4

/* Declaracion de semaforos */
sem_t mutex, sem_procesos[N], mutex2;

/* Estructuras */
typedef struct 
{
	int id_proceso;
	int numero_proceso;
	int prioridad;
} PCB;

/* Procedimientos */
void *proceso_prueba(PCB *p);
void creador_procesos(pthread_t procesos[], PCB pcb_procesos[]);
void ordenamiento_por_prioridad(PCB pcb_procesos[]);

int main()
{
	/* Variables */
	PCB pcb_procesos[N]; /* Cola de listos */
	pthread_t procesos[N]; /* Cola para ejecucion */
	int i, j = 0;

	/* Semilla para el srand */
	srand(time(NULL));

	/* Inicializacion de los semaforos */
	sem_init(&mutex, 0, 1);
	sem_init(&mutex2, 0, 1);

	/* Inicializacion del vector de semaforos */
	for (i = 0; i < N; i++)
			sem_init(&sem_procesos[i], 0, 1);

	/* Se crean los procesos */
	creador_procesos(procesos, pcb_procesos);
	
	/* Planificacion */
	printf("\e[92m\e[1m AVISO: Presione [ENTER] para continuar ->");
	getchar();

	/* Se ordenan por prioridad */
	ordenamiento_por_prioridad(pcb_procesos);

	/* Ejecucion de los hilos */
	while(j != N)
	{
		sem_post(&sem_procesos[j]);
		if(pthread_join(procesos[j], NULL))
			printf("\n \e[1m\e[91m ERROR: Error al ejecutar los hilos");
		j++;
	}

	printf("\e[0m");
	return 0;
}

void *proceso_prueba(PCB *p)
{
	sem_wait (&sem_procesos[p->numero_proceso]);
	printf("\e[1m\e[94m AVISO: Entre al proceso %d para su ejecucion\n", p->numero_proceso);
	printf("\e[93m\e[1m Prioridad del proceso %d es %d \n", p->numero_proceso, p->prioridad);

}

void creador_procesos(pthread_t procesos[], PCB pcb_procesos[])
{
	int i ;
	
	for(i = 0; i < N; i++)
	{
		pcb_procesos[i].numero_proceso = i;
		pcb_procesos[i].prioridad = rand()%6;

		sem_wait(&sem_procesos[i]);
		if(pthread_create(&procesos[i], NULL, (void *) &proceso_prueba, (void *) &pcb_procesos[i] ))
			printf("\e[1m\e[91m ERROR: Creacion del hilo %d invalido\n", i);
		else	
			printf("\e[1m\e[94m AVISO: Proceso %d creado exitosamente\n", i);
	}
}

void ordenamiento_por_prioridad(PCB pcb_procesos[N])
{
	int i, j;
	PCB aux;

	for(i = 0;i <= N; i++)
	{
		for(j = 0;j < N-1; j++)
		{
			if(pcb_procesos[j].prioridad > pcb_procesos[j+1].prioridad)
			{
				aux = pcb_procesos[j];
				pcb_procesos[j] = pcb_procesos[j+1];
				pcb_procesos[j+1] = aux;
			}
		}
	}
} 