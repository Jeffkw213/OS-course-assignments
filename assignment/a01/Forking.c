/*  
 *  NAME: Jeff Kwan 
 *  STUDENT ID: 216396764
 *  DATE: October 5 2020
 *  COURSE: EECS3221
 *  PROGRAM: A01.c
 *  
 *  Program description:
 *  Read the input file that contains Linux shell commands. 
 *  Parent process will read that.
 *  Execute the Linux shell commands read from the input file and execute them one by one. 
 *  A child process will be created to execute these commands and the output will be returned by the child process in the form of string using pipe.
 *  The parent process will write the output of commands to the screen.
 *  The following flow chart describes the flow of the program.
 */ 


#include <stdio.h>
#include <string.h> 
#include <unistd.h>
#include <sys/wait.h>  
#include <stdlib.h>

void writeOutput(char*, char*);

int main(int argc, char* argv[]){

    int rw[2]; //pipe 
                //rw[0] - reading 
                //rw[1] - writing  
    pid_t p; //forking 
    size_t len = 150;
    char** command;
    int i = 0;


    FILE* file = fopen(argv[1], "r");
    /* Conditions and ERRORS */
    if (file == NULL){ //file not there 
        printf("FILE CANNOT OPEN");
        fclose(file);
        return 1;
    }

    command = (char**)malloc(sizeof(char*)*len);
    if (command == NULL){return 1;}


    /* reading from file */
    while(!feof(file)){ //goes through the entire file
        command[i] = malloc(sizeof(char*)*len);
        fgets(command[i], __INT_MAX__, file); 
        // printf("%s\n", command[i]);
        strtok(command[i],"\n");
        strtok(command[i],"\r");
        i++;
    }
    // for(int j = 0 ; j < i; j++)
    //     printf("commandline[%d] = %s\n", j, command[j]);


    
    
    int x;
    for(x = 0 ; x < i; x++){  
        /* Pipe Errors */
        if(pipe(rw) == -1) {return 1;}
        // int x = 0;
        p = fork();

        
        if (p < 0){ return 1;}

        else if (p == 0){
            
            //Child process
            
            //cutting the command into sections between commands and saving args
            //command[1] = ls -l -a -F
            //I want args = ls
            
            /* cutting the commands on every " " */
            int j = 0;
            char ** args = malloc(sizeof(char*) * len);
            char * arguments= strtok(command[x], " ");
            while(arguments){
                args[j++] = arguments;
                // printf("%s\n", arguments);
                arguments = strtok(NULL, " ");
            }
            // printf("number of lines in arg: %d\n", i);
            // printf("%s\n", args[i]);
            // for(int j = 0 ; j < i+1 ; j++)
            //     printf("args[%d]: %s \n", j, args[j]);

            //writing to the pipe what was printed out from the execvp

            /* writing to the pipes */
            if (dup2(rw[1], STDOUT_FILENO) == -1) {return 1;};
            close(rw[0]);
            close(rw[1]);
            free(args);
            /* Execution as well as checking if failure */
            if (execvp(args[0], args)== -1){
                printf("Execution has failed\n");
                exit(1);
            }
                
            // char *argv[5];
            // argv[0] = "ls";
            // argv[1] = "-l";
            // argv[2] = "-a";
            // argv[3] = "-F";
            // argv[4] = NULL;
            // for (int i = 0 ; i < sizeof(argv); i ++)
            //     printf("args[%d]: %s \n", i, argv[i]);
            // execvp(argv[0], argv);

            
            return 0;
        }else{
            wait(NULL);
            close(rw[1]);

            char exe[200]; //variable that stores the output of the execution
            
            // printf("%s\n", command[x]);
            //Parent process

            //reading from the pipes 
            int c = read(rw[0], exe, sizeof(exe));

            exe[c] = '\0';
            // printf("process is parent process\n"); 
            close(rw[0]);
            // printf("output of: %s\n", command[x]);
            // printf(">>>>>>%s\n", exe);

            /* calling the writeOutput */
            writeOutput(command[x], exe);
        }
    }
    fclose(file);   

    /*freeing memory*/
    int j;
    for (j = 0 ; j < sizeof(command); j++){
        free(command[j]);
    }
    free(command);
    return 0;
}

/* Writing the output */
void writeOutput(char* command, char* output)
{
	printf("The output of: %s : is\n", command);
	printf(">>>>>>>>>>>>>>>\n%s<<<<<<<<<<<<<<<\n", output);	
}
/*
MAKING MORE LINES
MAKING MORE LINES
MAKING MORE LINES
MAKING MORE LINES
MAKING MORE LINES
MAKING MORE LINES
MAKING MORE LINES
MAKING MORE LINES
MAKING MORE LINES
MAKING MORE LINES
MAKING MORE LINES
MAKING MORE LINES
*/