#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "safe_funcs.h"
#include <ctype.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#define SPACE 32

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
	struct node* prev;
	struct node* next;
} node;

int cleanup_nodes(node *n) {
	if(n != NULL) {
		free(n->datum);
		cleanup_nodes(n->next);
	}
	free(n);
}

int main(int argc, char** argv) {

	char* bucket;
	char cmds[6] = {'&','>','<', ';', '|', ' '}; //don't mess with this order
	int bucket_read = 0;
	int bucket_size = 256;
	int reject_eof = 0;
	node *head = NULL;
	node * cur = NULL;

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
		while((c = getchar()) != '\n') {

			//deliberately not catching EOF here because the pipes send through
			//a bunch of EOFs which make the shell exit. still looking for a good 
			//solution to this problem. (ideas?)
			/*
			if(c == EOF) {
				cleanup_nodes(head);
				free(bucket);
				exit(0);
			} 
			*/

			if(isalnum(c) || c >= 33 && c <= 47 || c >= 91 && c <= 96 || c == 123 || c == 125 || c == 126) {
				bucket = (char *) chksize(bucket, bucket_read, &bucket_size, bucket_size*2+1);
				bucket[bucket_read] = c;
				bucket_read += 1;
			} else if(memchr(cmds, c, 6)) {

				if(bucket_read >= 1) {
					node *s = (node *) safe_malloc(sizeof(node), "error creating node");
					s->datum = (char *) safe_malloc(bucket_read*sizeof(char)+1, "could not malloc datum in node");
					bucket[bucket_read+1] = '\0';
					strncpy(s->datum, bucket, bucket_read+1);
					if(cur != NULL)
						cur->next = s;
					bucket_read = 0;
					s->prev = cur;
					s->next = NULL;
					cur = s;
					if(head == NULL)
						head = cur;
				} 

				if(c != SPACE) {
					node *n = safe_malloc(sizeof(node), "error creating node");
					if(cur != NULL)
						cur->next = n;
					n->datum = (char *) safe_malloc(2*sizeof(char), "could not malloc datum in node");
					n->datum[0] = c;
					n->datum[1] = '\0';
					n->prev = cur;
					n->next = NULL;
					cur = n;
					if(head == NULL)
						head = cur;
				}
			}
		}

		//parse single args
		if(bucket_read >= 1) {
			node *s = (node *) safe_malloc(sizeof(node), "error creating node");
			s->datum = (char *) safe_malloc(bucket_read*sizeof(char)+2, "could not malloc datum in node");
			strncpy(s->datum, bucket, bucket_read);
			s->datum[bucket_read+1] = '\0';
			bucket_read = 0;
			if(cur != NULL)
				cur->next = s;
			s->prev = cur;
			s->next = NULL;
			cur = s;
		}

		if(head == NULL)
			head = cur;

		cur=head;

		if(strstr(head->datum, "exit")) {
				cleanup_nodes(head);
				free(bucket);
				exit(0);
		}

		//arguments section

		int pid, status; 
		int args_size = 32; 
		char** args = (char **) safe_malloc(args_size*sizeof(char *), "fatal error mallocing args in pipe");
		int AMPERSAND = 0;
		while(cur != NULL) {
			int i = 0;

			//read in all the args until we hit end of input or another command
			while(cur != NULL && memchr(cmds, cur->datum[0], 5) == 0) {
				args = (char **) chksize(args,i, &args_size, args_size*2+1); //append to the buffer
				args[i] = cur->datum;
				i+=1;
				cur = cur->next;
			}
			args[i+1] = NULL;

			//start command states:

			if(cur != NULL && memchr(cmds,cur->datum[0], 1)) {
				AMPERSAND = 1; //WNOHANG
			}

			if(args != NULL && strstr(args[0], "cd")) { //cd as a builtin command
				if(args[1] == NULL) {
					perror("incorrect number of arguments to cd");
				} else if(chdir(args[1]) != 0) {
					perror("changing directory failed: invalid path");	
				}
			} else if(cur != NULL && cur->datum[0] == '|') { //pipe
				int fd[2];
				if(pipe(fd) == -1) {
					perror("error creating pipe");
					exit(-1);
				}

				if((pid = fork()) == -1) {
					perror("fatal error: could not fork in pipe");
					exit(-1);
				}

				if(pid > 0) { //parent
					if(close(fd[1]) == -1) { //parent doesn't write
						perror("fatal error: closing file descriptor in parent failed");
						exit(-1);
					}
					if(dup2(fd[0], STDIN_FILENO) == -1) { //read from stdin
						perror("fatal error: dup2 failed in file redirection");
						exit(-1);
					}
					if(close(fd[0]) == -1) { //close read side fd
						perror("fatal error: closing file descriptor in parent failed");
						exit(-1);
					}
					waitpid(-1, &status, AMPERSAND);
				} else { //child
						if(close(fd[0]) == -1) { //child doesn't read
							perror("fatal error: closing file descriptor in child failed");
							exit(-1);
						}
						if(dup2(fd[1], STDOUT_FILENO) == -1) { //write to stdout
							perror("fatal error: dup2 failed in file redirection");
							exit(-1);
						}
						if(close(fd[1]) == -1) { //close write side fd
							perror("fatal error: closing file descriptor in child failed");
							exit(-1);
						}

					execvp(args[0], args);
					perror("");
					exit(-1);
				}

			} else if (cur != NULL && cur->datum[0] == '>') { //redirect output

				if((pid = fork()) == -1) 
					perror("could not fork in output redirecton");

				if(pid == 0) { //child

					char* arg = cur->prev->datum;
					size_t fd;
					if(cur->next->datum[0] == '>') { //handle >> command
						cur = cur->next; //advance the args list past the second >
						if((fd = open(cur->next->datum, O_RDWR | O_CREAT | O_APPEND, 0555)) < 0)
							perror("could not open fd in input redirection");

					} else {
						if((fd = open(cur->next->datum, O_RDWR | O_CREAT, 0555)) < 0)
							perror("could not open fd in input redirection");
					}
					if(dup2(fd, STDOUT_FILENO) == -1) {
						perror("fatal error: dup2 failed in file redirection");
					}
					execlp(arg, arg, 0);
					perror("");
				} else {
					if(cur != NULL && cur->next->datum[0] == '>') 
						cur = cur->next; //advance the args list past the second >
					waitpid(-1, &status, AMPERSAND);
				} 

				//redirect file
			} else if (cur != NULL &&  cur->datum[0] == '<') {
				if((pid = fork()) == -1) 
					perror("could not fork in file redirecton");

				if(pid == 0) { //child
					char* arg = cur->prev->datum;
					size_t fd;
					if((fd = open(cur->next->datum, O_RDONLY)) < 0){
						perror("could not open fd in input redirection");
					} else {
						if(dup2(fd, STDIN_FILENO) == -1) {
							perror("fatal error: dup2 failed in file redirection");
						}
						execlp(arg, arg, NULL);
						perror("");
					}

				} else { //parent
					waitpid(-1, &status, AMPERSAND);
					for(int i = 0; i < args_size; i++) {args[i] = '\0';}
				}
			} else {
				if((pid = fork()) == -1) 
					perror("could not fork in run command");

				if(pid == 0) { //child
					execvp(args[0], args);
					perror("");
				} else {
					waitpid(-1, &status, AMPERSAND);
					for(int i = 0; i < args_size; i++) {args[i] = '\0';}
				}
			}

			if(cur != NULL)
				cur = cur->next; 
			for(int i = 0; i < args_size; i++) {args[i] = '\0';}
		} //while cur !NULL

		free(args);
		cleanup_nodes(head);
		head = NULL;
		for(int i = 0; i < bucket_size; i++) { bucket[i] = '\0';}
	} //while 1

	free(bucket);
	return 0;
} 

