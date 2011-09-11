#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Example of parsing with strsep(). */

main(int argc, char *argv[]) {

	int i;
	char **cmd;
	int cmd_count = 0;
	char **cmd_tmp;

	if ((cmd = (char **)malloc((cmd_count + 1) * sizeof(char *))) == NULL) {
		perror("malloc failed");
	}

	cmd_tmp = &argv[1];

	for (i = 0; cmd[i] = strsep(cmd_tmp, ";"); cmd[i] != '\0') {
		printf("%s\n", cmd[i]);
		cmd_count++;
		i++;
		if ((cmd = (char **)realloc(cmd, (cmd_count + 1)*sizeof(char *)))
			== NULL) {
			perror("realloc failed");
		}
	}

	printf("-----\n");

	for(i=0; cmd[i] != NULL; i++) {
		printf("%d:%s\n", i, cmd[i]);
	}

}

