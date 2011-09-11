#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define TRUE 1
#define FALSE 0

int main(int argc, char** argv) {

	//initialize with prompt
	char *prompt;
	if(argc >= 2)
		prompt = argv[1];
	else
		prompt = "myshell:";

	char **cmd; //MUST FREE
	int cmd_count = 0;
	char* input; //MUST FREE
	int input_size = 256; 
	int input_read = 0;

	//initialize cmd variable
	if ((cmd = (char **) malloc((cmd_count + 1) * sizeof(char *))) == NULL) {
		perror("malloc failed for cmd");
	}

	//initialize input variable
	if ((input = (char *) malloc(input_size * sizeof(char))) == NULL) {
		perror("malloc failed for input");
	}

	//main loop for the shell
	while(TRUE) {
		printf(prompt);

		char c;
		while ((c = getchar()) != '\n') {

			//exit and cleanup on CTRL-D
			if(c == EOF) {
				free(input);
				free(cmd);
				exit(0);
			}

			//grow the buffer size if we get beyond 255 (leave space for null terminator)
			if(input_read >= (input_size -1)) {
				if ((input = (char *) realloc(input, (input_size + 1)*sizeof(char *))) == NULL) {
					perror("realloc failed in input");
				}
			}

			input[input_read] = c;
			input_read++;
		}

		if (strcmp(input, "exit") == 0) {
				free(input);
				free(cmd);
				exit(0);
		}

		input[input_read] = '\0'; //null terminate the command
		//memcpy(cmd[cmd_count],input, input_read); //copy the input to the cmd variable
		//eval_cmd(input);
		printf("input was: %s\n", input);
		for(int i = 0; i < input_size; i++) { input[i] = '\0';} //null out input
		input_read = 0;

		//todo: reset
	}

	return 0;
} 

