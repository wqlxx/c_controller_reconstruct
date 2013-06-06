/*
 * cc_of_msg_handler functions.
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

#ifndef CC_OF_MSG_HANDLER_H
#define CC_OF_MSG_HANDLER_H 1

#include "cc_secure_channel_recv.h"
#include "cc_secure_channel_send.h"
#include "cc_buffer.h"
#include "cc_queue.h"
#include "cc_log.h"

struct msg_for_app {
	buffer* buf;
	uint64_t dpid;
};
typedef struct msg_for_app msg_for_app;

int cc_of_handler_recv_event(sw_info* cc_sw_info);

int cc_of_handler_send_event(sw_info* cc_sw_info);

int cc_of_handler_recv_app_request(uint8_t type_, buffer* buf);

int cc_of_handler_send_to_app(message_queue* app_queue);


#endif//END OF CC_OF_MSG_HANDLER_H

