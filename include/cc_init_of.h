/*
 * cc_of_msg_action functions.
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
#ifndef CC_INIT_OF_H
#define CC_INIT_OF_H 1

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

/****add ssl****/
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
/***************/
#include "cc_basic.h"
#include "cc_socket.h"
#include "cc_single_link.h"
#include "cc_log.h"
#include "cc_hash_table.h"
#include "cc_queue.h"
#include "cc_xid.h"
#include "cc_thread_pool.h"
#include "cc_cc_of_msg_handler.h"

#define CC_LISTEN_PORT 6633
#define CC_LENGTH_OF_LISTEN_QUEUE 20
 //#define CC_TRY_CONN_TIMES 5
#define CC_ACCEPT_FD 3
#define CC_CHILD 1
#define CC_PARENT 0
#define CC_CONN_TIMEOUT_SEC 0
#define CC_CONN_TIMEOUT_USEC 0
#define CC_MAX_SOCKET_BUFF 1600
#define CC_MAX_PORT 52
#define CC_MAX_THREAD_NUM 3
  
 
int cc_client_socket_create(char *server_ip, uint16_t port);
int cc_init_of_socket(cc_socket* cc_socket);
int cc_polling(cc_socket* cc_socket);
int cc_finalize_of_socket(int fd);

#endif //END OF CC_INIT_OF_H

