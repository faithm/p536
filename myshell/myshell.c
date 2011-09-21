#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "safe_funcs.h"
#include <ctype.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#define SPACE 32
char cmds[6] = {'&','>','<', ';', '|', ' '}; //don't mess with this order

/* Christopher Zakian | czakian
 * Class: P536
 * Date: Sept 19, 2011
 *
 * Overview: this program is arranged like a state machine.
 * there are three sections: parsing, argument creation, and commands
 *
 * Parsing section: reads cmds until EOF or \n and generates a doubly
 * linked list of arguments separated by special command characters or
 * by a space.
 *
 * Argument creation: walks over the nodes created in the parsing section
 * and reads nodes until a command is reached or the commands terminate on NULL.
 *
 * Commands: are passed the argument array generated in the argument section.
 * if a match for a special character is found, a process is forked and then
 * executed. Otherwise it is assumed that the args refer to an executable command
 * on the path. 
 *
 */

typedef struct node {
	char* datum;
	char ** args;
	struct node* prev;
	struct node* next;
	int fd[2];
} node;


int cleanup_nodes(node *n) {
	if(n != NULL) {
		free(n->datum);
		if(n->args != NULL) {
			for(int i = 0; n->args[i] != NULL; i++) {
				free(n->args[i]);
			}
		}
		cleanup_nodes(n->next);
	}
	free(n);
}

node *create_node(){
	node *n = (node *) safe_malloc(sizeof(node), "error creating node");
	n->args = NULL;
	n->prev = NULL;
	n->next = NULL;
	n->datum = NULL;
	return n;
}

void pipe_out(node *prev) {
	if(close(prev->fd[0]) == -1) { //child doesn't read
		perror("fatal error: closing file descriptor in child failed");
	}
	if(dup2(prev->fd[1], STDOUT_FILENO) == -1) { //write to stdout
		perror("fatal error: dup2 failed in file redirection");
	}
	if(close(prev->fd[1]) == -1) { //close write side fd
		perror("fatal error: closing file descriptor in child failed");
	}
}

void pipe_in(node *prev) {
	if(close(prev->fd[1]) == -1) { //don't write
		perror("fatal error: closing file descriptor in parent failed");
	}
	if(dup2(prev->fd[0], STDIN_FILENO) == -1) { //read from stdin
		perror("fatal error: dup2 failed in file redirection");
	}
	if(close(prev->fd[0]) == -1) { //close read side fd
		perror("fatal error: closing file descriptor in parent failed");
	}
}

void pipline(node *cur, int fd) {
	if(cur->datum != NULL)
		cur = cur->next;

	cur->fd[0] = -1;
	cur->fd[1] = -1;

	if(cur->next != NULL)
		if(pipe(cur->fd) == -1)
			perror("fatal error: pipe failed");

	int pid;
	if((pid = fork() == -1))
		perror("fork failed in creating pipeline");

	if(!pid) { //child
		if(fd != -1) {
			if(dup2(fd, STDIN_FILENO) == -1)
				perror("error in duping std in");

			if(close(fd) == -1)
				perror("error closing passed file descriptor");
		}

		if(cur->fd[1] != -1) {
			if(dup2(cur->fd[1], STDOUT_FILENO) == -1)
				perror("error in duping std out");

			if(close(cur->fd[1]) == -1)
					perror("error closing file descriptor");
		}

		if(cur->fd[0] != -1) {
			if(close(cur->fd[0]) == -1)
				perror("error closing file descriptor");
		}

		execvp(cur->args[0], cur->args);
		perror("");
	} else { //parent
		if(fd != -1) {
			close(fd);
		}

		if(cur->fd[1] != -1) {
			close(cur->fd[1]);
		}

		if(cur->next != NULL) 
			pipline(cur->next, cur->fd[0]);

		int status;
		waitpid(-1, &status, 0);
	}
}

