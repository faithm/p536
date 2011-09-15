#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void *safe_malloc(int size, char *error) {
	void* val;
	val = malloc(size);
	if (val == NULL) 
		perror(error);
	return val;
}

void *safe_realloc(void *ptr, int increment, char* error) {
	ptr = realloc(ptr, increment);
	if (ptr == NULL) 
		perror(error);

	return ptr;
}

void *chksize(void *arr, int cur_size, int *max_size, int increment) {
	if(cur_size >= *max_size) {
		arr = safe_realloc(arr, increment, "fatal error in chksize: realloc failed");
		*max_size = increment;
	}

	return arr;
}

/*
int main(int argc, char** argv) {
	char* str;
	str = (char*) safe_malloc(10*sizeof(char), "foo");
	str = (char*) safe_realloc(str, 16*sizeof(char), "foo");
	for(int i = 0; i<16; i++) {
		str[i] = 'c';
	}
	free(str);
	return 0;
}
*/
