#if 0
#include <stdio.h>
#include <stdio.h>
#include <sys/select.h>
#include <unistd.h>
#include <stdint.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <string.h>
#include <error.h>
#include <fcntl.h>
#include <errno.h>
#include <malloc.h>

#define CC_LISTEN_PORT 6633
#define CC_LENGTH_OF_LISTEN_QUEUE 20
#define CC_ACCEPT_FD 3
#define CC_CHILD 1
#define CC_PARENT 0
#define CC_CONN_TIMEOUT_SEC 0
#define CC_CONN_TIMEOUT_USEC 0
#define CC_MAX_SOCKET_BUFF 1600
#define CC_MAX_PORT 52
#define CC_MAX_THREAD_NUM 3
 
 
struct cc_socket_t{
    struct sockaddr_in addr;
    //uint16_t port;
    int fd;
     
};
typedef struct cc_socket_t cc_socket;
 
struct each_sw_t{
    pid_t pid;
    uint64_t dpid;//datapath_id come from switch_feature_request/reply
    cc_socket cc_socket;
    //each_sw *next;
    char* profile_path;
    uint8_t version;
    uint32_t n_buffers;
    uint8_t n_tables;
    uint32_t actions;
    uint32_t capabilities;
    //port_info* port_head;
    //port_info sw_port[CC_MAX_PORT];
};
typedef struct each_sw_t each_sw;


int 
set_socket_fd(void)
{
	int fd;
	fd = socket(AF_INET,SOCK_STREAM,0);
	return fd;
}


int
set_socket_nonblocking(int fd)
{
    int flags;
    int ret;
    if ((flags = fcntl(fd, F_GETFL, NULL)) < 0) {
        return -1;
    }
    if ((ret = fcntl(fd, F_SETFL, flags | O_NONBLOCK)) < 0) {
        return -1;
    }
    return 0;
}


int
set_socket_nodelay(int fd)
{
	int flag = 1;
  	int ret = setsockopt( fd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof( flag ) );
  	if ( ret < 0 ) {
    	//error( "Failed to set socket options ( fd = %d, ret = %d, errno = %s [%d] ).",fd, ret, strerror( errno ), errno );
    	return -1;
 	}
	return 0;
}


int
set_socket_reuseaddr(int fd)
{
	int ret;
	int flag = 1; // why it is 1?
    
    ret = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
    if(ret < 0){
        return -1;
	}
    return 0;

}

int 
set_recvbuf(int fd, size_t size)
{
	int ret;
	ret = setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));
    if(ret < 0){
         return -1;
	}
    return 0;       
}


int 
server_conn_create(cc_socket *socket)
{
	
	if((socket->fd= set_socket_fd()) < 0)
	{
		printf("|ERR|socket create failed\n");
		return 	-1;
	}

	memset(&(socket->addr), 0, sizeof(struct sockaddr_in));
	socket->addr.sin_family = AF_INET;
	socket->addr.sin_addr.s_addr = INADDR_ANY;//get_local_ip_main();
	socket->addr.sin_port = htons(CC_LISTEN_PORT);// don't forget to add htons, 
														//if not the poet will be in host_order
	
	int flag = 1;
	int ret;
	
	ret = set_socket_reuseaddr(socket->fd);
	if ( ret < 0 ) 
	{
    		perror("|ERR|socket create failed\n");
    		return -1;
  	}

	ret = set_socket_nodelay(socket->fd);
	if ( ret < 0 ) 
	{
    		perror("|ERR|socket create failed\n");
    		return -1;
  	}

	ret = set_socket_nonblocking(socket->fd);
	if ( ret < 0 ) 
	{
    		perror("|ERR|socket create failed\n");
    		return -1;
  	}

	ret = bind(socket->fd,(struct sockaddr*)&socket->addr,sizeof(struct sockaddr_in));
	if(ret < 0)
	{
    		perror("|ERR|socket create failed\n");
			close( socket->fd);
    		return -1;
  	}

	if(listen(socket->fd,CC_LENGTH_OF_LISTEN_QUEUE))
	{
    		perror("|ERR|socket create failed\n");
			close( socket->fd );
    		return -1;
  	}

	return socket->fd;
}


int 
close_socket(cc_socket *socket)
{
	close(socket->fd);
	free(socket);
	return 0;
}


/*hello,echo_request,need get the reply in a given time*/
/*
 void
switch_set_timeout( long sec, timer_callback callback, void *user_data ) {
  struct itimerspec interval;

  interval.it_value.tv_sec = sec;
  interval.it_value.tv_nsec = 0;
  interval.it_interval.tv_sec = 0;
  interval.it_interval.tv_nsec = 0;
  add_timer_event_callback( &interval, callback, user_data );
  switch_info.running_timer = true;
}
*/
/*int
socket_read_nonblock_loop()
*/


int
conn_init(cc_socket* socket)
{
	int ret;
	
	if((ret = (server_conn_create(socket)))<0)
		return -1;
	else	
		return 0;
	
}

