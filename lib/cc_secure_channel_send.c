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


#include "cc_secure_channel_send.h"


int
cc_send_to_secure_channel_app(sw_info* cc_sw_info,buffer* buf)
{
	int ret;
	buffer* msg;

    while(queue_is_empty(cc_sw_info->app_queue) == CC_E_ERR){
        msg = dequeue_message(cc_sw_info->app_queue);
        free_buffer(msg);
    }
/*
	if( cc_sw_info->app_send_queue == NULL )
		cc_sw_info->app_send_queue = create_message_queue();

	ret = enqueue_message(cc_sw_info->send_queue, buf);
*/
    
	return CC_E_NONE;
}


int
cc_flush_to_secure_channel(sw_info* cc_sw_info)
{
	buffer* msg;
	int count = 0;
	while(( msg = dequeue_message(cc_sw_info->send_queue)) != NULL && count < 50)
	{
		struct ofp_header* header = msg->data;
		int len = header->length;
		ssize_t write_length = write(cc_sw_info->cc_switch.cc_socket.fd, msg->data, msg->length);
		if( write_length < 0 )
		{
			if ( errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK ) 
			{
				free_buffer(msg);
				return CC_E_ERR;
			}
			log_err_for_cc("fail to write a message to secure channel");
			free_buffer(msg);
			return CC_E_ERR;
		}else if( (size_t)write_length > 0 && (size_t)write_length < msg->length ) {
			log_err_for_cc("write msg to secure channel error!");
			//write( cc_sw_info->cc_switch->cc_socket->fd,msg,sizeof(msg));
			//free_buffer(msg);
			continue;
		}
		free_buffer(msg);
		count++;
	}
	return CC_E_NONE;
}

