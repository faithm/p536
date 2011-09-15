#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "safe_funcs.h"
#define SPACE 32
#define REGISTRY_SIZE 7

char** init_registry() {
	char **registry; 

	//initialize the registry
	if ((registry = (char **) malloc(REGISTRY_SIZE * sizeof(char*))) == NULL) {
		perror("malloc failed for registry");
	}

	for(int i = 0; i < REGISTRY_SIZE; i++) {
		registry[i] = malloc(4 * sizeof(char));
	}

	strcpy(registry[0],"<");
	strcpy(registry[1],">");
	strcpy(registry[2],">>");
	strcpy(registry[3],"&");
	strcpy(registry[4],";");
	strcpy(registry[5],"|");
	return registry;
}

int cleanup_registry(char** registry) {
	for(int i = 0; i < REGISTRY_SIZE; i++) {
		free(registry[i]);
	}
	free(registry);
}

char** parse_args(char* args) {

	char** registry = init_registry(); //MUST FREE

	char** cmds; //MUST FREE
	int cmd_size = 6;
	int cmd_pos = 0;
	int arg_size = 256;
	int arg_pos = 0;
	char *tmp_arg; //MUST FREE

	//initialize tmp_args 
	tmp_arg = safe_malloc(arg_size*sizeof(char), "error in tmp_args");

	//initialize cmds
	cmds = safe_malloc(cmd_size*sizeof(char*), "error in cmds: malloc failed");

	//iterate over the input and parse it
	char c;
	for(int i = 0; args[i] != 0; i++) {
		c = args[i];

		//expand if we need it
		if(arg_pos >= (arg_size -1)) {
			arg_size *=2;
			tmp_arg = safe_realloc(tmp_arg, arg_size*sizeof(char), "error in realloc: tmp_arg increase failed");
		}

		//separate by space or by cmds in the registry
		if(c == SPACE || memchr(registry,c, REGISTRY_SIZE) != 0) {

			//expand if we need it
			if(cmd_pos >= (cmd_size -1)) {
				cmd_size *=2;
				cmds = safe_realloc(cmds, cmd_size*sizeof(char*), "error in realloc: cmd size increase failed");
			}

			char *tmp_cmd = (char*) safe_malloc(strlen((tmp_arg)+1)*sizeof(char), "error in malloc: tmp_cmd could not be alloced"); 
			strcpy(tmp_cmd, tmp_arg);
			cmds[cmd_pos] = tmp_cmd;

			cmd_pos++;
			for(int i = 0; i < arg_size; i++) { tmp_arg[i] = '\0';} //null out the tmp_arg
			arg_pos = 0; 
		} else {
			tmp_arg[i] = c;
		}


		cleanup_registry(registry);
		free(tmp_arg);
		return cmds;
	}
}


