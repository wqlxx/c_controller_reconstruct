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


int
client_socket_create(void)
{
    struct sockaddr_in sin;
    int fd;
    int one = 1;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        return fd;
    }
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET; 
    sin.sin_port = htons(6633);
    sin.sin_addr.s_addr = inet_addr("127.0.0.1");

    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));    
	int i = 1;
    while(1){
    	if (connect(fd, (struct sockaddr *)&sin, sizeof(struct sockaddr)) == -1) {
        	perror("connect");
            sleep(i);
            i++;
        	continue;
    	}
        else
		    break;
	}

    return fd;
}


int
main(int argc, char **argv)
{	
	int fd;
	int ret;
    char *sendline = "this is test";
REPEAT:
	fd = client_socket_create();
    if(fd < 0)
        goto REPEAT;

	while(1){
    	ret = write(fd, sendline, strlen(sendline));
    	if(ret < 0)
        	perror("write");

		printf ("ret is %d\n", ret);
        sleep(5);
        close(fd);
        goto REPEAT;
	}
    return 0;
}

