
#define limit_tiempo_en_cola 7//un limite estimado de las veces que un proceso puede apoderarse del cpu
#define limit_envio_tarjeta 2//asumiendo que un proceso puede enviar como maximo dos paquetes a la tarjeta de red
void antivirus()
{
	int i;

	for(i=0; i< N; i++)
	{
		if(((pcb_procesos[i].estatus== 0)||(pcb_procesos[i].estatus== 1))&& (pcb_procesos[i].tiempo_cola> limit_tiempo_en_cola) )
			quitprocess(i);
	}
}
