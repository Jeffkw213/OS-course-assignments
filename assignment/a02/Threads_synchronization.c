/*  
 *  NAME: Jeff Kwan 
 *  STUDENT ID: 216396764
 *  DATE: Oct 21 2020
 *  COURSE: EECS3221
 *  PROGRAM: A02.c
 *  
 *  Program description:
 *  Read the input file that contains thread properties: ID, start time and lifetime.
 *  Create a thread whenever it is the start time of some thread.•All the threads implement/run same function. 
 *  Implement this function so that the thread should terminate when it has consumed time units equal to its lifetime.
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>
#include <time.h>

void logStart(char *tID);  //function to log that a new thread is started
void logFinish(char *tID); //function to log that a thread has finished its time

void startClock();	   //function to start program clock
long getCurrentTime(); //function to check current time since clock was started
time_t programClock;   //the global timer/clock for the program
int done = 0; //know when all of threads are done

#define N 4

typedef struct thread //represents a single thread
{
	char tid[N]; //id of the thread as read from file
	pthread_t thread_id;
	long int start_time;
	long int life_time;
	int complete;
	//add more members here as per requirement
} Thread;

void *threadRun(void *t);						//the thread function, the code executed by each thread
int readFile(char *fileName, Thread **threads); //function to read the file content and build array of threads

int main(int argc, char *argv[])
{

	if (argc < 2)
	{
		printf("Input file name missing...exiting with error code -1\n");
		return -1;
	}
	Thread *threads[N];
	startClock();
	//you can add some suitable code here as per problem sepcification
	int i = readFile(argv[1], threads);
	// printf("%d\n", i);
	// printf("%d\n", (*threads)[0].complete);
	while (done != i) //put a suitable condition here to run your program
	{
		//write suitable code here to run the threads
		// printf("%ld\n",getCurrentTime());
		// printf("%ld\n",(*threads)[complete].start_time);
		for (int z = 0; z < i; z++)
		{
			if ((*threads)[z].start_time == getCurrentTime())
			{
				logStart((*threads)[z].tid);
				pthread_create(&((*threads)[z].thread_id), NULL, threadRun, (void *)(*threads + z));
			}
		}
		sleep(1);
	}

	return 0;
}

int readFile(char *fileName, Thread **threads) //use this method in a suitable way to read file
{
	FILE *in = fopen(fileName, "r");
	if (!in)
	{
		printf("Child A: Error in opening input file...exiting with error code -1\n");
		return -1;
	}

	struct stat st;
	fstat(fileno(in), &st);
	char *fileContent = (char *)malloc(((int)st.st_size + 1) * sizeof(char));
	fileContent[0] = '\0';
	while (!feof(in))
	{
		char line[100];
		if (fgets(line, 100, in) != NULL)
		{
			strncat(fileContent, line, strlen(line));
		}
	}
	fclose(in);

	char *command = NULL;
	int threadCount = 0;
	char *fileCopy = (char *)malloc((strlen(fileContent) + 1) * sizeof(char));
	strcpy(fileCopy, fileContent);
	command = strtok(fileCopy, "\r\n");
	while (command != NULL)
	{
		threadCount++;
		command = strtok(NULL, "\r\n");
	}
	*threads = (Thread *)malloc(sizeof(Thread) * threadCount);

	char *lines[threadCount];
	command = NULL;
	int i = 0;
	command = strtok(fileContent, "\r\n");
	while (command != NULL)
	{
		lines[i] = malloc(sizeof(command) * sizeof(char));
		strcpy(lines[i], command);
		i++;
		command = strtok(NULL, "\r\n");
	}

	for (int k = 0; k < threadCount; k++)
	{
		char *token = NULL;
		int j = 0;
		token = strtok(lines[k], ";");
		while (token != NULL)
		{
			if (j == 0)
				strcpy((*threads)[k].tid, token);
			else if (j == 1)
				(*threads)[k].start_time = atol(token);
			else
				(*threads)[k].life_time = atol(token);
			(*threads)[k].complete = 0;
			token = strtok(NULL, ";");
			//this loop tokenizes each line of input file
			j++;
			//write your code here to populate instances of Thread to build a collection
		}

		// printf("%s %ld %ld\n", (*threads)[k].tid, (*threads)[k].start_time , (*threads)[k].life_time);
	}
	return threadCount;
}

void logStart(char *tID) //invoke this method when you start a thread
{
	printf("[%ld] New Thread with ID %s is started.\n", getCurrentTime(), tID);
}

void logFinish(char *tID) //invoke this method when a thread is over
{
	printf("[%ld] Thread with ID %s is finished.\n", getCurrentTime(), tID);
}

void *threadRun(void *t) //implement this function in a suitable way
{
	Thread *a = (Thread *)t;
	// printf("%s %ld %ld\n", a->tid, a->start_time, a->life_time);
	// printf("ID:[%s] start:[%ld] finish:[%ld] current:[%ld]\n", a->tid, a->start_time, a->start_time + a->life_time, getCurrentTime());
	// printf("%ld\n", (a->life_time + a->start_time));
	sleep(a->life_time);
	// while ((a->life_time + a->start_time) != getCurrentTime())
	// 	sleep(1);
	logFinish(a->tid);
	a->complete = 1;
	done++;
	// printf("%d\n", done);

	pthread_exit(0);
}

void startClock() //invoke this method when you start servicing threads
{
	programClock = time(NULL);
}

long getCurrentTime() //invoke this method whenever you want to check how much time units passed
//since you invoked startClock()
{
	time_t now;
	now = time(NULL);
	return now - programClock;
}