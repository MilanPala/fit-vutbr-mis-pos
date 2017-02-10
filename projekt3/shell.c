/**
 * Projekt 3 do predmetu POS 2010/2011
 * Author: xpalam00
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

#define DBG 0

pthread_cond_t cond;
pthread_mutex_t mutex;

char input_buffer[513];

void *nacitani(void *arg)
{
	/* Predani argumentu prikazove radky do vlakna */
	/* char **argv = (char **)arg; */
	int nacteno = 0;
	while(1)
	{
		while( (nacteno=read(STDIN_FILENO, input_buffer, 513)) >= 513 )
		{
			fprintf(stderr, "Prilis dlouhy vstup.\n");
			while( read(STDIN_FILENO, input_buffer, 513) < 513 ) continue;
		}
		input_buffer[nacteno-1] = '\0';
		if( DBG ) printf("Nacteno %s\n", input_buffer);

		pthread_cond_signal(&cond);
		pthread_cond_wait(&cond, &mutex);
	}
	return 0;
}

void *obsluha(void *arg)
{
	int i=0, j=0;
	int napozadi = 0;
	int ret;

	char *argumenty[128];
	size_t pocet_argumentu = 0;
	int velikost = 1;

	pid_t child_pid;
	int child_status;

	if( DBG ) printf("Obsluha ceka\n");

	while(1)
	{
		pthread_cond_wait(&cond, &mutex);

		i = 0;
		j = 0;
		napozadi = 0;
		argumenty[0] = '\0';
		pocet_argumentu = 0;
		velikost = 1;

		if( DBG ) printf("Obsluha provadi\n");
		if( DBG ) printf("Obsluha vypisuje: %s\n", input_buffer);

		while(input_buffer[i] != '\0')
		{
			while( input_buffer[i] == ' ' ) input_buffer[i++] = '\0';
			argumenty[j++] = &(input_buffer[i]);
			while( input_buffer[i] != '\0' && input_buffer[i] != ' ') i++;
		}
		argumenty[j] = '\0'; /* zarazka argumentu */

		if( strcmp(argumenty[j-1], "&") == 0 ) napozadi = 1;

		if( strcmp(argumenty[0], "exit") == 0 )
		{
			if( DBG ) printf("Ukonceni aplikace\n");
			exit(0);
		}

		/* rozvetveni parent/child */
		if( (child_pid = fork()) == 0 ) /* child */
		{
			if( DBG ) printf("Spusti se program: \"%s\" s parametry \"%s\"\n", argumenty[0], argumenty[1]);

			if( (ret = execvp(argumenty[0], (char * const *)argumenty)) == -1 )
			{
				fprintf(stderr, "Nepodarilo se spustit program %s\n", argumenty[0]);
			}
			else return (void*)ret;
		}
		else /* parent */
		{
			if( napozadi == 0 ) wait(&child_status); /* cekani na skonceni procesu child */
			if( DBG ) printf("Konec vyvolaneho procesu.\n");
		}
		pthread_cond_signal(&cond);
	}

	return 0;
}

int main(int argc, char *argv[])
{
	pthread_t pt_nacitani, pt_obsluha; /* identifikátor vlákna */
	int res;
	void *result;
	pthread_attr_t attr;

	/* vytvoření implicitních atributů */
	if( (res = pthread_attr_init(&attr)) != 0 )
	{
		fprintf(stderr, "pthread_attr_init() err %d\n", res);
		return 1;
	}

	pthread_cond_init(&cond, NULL);

	/* nastavení typu vlákna v atributech */
	/*if( (res = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE)) != 0 )
	{
		printf("pthread_attr_setdetachstate() err%d\n", res);
		return 1;
	}*/

	if( DBG ) printf("Zacatek programu.\n");
	pthread_create(&pt_nacitani, NULL, nacitani, (char*)(argv+1));
	pthread_create(&pt_obsluha, NULL, obsluha, (char*)(argv+1));

	/* čekání na dokončení a převzetí stavu */
	if( (res = pthread_join(pt_nacitani, &result)) != 0 )
	{
		fprintf(stderr, "pthread_attr_init() err %d\n",res);
		return 1;
	}

	/* čekání na dokončení a převzetí stavu */
	if( (res = pthread_join(pt_obsluha, &result)) != 0 )
	{
		fprintf(stderr, "pthread_attr_init() err %d\n",res);
		return 1;
	}


	if( DBG ) printf("Konec programu.\n");
	return 0;
}

