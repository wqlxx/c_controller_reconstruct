/*
 * cc_socket functions.
 *
 * Author: qiang wang <wqlxx@yahoo.com.cn>
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2, as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "cc_socket.h"


void
debug_wait(char *tag_file)
{
	int fd;
	while(1)
	{
		if(fd = access(tag_file, F_OK))
			sleep(10);
		else
			break;
	}
}
		

int 
cc_set_socket_fd(void)
{
	int fd;
	fd = socket(AF_INET,SOCK_STREAM,0);
	return fd;
}


int
cc_set_socket_nonblocking(int fd)
{
    int flags;
    int ret;
    if ((flags = fcntl(fd, F_GETFL, NULL)) < 0) {
        return CC_E_ERR;
    }
    if ((ret = fcntl(fd, F_SETFL, flags | O_NONBLOCK)) < 0) {
        return CC_E_ERR;
    }
    return CC_E_NONE;
}


int
cc_set_socket_nodelay(int fd)
{
	int flag = 1;
  	int ret = setsockopt( fd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof( flag ) );
  	if ( ret < 0 ) {
    	//error( "Failed to set socket options ( fd = %d, ret = %d, errno = %s [%d] ).",fd, ret, strerror( errno ), errno );
    	return CC_E_ERR;
 	}
	return CC_E_NONE;
}

int 
cc_set_recvbuf(int fd,size_t size)
{
	return setsockopt(fd,SOL_SOCKET,SO_RCVBUF,&size,sizeof(size));
}

int 
cc_server_conn_create( cc_socket *cc_socket)
{
	
	if((cc_socket->fd= cc_set_socket_fd()) < 0)
	{
		printf("|ERR|socket create failed\n");
		return 	CC_E_ERR;
	}

	memset(&(cc_socket->cc_addr),0,sizeof(struct sockaddr_in));
	cc_socket->cc_addr.sin_family = AF_INET;
	cc_socket->cc_addr.sin_addr.s_addr = inet_addr("127.0.0.1");//get_local_ip_main();
	cc_socket->cc_addr.sin_port = CC_LISTEN_PORT;
	
	int flag = 1;
	int ret;
	
	ret = setsockopt(cc_socket->fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof( flag ));
	if ( ret < 0 ) 
	{
    		printf("|ERR|socket create failed\n");
    		return CC_E_ERR;
  	}

	//ret = setsockopt(cc_socket->fd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof( flag ));
	ret = cc_set_socket_nodelay(cc_socket->fd);
	if ( ret < 0 ) 
	{
    		printf("|ERR|socket create failed\n");
    		return CC_E_ERR;
  	}

	ret = cc_set_socket_nonblocking(cc_socket->fd);
	if ( ret < 0 ) 
	{
    		printf("|ERR|socket create failed\n");
    		return CC_E_ERR;
  	}

	ret = bind(cc_socket->fd,(struct sockaddr*)&cc_socket->cc_addr,sizeof(struct sockaddr_in));
	if(ret < 0)
	{
    		printf("|ERR|socket create failed\n");
		close( cc_socket->fd);
    		return CC_E_ERR;
  	}

	if(listen(cc_socket->fd,CC_LENGTH_OF_LISTEN_QUEUE))
	{
    		printf("|ERR|socket create failed\n");
		close( cc_socket->fd );
    		return CC_E_ERR;
  	}

	return cc_socket->fd;
}


int
cc_client_socket_create(char *server_ip, uint16_t port)
{
    struct sockaddr_in sin;
    int                fd;
    int                one = 1;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        return fd;
    }

    sin.sin_family = AF_INET; 
    sin.sin_port = htons(port);
    if (!inet_aton(server_ip, &sin.sin_addr)) {
        return -1;
    }

	
    //cc_set_socket_nonblocking();
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));    

    memset(sin.sin_zero, 0, sizeof sin.sin_zero);
    if (connect(fd, (struct sockaddr *)&sin, sizeof(struct sockaddr)) == -1) {
        perror("connect");
        return -1;
    }

    return fd;
}


int 
cc_close_socket( cc_socket *cc_socket)
{
	log_info_for_cc("close listen fd!");
	close(cc_socket->fd);
	free(cc_socket);
	return CC_E_NONE;
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
cc_socket_read_nonblock_loop()
*/

