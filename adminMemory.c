#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

#define SIZE_MEMORY 39
#define TRUE 1
#define FALSE 0
#define ERROR -1
#define N 4

typedef struct //tabla de segmento
{
    int base;
    int limit;
    int n;
}tds;

typedef struct //psb
{
    int pid; //identificador
    int estatus; //1 listo, 2 en espera, 0 bloqueado
    tds *tdsProcess

}pcb;

tds* allocate_memory(int request); //retorna puntero a tabla de segmento, si no se asigno, retorna nulo
int mmu(int pid, int nSegment, int displacement); //retorna la direccion de memoria solicitada, -1 si error, el desplazamientio es index 0
void compact(); //compacta la memoria
void quitprocess(int id); //quita un proceso de la memoria

//variables globales para el administrador
int memory[SIZE_MEMORY];
int bitmap[SIZE_MEMORY];
pcb pcbProcess[N];
sem_t mutexMemoryAccess;

int main()
{
    int i, j;
    int physical_address;//direccion fisica
    tds *tdsP;//tabla de segmentos

    //iniciar pcb, esto se hace al inicio de todo el sistema
    for(i=0; i<N; i++)
        pcbProcess[i].tdsProcess=NULL;

    //iniciar memoria y mapa de bits | void startAdmin()
    for(i=0; i<SIZE_MEMORY; i++)
    {
         if(i<24)
            bitmap[i]=memory[i]=1; // los primeros 24 kb estan usados por el SO
         else
            bitmap[i]=memory[i]=0;
    }

    //iniciar semaforos
    sem_init(&mutexMemoryAccess, 0, 1);

    for(j=0; j<N; j++)//genera N psb con y les asgina memoria, solo con fines demostrativos
    {
        printf("\nentra el proceso: %d\n", j);
        tdsP=allocate_memory(5); //prueba, pide memoria 5kb
        //esta comprobacion la debe hacer el que crea los procesos
        if(tdsP!=NULL) //si se creo la tabla de segmentos es porque le asgino memoria
        {
            //esto lo hace que crea los hilos una vez que se le asigno memoria
            pcbProcess[j].pid=j;//puede ser el pid que quiera
            pcbProcess[j].estatus=1;//1, 2 o 3
            pcbProcess[j].tdsProcess=tdsP;// se le asigna la tabla de segmento creada por allocate_memory(int)

            //printf("base: %d limite: %d\n", pcbProcess[j].tdsProcess->base, pcbProcess[j].tdsProcess->limit);
            //printf("vas bien!!!\n");

            //prueba de requerimiento de direccion
            printf("direccion logica del proceso %d s:%d d:%d\n", j, 0, 4);
            physical_address=mmu(j,0 ,4);//mmu convierte direccion logia a fisica, retorna -1 si hay error
            if(physical_address != ERROR)
            {
                printf("\ndireccion fisica: %d\n", physical_address);
                printf("vas bien!!!\n");
            }

            //probar estado de memoria
            printf("\nmemoria:\n");
            for(i=0; i<SIZE_MEMORY; i++)
                printf("%d ",memory[i]);
            printf("\nmapa de bits:\n");
            for(i=0; i<SIZE_MEMORY; i++)
                printf("%d ",bitmap[i]);
        }
        else
            printf("no hay memoria para el proceso %d\n", j);
    }

   quitprocess(0);//retira el proceso 0
   /*ver como queda la memoria y el mapa de bits*/
    printf("quitar:\n");
    for(i=0;i<SIZE_MEMORY;i++)
        printf("%d ",memory[i]);
    printf("\n");
    for(i=0;i<SIZE_MEMORY;i++)
        printf("%d ",bitmap[i]);
    printf("\n");

    //prueba de compactacion
    printf("compactar\n");
    compact();//compacta la  memoria
    //con fines demostrativos
    printf("\nmemoria:\n");
    for(i=0; i<SIZE_MEMORY; i++)
        printf("%d ",memory[i]);
    printf("\nmapa de bits:\n");
    for(i=0; i<SIZE_MEMORY; i++)
        printf("%d ",bitmap[i]);
    printf("\n");
    return 0;
}

//SUB-RUTINAS
tds* allocate_memory(int request)
{
    int cont, i, j, start;
    tds *tdsProcess;

    i=24;//asigna desde el bit 24 porque los primeros espacios son ocupados por el S.O
    cont=0;//para contar los espacios libres continuos

    sem_wait(&mutexMemoryAccess);//bloque el acceso a memoria

    do//busca los espacios libres necesarios para el segmento
    {
        if(bitmap[i]==0)
            cont++;
        i++;
    }
    while((cont < request) && (i<SIZE_MEMORY));
    i--;

    if(cont==request)//si encontro los espacios necesarios
    {
        start=(i-request+1);

        //llena la tabla de segmento del proceso
        tdsProcess=(tds*)malloc(sizeof(tds));
        tdsProcess->base=start;
        tdsProcess->limit=request;
        tdsProcess->n=0;
        //fin llenado

        for(j=i; j>=start; j--)
            memory[j]=bitmap[j]=1;//llena el mapa de bits y la memoria
    }
    else
        tdsProcess=NULL;//retorna null si no hay espacio
    sem_post(&mutexMemoryAccess);//libera la memoria
    return tdsProcess;
}

