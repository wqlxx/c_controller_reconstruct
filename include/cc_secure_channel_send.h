/*
 * cc_secure_channel_send functions.
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

#ifndef CC_SECURE_CHANNEL_SEND_H
#define CC_SECURE_CHANNEL_SEND_H 1

#include "cc_basic.h"
#include "cc_of_msg_send.h"
#include "cc_queue.h"
#include "cc_buffer.h"
#include "cc_log.h"
#include "cc_of_msg_prep.h"
#include "cc_error.h"


struct cc_send_ops_s{
    int version;
    int (*send_hello)(sw_info*);
    int (*send_error)();
    int (*send_echo_reply)();
    int (*send_echo_request)();
    int (*send_feature_request)();
    int (*send_get_config_request)();
    int (*send_set_config)();
    int (*send_packet_out)();
    int (*send_flow_stats_request)();
    int (*send_get_desc_stats)();
    int (*send_aggregate_stats_request)();
    int (*send_table_stats_request)();
    int (*send_port_stats_request)();
    int (*send_queue_stats_request)();
    int (*send_vendor_stats_request)();
}

extern const struct cc_send_ops_s cc_send_ops;

int cc_send_to_secure_channel(sw_info* cc_sw_info,buffer* buf);

int cc_send_to_secure_channel_app(sw_info* cc_sw_info,buffer* buf);

int cc_flush_to_secure_channel(sw_info* cc_sw_info);

#endif