int
init_listenfd(cc_socket* socket)
{
	int ret;

//	socket_ = (socket*)malloc(sizeof(socket));
	
	ret = conn_init(socket);
	if( ret < 0 )
		return -1;
	else
		return 0;
}


/*
 *func:init_of_socket can be used to init listenfd and connfd
 *param@ socket_ is define in basic.h
 */
int
init_of_socket(cc_socket* socket)
{
	int ret;

	ret = init_listenfd(socket);
	if(ret < 0)
	{
		perror("create sock error!");
		return -1;
	}
	return 0;
}


int
finalize_of_socket(int fd)
{
	close(fd);
	return 0;
}


int 
conn_accept(cc_socket* socket)
{
	struct sockaddr_in switch_addr;
	socklen_t addr_len;
	pid_t pid;
	int accept_fd;
	int ret;
	socklen_t *len;
    char *data;

	addr_len = sizeof(struct sockaddr_in);
	accept_fd = accept(socket->fd,(struct sockaddr*)&switch_addr,&addr_len);
    printf("accept_fd is %d\n", accept_fd);
	if(accept_fd < 0)
	{
		perror("|ERR|accept failed\n");
		close(accept_fd);
		return -1;
	}else if( accept_fd > FD_SETSIZE ) {
		close(accept_fd);
	} else {
		set_socket_nonblocking(accept_fd);
		set_recvbuf(accept_fd,CC_MAX_SOCKET_BUFF);
		set_socket_nodelay(accept_fd);

		pid = fork();
		if(pid < 0)
		{
			//TODO: close the listen socket
			//printf("|ERR|fork failed\n");
			perror("create child process failed!");
			perror("create child process failed!");
			close(accept_fd);
			return -1;
		}

		if(pid == 0)
		{
			close(socket->fd);
			//close( socket_->fd );
			printf("this is child\n");
			if( accept_fd < CC_ACCEPT_FD)
			{
				dup2(accept_fd, CC_ACCEPT_FD);//avoid the fd is smaller than 3,0 is for standard input, is for standard output 2 is for standard error
				close(accept_fd);
				accept_fd = CC_ACCEPT_FD;
			}

			struct timeval timeout;
			fd_set writefds;
			fd_set readfds;
			//pool_init(sw_info->recv_thread_pool, CC_MAX_THREAD_NUM);
			while(1)
			{
				FD_ZERO(&readfds);
				FD_ZERO(&writefds);
				FD_SET(accept_fd,&readfds);
				FD_SET(accept_fd,&writefds);
				timeout.tv_sec = CC_CONN_TIMEOUT_SEC;
				timeout.tv_usec = CC_CONN_TIMEOUT_USEC;
				ret = select(accept_fd+1, &readfds, &writefds, NULL, &timeout);
				if( ret == -1 )
				{
					if( errno == EINTR )				
						continue;
					else
						break;
				}else if( ret == 0 ){
					continue;
				}else{
					printf("in select\n");
					if(FD_ISSET(accept_fd,&readfds)){
                        printf("in FD_ISSET\n");
                        data = malloc(sizeof(char)*1600);
                    	while((ret = read(accept_fd, data, 1600)) > 0)
                        {
                        	printf("the recv data is %s, len is %d\n", data, ret);
                            memset(data, 0, sizeof(char)*1600);
                        }
                        if( ret < 0)
						//recv_from_secure_channel(sw_info);
							break;
                        data = NULL;
					}
				}
			}
			/*modified by wangq 20130414*/
			return 0;
			/*may be we should throw a signal to parent to delete the
			*the record of this switch 
			*/
		}
        if(pid > 0){
			/* this is parent*/
			//sw_info->switch->pid = pid;
			close(accept_fd);
			return 0;
		}
	}

	return 0;
}


int
polling(cc_socket* socket)
{
	int ret;
	fd_set listen_fdset;
	int max_fd = socket->fd + 1;

	FD_ZERO(&listen_fdset);
	FD_SET(socket->fd, &listen_fdset);
	while(1)
	{
		
		FD_ZERO(&listen_fdset);
		FD_SET(socket->fd,&listen_fdset);
		ret = select(max_fd,&listen_fdset,NULL,NULL,0);
		if( ret == -1 )
		{
			if( errno == EINTR )				
				continue;
			else
				return -1;
		}else if( ret == 0 ){
			continue;
		}else{
			if(FD_ISSET(socket->fd, &listen_fdset))
			{
				ret = conn_accept(socket);
				if( ret < 0 ){
					perror("accept failed!");
					return -1;
				}
			}
		}
	}
	return 0;
}


void
sig_child_exit(int signo)
{
	pid_t pid;
    int stat;

    while((pid = waitpid(-1, &stat, WNOHANG)) > 0)
        printf("child %d terminated\n", pid);

    return;
}


int
main(int argc, char **argv)
{

	signal(SIGCHLD, sig_child_exit);
	signal(SIGPIPE, SIG_IGN);
    
	cc_socket* listen_socket;
	listen_socket = malloc(sizeof(cc_socket));

	init_of_socket(listen_socket);

	polling(listen_socket);

	finalize_of_socket(listen_socket->fd);

	return 0;
}
#endif
