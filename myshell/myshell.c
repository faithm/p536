#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "safe_funcs.h"
#include <ctype.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#define TRUE 1
#define FALSE 0
#define SPACE 32

extern void connect_pipe(char* ps1, char* ps2);

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
	int bucket_read = 0;
	int bucket_size = 256;
	node *head = NULL;
	node * cur = NULL;

	//initialize with prompt
	char *prompt;
	if(argc >= 2)
		prompt = argv[1];
	else
		prompt = "myshell:";

	while(TRUE) {
		printf("%s", prompt);
		bucket = (char *) safe_malloc(bucket_size*sizeof(char), "fatal error in main: bucket could not be malloced");

		char c;
		while((c = getchar()) != '\n') {
			if(c == EOF) {
				cleanup_nodes(head);
				free(bucket);
				exit(0);
			}

			if(isalnum(c)) {
				bucket = (char *) chksize(bucket, bucket_read, &bucket_size, bucket_size*2+1);
				bucket[bucket_read] = c;
				bucket_read += 1;
			} else if(c == ';') {

				if(bucket_read >= 1) {
					node *s = (node *) safe_malloc(sizeof(node), "error creating node");
					s->datum = (char *) safe_malloc(bucket_read*sizeof(char)+1, "could not malloc datum in node");
					strncpy(s->datum, bucket, bucket_read);
					if(cur != NULL)
						cur->next = s;
					bucket_read = 0;
					s->prev = cur;
					s->next = NULL;
					cur = s;
					if(head == NULL)
						head = cur;
				} 

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
			} else if(c == '|') {

				if(bucket_read >= 1) {
					node *s = (node *) safe_malloc(sizeof(node), "error creating node");
					s->datum = (char *) safe_malloc(bucket_read*sizeof(char)+1, "could not malloc datum in node");
					strncpy(s->datum, bucket, bucket_read);
					if(cur != NULL)
						cur->next = s;
					bucket_read = 0;
					s->prev = cur;
					s->next = NULL;
					cur = s;
					if(head == NULL)
						head = cur;
				} 

				node *n = safe_malloc(sizeof(node), "error creating node");
				if(cur != NULL)
					cur->next = n;
				n->datum = (char *) safe_malloc(2*sizeof(char), "could not malloc datum in node");
				n->datum[0] = '|';
				n->datum[1] = '\0';
				n->prev = cur;
				n->next = NULL;
				cur = n;
				if(head == NULL)
					head = cur;
			} else if(c == '&') {

				if(bucket_read >= 1) {
					node *s = (node *) safe_malloc(sizeof(node), "error creating node");
					s->datum = (char *) safe_malloc(bucket_read*sizeof(char)+1, "could not malloc datum in node");
					strncpy(s->datum, bucket, bucket_read);
					if(cur != NULL)
						cur->next = s;
					bucket_read = 0;
					s->prev = cur;
					s->next = NULL;
					cur = s;
					if(head == NULL)
						head = cur;
				} 

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
			} else if(c == '>') {

				if(bucket_read >= 1) {
					node *s = (node *) safe_malloc(sizeof(node), "error creating node");
					s->datum = (char *) safe_malloc(bucket_read*sizeof(char)+1, "could not malloc datum in node");
					strncpy(s->datum, bucket, bucket_read);
					if(cur != NULL)
						cur->next = s;
					bucket_read = 0;
					s->prev = cur;
					s->next = NULL;
					cur = s;
					if(head == NULL)
						head = cur;
				} 

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
			} else if(c == '<') {

				if(bucket_read >= 1) {
					node *s = (node *) safe_malloc(sizeof(node), "error creating node");
					s->datum = (char *) safe_malloc(bucket_read*sizeof(char)+1, "could not malloc datum in node");
					strncpy(s->datum, bucket, bucket_read);
					if(cur != NULL)
						cur->next = s;
					bucket_read = 0;
					s->prev = cur;
					s->next = NULL;
					cur = s;
					if(head == NULL)
						head = cur;
				} 

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

		if(bucket_read >= 1) {
			node *s = (node *) safe_malloc(sizeof(node), "error creating node");
			s->datum = (char *) safe_malloc(bucket_read*sizeof(char)+1, "could not malloc datum in node");
			strncpy(s->datum, bucket, bucket_read);
			bucket_read = 0;
			if(cur != NULL)
				cur->next = s;
			s->prev = cur;
			s->next = NULL;
			cur = s;
		}

		if(head == NULL)
			head = cur;

		/*
			 cur=head;
			 while(cur != NULL && cur->datum != NULL) {
			 printf("%s ", cur->datum);
			 cur = cur->next; 
			 }
			 printf("\n");
			 */


		cur=head;
		int pid, status;
		while(cur != NULL) {
			//pipe
			if(cur->datum[0] == '|') {
				int fd[2], pid;
				char* ps1 = cur->prev->datum;
				char* ps2 = cur->next->datum;

				if(ps1 != NULL && ps2 != NULL) {
					int fd[2];
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

				}
				//redirect inside
			} else if (cur->datum[0] == '>') {

				if((pid = fork()) == -1) 
					perror("could not fork in connect_pipe");


				if(pid == 0) { //child
					char* arg = cur->prev->datum;
					int fd1 = open(cur->prev->datum, O_RDONLY);
					int fd2;

					//handle >> command
					if(cur->next->datum[0] = '>') {
						cur = cur->next; //advance the args list past the second >
						fd2 = open(cur->next->datum, O_RDWR | O_CREAT | O_APPEND);
					} else {
						fd2 = open(cur->next->datum, O_RDWR | O_CREAT);
					}

					int buf_size = 32;
					char buff[buf_size];
					int bytes_read;
					dup2(fd1, STDOUT_FILENO);
					close(fd1);
					dup2(fd2, STDIN_FILENO);
					close(fd2);
					while((bytes_read = read(STDIN_FILENO, buff, buf_size)) > 0){
						write(STDOUT_FILENO, buff, buf_size);
					}
					char *params[] = {arg, (char *) 0};
					execve(arg, params, 0);
					perror("");
				} else {
					waitpid(-1, &status, 0);
				} 

			} else {

			}

			cur = cur->next;
		}
		cleanup_nodes(head);
		head = NULL;
	} //while TRUE

	//cleanup_nodes(head);
	free(bucket);
	return 0;
} 

