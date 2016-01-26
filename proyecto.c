#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>


#define MOD %
#define AND &&
#define OR ||
#define total_memoria_principal 500	//la cantidad debe ir expresada en kb

//definiciones del creador de procesos
#define limits_pth 15	//total procesos aleatorios a crear	
#define N 30	//total de procesos permitidos
#define limits_procesos_basicos 10 	/*indica la cantidad de procesos basicos o del sistema, en total son 10
									(tarjeta de red, firewall, planificador, controlador de disco, demonio compactacion, etc)*/

//definiciones del planificador
#define planificador_pos 1	//indica la posicion en el vector en donde se encuentra el proceso(hilo) del planificador de procesos

//definiciones del manejador de interrupciones
#define manejador_pos 0		//indica la posicion en el vector en donde se encuentra el proceso(hilo) del manejador de interrupciones


///variables globales
//estructuras globales principales
typedef struct 
{
	int id_proceso, numero_proceso;
	int prioridad, tamano;
	char estado_proceso[10], nombre_proceso[20];
}PCB;
PCB pcb_procesos[N+ limits_procesos_basicos ];	//vector de PCB
sem_t sem_procesos[N+ limits_procesos_basicos ];//Vector de semaforos de los procesos
pthread_t procesos[N+ limits_procesos_basicos ];//vecttor de procesos (hilos)

//globales del planificador de procesos
int planificador_parametro;		//mediante esta variable se le puebe indicar al planificador que va hacer en su llamada
int planificador_idproceso;		//sirve para indicar sobre cual proceso se va a trabajar
sem_t mutex_planificador_parametro;//semaforo de control para garantizar exclusion mutua sobre la variable compartida (planificador_parametro)
sem_t mutex_planificador_idproceso;//semaforo de control para garantizar exclusion mutua sobre la variable compartida (planificador_idproceso)

///globales del manejador de interrupciones
int IRQ;	//indica el tipo de instruccion que recibira el manejador de interrupciones
int manejador_quien;	//hace referencia a la posicion en el vector de pcb del proceso que esta haciendo una interrupcion
sem_t mutex_IRQ;	//semaforo de control para garantizar exclusion mutua sobre la variable compartida (IRQ)
sem_t mutex_manejador_quien;//semaforo de control para garantizar exclusion mutua sobre la variable compartida (manejador_quien)


void *proceso_prueba(int *p);
void creador_procesos();
void procesos_basicos();
void planificador_procesos();
void manejador_interrupciones();
void generar_interrupcion(int irq, int id);

int main()
{
	int i, j;

	srand(time(NULL));
	for (i = 0; i < N; i++)
	{
			sem_init(&sem_procesos[i], 0,1);//se inicializa cada semaforo de cada proceso
			sem_wait(&sem_procesos[i]);		//se bloquea cada semaforo de cada proceso
	}
	sem_init(&mutex_IRQ ,0,1);
	sem_init(&mutex_manejador_quien ,0,1);
	sem_init(&mutex_planificador_idproceso ,0,1);
	sem_init(&mutex_planificador_parametro ,0,1);

	if((limits_procesos_basicos+ limits_pth) > N)
		printf("la cantidad de procesos a crear excedera el limite del vector de procesos, por favor aumente el limite\ndel vector de procesos o disminuya el limits_pth\n");

	else
	{
		procesos_basicos();
		creador_procesos();

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
				printf("\nERROR en ejecucion del hilo %d, %s\n", j, pcb_procesos[j].nombre_proceso);
	      	}
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
	for (i= limits_procesos_basicos; i < limits_procesos_basicos+limits_pth; i++)
	{
		pcb_procesos[i].numero_proceso= i;
		pcb_procesos[i].prioridad= (rand()MOD 5)+1;
		pcb_procesos[i].tamano= ((rand()MOD 3== 0)? 8:((rand()MOD 3== 1)? 12: 16) );
		strcpy(pcb_procesos[i].nombre_proceso, "proceso_basico");
		strcpy(pcb_procesos[i].estado_proceso, "nuevo");

		//sem_wait(&sem_procesos[i]); //se bloquea cada semaforo de cada proceso
		if(pthread_create(&procesos[i], NULL, (void *) &proceso_prueba, (void *) &pcb_procesos[i].numero_proceso))
			printf("error al crear el hilo %d\n", i );
		else
		{
			printf("create process %d sucessfull\n", i);
			generar_interrupcion(1,pcb_procesos[i].numero_proceso);

		}
	}
}

void procesos_basicos()
{
	printf("entre al creador de procesos basicos\n");

	//manejador de IRQ
	pcb_procesos[manejador_pos].numero_proceso= manejador_pos;
	pcb_procesos[manejador_pos].prioridad= 0;
	pcb_procesos[manejador_pos].tamano= ((rand()MOD 3== 0)? 8:((rand()MOD 3== 1)? 12: 16) );
	strcpy(pcb_procesos[manejador_pos].nombre_proceso, "manejador_interrupciones");
	strcpy(pcb_procesos[manejador_pos].estado_proceso, "nuevo");
	if(pthread_create(&procesos[manejador_pos], NULL, (void *) &manejador_interrupciones, NULL))
			printf("error al crear el hilo del manejador de interrupciones\n" );
	else
	{
		printf("create process manejador de interrupciones sucessfull\n");
		//generar_interrupcion(1,pcb_procesos[manejador_pos].numero_proceso);
	}


	//planificador de procesos
	pcb_procesos[planificador_pos].numero_proceso= planificador_pos;
	pcb_procesos[planificador_pos].prioridad= 0;
	pcb_procesos[planificador_pos].tamano= ((rand()MOD 3== 0)? 8:((rand()MOD 3== 1)? 12: 16) );
	strcpy(pcb_procesos[planificador_pos].nombre_proceso, "planificador_procesos");
	strcpy(pcb_procesos[planificador_pos].estado_proceso, "nuevo");
	if(pthread_create(&procesos[planificador_pos], NULL, (void *) &planificador_procesos, NULL))
			printf("error al crear el hilo del planificador de procesos\n" );
	else
	{
		printf("create process planificador de procesos sucessfull\n");
		//generar_interrupcion(1,pcb_procesos[planificador_pos].numero_proceso);
	}

}

