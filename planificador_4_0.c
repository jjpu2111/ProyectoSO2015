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
#define MAX 20
#define MOD %
#define AND &&
#define OR ||
#define FALSE 0
#define TRUE 1

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

typedef int bool;

typedef struct
{
	int Buffer_In, Buffer_Out;
	sem_t Bloqueado;
	sem_t total;
	sem_t In, Out;
} Tarjeta_Red;

/* Procedimientos */
void *proceso_en_ejecucion(PCB *p);
void creador_procesos(pthread_t procesos[], PCB pcb_procesos[]);
void ordenamiento_por_prioridad(PCB pcb_procesos[]);
/* -------------------------------------------------------------- */
void Insertar_Peticion_Buffer(int *Buffer);
void Envio_Peticion(int *Buffer);
void *Eliminar_Peticion_Buffer(int *Buffer);
void Liberar_Buffer(int *Buffer);
void Solicitar_Tarjeta_Red(Tarjeta_Red *TR);
/* --------------------------------------------------------------- */

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

/* -------------------------------------------------------------------------------------------------- */

void *proceso_en_ejecucion(PCB *p)
{
	Tarjeta_Red TR;
	
	TR.Buffer_In = 0;
	TR.Buffer_Out = 0;
	sem_init(&TR.Bloqueado, 0, 1);
	sem_init(&TR.In, 0, 1);
	sem_init(&TR.Out, 0, 1);
	sem_wait(&TR.Bloqueado);
	sem_wait(&TR.In);
	sem_wait(&TR.Out);

	sem_wait (&sem_procesos[p->numero_proceso]);
	if(p->id_modulo == 0)
	{
		printf("\e[1m\e[94m AVISO: Entra al proceso %d para su ejecucion\n", p->numero_proceso);
		printf("\e[93m\e[1m Prioridad del proceso %d es %d \n", p->numero_proceso, p->prioridad);
		printf("\e[93m\e[1m ID del proceso %d es %d \n", p->numero_proceso, p->id_modulo);
		Solicitar_Tarjeta_Red(&TR);
	}
	else
	{
		printf("\e[1m\e[94m AVISO: Entra al proceso %d para su ejecucion\n", p->numero_proceso);
		printf("\e[93m\e[1m Prioridad del proceso %d es %d \n", p->numero_proceso, p->prioridad);
		printf("\e[93m\e[1m ID del proceso %d es %d \n", p->numero_proceso, p->id_modulo);
		system("sleep 3.0");
	}

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

/* -------------------------------------------------------------------------------------------------- */

void Insertar_Peticion_Buffer(int *Buffer)
{
	*Buffer = (*Buffer + 1);
	printf("\nNuevo Paquete a Enviar...\n");
	system("sleep 1.0");
}

void Envio_Peticion(int *Buffer)
{
	if(*Buffer < MAX)
	{
		printf("\nPaquete Guardado en el Buffer...\n");
		system("sleep 1.0");
		Insertar_Peticion_Buffer(Buffer);
	}
	else
	{
		printf("\nBuffer LLeno...\n");
		system("sleep 2.0");
	}
}

void *Eliminar_Peticion_Buffer(int *Buffer) /* Parametro *Buffer */
{
	if(*Buffer > 0)
	{
		printf("\nLiberando Buffer...\n");
		system("sleep 1.0");
		Liberar_Buffer(Buffer);
	}
}

void Liberar_Buffer(int *Buffer) /* Parametro *Buffer */
{
	*Buffer = (*Buffer - 1);
	printf("\nPaquete enviado...\n");
	printf("\nBuffer Liberado...\n");
	system("sleep 1.0");
}

void Solicitar_Tarjeta_Red(Tarjeta_Red *TR)
{
	int Valor;

	/* Si el firewall no esta bloqueado puedo recibir y enviar de una vez... */
	if((sem_getvalue(&TR->Bloqueado, &Valor) == 0) AND (Valor == 1))	
	{
		/* Si valor es igual 0, entonces esa es la señal de que es un paquete de entrada. */
		if((sem_getvalue(&TR->In, &Valor) == 0) AND (Valor == 0))	
		{
			printf("\n\t*Paquete de Entrada*\n");
			system("sleep 1.0");
			Envio_Peticion(&TR->Buffer_In);
			printf("\nCapacidad del Buffer Entrada ->: %d\n", TR->Buffer_In);
			system("sleep 1.0");
			Eliminar_Peticion_Buffer(&TR->Buffer_In);
			printf("\nCapacidad del Buffer Entrada ->: %d\n", TR->Buffer_In);
			system("sleep 1.0");
			sem_post(&TR->In);
		}

		/* Si valor es igual 0, entonces esa es la señal de que es un paquete de salida. */
		if((sem_getvalue(&TR->Out, &Valor) == 0) AND (Valor == 0))	
		{
			printf("\n\t*Paquete de Salida*\n");
			system("sleep 1.0");
			Envio_Peticion(&TR->Buffer_Out);
			printf("\nCapacidad del Buffer Salida ->: %d\n", TR->Buffer_Out);
			system("sleep 1.0");
			Eliminar_Peticion_Buffer(&TR->Buffer_Out);
			printf("\nCapacidad del Buffer Salida ->: %d\n", TR->Buffer_Out);
			system("sleep 1.0");
			sem_post(&TR->Out);
		}
	}
	else
	{
		/* En este caso el Firewall bloqueo la tarjeta de Red, pero el igual puede recibir los 
		paquetes pero no los envia...*/
		printf("\nFirewall Bloqueo el Envio y Recepcion de Datos...\n");	
		system("sleep 2.0");
		if((sem_getvalue(&TR->In, &Valor) == 0) AND (Valor == 0))
		{
			printf("\n\t*Paquete de Entrada*\n");
			system("sleep 1.0");
			Envio_Peticion(&TR->Buffer_In);
			printf("\nCapacidad del Buffer Entrada ->: %d\n", TR->Buffer_In);
			system("sleep 1.0");
			printf("\nNo se puede enviar porque el Firewall bloqueo la _Tarjeta de Red...\n");
			system("sleep 1.0");
			sem_post(&TR->In);
		}
		if((sem_getvalue(&TR->Out, &Valor) == 0) AND (Valor == 0))
		{
			printf("\n\t*Paquete de Salida*\n");
			system("sleep 1.0");
			Envio_Peticion(&TR->Buffer_Out);
			printf("\nCapacidad del Buffer Salida ->: %d\n", TR->Buffer_Out);
			system("sleep 1.0");
			printf("\nNo se puede enviar porque el Firewall bloqueo la _Tarjeta de Red...\n");
			system("sleep 1.0");
			sem_post(&TR->Out);
		}
	}
}