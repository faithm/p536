#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "safe_funcs.h"
#define TRUE 1
#define FALSE 0
#define SPACE 32
#define REGISTRY_SIZE 7

char** init_registry() {
	char **registry; 
	registry = (char **) safe_malloc(REGISTRY_SIZE * sizeof(char*), "malloc failed for registry");

	for(int i = 0; i < REGISTRY_SIZE; i++) {
		registry[i] = (char*) safe_malloc(4 * sizeof(char), "error in malloc for registry elements");
	}

	strcpy(registry[0],"<");
	strcpy(registry[1],">>");
	strcpy(registry[2],">");
	strcpy(registry[3],"&");
	strcpy(registry[4],";");
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
}

//get the left most instruction
int select_instruction(char* expr, char** registry) {
	int min = -1;
	int registry_loc = -1;
	if(expr != NULL && registry != NULL) {
		char *tmp = "\0";

		for(int i =0; i < REGISTRY_SIZE; i++) {
			tmp = strstr(expr, registry[i]);

			if((tmp -expr) > 0 && (tmp - expr) < REGISTRY_SIZE) {
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

node *interp(char* expr, node* prev, char** registry) {
	node *n = safe_malloc(sizeof(node), "fatal error: node could not be created");
	node *cmd = safe_malloc(sizeof(node), "fatal error: node could not be created");
	char* instruction = registry[select_instruction(expr, registry)];
	char* tmp;
	puts(instruction);

	if((tmp = strtok(expr,instruction)) != NULL) {
		char* datum = safe_malloc(strlen(tmp)*sizeof(char)+1, "fatal error: interp could not malloc data for node");
		strcpy(datum,tmp);
		strcat(datum,"\0");

		char* new_expr = safe_malloc(strlen(tmp)*sizeof(char)+1, "fatal error: interp could not malloc data for expression");
		strncpy(new_expr,expr, tmp-expr);
		strcat(new_expr,"\0");
		free(expr);

		n->datum = datum;
		n->prev = prev;
		n->next = cmd;

		cmd->datum = instruction;
		cmd->prev = n;
		cmd->next = interp(new_expr, n, registry);

	} else {
		perror("executing instructions failed: no match found");
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
				cleanup_registry(registry);
				exit(0);
			}

			bucket = (char *) chksize(bucket, read, &bucket_size, bucket_size*2+1);
			bucket[read] = c;
			read += 1;
		}
		bucket[read] = '\0';

		printf("input: %s\n", bucket);
		interp(bucket, NULL, registry);
		for(int i = 0; i < read; i++) {
			bucket[i] = '\0';
		}
		read = 0;
		//free(bucket);
	} //while TRUE

	cleanup_registry(registry);
	return 0;
} 

