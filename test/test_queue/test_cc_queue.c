#include <stdio.h>
#include <pthread.h>
#include <malloc.h>

#include "cc_queue.h"
#include "cc_buffer.h"

char* sample_string[10] = {"hi","this","is","test"};

int
main(int argc, char **argv)
{
/*****************test 1********************************/
	message_queue *tmp_queue;
	if((tmp_queue = create_message_queue()) == NULL)
	{
		printf("create queue error!\n");
    	return -1;
    }

    int i = 1;
    buffer *tmp_buf;
    char *tmp_char;

	tmp_char = *sample_string;
    while( tmp_char != NULL )
    {
       	tmp_buf = alloc_buffer_with_length(10);
        strcpy(tmp_buf->data, tmp_char);
        printf("the insert is %s, and %d\n",(char*)tmp_buf->data,sizeof(*(tmp_buf->data)));
        enqueue_message(tmp_queue, tmp_buf);
		tmp_char = sample_string[i];
        if(tmp_char == NULL)
            break;
        i++;
    }  

    int len;
    buffer *out;
    for( len = tmp_queue->length; len > 0; len-- )
    {
    	out = dequeue_message(tmp_queue);
        if(out->data == NULL)
            break;
        printf("%s\n", out->data);
        free_buffer(out);
  	}

    delete_message_queue(tmp_queue);
/*****************test 2********************************/

	message_queue *tmp_queue_;
   	if((tmp_queue_ = create_message_queue()) == NULL)
	{
		printf("create queue error!\n");
    	return -1;
    }
    int j = 1;
    buffer *tmp_buf_;
    char *tmp_char_;    
    
	tmp_char_ = *sample_string;
    while( tmp_char_ != NULL )
    {
       	tmp_buf_ = alloc_buffer_with_length(10);
        strcpy(tmp_buf_->data, tmp_char_);
        printf("the insert is %s, and %d\n",(char*)tmp_buf_->data,sizeof(*(tmp_buf_->data)));
        enqueue_message(tmp_queue_, tmp_buf_);
		tmp_char_ = sample_string[j];
        if(tmp_char_ == NULL)
            break;
        j++;
    }  

    delete_message_queue(tmp_queue_);
/*****************end of test********************************/


    return 0;
}


