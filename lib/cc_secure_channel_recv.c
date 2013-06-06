/*
 * cc_secure_channel_recv functions.
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

#include "cc_secure_channel_recv.h"


int
cc_ofpmsg_handle(sw_info* cc_sw_info, uint8_t type)
{
	int ret;
	struct ofp_header *header;
	uint16_t error_type,error_code;
	log_info_for_cc("get a msg from the secure channel");

	if(func_cb == NULL)
	{
		log_err_for_cc("handlerofmsg error!");
		return CC_E_ERR;
	}
	ret = pool_add_worker(cc_sw_info->cc_recv_thread_pool, func_cb, cc_sw_info);
	
	return CC_E_NONE;
}

uint8_t
cc_recv_from_secure_channel(sw_info *cc_sw_info)
{

	buffer *tmp_buff; 
	size_t tmp_recv_buff_length;
	size_t msg_len;
	struct ofp_header *header;
	
	tmp_buff = alloc_buffer_with_length(CC_RECV_BUFFER_SIZE);
	if(tmp_buff == NULL)
	{
		log_debug_for_cc("create buffer failed!");
		return NULL;
	}

	tmp_recv_buff_length = read(cc_sw_info->cc_switch.cc_socket.fd, tmp_buff->data, CC_BUFFER_SIZE);
	if(tmp_recv_buff_length < 0)
	{
		log_err_for_cc("recv failed!");
		if( errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK )
		{
			return NULL;
		}
		//reconnect();
		return NULL;
	}else if(tmp_recv_buff_length == 0){
		//return CC_E_ERR;
		return NULL;
	}
		
	/*input the msg in queue*/
	//enqueue_message(cc_sw_info->recv_queue,tmp_buff);

	if( tmp_recv_buff_length >= sizeof(struct ofp_header) ) 
	{
		
		/*it will reload the tmp_buff->length with thc really recv length ,
		 *no more than the CC_BUFFER_SIZE
		 */
		tmp_buff->length = tmp_recv_buff_length;

		header = (struct ofp_header*)(tmp_buff->data);
		if ( header->version != OFP_VERSION ) {
      		log_err_for_cc( "Receive error: invalid version ");
      		cc_send_error_msg( cc_sw_info,OFPET_BAD_REQUEST, OFPBRC_BAD_VERSION, NULL );
      		return CC_E_ERR;
   		}
		
		uint16_t message_length = ntohs( header->length );
		if( message_length > CC_RECV_BUFFER_SIZE )
		{
			log_err_for_cc("recv msg size is larger than buff");
			return CC_E_ERR;
		}

		enqueue_message(cc_sw_info->recv_queue, tmp_buff);
	}	

	return header->type;
}


int
cc_secure_channel_read(sw_info* cc_sw_info)
{
	int ret;
	buffer* buf;
	uint8_t type;
	
    type = cc_recv_from_secure_channel(cc_sw_info);
	if(ret == -1)
		return CC_E_ERR;

	cc_ofpmsg_handle(cc_sw_info, type);

	return CC_E_NONE;
}

