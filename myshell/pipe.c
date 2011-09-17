#include <stdio.h>
#include <unistd.h>

void connect_pipe(char* ps1, char* ps2) {
	if(ps1 != NULL && ps2 != NULL) {
		int fd[2], pid;
		if(pipe(fd) == -1)
			perror("error creating pipe");

		if((pid = fork()) == -1) 
			perror("could not fork in connect_pipe");

		if(pid == 0) { //child
			printf("in child of pipe");
			close(fd[0]); //close read side
			dup2(fd[1], STDOUT_FILENO); //redirect to stdout
			close(fd[1]); //already redirecting to stdout;
			execlp(ps1, ps1, NULL);
			perror("");
		}

		//parent code
		close(fd[1]); //don't write to pipe
		dup2(fd[0], STDIN_FILENO); //dup to stdin
		close(fd[0]);
		execlp(ps2, ps2, NULL);
		perror("");
	} else {
		perror("arguments to connect_pipe must not be null");
	}
}

