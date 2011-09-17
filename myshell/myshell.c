#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "safe_funcs.h"
#include <ctype.h>
#include <sys/wait.h>
#include <unistd.h>
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
	int read = 0;
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
				bucket = (char *) chksize(bucket, read, &bucket_size, bucket_size*2+1);
				bucket[read] = c;
				read += 1;
			} else if(c == ';') {

				if(read >= 1) {
					node *s = (node *) safe_malloc(sizeof(node), "error creating node");
					s->datum = (char *) safe_malloc(read*sizeof(char)+1, "could not malloc datum in node");
					strncpy(s->datum, bucket, read);
					if(cur != NULL)
						cur->next = s;
					read = 0;
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

				if(read >= 1) {
					node *s = (node *) safe_malloc(sizeof(node), "error creating node");
					s->datum = (char *) safe_malloc(read*sizeof(char)+1, "could not malloc datum in node");
					strncpy(s->datum, bucket, read);
					if(cur != NULL)
						cur->next = s;
					read = 0;
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

				if(read >= 1) {
					node *s = (node *) safe_malloc(sizeof(node), "error creating node");
					s->datum = (char *) safe_malloc(read*sizeof(char)+1, "could not malloc datum in node");
					strncpy(s->datum, bucket, read);
					if(cur != NULL)
						cur->next = s;
					read = 0;
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

				if(read >= 1) {
					node *s = (node *) safe_malloc(sizeof(node), "error creating node");
					s->datum = (char *) safe_malloc(read*sizeof(char)+1, "could not malloc datum in node");
					strncpy(s->datum, bucket, read);
					if(cur != NULL)
						cur->next = s;
					read = 0;
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

				if(read >= 1) {
					node *s = (node *) safe_malloc(sizeof(node), "error creating node");
					s->datum = (char *) safe_malloc(read*sizeof(char)+1, "could not malloc datum in node");
					strncpy(s->datum, bucket, read);
					if(cur != NULL)
						cur->next = s;
					read = 0;
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

		if(read >= 1) {
			node *s = (node *) safe_malloc(sizeof(node), "error creating node");
			s->datum = (char *) safe_malloc(read*sizeof(char)+1, "could not malloc datum in node");
			strncpy(s->datum, bucket, read);
			read = 0;
			if(cur != NULL)
				cur->next = s;
			s->prev = cur;
			s->next = NULL;
			cur = s;
		}

		if(head == NULL)
			head = cur;

		cur=head;
		while(cur != NULL && cur->datum != NULL) {
			printf("%s ", cur->datum);
			cur = cur->next; 
		}
		printf("\n");


		cur=head;
		while(cur != NULL) {
			if(cur->datum[0] == '|') {
				int pid, status;
				if(pid = fork() == -1) 
					perror("fork failed in main");
				if(pid != 0) {
					waitpid(-1, &status, 0);
				} else {
					connect_pipe(cur->prev->datum, cur->next->datum);
				}
			}
		}
		cleanup_nodes(head);
		head = NULL;
	} //while TRUE

	//cleanup_nodes(head);
	free(bucket);
	return 0;
} 