int mmu(int pid, int nSegment, int displacement)
{
	int physical_address; //direccion fisica

	sem_wait(&mutexMemoryAccess);//se bloque si estan compactando

	if(pcbProcess[pid].tdsProcess->n == nSegment) //si el segmento indicado es el correcto
	{
		if(pcbProcess[pid].tdsProcess->limit > displacement && displacement >=0) //comprueba los limites del segmento
			physical_address=pcbProcess[pid].tdsProcess->base + displacement; //base + desplazamiento
		else
             		physical_address=ERROR;//interrupcion al monitor del sistema, error de direccionamiento
	}
	else
        physical_address=ERROR;//interrupcion al monitor del sistema, error de direccionamiento

	sem_post(&mutexMemoryAccess);//desbloque el semaforo para el compactador

	return physical_address;
}

void compact()
{
	int i, j, k, l;
	int found, exitCompact;

    found=FALSE;
    exitCompact=FALSE;

    sem_wait(&mutexMemoryAccess);//bloque a todos los accesos a memoria para poder compactar

    i=0;
    do //hacer mientras hay espacios vacios
    {
        //busacar en memoria el primer espacio vacio
        while(!found && i<SIZE_MEMORY)
        {
            if(bitmap[i]!=0)
                i++;
            else
                found=TRUE;
        }

        if(i<SIZE_MEMORY)//se encontro espacios vacios
        {
            //buscar donde termina el agujero
            j=i+1;
            found=FALSE;
            while(!found && j<SIZE_MEMORY)
            {
                if(bitmap[j]!=1)
                    j++;
                else
                    found=TRUE;
            }

            if(j<=SIZE_MEMORY)//encontro el primer espacio ocupado
            {
                //buscar a que proceso le pertenece el espacio de memoria ocupado donde termina el agujero
                k=0;
                found=FALSE;
                while(!found && k<N)
                {
                    if(pcbProcess[k].tdsProcess!=NULL) //si hay apuntador a una tds entonces esta en memoria el proceso
                    {
                        if(pcbProcess[k].tdsProcess->base==j)
                            found=TRUE;
                    }
                    k++;
                }
                k--;
                if(k<N && found)//encontro el proceso que hay que mover
                {
                    //mover proceso hacia el primer espacio de memoria vacio
                        //cambio la tds del proceso
                    pcbProcess[k].tdsProcess->base=i;
                    //pcbProcess[k].tdsProcess->limit se mantiene igual
                        //muevo el mapa de bits y la memoria
                    for(l=i; l<j; l++)
                    {
                        bitmap[l]=memory[l]=1;
                        bitmap[j-(i-l)]=memory[j-(i-l)]=0;
                    }
                }
                else
                   exitCompact=TRUE;
            }
            else
                exitCompact=TRUE;
        }
        else
            exitCompact=TRUE;
        i=j;
    }
    while(!exitCompact);

    sem_post(&mutexMemoryAccess);//levanta el semaforo
}

void quitprocess(int id)
{
	int i,band, fin, j;

	sem_wait(&mutexMemoryAccess);

	band= i = FALSE;
	if(id>=0 && id<N)
    {
        //busco en el vector de pcb para encontrar la base y limite del proceso
        while(i<N && !band)
        {
            if(pcbProcess[i].pid == id)
                band = TRUE;
            else
                i = i +1;
        }
        if(i<N && band)
        {
            //verificar estatus para comprovar si termino
            if(pcbProcess[i].estatus ==1)
            {
                //liberar la memoria de la tabla de segmentos y el mapa de bits
                fin= pcbProcess[i].tdsProcess->limit+pcbProcess[i].tdsProcess->base-1;
                for(j = pcbProcess[i].tdsProcess->base  ;j<=fin ;j++)
                    memory[j]=bitmap[j]=0;
                //liberar la tabla de segmentos
                pcbProcess[i].tdsProcess->base=0;
                pcbProcess[i].tdsProcess->limit=0;
                free(pcbProcess[i].tdsProcess);
                pcbProcess[i].tdsProcess=NULL;
            }
        }
        else
            printf("\nno se encontro el proceso %d\n", id);//solo con fines demostrativos
    }

    sem_post(&mutexMemoryAccess);
}
