#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_CLI 500
#define MAX_PET 1000
#define LEN 250

double base;
double inicio;
double final;
int media;

pthread_t threads[MAX_CLI];

typedef struct datoshilo{
	int num_pet;
	double tRflx[MAX_PET];
	double tResp[MAX_PET];
}datoshilo;

datoshilo datos[MAX_CLI];

double NumAleatorio (double inferior, double superior)
{
	double num;
	num=(double)rand();
	num=num*(superior-inferior)/RAND_MAX;
	num+=inferior;
	return num;
}


double DistExponencial(double med)
{
	return (-med)*log(NumAleatorio(0,1));
}


double time2double (struct timespec t)
{
	double d_tiempo;

	d_tiempo=(double)t.tv_nsec/1000000000.0;
	d_tiempo+=(double)t.tv_sec;

	return d_tiempo;
}


void * Usuario (void * arg)
{
	int num;
	int s;
	int i;
	char mensaje[LEN];
	char respuesta[LEN];
	double dormir;
	num = *((int *)arg);

	struct timespec t1, t2;
	double d1, d2, diferencia;
	
	printf("Soy el hilo %d\n", num);

	//Rellenar estructura servidor
	struct sockaddr_in servidor;
	servidor.sin_family=AF_INET;
	servidor.sin_port=htons(7000+num);
	inet_aton("192.168.0.20", &servidor.sin_addr.s_addr);
	
	for(i=0;i<npet;i++)
	{
		//crear socket
		clock_gettime(CLOCK_MONOTONIC_RAW, &t1);

		s=socket( PF_INET,  SOCK_STREAM, 0);
		//connect
		connect(s,(struct sockaddr *)&servidor,sizeof(servidor));
		//write
		write ( s, mensaje, LEN );
		//read
		read ( s, respuesta, LEN );
		//close
		close (s);
		dormir=DistExponencial(media);
		usleep(dormir*1000000);

		clock_gettime(CLOCK_MONOTONIC_RAW, &t2);

		//incrementar numpet
		datos[num].num_pet++;
		//guardar tiempo rflx
		datos[num].tRflx[i]=dormir;

		d1=time2double(t1);
		d2=time2double(t2);

		diferencia=d2-d1;

		//guardar tiempo de respuesta
		datos[num].tResp[i]=diferencia;
		
	}
}


int main (int arcg, char * argv[])
{
	int trans_med;
	int int_med;
	int nhilos;
	int i;
	int j;
	int retorno;
	int parametro[MAX_CLI];
	
	printf("Introduce el numero de hilos: \n");
	scanf("%d", &nhilos);
	printf("Cuanto dura el transitorio (s): \n");
	scanf("%d", &trans_med);
	printf("Media de tiempo: \n");
	scanf("%d", &int_med);
//////////////////////
	struct timespec time_base;

	clock_gettime(CLOCK_MONOTONIC_RAW, &time_base);
	base=
//////////////////////
	for (i=0;i<nhilos;i++)
	{
		parametro[i]=i;
		retorno = pthread_create(&(threads[i]), NULL, &Usuario, &parametro[i]);
		if (retorno!=0)
		{
			printf("Error al crear el hilo\n");
			return -1;
		}
	}
	for (i=0;i<nhilos;i++)
	{
		retorno=pthread_join(threads[i], NULL);
		if (retorno!=0)
		{
			printf("Error esperando a la finalizacion de los hilos\n");
			return -1;
		}
	}

	FILE * f;
	f = fopen("tRflex.txt","w+");
	for(i=0;i<nhilos;i++)
	{	for(j=0;j<npet;j++)
		{

			fprintf(f,"%f\n", datos[i].tRflx[j]);	
		}
	}
	fclose (f);

	FILE * g;
	g = fopen("tResp.txt","w+");
	for(i=0;i<nhilos;i++)
	{	for(j=0;j<npet;j++)
		{

			fprintf(f,"%f\n", datos[i].tResp[j]);	
		}
	}
	fclose (g);

}