int main(int argc, char** argv) {

	char* bucket;
	int bucket_read = 0;
	int bucket_size = 256;
	int arg_c = 0;
	int args_size = 256;
	node *head = NULL;
	node *cur = NULL;

	//initialize with prompt
	char *prompt;
	if(argc >= 2)
		prompt = argv[1];
	else
		prompt = "myshell:";

	//parsing section

	bucket = (char *) safe_malloc(bucket_size*sizeof(char), "fatal error in main: bucket could not be malloced");
	while(1) {
		printf("\n%s", prompt);

		char c;
		cur = create_node();
		while((c = getchar())) {

			if(c == EOF) {
				cleanup_nodes(head);
				free(bucket);
				exit(0);
			} 

			if(isalnum(c) || c >= 33 && c <= 47 || c >= 91 && c <= 96 || c == 123 || c == 125 || c == 126) {
				bucket = (char *) chksize(bucket, bucket_read, &bucket_size, bucket_size*2+1);
				bucket[bucket_read] = c;
				bucket_read += 1;
			} else if (c == SPACE || c == '\n'){

				if(bucket_read >= 1) {
					if(cur->args == NULL) 
						cur->args = safe_malloc(args_size*sizeof(char *), "fatal error: could not malloc args array");
					cur->args[arg_c] = (char *) safe_malloc(bucket_read*sizeof(char)+1, "could not malloc datum in node");
					strncpy(cur->args[arg_c], bucket, bucket_read);
					cur->args[arg_c+1] = '\0';
					cur->args[arg_c][bucket_read+1] = '\0';
					bucket_read = 0;
					arg_c += 1;
				}
				if(c == '\n') {
					arg_c = 0;
					if(head == NULL)
						head = cur;
					break;
				}

			} else if(memchr(cmds, c, 5)) {

				if(bucket_read >= 1) {
					if(cur->args == NULL) 
						cur->args = safe_malloc(args_size*sizeof(char *), "fatal error: could not malloc args array");
					cur->args[arg_c] = (char *) safe_malloc(bucket_read*sizeof(char)+1, "could not malloc datum in node");
					strncpy(cur->args[arg_c], bucket, bucket_read);
					cur->args[arg_c+1] = '\0';
					cur->args[arg_c][bucket_read] = '\0';
					bucket_read = 0;
					arg_c += 1;
				}

				node *next = create_node();
				cur->next = next;
				next->prev = cur;
				next->datum = (char *) safe_malloc(2*sizeof(char), "could not malloc datum in node");
				next->datum[0] = c;
				next->datum[1] = '\0';
				if(head == NULL)
					head = cur;
				cur = cur->next;

				node *tmp = create_node();
				cur->next = tmp;
				tmp->prev = cur;
				cur = tmp;
				arg_c = 0;
			}
		}

		cur=head;
		if(head != NULL && head->args != NULL && strstr(head->args[0], "exit")) {
			cleanup_nodes(head);
			free(bucket);
			exit(0);
		}

		/*
		while(cur != NULL) {
			int a = 0;
			while(cur->args != NULL && cur->args[a] != NULL) {
				printf("arg %d is: %s\n",a,cur->args[a]);
				a++;
			}
			cur = cur->next;
		}
		*/
		cur = head;

		//arguments section

		int pid, status;
		int AMPERSAND = 0;
		while(cur != NULL) {

			//start command states:

			if(cur != NULL && cur->datum != NULL && memchr(cmds,cur->datum[0], 1)) {
				AMPERSAND = 1; //WNOHANG
			}

			if(cur != NULL && cur->datum != NULL && cur->datum[0] == ';') { //skip over this command since we don't want to execute it
				cur = cur->next;
			}

			if(cur->args != NULL && cur->args[0] != NULL && strstr(cur->args[0], "cd")) { //cd as a builtin command

				if(chdir(cur->args[1]) != 0) {
					perror("changing directory failed: invalid path");	
				}

			} else if(cur != NULL && cur->datum != NULL && cur->datum[0] == '|') { //pipe
				pipline(cur->prev, -1);

			} else if (cur != NULL  && cur->datum != NULL && cur->datum[0] == '>') { //redirect output

				if((pid = fork()) == -1) 
					perror("could not fork in output redirecton");

				if(pid == 0) { //child

					char* arg = cur->prev->datum;
					int fd;
					if(cur->next->datum[0] == '>') { //handle >> command
						cur = cur->next; //advance the args list past the second >
						if((fd = open(cur->next->datum, O_RDWR | O_CREAT | O_APPEND, 0666)) < 0)
							perror("could not open fd in input redirection");

					} else {
						if((fd = open(cur->next->datum, O_RDWR | O_CREAT, 0666)) < 0)
							perror("could not open fd in input redirection");
					}
					if(dup2(fd, STDOUT_FILENO) < 0) {
						perror("fatal error: dup2 failed in file redirection");
					}
					if(close(fd) < 0) {
						perror("close operation in redirection failed");
					}
					execvp(cur->args[0], cur->args);
					perror("");
				} else {
					if(cur != NULL && cur->datum != NULL && cur->next->datum[0] == '>') 
						cur = cur->next; //advance the args list past the second >
					cur = cur->next;
					waitpid(-1, &status, AMPERSAND);
				} 

				//redirect file
			} else if (cur != NULL && cur->datum != NULL && cur->datum[0] == '<') {
				if((pid = fork()) == -1) 
					perror("could not fork in file redirecton");

				if(pid == 0) { //child
					char* arg = cur->prev->datum;
					int fd;
					if((fd = open(cur->next->datum, O_RDONLY)) < 0){
						perror("could not open fd in input redirection");
					} else {
						if(dup2(fd, STDIN_FILENO) == -1) {
							perror("fatal error: dup2 failed in file redirection");
						}
						if(close(fd) < 0) {
							perror("close operation in redirection failed");
						}
						execvp(cur->args[0], cur->args);
						perror("");
					}

				} else { //parent
					cur = cur->next;
					waitpid(-1, &status, AMPERSAND);
				}
			} else {
				if((pid = fork()) == -1) 
					perror("could not fork in run command");

				if(pid == 0) { //child
					execvp(cur->args[0], cur->args);
					perror("");
				} else {
					waitpid(-1, &status, AMPERSAND);
				}
			}

			if(cur != NULL)
				cur = cur->next; 
		} //while cur !NULL

		cleanup_nodes(head);
		head = NULL;
		cur = NULL;
		for(int i = 0; i < bucket_size; i++) { bucket[i] = '\0';}
		arg_c = 0;
	} //while 1

	free(bucket);
	return 0;
} 

