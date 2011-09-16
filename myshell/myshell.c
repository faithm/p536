#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "safe_funcs.h"
#define TRUE 1
#define FALSE 0
#define SPACE 32
#define REGISTRY_SIZE 6

char** init_registry() {
	char **registry; 
	registry = (char **) safe_malloc(REGISTRY_SIZE * sizeof(char*), "malloc failed for registry");

	for(int i = 0; i < REGISTRY_SIZE; i++) {
		registry[i] = (char*) safe_malloc(4 * sizeof(char), "error in malloc for registry elements");
	}

	strcpy(registry[0],"<");
	strcat(registry[0], "\0");

	strcpy(registry[1]," ");
	strcat(registry[1], "\0");

	strcpy(registry[2],">");
	strcat(registry[2], "\0");

	strcpy(registry[3],"&");
	strcat(registry[3], "\0");

	strcpy(registry[4],";");
	strcat(registry[4], "\0");

	strcpy(registry[5],"|");
	strcat(registry[5], "\0");
	return registry;
}

typedef struct node {
	char* datum;
	struct node* prev;
	struct node* next;
} node;

int cleanup_registry(char** registry) {
	for(int i = 0; i < REGISTRY_SIZE; i++) {
		free(registry[i]);
	}
	free(registry);
	return 0;
}

int cleanup_nodes(node *n) {
	if(n != NULL) {
		free(n->datum);
		cleanup_nodes(n->next);
	}
	free(n);
	return 0;
}

//get the left most instruction
int select_instruction(char* expr, char** registry) {
	int min = -1;
	int registry_loc = -1;
	if(expr != NULL && registry != NULL) {
		char *tmp = "\0";

		for(int i =0; i < REGISTRY_SIZE; i++) {
			tmp = strstr(expr, registry[i]);

			if((tmp - expr) >= 0 && (tmp - expr) < REGISTRY_SIZE) {
				if(min == -1) {
					min = tmp - expr;
					registry_loc = i;
				}	else if((tmp - expr) < min) {
					min = tmp - expr;
					registry_loc = i;
				}
			}
		}

	} else {
		perror("fatal error in select instructions: NULL values passed to parser");
	}

	return registry_loc;
}

node *interp(char* expr, node *prev,  node* n, char** registry) {
	node *cmd = safe_malloc(sizeof(node), "fatal error: node could not be created");//freed in cleanup_nodes
	int registry_loc = select_instruction(expr, registry);
	char *instruction;
	if(registry_loc != -1){
		instruction = registry[registry_loc];
		puts(instruction);
	}
	//freed in cleanup_nodes as datum
	char *cpy = safe_malloc(strlen(expr)*sizeof(char)+1, "fatal error: interp could not malloc data for expression");
	strcpy(cpy, expr);

	if(registry_loc != -1 && (strtok(cpy,instruction)) != NULL) {
		n->datum = safe_malloc(strlen(cpy)*sizeof(char)+1, "fatal error: interp could not malloc data for node");
		strcpy(n->datum,cpy);

		int cpylen = strlen(cpy);
		char* new_expr = safe_malloc(((strlen(expr)+1)-cpylen)*sizeof(char)+1, "fatal error: interp could not malloc data for expression");
		strcpy(new_expr,expr+cpylen+1);
		free(expr);

		n->prev = safe_malloc(sizeof(node), "fatal error: malloc could not allocate memory for node");
		n->next = safe_malloc(sizeof(node), "fatal error: malloc could not allocate memory for node");
		n->prev = prev;
		n->next = cmd;

		cmd->datum = safe_malloc(4*sizeof(char), "fatal error: malloc could not allocate memory for node");
		cmd->datum = instruction;

		cmd->prev = safe_malloc(sizeof(node), "fatal error: malloc could not allocate memory for node");
		cmd->next = safe_malloc(sizeof(node), "fatal error: malloc could not allocate memory for node");
		cmd->prev = n;
		node *new_node = safe_malloc(sizeof(node), "fatal error: node could not be created");
		cmd->next = interp(new_expr, cmd, new_node, registry);

	} else if (expr != NULL) {
		n->datum = expr;
		n->next = NULL;
		n->prev = prev;
	} else {
		perror("Error: no matching command found on input");
	}

	return n;
}

int main(int argc, char** argv) {

	char** registry = init_registry();
	char* bucket;
	int read = 0;
	int bucket_size = 256;
	char** cmd;
	//initialize with prompt
	char *prompt;
	node *n = safe_malloc(sizeof(node), "fatal error: node could not be created");
	node *head = n;
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
				free(bucket);
				cleanup_nodes(head);
				cleanup_registry(registry);
				exit(0);
			}

			bucket = (char *) chksize(bucket, read, &bucket_size, bucket_size*2+1);
			bucket[read] = c;
			read += 1;
		}
		bucket[read] = '\0';

		printf("input: %s\n", bucket);
		n =	interp(bucket, NULL, n, registry);
		read = 0;

		node *tmp = head;
		printf("instruction is: ");
		while(tmp != NULL) {
			printf("(%s)", tmp->datum);
			tmp = tmp->next;
		}
		printf("\n");
	} //while TRUE

	cleanup_registry(registry);
	cleanup_nodes(head);
	return 0;
} 

