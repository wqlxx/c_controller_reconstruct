/*
 * cc_init_of functions.
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

#include "cc_init_of.h"


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
cc_set_socket_reuseaddr(int fd)
{
	int ret;
	int flag = 1; // why it is 1?
    
    ret = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
    if(ret < 0){
        return CC_E_ERR;
	}
    return CC_E_NONE;

}

int 
cc_set_recvbuf(int fd, size_t size)
{
	int ret;
	ret = setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));
    if(ret < 0){
         return CC_E_ERR;
	}
    return CC_E_NONE;       
}


int 
cc_server_conn_create(cc_socket *cc_socket)
{
	
	if((cc_socket->fd= cc_set_socket_fd()) < 0)
	{
		printf("|ERR|socket create failed\n");
		return 	CC_E_ERR;
	}

	memset(&(cc_socket->cc_addr), 0, sizeof(struct sockaddr_in));
	cc_socket->cc_addr.sin_family = AF_INET;
	cc_socket->cc_addr.sin_addr.s_addr = INADDR_ANY;//get_local_ip_main();
	cc_socket->cc_addr.sin_port = htons(CC_LISTEN_PORT);// don't forget to add htons, 
														//if not the poet will be in host_order
	
	int flag = 1;
	int ret;
	
	ret = cc_set_socket_reuseaddr(cc_socket->fd);
	if ( ret < 0 ) 
	{
    		log_err_for_cc("|ERR|socket create failed\n");
    		return CC_E_ERR;
  	}

	ret = cc_set_socket_nodelay(cc_socket->fd);
	if ( ret < 0 ) 
	{
    		log_err_for_cc("|ERR|socket create failed\n");
    		return CC_E_ERR;
  	}

	ret = cc_set_socket_nonblocking(cc_socket->fd);
	if ( ret < 0 ) 
	{
    		log_err_for_cc("|ERR|socket create failed\n");
    		return CC_E_ERR;
  	}

	ret = bind(cc_socket->fd,(struct sockaddr*)&cc_socket->cc_addr,sizeof(struct sockaddr_in));
	if(ret < 0)
	{
    		log_err_for_cc("|ERR|socket create failed\n");
			close( cc_socket->fd);
    		return CC_E_ERR;
  	}

	if(listen(cc_socket->fd,CC_LENGTH_OF_LISTEN_QUEUE))
	{
    		log_err_for_cc("|ERR|socket create failed\n");
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
        log_err_for_cc("socket");
        return fd;
    }

    sin.sin_family = AF_INET; 
    sin.sin_port = htons(port);
    if (!inet_aton(server_ip, &sin.sin_addr)) {
        return CC_E_ERR;
    }

    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));    

    memset(sin.sin_zero, 0, sizeof sin.sin_zero);
    if (connect(fd, (struct sockaddr *)&sin, sizeof(struct sockaddr)) == -1) {
        log_err_for_cc("connect");
        return CC_E_ERR;
    }

    return fd;
}


int 
cc_close_socket(cc_socket *cc_socket)
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
cc_init_listenfd(cc_socket* cc_socket)
{
	int ret;

//	cc_socket_ = (cc_socket*)malloc(sizeof(cc_socket));
	
	ret = cc_conn_init(cc_socket);
	if( ret < 0 )
		return CC_E_ERR;
	else
		return CC_E_NONE;
}


/*
 *func:cc_init_of_socket can be used to init listenfd and connfd
 *param@ cc_socket_ is define in cc_basic.h
 */
int
cc_init_of_socket(cc_socket* cc_socket)
{
	int ret;

	ret = cc_init_listenfd(cc_socket);
	if(ret < 0)
	{
		log_err_for_cc("create sock error!");
		return CC_E_ERR;
	}
	return CC_E_NONE;
}


int
cc_finalize_of_socket(int fd)
{
	close(fd);
	return CC_E_NONE;
}


void
cc_init_sw_info(sw_info* cc_sw_info)
{
	//sw_info* cc_sw_info;
	//cc_sw_info = (sw_info*)malloc(sizeof(cc_sw_info));
	cc_sw_info->xid_latest= 0;
	cc_sw_info->recv_queue = create_message_queue();
	cc_sw_info->send_queue = create_message_queue();
	pool_init(&(cc_sw_info->cc_recv_thread_pool), CC_MAX_THREAD_NUM);
	cc_init_xid_table(&(cc_sw_info->xid_table_));
	cc_sw_info->xid_latest = 0;
	cc_sw_info->config_flags = CC_MAX_THREAD_NUM;
	cc_sw_info->miss_send_len = OFP_DEFAULT_MISS_SEND_LEN;
	cc_sw_info->cookie = 0;
	return;
}


int
cc_finalize_sw_info(sw_info *cc_sw_info)
{
	delete_message_queue(cc_sw_info->recv_queue);
	delete_message_queue(cc_sw_info->send_queue);
	pool_destroy(cc_sw_info->cc_recv_thread_pool);
	free(cc_sw_info);
    
	return CC_E_NONE;
}


int 
cc_conn_accept(cc_socket* cc_socket)
{
	struct sockaddr_in switch_addr;
	socklen_t addr_len;
	pid_t pid;
	int accept_fd;
	int ret;
	socklen_t *len;

	addr_len = sizeof(struct sockaddr_in);
	accept_fd = accept(cc_socket->fd,(struct sockaddr*)&switch_addr,&addr_len);
	if(accept_fd < 0)
	{
		log_err_for_cc("|ERR|accept failed\n");
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
			close(cc_socket->fd);// close the listen socket in child process
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
		}
        if(pid > 0){
			/* this is parent*/
			//cc_sw_info->cc_switch->pid = pid;
			close(accept_fd);
			return CC_E_NONE;
		}
	}

	return CC_E_NONE;
}


int
cc_polling(cc_socket* cc_socket)
{
	int ret;
	fd_set listen_fdset;
	int max_fd = cc_socket->fd + 1;

	FD_ZERO(&listen_fdset);
	FD_SET(cc_socket->fd, &listen_fdset);
	while(1)
	{
		
		FD_ZERO(&listen_fdset);
		FD_SET(cc_socket->fd,&listen_fdset);
		ret = select(max_fd,&listen_fdset,NULL,NULL,0);
		if( ret == -1 )
		{
			if( errno == EINTR )				
				continue;
			else
				return CC_E_ERR;
		}else if( ret == 0 ){
			continue;
		}else{
			if(FD_ISSET(cc_socket->fd, &listen_fdset))
			{
				ret = cc_conn_accept(cc_socket);
				if( ret < 0 ){
					log_err_for_cc("accept failed!");
					return CC_E_ERR;
				}
			}
		}
	}
	return CC_E_NONE;
}

#if 0
void
cc_app_init(const char** app_name,int num)
{
	int ret;
	int i;
	
	for(i = 0; i < num; i++)
	{
		ret = cc_app_register(*app_name);
		if(ret < 0)
		{
			log_err_for_cc("register app error!");
		}
	}
}


void
cc_app_run(struct cc_app* cc_app)
{
	;
}
#endif