int
cc_conn_init(cc_socket* cc_socket)
{
	int ret;
	
	if((ret = (cc_server_conn_create(cc_socket)))<0)
		return CC_E_ERR;
	else	
		return CC_E_NONE;
	
}

int
cc_init_listenfd(cc_socket* cc_socket_)
{
	int ret;

//	cc_socket_ = (cc_socket*)malloc(sizeof(cc_socket));
	
	ret = cc_conn_init(cc_socket_);
	if( ret < 0 )
		return CC_E_ERR;
	else
		return CC_E_NONE;
}


int 
cc_conn_accept(cc_socket* cc_socket_)
{
	struct sockaddr_in switch_addr;
	socklen_t addr_len;
	pid_t pid;
	int accept_fd;
	int ret;
	socklen_t *len;

	addr_len = sizeof(struct sockaddr_in);
	accept_fd = accept(cc_socket_->fd,(struct sockaddr*)&switch_addr,&addr_len);
	if(accept_fd < 0)
	{
		printf("|ERR|accept failed\n");
		close(accept_fd);
		return CC_E_ERR;
	}else if( accept_fd > FD_SETSIZE ) {
		close(accept_fd);
	} else {
		cc_set_socket_nonblocking(accept_fd);
		cc_set_recvbuf(accept_fd,CC_MAX_SOCKET_BUFF);
		cc_set_socket_nodelay(accept_fd);

		pid = fork();
		if(pid < 0)
		{
			//TODO: close the listen socket
			//printf("|ERR|fork failed\n");
			perror("create child process failed!");
			log_err_for_cc("create child process failed!");
			close(accept_fd);
			return CC_E_ERR;
		}

		if(pid == 0)
		{
			//debug_wait("/home/ovs/debug");
			sw_info *cc_sw_info;
			cc_sw_info = (sw_info*)malloc(sizeof(sw_info));;
			cc_init_sw_info(cc_sw_info);

			cc_sw_info->cc_switch.pid = getpid();
			/*here we can add a function to build
		 	*a file to restore the cc_sw_info with 
			 *a special name, such as "sw_$pid.txt".
		 	*then main loop can throught search these files
		 	*to make a list which can be used to build a virtual network manager
	     		*/
			cc_sw_info->cc_switch.cc_socket.fd = accept_fd;
			cc_sw_info->cc_switch.cc_socket.cc_addr = switch_addr;
			//close( cc_socket_->fd );

			if( accept_fd < CC_ACCEPT_FD)
			{
				dup2(accept_fd, CC_ACCEPT_FD);//avoid the fd is smaller than 3,0 is for standard input, is for standard output 2 is for standard error
				close(accept_fd);
				accept_fd = CC_ACCEPT_FD;
			}

			struct timeval timeout;
			fd_set writefds;
			fd_set readfds;
			//pool_init(cc_sw_info->cc_recv_thread_pool, CC_MAX_THREAD_NUM);
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
					if(FD_ISSET(accept_fd,&readfds))
						ret = cc_of_handler_recv_event(cc_sw_info);
						if(ret < 0)
							break;
						//cc_recv_from_secure_channel(cc_sw_info);
					if(FD_ISSET(accept_fd,&writefds))
						ret = cc_of_handler_send_event(cc_sw_info);
						if( ret < 0 )
							break;
						//cc_flush_to_secure_channel(cc_sw_info);
				}
			}
			/*modified by wangq 20130414*/
			cc_finalize_sw_info(cc_sw_info);
			return CC_E_NONE;
			/*may be we should throw a signal to parent to delete the
			*the record of this switch 
			*/
		}else{
			/* this is parent*/
			//cc_sw_info->cc_switch->pid = pid;
			close(accept_fd);
			return CC_E_NONE;
		}
	}
	close(accept_fd);
	return CC_E_NONE;
}

	

