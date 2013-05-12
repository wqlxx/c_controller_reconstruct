#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include "cc_buffer.h"
	
int 
main(int argc, char **argv)
{
	buffer* tmp;
	size_t tmp_len;


	printf("**********test alloc_buffer_with_length***********\n\n");
    tmp_len = 8;
    tmp = alloc_buffer_with_length(tmp_len);
	printf("the allocat length is %d\n", sizeof(tmp));
    printf("**********end of test***************************\n\n");

	printf("**********test alloc_buffer_with_length***********\n\n");
	char *tmp_string = (char*)malloc(10 * sizeof(char));
 	if( tmp_string == NULL )
        return -1;

	char* ret;
    strcpy(tmp_string, "test");
    tmp->data = (void*)tmp_string;
    printf("the string is %s\n", (char*)tmp->data);
    //free(tmp_string);
	ret = tmp->data;
	if( ret == NULL )
		printf("ret is NULL \n");
    free_buffer(tmp);
	if( ret == NULL)
		printf("success free data\n");
    printf("the allocat length is %d,ret is %s\n", sizeof(tmp),ret);
    printf("**********end of test***************************\n\n");

	return 0;
}

