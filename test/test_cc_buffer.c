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

    srtcpy(tmp_string, "test");
    tmp->data = (void*)tmp_string;
    pritntf("the string is %s", (char*)tmp->data);
    free(tmp_string);
    free_buffer(tmp);
    printf("the allocat length is %d\n", sizeof(tmp));
    printf("**********end of test***************************\n\n");

	return 0;
}

