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

#include "cc_of_msg_handler.h"


int 
cc_of_handler_recv_event(message_queue *mq)
{
	int ret;
	ret = cc_secure_channel_read(mq);
	if( ret < 0 )
	{
		log_err_for_cc("recv_from_secure_channel error!");
		return CC_E_ERR;
	}

	return CC_E_NONE;
}


int
cc_of_handler_send_event(message_queue *mq)
{
	int ret;
	
	ret = cc_flush_to_secure_channel(mq);
	if( ret < 0 )
	{
		log_err_for_cc("cc_flush_to_secure_channel error");
		return CC_E_ERR;
	}

	return CC_E_NONE;
}


int
cc_of_handler_recv_app_request(uint8_t type_, buffer* buf)
{
	/*parse the request from app*/
	return CC_E_NONE;
}


int
cc_of_handler_send_to_app(message_queue* app_queue)
{
	int ret;
	/*shared memory*/
	return CC_E_NONE;
}

#endif

