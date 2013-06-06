/*
 * cc_of_msg_recv functions.
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

#ifndef CC_OF_HANDLER_H
#define CC_OF_HANDLER_H 1

#include "cc_basic.h"
#include "cc_of_err.h"
#include "cc_of_msg_prep.h"
#include "cc_of_action.h"
#include "cc_xid.h"
#include "cc_sw_info.h"
#include "queue.h"
#include "cc_single_link.h"
#include "cc_buffer.h"
#include "cc_log.h"

#define CC_DEFAULT_MISS_SEND_LEN 128
#define CC_DEFAULT_CONFIG_FLAGS 0

struct flow {
    uint32_t            nw_src;            /* IP source address. */
    uint32_t            nw_dst;            /* IP destination address. */
    uint16_t            in_port;           /* Input switch port. */
    uint16_t            dl_vlan;           /* Input VLAN id. */
    uint16_t            dl_type;           /* Ethernet frame type. */
    uint16_t            tp_src;            /* TCP/UDP source port. */
    uint16_t            tp_dst;            /* TCP/UDP destination port. */
    uint8_t             dl_src[6];         /* Ethernet source address. */
    uint8_t             dl_dst[6];         /* Ethernet destination address. */
    uint8_t             dl_vlan_pcp;       /* Input VLAN priority. */
    uint8_t             nw_tos;            /* IPv4 DSCP. */
    uint8_t             nw_proto;          /* IP protocol. */
	//uint8_t				mpls_label;		   /* MPLS LABLE*/
    uint8_t             pad[3];
};

struct ofmsg_recv_s{
    message_queue *rmq;
    message_queue *smq;
    message_queue *amq;
    each_sw *cc_switch;
    xid_table *xid_table_;
    uint32_t *xid_lateet;
};
typedef struct ofmsg_recv_s ofmsg_recv_t;

/*declear the recv function
*/
int cc_insert_to_send_queue(message_queue *smq, buffer* buf);

int cc_insert_to_app_queue(message_queue *asq, buffer* buf);

char* cc_dump_flow(struct flow* flow,uint32_t wildcards);

int	cc_recv_hello_msg(ofmsg_recv_t *ort);

int cc_recv_err_msg(ofmsg_recv_t *ort);

int cc_recv_echo_request(ofmsg_recv_t ort);

int cc_recv_echo_reply(ofmsg_recv_t *ort);

int cc_recv_vendor(ofmsg_recv_t *ort);

int cc_recv_get_config_reply(ofmsg_recv_t *ort);

int cc_recv_flow_removed(ofmsg_recv_t *ort);

int cc_recv_barrier_reply(oomsg_recv_t *ort);

int cc_recv_features_reply(ofmsg_recv_t *ort);

int cc_recv_packet_in(ofmsg_recv_t *ort);

int cc_recv_port_status(ofmsg_recv_t *ort);

int cc_recv_stats_reply(ofmsg_recv_t *ort);

int cc_recv_flow_stats_reply(ofmsg_recv_t *ort);
#endif //end of cc_of_msg_recv

