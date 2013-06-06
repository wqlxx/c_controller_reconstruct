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
 
 
 struct cc_socket_t{
     struct sockaddr_in cc_addr;
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
     //struct ofp_phy_port port[CC_MAX_PORT];
     list_element *ports;
 };
 typedef struct each_sw_t each_sw;
 
 
 struct sw_info_t{
     each_sw cc_switch;
     pthread_t cc_sw_thread[CC_MAX_THREAD_NUM];
 
     CThread_pool* cc_recv_thread_pool;
 
     message_queue *send_queue;
     message_queue *recv_queue;
     
     xid_table* xid_table_;
     uint32_t xid_latest;
     /*for app func to handle the msg send to app
      *param: app_fd is the file description to connect to app server
      *param: app_server_ip is the ip address of app server
      *param: message to restore the app_msg to send to app server
      */
     /*int app_fd;
     char* app_server_ip;
     message_queue *app_recv_queue;
     message_queue *app_send_queue;*/
 
     uint32_t cookie;
 
     /*set config*/
     uint16_t config_flags;        // OFPC_* flags
     uint16_t miss_send_len;       /* Max bytes of new flow that datapath should
                                    send to the controller. */
                                     
 };
 typedef struct sw_info_t sw_info;
 
 
int cc_client_socket_create(char *server_ip, uint16_t port);
int cc_init_of_socket(cc_socket* cc_socket);
int cc_polling(cc_socket* cc_socket);
int cc_finalize_of_socket(int fd);

#endif //END OF CC_INIT_OF_H

