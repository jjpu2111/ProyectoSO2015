#include <stdio.h>		///Falta mejorar la Sincronizacion y Señalizacion...///
/**Tarjeta de Red**/
#include <time.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

#define MAX 20
#define N 10		// Numero de Hilos a Crear...//
#define MOD %
#define AND &&
#define OR ||
#define FALSE 0
#define TRUE 1

typedef int bool;

typedef struct
{
	int Buffer_In, Buffer_Out;
	sem_t Bloqueado;// semaforo  usado por el firewall
	sem_t total; //semaforo para el manejador de interrupciones
	sem_t In, Out;
}Tarjeta_Red;	///Los semaforos ueden colocarse de forma global...///

int Prioridad_Proceso(pthread_t Hilo);
void Cierre_Envio_Recepcion_Tarjeta_Red(Tarjeta_Red *TR);
void Habilitacion_Envio_Recepcion_Tarjeta_Red(Tarjeta_Red *TR);
void Insertar_Peticion_Buffer(int *Buffer);
void Envio_Peticion(int *Buffer);
void *Eliminar_Peticion_Buffer(int *Buffer);
void Liberar_Buffer(int *Buffer);
void Solicitar_Tarjeta_Red(Tarjeta_Red *TR);

int main()
{
	int i;
	pthread_t Paquetes[N];
	pthread_attr_t Hilo;
	struct sched_param fifo_param; 
	Tarjeta_Red TR;
	
	TR.Buffer_In = 0;
	TR.Buffer_Out = 0;
	sem_init(&TR.Bloqueado, 0, 1);
	sem_init(&TR.In, 0, 1);
	sem_init(&TR.Out, 0, 1);
	sem_wait(&TR.Bloqueado);
	sem_wait(&TR.In);
	sem_wait(&TR.Out);
	Solicitar_Tarjeta_Red(&TR);
	
	return 0;
}

//	Cuando ocurre una IRQ6...	esto deberia se manejado por el modulo de interrupciones creo //
void Cierre_Envio_Recepcion_Tarjeta_Red(Tarjeta_Red *TR)
{
	sem_wait(&TR->Bloqueado);// deberia estar ser el nuevo semaforo
}

void Habilitacion_Envio_Recepcion_Tarjeta_Red(Tarjeta_Red *TR)
{
	sem_post(&TR->Bloqueado);
}

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
	}else{
		printf("\nBuffer LLeno...\n");
		system("sleep 2.0");
	}
}

void *Eliminar_Peticion_Buffer(int *Buffer) // Parametro *Buffer
{
	if(*Buffer > 0)
	{
		printf("\nLiberando Buffer...\n");
		system("sleep 1.0");
		Liberar_Buffer(Buffer);
	}
}

void Liberar_Buffer(int *Buffer) // Parametro *Buffer
{
	*Buffer = (*Buffer - 1);
	printf("\nPaquete enviado...\n");
	printf("\nBuffer Liberado...\n");
	system("sleep 1.0");
}

void Solicitar_Tarjeta_Red(Tarjeta_Red *TR)
{
	int Valor;
	if((sem_getvalue(&TR->Bloqueado, &Valor) == 0) AND (Valor == 1))	//Si el firewall no esta bloqueado puedo recibir y enviar de una vez...//
	{
		if((sem_getvalue(&TR->In, &Valor) == 0) AND (Valor == 0))	///Si valor es igual 0, entonces esa es la señal de que es un paquete de entrada.///
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
		if((sem_getvalue(&TR->Out, &Valor) == 0) AND (Valor == 0))	///Si valor es igual 0, entonces esa es la señal de que es un paquete de salida.///
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
	}else{
		printf("\nFirewall Bloqueo el Envio y Recepcion de Datos...\n");	///En este caso el Firewall bloqueo la tarjeta de Red, pero el igual puede recibir los paquetes pero no los envia...///
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
