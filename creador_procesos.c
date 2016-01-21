#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include <stdlib.h>

#define limits_pth 30
#define MOD %
#define AND &&
#define OR ||
#define N 10


typedef struct 
{
	int id_proceso, numero_proceso;
	int prioridad;
	//sem_t semaforo;
}PCB;

PCB pcb_procesos[N+1];
sem_t sem_procesos[N+1];
pthread_t procesos[N+1];

void *proceso_prueba(int *p);
void creador_procesos();

int main()
{
	int i, j;

	srand(time(NULL));
	for (i = 0; i < N; i++)
			sem_init(&sem_procesos[i], 0,1);

	creador_procesos(procesos, pcb_procesos );

	printf("press enter to continue\n");
	getchar();

	while(1)
	{
		printf("que hilo desea ejecutar?: ");
		scanf("%d", &j);
		printf("se va a ejecutar el hilo %d\n", j);
		sem_post(&sem_procesos[j]);
		if(pthread_join(procesos[j], NULL))
		{
			printf("\nERROR uniendo los hilos\n");
      	}
	}



	return 0;
}

void *proceso_prueba(int *p)
{
	sem_wait (&sem_procesos[pcb_procesos[*p].numero_proceso]);
	printf("entre al proceso de prueba %d\n", pcb_procesos[*p].numero_proceso);
	printf("this is a test, here was process %d \n", pcb_procesos[*p].numero_proceso );
	pthread_exit(NULL);
}

void creador_procesos()
{
	int i ;
	for (i= 0; i < N; i++)
	{
		pcb_procesos[i].numero_proceso= i;
		pcb_procesos[i].prioridad= rand()MOD 6;

		sem_wait(&sem_procesos[i]);
		if(pthread_create(&procesos[i], NULL, (void *) &proceso_prueba, (void *) &pcb_procesos[i].numero_proceso))
			printf("error al crear el hilo %d\n", i );
		else			
			printf("create process %d sucessfull\n", i);
	}
}
