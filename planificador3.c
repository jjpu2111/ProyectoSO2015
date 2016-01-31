/* 
NOTA 

Prioridades: 0 - 5 
id_modulo: 0 - 6

id_modulo 0 = Tarjeta de red
id_modulo 1 = Servidor de impresion
id_modulo 2 = Demonio de compactacion
id_modulo 3 = Antivirus y Firewall
id_modulo 4 = Procesos que soliciten CPU
id_modulo 5 = Procesos que soliciten espacio en disco
id_modulo 6 = Procesos que envien y soliciten datos a traves de la tarjeta de red;

Los modulos del 0 - 3 tienen prioridad 0
Los modulos del 4 - 6 tienen prioridad 1 - 5
*/  

/* POLITICA DE PRIORIDADES SIN DESALOJO */

/* Librerias */
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include <stdlib.h>

/* Define */
#define N 10

/* Declaracion de semaforos */
sem_t mutex, sem_procesos[N], mutex2;

/* Estructuras */
typedef struct 
{
	int id_proceso;
	int numero_proceso;
	int prioridad;
	int id_modulo;
} PCB;

/* Procedimientos */
void *proceso_en_ejecucion(PCB *p);
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

void *proceso_en_ejecucion(PCB *p)
{
	sem_wait (&sem_procesos[p->numero_proceso]);
	printf("\e[1m\e[94m AVISO: Entra al proceso %d para su ejecucion\n", p->numero_proceso);
	printf("\e[93m\e[1m Prioridad del proceso %d es %d \n", p->numero_proceso, p->prioridad);
	printf("\e[93m\e[1m ID del proceso %d es %d \n", p->numero_proceso, p->id_modulo);
	system("sleep 3.0");
}

void creador_procesos(pthread_t procesos[], PCB pcb_procesos[])
{
	int i ;
	
	for(i = 0; i < N; i++)
	{
		pcb_procesos[i].numero_proceso = i;
		pcb_procesos[i].prioridad = rand() % 6; /* Prioridad va de 0 - 5 */
		
		if(pcb_procesos[i].prioridad == 0)
			pcb_procesos[i].id_modulo = rand() % 3; /* id_modulo va de 0 - 3 */
		else
			pcb_procesos[i].id_modulo = 4 + rand() % 3; /* id_modulo va de 4 - 6 */					

		sem_wait(&sem_procesos[i]);
		if(pthread_create(&procesos[i], NULL, (void *) &proceso_en_ejecucion, (void *) &pcb_procesos[i] ))
			printf("\e[1m\e[91m ERROR: Creacion del hilo %d invalido\n", i);
		else	
		{	
			printf("\e[1m\e[94m AVISO: Proceso %d creado exitosamente\n", i);
			system("sleep 1.0");
		}
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