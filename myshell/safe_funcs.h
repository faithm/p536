#ifndef _safe_funcs_
#define _safe_funcs_

void *safe_malloc(int size, char *error);
void *safe_realloc(void *ptr, int increment, char* error);
void *chksize(void *arr, int cur_size, int *max_size, int increment);

#endif
