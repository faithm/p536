#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "parse.h"
#define TRUE 1
#define FALSE 0

int main(int argc, char** argv) {

	//initialize with prompt
	char *prompt;
	if(argc >= 2)
		prompt = argv[1];
	else
		prompt = "myshell:";

	char* input; //MUST FREE
	int input_size = 256; 
	int input_read = 0;

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
				exit(0);
			}

			//grow the buffer size if we get beyond 255 (leave space for null terminator)
			if(input_read >= (input_size -1)) {
				input_size *=2;
				if ((input = (char *) realloc(input,input_size*sizeof(char))) == NULL) {
					perror("realloc failed in input");
				}
			}

			input[input_read] = c;
			input_read++;
		}

		//exit and cleanup on exit being typed in the prompt
		if (strcmp(input, "exit") == 0) {
				free(input);
				exit(0);
		}

		input[input_read] = '\0'; //null terminate the command

		char** parsed_input = parse_args(input);
		printf("input was: %s\n", input);
		printf("parsed input was: %s\n", parsed_input);
		for(int i = 0; i < input_size; i++) { input[i] = '\0';} //null out input
		input_read = 0;

		free(parsed_input);
	}

	return 0;
} 

