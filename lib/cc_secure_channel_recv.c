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

static const struct cc_recv_ops_s *cc_of_handler_list[] = {
    &cc_recv_ops
};

struct cc_recv_ops_s*
cc_ofpmsg_handle_init(int version)
{
    if( version == 0x01 )
        return cc_of_handler_list[0];
    else
        return NULL;
}

static trans_info_t*
create_trans_info(sw_info *cc_sw_info)
{
    trans_info_t *_trans_info;
    _trans_info = malloc(sizeof(trans_info_t));
    _trans_info->recv_buf     = dequeue_message(cc_sw_info->recv_queue);
    _trans_info->xid_latest   = cc_sw_info->xid_latest;
    _trans_info->send_queue   = cc_sw_info->send_queue;
    _trans_info->app_queue    = cc_sw_info->app_queue;
    _trans_info->xid_table_   = cc_sw_info->xid_table_;
    _trans_info->sw_info      = cc_sw_info->cc_switch;
    return _trans_info;
}

int
cc_ofpmsg_handle(trans_info_t *trans_info)
{
	int ret;
	struct ofp_header *header;
    buffer *recv_buf;
    struct cc_recv_ops_s *of_ops;
    void *func_cb(sw_info*);

    if(cc_of_handler_list[0] == NULL)
	{
		log_err_for_cc("handler ofmsg error!");
		return CC_E_ERR;
	}
    of_ops = cc_of_handler_list[0]; 

    recv_buf = dequeue_message(trans_info->recv_queue);
    header = (struct ofp_header*)recv_buf->data;
    switch(header->type){
        case OFPT_HELLO:
            if(of_ops->recv_hello)
                of_ops->recv_hello(trans_info);
            break;
        case OFPT_ECHO_REQUEST:
            if(of_ops->recv_echo_request)
               of_ops->recv_echo_request(trans_info); 
            break;        
        case OFPT_ECHO_REPLY:
            if(of_ops->recv_echo_reply)
                of_ops->recv_echo_reply(trans_info);
            break;        
        case OFPT_ERROR:
            if(of_ops->recv_error)
                of_ops->recv_error(trans_info);
            break;        
        case OFPT_PACKET_IN:
            if(of_ops->recv_packet_in)
                of_ops->recv_packet_in(trans_info);
            break;        
        case OFPT_VENDOR:
            if(of_ops->recv_vendor)
                of_ops->recv_vendor(trans_info);   
            break;           
        case OFPT_PORT_STATUS:
            if(of_ops->recv_port_status)
                of_ops->recv_port_status(trans_info);
            break;  
        case OFPT_FLOW_REMOVED:
            if(of_ops->recv_flow_rm)
                of_ops->recv_flow_rm(trans_info);
            break;  
        case OFPT_STATS_REPLY:
            if(of_ops->recv_flow_stats_reply)
                of_ops->recv_flow_stats_reply(trans_info);
            break;  
        case OFPT_GET_CONFIG_REPLY:
            if(of_ops->recv_get_config_reply)
                of_ops->recv_get_config_reply(trans_info);
            break;  
        default:
            func_cb = NULL;
            break;
    }
    
    if(func_cb == NULL){
       log_err_for_cc("error type of openflow msg");
       goto ERR_H;
    } 
	if(ret == CC_E_ERR)
        goto ERR_H;
    
	return CC_E_NONE;

    
ERR_H:
    free_buffer(recv_buf);
    return CC_E_ERR;
}

int
cc_recv_from_secure_channel(sw_info *cc_sw_info)
{

	buffer *tmp_buff; 
	size_t tmp_recv_buff_length;
	size_t msg_len;
	struct ofp_header *header;
    int ret;
    trans_info_t *trans_info;
	
	tmp_buff = alloc_buffer_with_length(CC_RECV_BUFFER_SIZE);
	if(tmp_buff == NULL)
	{
		log_debug_for_cc("create buffer failed!");
		return CC_E_ERR;
	}

	tmp_recv_buff_length = read(cc_sw_info->cc_switch.cc_socket.fd, tmp_buff->data, CC_RECV_BUFFER_SIZE);
	if(tmp_recv_buff_length < 0)
	{
		log_err_for_cc("recv failed!");
		if( errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK )
		{
			return CC_E_ERR;
		}
		//reconnect();
		return NULL;
	}else if(tmp_recv_buff_length == 0){
		return CC_E_ERR;
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
    trans_info = create_trans_info(cc_sw_info);
	ret = pool_add_worker(cc_sw_info->cc_recv_thread_pool, cc_ofpmsg_handle, trans_info);
	if(ret == CC_E_ERR)
        return CC_E_ERR;
    return CC_E_NONE;
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

