#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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
	if ((tmp_arg = (char *) malloc(arg_size * sizeof(char))) == NULL) {
		perror("malloc failed for arg parser");
	}

	//initialize cmds
	if ((cmds = (char **) malloc(cmd_size * sizeof(char*))) == NULL) {
		perror("malloc failed for cmd");
	}

	//iterate over the input and parse it
	char c;
	for(int i = 0; args[i] != 0; i++) {
		c = args[i];

		//expand if we need it
		if(arg_pos >= (arg_size -1)) {
			arg_size *=2;
			if ((tmp_arg = (char *) realloc(tmp_arg, arg_size*sizeof(char))) == NULL) {
				perror("realloc failed in tmp_arg");
			}
		}

		if(c > 64 && c < 123) { //don't check alphabetic chars
			tmp_arg[i] = c;

			//separate by space or by cmds in the registry
		} else if(c == SPACE || memchr(registry,c, REGISTRY_SIZE) != 0) {

			//expand if we need it
			if(cmd_pos >= (cmd_size -1)) {
				cmd_size *=2;
				if ((cmds = (char **) realloc(cmds, cmd_size*sizeof(char*))) == NULL) {
					perror("realloc failed in cmds");
				}
			}

			//TODO: fix this part
			cmds[cmd_pos] = tmp_arg; //probably have to memcpy...
			cmd_pos++;
			for(int i = 0; i < arg_size; i++) { tmp_arg[i] = '\0';} //null out the tmp_arg
			arg_pos = 0; 
		}

		cleanup_registry(registry);
		free(tmp_arg);
		return cmds;
	}
}


