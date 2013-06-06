/*
 * Utility functions.
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

#ifndef CC_SECURE_CHANNLE_RECV_H
#define CC_SECURE_CHANNEL_RECV_H 1

#include "cc_basic.h"
#include "cc_of_msg_recv.h"
#include "queue.h"
#include "cc_thread_pool.h"
#include "cc_sw_info.h"
#include "cc_log.h"
#include "cc_buffer.h"
#include "cc_of_msg_check.h"


#define CC_RECV_BUFFER_SIZE 1024

struct trans_info{
    buffer *recv_buf;
    message_queue *send_queue;
    message_queue *app_queue;
    uint32_t xid_latest;
    xid_table *xid_table_;
    each_sw_t *sw_info;
};
typedef struct trans_info trans_info_t;

/*
struct ofmsg_recv_s{
    message_queue *rmq;
    message_queue *smq;
    message_queue *amq;
    each_sw *cc_switch;
    xid_table *xid_table_;
    uint32_t *xid_lateet;
};
typedef struct ofmsg_recv_s ofmsg_recv_t;
*/

struct cc_writev_args {
	struct iovec *iov;
	int iovcnt;
};
typedef struct cc_writev_args cc_writev_args;

struct cc_recv_ops_s{

    int version;
    
	int (*recv_hello)(trans_info_t *);
    
    int (*recv_error)(trans_info_t *);
    
    int (*recv_echo_request)(trans_info_t *);

    int (*recv_echo_reply)(trans_info_t *);

    int (*recv_vendor)(trans_info_t *);

    int (*recv_get_config_reply)(trans_info_t *);

    int (*recv_flow_rm)(trans_info_t *);  

    int (*barrier_reply)(trans_info_t *);

    int (*recv_packet_in)(trans_info_t *);

    int (*recv_port_status)(trans_info_t *);

	int (*recv_flow_stats_reply)(trans_info_t *);
};


//FUNC_CB cc_select_handler(uint16_t type);
extern const struct cc_recv_ops_s cc_recv_ops;
//extern const struct cc_recv_1_3_ops_s cc_recv_1_3_ops;/*prepare for spec 1.3*/

int cc_secure_channel_read(sw_info* cc_sw_info);

uint8_t cc_recv_from_secure_channel(sw_info *cc_sw_info);

int cc_ofpmsg_handle(sw_info* cc_sw_info, uint8_t type);

#endif


