/*  
 *  NAME: Jeff Kwan 
 *  STUDENT ID: 216 396 764
 *  DATE: Nov 29 2020
 *  COURSE: EECS3221
 *  PROGRAM: A03.c
 *  
 *  Program description:
 *  The code provided reads the content of file for you and populate the threads information in a dynamic array of type struct thread. 
 *  You may add some more members to this data structureif required.
 *  If you want to initialize those members,then you can possibly do that during the file read.
 *  The main() already contains the code to create and invoke threads. 
 *  However, there is no synchronization logic added to it. 
 *  If required, you will add some suitable code in the while loops to perform the tasks required.
 * 	The threadRun()function also contains the code that a thread must run.
 *  However, again the synchronization logic is missing. Add the suitable code before and after the critical section.
 *  You will need to create and use POSIX semaphore(s) to implement the required logic.

 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>
#include <time.h>
#include <semaphore.h>

void logStart(char* tID);//function to log that a new thread is started
void logFinish(char* tID);//function to log that a thread has finished its time

void startClock();//function to start program clock
long getCurrentTime();//function to check current time since clock was started
time_t programClock;//the global timer/clock for the program
sem_t even;
sem_t odd;
int oddleft = 0;
int evenleft = 0;


typedef struct thread //represents a single thread, you can add more members if required
{
	char tid[4];//id of the thread as read from file
	unsigned int startTime;
	int state;
	pthread_t handle;
	int retVal;
	int check_odd; //if the id is even or odd
} Thread;

//you can add more functions here if required

int threadsLeft(Thread* threads, int threadCount);
int threadToStart(Thread* threads, int threadCount);
void* threadRun(void* t);//the thread function, the code executed by each thread
int readFile(char* fileName, Thread** threads);//function to read the file content and build array of threads
void check_even(Thread* threads, int threadCount);
int countOdd(Thread* threads, int threadCount);
int countEven(Thread* threads, int threadCount);

int main(int argc, char *argv[])
{
	if(argc<2)
	{
		printf("Input file name missing...exiting with error code -1\n");
		return -1;
	}

    //you can add some suitable code anywhere in main() if required

	Thread* threads = NULL;
	int threadCount = readFile(argv[1],&threads);
	sem_init(&even, 0, 0);
	sem_init(&odd, 0, 0);
	int i = 0;
	startClock();
	check_even(threads, threadCount);

	while(threadsLeft(threads, threadCount)>0)//put a suitable condition here to run your program
	{
    //you can add some suitable code anywhere in this loop if required
		
		if (threads[i].check_odd == 1)
			sem_post(&odd);
		else
			sem_post(&even);
		
		// oddleft = countOdd(threads, threadCount);
		// evenleft = countEven(threads, threadCount);
		
		while((i=threadToStart(threads, threadCount))>-1)
		{
			oddleft = countOdd(threads, threadCount);
			evenleft = countEven(threads, threadCount);
		    //you can add some suitable code anywhere in this loop if required
			threads[i].state = 1;
			threads[i].retVal = pthread_create(&(threads[i].handle),NULL,threadRun,&threads[i]);
			
		}
		//check number of remaining threads counts # of even and odd
		//if one of them reaches 0, then the opposite check_odd is called.
		if (oddleft == 0)
			sem_post(&even);
		else if (evenleft == 0)
			sem_post(&odd);
	}
	sem_destroy(&even);
	sem_destroy(&odd);
	return 0;
}

int readFile(char* fileName, Thread** threads)//do not modify this method
{
	FILE *in = fopen(fileName, "r");
	if(!in)
	{
		printf("Child A: Error in opening input file...exiting with error code -1\n");
		return -1;
	}

	struct stat st;
	fstat(fileno(in), &st);
	char* fileContent = (char*)malloc(((int)st.st_size+1)* sizeof(char));
	fileContent[0]='\0';	
	while(!feof(in))
	{
		char line[100];
		if(fgets(line,100,in)!=NULL)
		{
			strncat(fileContent,line,strlen(line));
		}
	}
	fclose(in);

	char* command = NULL;
	int threadCount = 0;
	char* fileCopy = (char*)malloc((strlen(fileContent)+1)*sizeof(char));
	strcpy(fileCopy,fileContent);
	command = strtok(fileCopy,"\r\n");
	while(command!=NULL)
	{
		threadCount++;
		command = strtok(NULL,"\r\n");
	}
	*threads = (Thread*) malloc(sizeof(Thread)*threadCount);

	char* lines[threadCount];
	command = NULL;
	int i=0;
	command = strtok(fileContent,"\r\n");
	while(command!=NULL)
	{
		lines[i] = malloc(sizeof(command)*sizeof(char));
		strcpy(lines[i],command);
		i++;
		command = strtok(NULL,"\r\n");
	}

	for(int k=0; k<threadCount; k++)
	{
		char* token = NULL;
		int j = 0;
		token =  strtok(lines[k],";");
		while(token!=NULL)
		{
//if you have extended the Thread struct then here
//you can do initialization of those additional members
//or any other action on the Thread members
			(*threads)[k].state=0;
			if(j==0)
				strcpy((*threads)[k].tid,token);
			if(j==1)
				(*threads)[k].startTime=atoi(token);
			j++;
			token = strtok(NULL,";");
		}
		// Having alternating threads go into Threadrun.
		// printf("threads[%d].check_odd = %d\n", k, (*threads)[k].check_odd);
	}
	return threadCount;
}

void logStart(char* tID)
{
	printf("[%ld] New Thread with ID %s is started.\n", getCurrentTime(), tID);
}

void logFinish(char* tID)
{
	printf("[%ld] Thread with ID %s is finished.\n", getCurrentTime(), tID);
}

void check_even(Thread* threads, int threadCount){
	
	for(int k=0; k<threadCount; k++)
	{
		int x = (int)threads[k].tid[strlen(threads[k].tid)-1];
		if(x % 2 == 0)
			threads[k].check_odd = 0;
		else 
			threads[k].check_odd = 1;
	}
}

int threadsLeft(Thread* threads, int threadCount)
{
	int remainingThreads = 0;
	for(int k=0; k<threadCount; k++)
	{
		if(threads[k].state>-1)
			remainingThreads++;
	}
	return remainingThreads;
}

int countOdd(Thread* threads, int threadCount)
{
	int count = 0;
	for(int i = 0 ; i < threadCount; i++){
		if(threads[i].state != -1 && threads[i].check_odd == 1){
			count++;
		}
	}
	return count;
}

int countEven(Thread* threads, int threadCount)
{
	int count = 0;
	for(int i = 0 ; i < threadCount; i++){
		if(threads[i].state != -1 && threads[i].check_odd == 0){
			count++;
		}
	}
	return count;
}

int threadToStart(Thread* threads, int threadCount)
{
	for(int k=0; k<threadCount; k++)
	{
		if(threads[k].state==0 && threads[k].startTime==getCurrentTime())
			return k;
	}
	return -1;
}

void* threadRun(void* t)//implement this function in a suitable way
{
	logStart(((Thread*)t)->tid);
//your synchronization logic will appear here
	if (((Thread*)t)->check_odd == 1)
		sem_wait(&odd);
	else
		sem_wait(&even);
	//critical section starts here
	printf("[%ld] Thread %s is in its critical section\n",getCurrentTime(), ((Thread*)t)->tid);
	//critical section ends here
	if (((Thread*)t)->check_odd == 0)
		sem_post(&odd);
	else
		sem_post(&even);
//your synchronization logic will appear here


	logFinish(((Thread*)t)->tid);
	((Thread*)t)->state = -1;
	// count number of evens and odds that are left and not finished yet.

	pthread_exit(0);
}

void startClock()
{
	programClock = time(NULL);
}

long getCurrentTime()//invoke this method whenever you want check how much time units passed
//since you invoked startClock()
{
	time_t now;
	now = time(NULL);
	return now-programClock;
}

/*  
 *  NAME: Jeff Kwan 
 *  STUDENT ID: 216 396 764
 *  DATE: Nov 29 2020
 *  COURSE: EECS3221
 *  PROGRAM: A03.c
 *  
 *  Program description:
 *  The code provided reads the content of file for you and populate the threads information in a dynamic array of type struct thread. 
 *  You may add some more members to this data structureif required.
 *  If you want to initialize those members,then you can possibly do that during the file read.
 *  The main() already contains the code to create and invoke threads. 
 *  However, there is no synchronization logic added to it. 
 *  If required, you will add some suitable code in the while loops to perform the tasks required.
 * 	The threadRun()function also contains the code that a thread must run.
 *  However, again the synchronization logic is missing. Add the suitable code before and after the critical section.
 *  You will need to create and use POSIX semaphore(s) to implement the required logic.

 */
