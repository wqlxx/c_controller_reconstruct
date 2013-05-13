#include "cc_thread_pool.h"
#include "cc_queue.h"
#include "cc_buffer.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <malloc.h>

char* sample_string[10] = {"hi","this","is","test"};

void *myprocess (void *arg)   
{   
     printf("threadid is 0x%x, the msg in queue is %s\n", pthread_self (),(char*)arg);   
     return NULL;   
}   

int main (int argc, char **argv)   
{   
	CThread_pool *tmp_pool;
	pool_init(&tmp_pool, 3);

	int i = 0;
	char *tmp_char;
    tmp_char = sample_string[i];
	while( i < 4 )
    {    
		pool_add_worker(tmp_pool, myprocess, (void*)sample_string[i]);
		i++;
        sleep(1);
        tmp_char = sample_string[i];
    }

    pool_destroy(tmp_pool);
    return 0;
/* test for socket and thread pool*/
#if 0
	pool_init(3);
	
	int listenfd,connfd;
	int len;
	struct sockaddr_in servaddr,cliaddr;
	char buff[BUFFER_SIZE];
	
	listenfd = socket(AF_INET,SOCK_STREAM,0);

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(6633);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	int ret;
	ret = bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr));
	if(ret < 0)
	{
		printf("bind error!\n");
		return -1;
	}
	
	ret = listen(listenfd,10);
	int str_len;
	for(;;)
	{
		len = sizeof(cliaddr);
		connfd = accept(listenfd,(struct sockaddr*)&cliaddr,&len);
		if(connfd < 0)
		{
			printf("accept error!\n");
			return -1;
		}
		
		while((str_len = read(connfd,buff,1024)) > 0)
		{
			printf("recv:%s\n",buff);
			switch(buff){
				case 
			pool_add_worker(myprocess,buff);
			printf("back\n");
		}
	}
 	sleep(5);
	pool_destory();
	close(listenfd);
	return 0;  
#endif
}  

