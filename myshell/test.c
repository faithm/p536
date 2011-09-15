#include <string.h>
#include <stdio.h>

int main() {

	char c = '|';
	char str[] = "a |>> b> ; c |d ;";
	char* token;
	token = strstr(str,">");
		printf("%d\n", token-str);
	return 0;
}