void generar_interrupcion(int tipo_irq, int id)
{
	printf("El proceso %d (%s)genera una interrupcion del tipo %d\n", id, pcb_procesos[id].nombre_proceso, tipo_irq);
	sem_wait(&mutex_IRQ);//aparta el irq
		IRQ= tipo_irq;//le asigna un valor a la interrupcion
	sem_post(&mutex_IRQ);//se libera el semaforo del IRQ
	sem_wait(&mutex_manejador_quien);//aparta el manejador_quien
		manejador_quien= id;//le asigna un id al manejador(indica a cual proceso hay que tratar)
	sem_post(&mutex_manejador_quien);////se libera el semaforo del manejador_quien
	sem_post(&sem_procesos[manejador_pos]);//se libera el semaforo del manejador de interrupciones	
	if(pthread_join(procesos[manejador_pos], NULL))
	{
		printf("\nERROR en ejecucion del hilo %d, %s\n", manejador_pos, pcb_procesos[manejador_pos].nombre_proceso);
	}
}

void planificador_procesos( )
{
	PCB *listos[7][N], *bloqueados[7][N];
	int ocupados[7];//indica la cantidad de elementos guardados en cada cola
	int espacio_total=total_memoria_principal, i ,j;

	for (i = 0; i < 7; i++)
	{
		ocupados[i]=0;
		for (j = 0; j < N; j++)
		{
			listos[i][j]=NULL;
			bloqueados[i][j]=NULL;
		}
	}

	while(1)
	{
		sem_wait(&sem_procesos[planificador_pos]);
		printf("entra al planificador de procesos\n");
		if(planificador_parametro<0 || planificador_parametro>3)
			printf("valor incorrecto para el planificador_parametro\n");

		else
		{
			switch(planificador_parametro)
			{
				case 0://entra un proceso a memoria principal
				{
					printf("el planificador entro al case 0\n");
					printf("se le asignara memoria al proceso %d \n", planificador_idproceso);

					listos[pcb_procesos[planificador_idproceso].prioridad][ocupados[pcb_procesos[planificador_idproceso].prioridad]]= &pcb_procesos[planificador_idproceso];
					ocupados[pcb_procesos[planificador_idproceso].prioridad]++;
					strcpy(pcb_procesos[planificador_idproceso].estado_proceso, "listo");
					espacio_total= espacio_total- pcb_procesos[planificador_idproceso].tamano;

					printf("ya no se que hacer xD\n");
					getchar();

				}
				case 1:
				{
					printf("el planificador entro al case 1\n");

				}break;

				case 2:
				{
					printf("el planificador entro al case 2\n");

				}break;

				case 3:
				{
					printf("el planificador entro al case 3\n");

				}break;

			}
		}
		//signal a otro semaforo
	}
}

void manejador_interrupciones()
{
	while(1)
	{
		sem_wait(&sem_procesos[manejador_pos]);
		printf("entra al manejador de interrupciones\n");
		if(IRQ<0 || IRQ>7)
			printf("valor incorrecto para la IRQ\n");

		else
		{
			switch(IRQ)
			{
				case 0://solictud de tiempo de cpu
				{
					printf("el manejador entro al case 0\n");

				}
				case 1://solictud de espacio en memoria principal
				{
					printf("el manejador entro al case 1\n");
					printf("solicitud de espacio en memoria principal por el proceso %d\n", manejador_quien);

					sem_wait(&mutex_planificador_parametro);//aparta el planificador_parametro
						planificador_parametro= 0;//le indica que tiene que hacer sobre el proceso id
						sem_wait(&mutex_planificador_idproceso);//aparta el planificador_idproceso
							planificador_idproceso= manejador_quien;//le asigna un id al planificador_idproceso(indica a cual proceso hay que tratar)
							sem_post(&sem_procesos[planificador_pos]);//se libera el semaforo del planificador de proceso
							if(pthread_join(procesos[planificador_pos], NULL))
							{
								printf("\nERROR en ejecucion del hilo %d, %s\n", planificador_pos, pcb_procesos[planificador_pos].nombre_proceso);
							}
						sem_post(&mutex_planificador_idproceso);////se libera el semaforo del planificador_idproceso
					sem_post(&mutex_planificador_parametro);//se libera el semaforo del planificador_parametro

				}break;

				case 2:
				{
					printf("el manejador entro al case 2\n");
					
				}break;

				case 3:
				{
					printf("el manejador entro al case 3\n");

				}break;

				case 4:
				{
					printf("el manejador entro al case 3\n");

				}break;

				case 5:
				{
					printf("el manejador entro al case 3\n");

				}break;

				case 6:
				{
					printf("el manejador entro al case 3\n");

				}break;

				case 7:
				{
					printf("el manejador entro al case 3\n");

				}break;

			}
		}
		//signal a otro semaforo
	}
}
