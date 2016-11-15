#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_CLI 500
#define MAX_PET 1000

pthread_t threads[MAX_CLI];

typedef struct datoshilo{
	int num_pet;
	int tRflx[MAX_PET];
}datoshilo;

double NumeroAleatorio (double inferior, double superior)
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


void * Usuario (void * arg)
{
	int num;
	num = *((int *)arg);
	
	printf("Soy el hilo %d\n", num);

	//Rellenar estructura servidor
	struct sockaddr_in servidor;
	servidor.sin_family=AF_INET;
	servidor.sin_port=htons(7000+num);
	inet_aton("192.168.0.20", &servidor.sin_addr.s_addr);
	
	//crear socket
	int socket(int PF_INET, int SOCK_STREAM, int TCP);
	//connect
	int connect (int s, const struct sockaddr * dir, socklen_t len);
	int retorno;
	retorno=connect(s,(struct sockaddr *)&servidor,sizeof(servidor));
	//write
	ssize_t write (int s, const void * buf, size_t len);
	//read
	ssize_t read (int s, void * buf, size_t len);
	//close
	int close (int s);
}


int main (int arcg, char * argv[])
{
	int nhilos;
	int i;
	int retorno;
	int parametro[MAX_CLI];

	printf("Introduce el numero de hilos: \n");
	scanf("%d", &nhilos);

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

}



