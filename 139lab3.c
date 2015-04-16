/*John Jones
CSC 139
Assignment 3
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/uio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>

#define SLOTSIZE 18
#define SLOTCOUNT 9

char buffer[SLOTCOUNT][SLOTSIZE];
int done = 0;
char* infile_name;
char* outfile_name;
FILE *infile, *outfile;
pthread_cond_t	empty_slot=PTHREAD_COND_INITIALIZER;
pthread_cond_t  avail_item=PTHREAD_COND_INITIALIZER;
pthread_mutex_t	buf_lock=PTHREAD_MUTEX_INITIALIZER;
void *producer();
void *consumer();
	
main (int argc, char *argv[])
{
	//sem_init(&buf_lock, 0,1);
	//sem_init(&slot_avail,0,9);
	//sem_init(&item_avail,0,0); //init all semaphores. Slots available at init = 9. Items available = 0;
	int count = 1, argvcount = 0, matrixValueCount = 0;
	size_t length = 0;
	
	pthread_t m0, m1; //create two concurrent threads
	if (argc != 3)
	{
		printf("Error - Incorrect Usage: Syntax is: prog3 infile outfile\n");
		exit(-1);
	}
	infile_name = argv[1];
	outfile_name = argv[2];
	
infile = fopen(infile_name, "r");

if (infile == NULL)
{printf("something went wrong with opening the input file");exit(1);}

outfile = fopen(outfile_name, "w");

if (outfile == NULL)
{printf("something went wrong with opening the output file");exit(1);}

/*int c;
while  ((c = fgetc(infile)) != EOF)
{
fputc(c, outfile);
}
*/
	printf("main: making thread for Producer...\n"); //create the threads and wait
	pthread_create(&m0, NULL, (void *)producer, NULL);
	printf("main: making thread for Consumer...\n");
	pthread_create(&m1, NULL, (void *)consumer, NULL);
	pthread_join(m0, NULL); //This section waits for all threads to finish.
	pthread_join(m1, NULL);
	
	printf("main: EXITING...\n");
	fclose(infile);
	fclose(outfile);	
	exit(0);
	}
	
void *producer()
	{
int in = 0;
int full = 0;
	pthread_t self_id;
	self_id = pthread_self();
	printf("producer: reporting in. Thread-id: %d Calculating..\n", self_id);	
while(1)
{
while(!feof(infile))
{
	pthread_mutex_lock(&buf_lock);
	//sem_wait(&slot_avail);
	//sem_wait(&buf_lock);
	/*while(full == 8)
	{
		pthread_cond_wait(&empty_slot, &buf_lock);
	}
	*/
	printf("buffer slot: %d\n", in);
	fgets(buffer[in], 18, infile);
	//puts(buffer[in]);
	//	fputs(buffer[in], infile);
		in = (in +1 ) % SLOTCOUNT;
	//	printf("Buffer %d String: %s\n", in, buffer[in]);
	pthread_cond_signal(&avail_item);
	pthread_mutex_unlock(&buf_lock);
	//sem_post(&buf_lock);
	//sem_post(&item_avail);	
	}
	done=1;printf("Consumer: FOUND EOF.  Exiting\n");break;
}
	pthread_exit(0);
}
	
void *consumer()
	{
	int out=0;
	int slots = 0;
	int full = 0;
	pthread_t self_id;
	self_id = pthread_self();
	printf("Consumer: reporting in. Thread-id: %d Calculating..\n", self_id);	
	while(1)
	{
	//printf("Done? %d\n", done);
	//sem_getvalue(&avail_item, &slots);
	if(done==1 && slots==0){break;}
	pthread_mutex_lock(&buf_lock);
	/*while(out == 0)
	{
		pthread_cond_wait(&empty_slot, &buf_lock);
	}
	*/
	//sem_wait(&item_avail);
	//sem_wait(&buf_lock);
	fputs(buffer[out], outfile);
	//buffer[out][0] = '\0';
	printf("buffer out to file %d %s\n", out, buffer[out]);
	out--;
	pthread_cond_signal(&empty_slot);
	pthread_mutex_unlock(&buf_lock);
	//sem_post(&buf_lock);
	//sem_post(&slot_avail);
	}	
	printf("Consumer got done = 1 signal\n");
	pthread_exit(0);
	}
