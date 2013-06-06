#ifndef CC_BASIC_H
#define CC_BASIC_H 1

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

#include "cc_queue.h"
#include "cc_buffer.h"
#include "cc_single_link.h"
#include "cc_thread_pool.h"
#include "cc_xid.h"

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
    message_queue *app_queue;
    
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

enum cc_ofp_version{
    CC_OFP_VER_1_0 0x01
};

#endif

