#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX 1024
void executePipe();
int parse();

void runCmd();

int main()
{
	int pid, status;
	char line[MAX];
	char *argv[64];
	
	while(1){
		printf("ShellPipe --> ");
		if(fgets(line,MAX,stdin) != 0) {
			line[strcspn(line, "\n")] = '\0';
			printf("\n");
		int cmdNum;
		cmdNum = parse(line, argv);

		if(strcmp(argv[0],"exit") == 0) {
				exit(0);
				}
		

		if(cmdNum <= 1 && *argv != NULL){
			runCmd(argv);
		}
		else {
			printf("Printing piped commands\n");
			char **cmdList[64];
			int curCmd = 0;
			int j;
			for(j = 0; j < cmdNum; j++) {
				cmdList[j] = &argv[curCmd];
				while(argv[curCmd] != NULL) {
					curCmd++;
				}
				curCmd++;
			}
			

			//Execute piped commands
			int k;
			int pfd[2];
			int prevPipe = 0;
			for(k = 0; k < cmdNum-1; k++){
				pipe(pfd);
				executePipe(cmdList[k], prevPipe, pfd[1]);
				close(pfd[1]);
				prevPipe = pfd[0];		

			}
			executePipe(cmdList[k], prevPipe, 1);
		}

	}
}
}
int parse(char *line, char **argv)
{
	int cmdNum = 1;
     while (*line != '\0') { 	/* if not the end of line */ 
          while (*line == ' ' || *line == '\t' || *line == '\n')
               *line++ = '\0';  /* replace white spaces with 0*/
          *argv++ = line;       /* save the argument position */
          
          while (*line != '\0' && *line != ' ' && 
                 *line != '\t' && *line != '\n'){
		if(*line == '|')
		{
			cmdNum++;
		}
               line++;      /* skip the argument until ...    */
     		}
     *argv = '\0';          /* mark the end of argument list  */
	return cmdNum;
}
void runCmd (char **argv)
{
	pid_t pid;
	int status;
	
	if((pid=fork()) < 0) //Error in forking
	{
		printf("ERROR IN FORKING");
	}
	else if(pid == 0) //Child process
	{
		if(execvp(*argv, argv) < 0) 
		{
			printf("Failure in executing");
			exit(1);
		}
	}
	else //Parent process
	{
		while(wait(&status) != pid);
	}
}

void executePipe (char **cmd, int in_fd, int out_fd) 
{
  pid_t pid;
  int status;
  
  if ((pid = fork()) == -1) {
    printf("*** ERROR: forking child process failed\n");
    exit(1);
  }
  else if (pid == 0) {
	printf("entered pid");
    if (in_fd != 0) {
      dup2(in_fd, 0);
      close(in_fd);
    }
    if (out_fd != 1) {
      dup2(out_fd, 1);
      close(out_fd);
    }
    if (execvp(*cmd, cmd) < 0) {     /* exec command  */
      printf("*** ERROR: exec failed\n");
      exit(1);
    }
  }
  else {
    while (wait(&status) != pid) /* wait for completion */
               ;
  }
}

